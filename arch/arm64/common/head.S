#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <target/arch.h>
#include <target/smp.h>
#include <target/paging.h>
#include <asm/assembler.h>
#include <asm/asm-offsets.h>

#define ARM_ROM_OFFSET	0x850

	__HEAD

__head:
#ifdef CONFIG_BOOT_ROM
.org 0
	b	__arm_reset
.org 0x080
	b	.
.org 0x100
	b	.
.org 0x180
	b	.
.org 0x200
	b	.
.org 0x280
	b	.
.org 0x300
	b	.
.org 0x380
	b	.
.org 0x400
	b	__psci_handler
.org 0x480
	b	.
.org 0x500
	b	.
.org 0x580
	b	.
.org 0x600
	b	.

__arm_reset:
	mov	x0, xzr
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
	mov	x4, xzr
	mov	x5, xzr
	mov	x6, xzr
	mov	x7, xzr
	mov	x8, xzr
	mov	x9, xzr
	mov	x10, xzr
	mov	x11, xzr
	mov	x12, xzr
	mov	x13, xzr
	mov	x14, xzr
	mov	x15, xzr
	mov	x16, xzr
	mov	x17, xzr
	mov	x18, xzr
	mov	x19, xzr
	mov	x20, xzr
	mov	x21, xzr
	mov	x22, xzr
	mov	x23, xzr
	mov	x24, xzr
	mov	x25, xzr
	mov	x26, xzr
	mov	x27, xzr
	mov	x28, xzr
	mov	x29, xzr
	mov	x30, xzr
#else
#ifdef CONFIG_XIP
	.fill ARM_ROM_OFFSET
#endif
#endif

ENTRY(__start)
#ifdef CONFIG_SMP
	# Using one page for each CPU stack:
	mrs	x0, MPIDR_EL1
	and	x1, x0, #0xff
	and	x0, x0, #0xff00
	lsr	x0, x0, #7
	add	x0, x0, x1

	add	x0, x0, #1
	lsl	x0, x0, #PERCPU_STACK_SHIFT
	ldr	x3, =PERCPU_STACKS_START
	add	x0, x3, x0
	mov	sp, x0
#else
	ldr	x0, =PERCPU_STACKS_END
	mov	sp, x0
#endif

	bl	__init_exception_level

	# Exit to suitable exception level
	# monitor: keep EL3
	# linux bootloader: exit to EL2
	# kernel: exit to EL1
#if defined(CONFIG_BOOT_LINUX) || defined(CONFIG_SYS_HYPERVISOR)
	# Linux requirement - exit to EL2h
#define PSR_MODE_KERNEL		PSR_MODE_EL2h
#else
#define PSR_MODE_KERNEL		PSR_MODE_EL1h
#endif
#ifdef CONFIG_SYS_KERNEL
	# switch different EL entries
	mrs	x0, CurrentEL
	cmp	x0, #0xc
	b.ne	init_lo_el

	ldr	x0, =init_lo_el
	mov	x1, #(PSR_E | PSR_A | PSR_I | PSR_F | PSR_MODE_KERNEL)
	msr	ELR_EL3, x0
	msr	SPSR_EL3, x1
	eret
#endif
init_lo_el:
	bl	__create_page_tables

#ifdef CONFIG_SYS_MONITOR
	# Enable cache and stack alignment check
	# Disable MMU, alignment check, write XN
	# Keep endianness unchanged
	ldr	x0, =(SCTLR_EL3_RES1 | SCTLR_I | SCTLR_C | SCTLR_SA)
#endif
#ifdef CONFIG_SYS_KERNEL
	# TODO: configure SCTLR_EL1
#endif

#ifdef CONFIG_MMU
        ldr     x27, =stext		// address to jump to after
	# MMU has been enabled
        bl	__enable_mmu		// return (PIC) address
#endif
	bl	stext
ENDPROC(__start)

ENTRY(stext)
	# zero init BSS region
	adr_l	x0, __bss_start
	adr_l	x1, __bss_stop
	mov	x2, xzr
bss_init_loop:
	stp	x2, x2, [x0], #16
	cmp	x0, x1
	b.lt	bss_init_loop
	dsb	ishst

#ifdef CONFIG_SMP
	# Using one page for each CPU stack:
	mrs	x0, MPIDR_EL1
	and	x1, x0, #0xff
	and	x0, x0, #0xff00
	lsr	x0, x0, #7
	add	x0, x0, x1

	add	x0, x0, #1
	lsl	x0, x0, #PERCPU_STACK_SHIFT
	ldr	x3, =PERCPU_STACKS_START
	add	x0, x3, x0
	mov	sp, x0
#else
	ldr	x0, =PERCPU_STACKS_END
	mov	sp, x0
#endif

	bl	__linux_boot_el2
	bl	system_init
ENDPIPROC(stext)

ENTRY(__init_exception_level)
	# switch different EL entries
	mrs	x0, CurrentEL
	cmp	x0, #0xc
	b.ne	init_el1

init_el3:
#ifdef CONFIG_SYS_MONITOR
	# Do not trap WFE/WFI/SMC/HVC
	# Route SError and External Aborts, IRQ, FIQ
	# Permit non-secure secure instruction fetch
#define SCR_EL3_DEFAULT	(SCR_EA | SCR_IRQ | SCR_FIQ | SCR_ST)
#else
#define SCR_EL3_DEFAULT	SCR_HCE
#endif
	# Lower EL is AARCH64 and non-secure
	ldr	x0, =(SCR_EL3_RES1 | SCR_NS | SCR_RW | \
		      SCR_EL3_DEFAULT)
	msr	SCR_EL3, x0

	# No feature trap
	msr	CPTR_EL3, xzr

	bl	__linux_boot_el3

init_el2:
	# Disable VM and VM traps
	# Lower EL is AARCH64
	ldr	x0, =HCR_RW
	msr	HCR_EL2, x0

init_el1:
	# Enable hardware coherency between cores via
	# CPUECTLR_EL1.SMPEN
	mrs	x0, S3_1_c15_c2_1
	orr	x0, x0, #(1 << 6)
	msr	S3_1_c15_c2_1, x0
	isb
	ret
ENDPROC(__init_exception_level)

#ifdef CONFIG_BOOT_LINUX
ENTRY(__linux_boot_el3)
	# EL3/EL2 kernel bootloader
	# early delay initialization
	bl	delay_init

	# Only proceed on boot CPU
	mrs	x0, MPIDR_EL1
	tst     x0, #0xf
1:
	b.ne	1b
	# early GICD/GICR initialization
	bl	irq_init
	ret
ENDPROC(__linux_boot_el3)
ENTRY(__linux_boot_el2)
	# Only proceed on boot CPU
	mrs	x4, MPIDR_EL1
	tst	x4, #0xf
	b.eq    3f
2:
	wfe
	# Check if non-boot CPUs can jump
	ldr	x4, =0x8000fff8
	ldr	x4, [x4]
	cbz	x4, 2b
	br	x4

3:
	# early console initialization
	bl	console_init

	ldr	x0, =0x88000000
	ldr	x6, =0x80080000
	br	x6
	ret
ENDPROC(__linux_boot_el2)
#else
ENTRY(__linux_boot_el3)
	ret
ENDPROC(__linux_boot_el3)
ENTRY(__linux_boot_el2)
	ret
ENDPROC(__linux_boot_el2)
#endif

#define SDFIRM_END	__end

# Macro to create a table entry to the next page.
#	tbl:	page table address
#	virt:	virtual address
#	shift:	#imm page table shift
#	ptrs:	#imm pointers per table page
# Preserves:	virt
# Corrupts:	tmp1, tmp2
# Returns:	tbl -> next level table page address
	.macro	create_table_entry, tbl, virt, shift, ptrs, tmp1, tmp2
	lsr	\tmp1, \virt, #\shift
	and	\tmp1, \tmp1, #\ptrs - 1	// table index
	add	\tmp2, \tbl, #PAGE_SIZE
	orr	\tmp2, \tmp2, #PMD_TYPE_TABLE	// address of next table and entry type
	str	\tmp2, [\tbl, \tmp1, lsl #3]
	add	\tbl, \tbl, #PAGE_SIZE		// next level table page
	.endm

# Macro to populate the PGD (and possibily PUD) for the corresponding
# block entry in the next level (tbl) for the given virtual address.
# Preserves:	tbl, next, virt
# Corrupts:	tmp1, tmp2
	.macro	create_pgd_entry, tbl, virt, tmp1, tmp2
	create_table_entry \tbl, \virt, PGDIR_SHIFT, PTRS_PER_PGD, \tmp1, \tmp2
#if PGTABLE_LEVELS > 3
	create_table_entry \tbl, \virt, PUD_SHIFT, PTRS_PER_PUD, \tmp1, \tmp2
#endif
#if PGTABLE_LEVELS > 2
	create_table_entry \tbl, \virt, BPGT_TABLE_SHIFT, PTRS_PER_PTE, \tmp1, \tmp2
#endif
	.endm

# Macro to populate block entries in the page table for the start..end
# virtual range (inclusive).
# Preserves:	tbl, flags
# Corrupts:	phys, start, end, pstate
	.macro	create_block_map, tbl, flags, phys, start, end
	lsr	\phys, \phys, #BPGT_BLOCK_SHIFT
	lsr	\start, \start, #BPGT_BLOCK_SHIFT
	and	\start, \start, #PTRS_PER_PTE - 1		// table index
	orr	\phys, \flags, \phys, lsl #BPGT_BLOCK_SHIFT	// table entry
	lsr	\end, \end, #BPGT_BLOCK_SHIFT
	and	\end, \end, #PTRS_PER_PTE - 1			// table end index
9999:	str	\phys, [\tbl, \start, lsl #3]			// store the entry
	add	\start, \start, #1				// next entry
	add	\phys, \phys, #BPGT_BLOCK_SIZE			// next block
	cmp	\start, \end
	b.ls	9999b
	.endm

# Setup the initial page tables. We only setup the barest amount which is
# required to get the kernel running. The following sections are required:
#   - identity mapping to enable the MMU (low address, TTBR0)
#   - first few MB of the kernel linear mapping to jump to once the MMU
#     has been enabled
ENTRY(__create_page_tables)
#ifdef CONFIG_MMU
	adrp	x25, mmu_id_map
	adrp	x26, mmu_pg_dir
	mov	x27, lr

	# Invalidate the idmap and swapper page tables to avoid potential
	# dirty cache lines being evicted.
	mov	x0, x25
	add	x1, x26, #BPGT_DIR_SIZE
	bl	__inval_dcache_area

	# Clear the idmap and boot page tables.
	mov	x0, x25
	add	x6, x26, #BPGT_DIR_SIZE
1:	stp	xzr, xzr, [x0], #16
	stp	xzr, xzr, [x0], #16
	stp	xzr, xzr, [x0], #16
	stp	xzr, xzr, [x0], #16
	cmp	x0, x6
	b.lo	1b

	ldr	x7, =BPGT_MM_MMUFLAGS

	# Create the identity mapping.
	mov	x0, x25				// idmap_pg_dir
	adrp	x3, __idmap_text_start		// __pa(__idmap_text_start)

#if VMSA_VA_SIZE_SHIFT != 48
#define EXTRA_SHIFT	(PGDIR_SHIFT + PAGE_SHIFT - 3)
#define EXTRA_PTRS	(1 << (48 - EXTRA_SHIFT))

	# If VA_BITS < 48, it may be too small to allow for an ID mapping
	# to be created that covers system RAM if that is located
	# sufficiently high in the physical address space. So for the ID
	# map, use an extended virtual range in that case, by configuring
	# an additional translation level.
	# First, we have to verify our assumption that the current value
	# of VA_BITS was chosen such that all translation levels are fully
	# utilised, and that lowering T0SZ will always result in an
	# additional translation level to be configured.
#if VA_BITS != EXTRA_SHIFT
#error "Mismatch between VA_BITS and page size/number of translation levels"
#endif

	# Calculate the maximum allowed value for TCR_EL1.T0SZ so that the
	# entire ID map region can be mapped. As
	#  T0SZ == (64 - #bits used),
	# this number conveniently equals the number of leading zeroes in
	# the physical address of __idmap_text_end.
	adrp	x5, __idmap_text_end
	clz	x5, x5
	cmp	x5, TCR_T0SZ(VA_BITS)	// default T0SZ small enough?
	b.ge	1f			// .. then skip additional level

	adr_l	x6, idmap_t0sz
	str	x5, [x6]
	dmb	sy
	dc	ivac, x6		// Invalidate potentially stale cache line

	create_table_entry x0, x3, EXTRA_SHIFT, EXTRA_PTRS, x5, x6
1:
#endif

	create_pgd_entry x0, x3, x5, x6
	mov	x5, x3				// __pa(__idmap_text_start)
	adr_l	x6, __idmap_text_end		// __pa(__idmap_text_end)
	create_block_map x0, x7, x3, x5, x6

	# Map the sdfirm image (starting with PHYS_OFFSET).
	mov	x0, x26				// mmu_pg_dir
	mov	x5, #PAGE_OFFSET
	create_pgd_entry x0, x5, x3, x6
	ldr	x6, =SDFIRM_END			// __va(SDFIRM_END)
	mov	x3, x24				// phys offset
	create_block_map x0, x7, x3, x5, x6

	# Since the page tables have been populated with non-cacheable
	# accesses (MMU disabled), invalidate the idmap and swapper page
	# tables again to remove any speculatively loaded cache lines.
	mov	x0, x25
	add	x1, x26, #BPGT_DIR_SIZE
	dmb	sy
	bl	__inval_dcache_area

	mov	lr, x27
#endif
	ret
ENDPROC(__create_page_tables)
	.ltorg

# Enable the MMU.
#  x0  = SCTLR_EL1 value for turning on the MMU.
#  x27 = *virtual* address to jump to upon completion
# Other registers depend on the function called upon completion.
# Checks if the selected granule size is supported by the CPU.If it isn't,
# park the CPU.
	.section	".idmap.text", "ax"
ENTRY(__enable_mmu)
#if defined(CONFIG_SYS_MONITOR)
	ldr	x5, =vectors_el3
	msr	VBAR_EL3, x5
	msr	TTBR0_EL3, x26			// load TTBR0
	msr	SCTLR_EL3, x0
	isb
#elif defined(CONFIG_SYS_HYPERVISOR)
#elif defined(CONFIG_SYS_KERNEL)
	ldr	x5, =vectors_el1
	msr	VBAR_EL1, x5
	msr	TTBR0_EL1, x25			// load TTBR0
	msr	TTBR1_EL1, x26			// load TTBR1
	isb
	msr	SCTLR_EL1, x0
	isb
#endif
	# Invalidate the local I-cache so that any instructions fetched
	# speculatively from the PoC are discarded, since they may have
	# been dynamically patched at the PoU.
	ic	iallu
	dsb	nsh
	isb
ENDPROC(__enable_mmu)
