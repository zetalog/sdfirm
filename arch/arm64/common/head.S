#include <target/config.h>
#include <target/compiler.h>
#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>
#include <asm/asm-offsets.h>

#ifndef ARM_ROM_OFFSET
#define ARM_ROM_OFFSET	0
#endif

#ifndef CONFIG_ARCH_HAS_BOOT0
	.macro	boot0_hook
	nop
	.endm
#endif

#define __PHYS_OFFSET	(SDFIRM_START - TEXT_OFFSET)

	__HEAD

__head:
	boot0_hook

#ifdef CONFIG_BOOT_ROM
//.org 0
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

	.macro	init_sp
#ifdef CONFIG_SMP
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
	.endm

.macro copy_sect, sla, sva, eva, lab_exit, lab_loop
	adr_l	x2, \sla
	adr_l	x0, \sva
	adr_l	x1, \eva
	b	\lab_exit
\lab_loop:
	ldp	x3, x4, [x2], #16
	stp	x3, x4, [x0], #16
\lab_exit:
	cmp	x0, x1
	b.lt	\lab_loop
	dsb	ishst
.endm

.macro zero_sect, sva, eva, lab_exit, lab_loop
	adr_l	x0, \sva
	adr_l	x1, \eva
	mov	x2, xzr
	b	\lab_exit
\lab_loop:
	stp	x2, x2, [x0], #16
\lab_exit:
	cmp	x0, x1
	b.lt	\lab_loop
	dsb	ishst
.endm

ENTRY(__start)
	init_sp
#ifdef CONFIG_LOAD_TEXT
	copy_sect __text_loc, _stext, _etext, copy_text_exit, copy_text_loop
#endif
#ifdef CONFIG_LOAD_DATA
	copy_sect __data_loc, __sdata, __edata, copy_data_exit, copy_data_loop
#endif
	zero_sect __bss_start, __bss_stop, init_bss_exit, init_bss_loop

#ifdef CONFIG_SMP
        /* Enable hardware coherency:
	 * Read EL1 CPU Extended Control Register
	 * Set the SMPEN bit
	 * Write EL1 CPU Extended Control Register
	 */
	mrs	x0, S3_1_c15_c2_1
	orr	x0, x0, #(1 << 6)
	msr	S3_1_c15_c2_1, x0
	isb
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

	# Enable CPU interfaces before exiting to EL1
	# early timer initialization
	call_l	delay_init
#ifdef CONFIG_BOOT_LINUX
	# Let the SMP handling code to initialize GICC where the GICC
	# register addresses are valid mappings for the GIC drivers.
	# However, when Linux kernel is booted, SMP code won't do that for
	# kernel which requires GICC to be initialized by the bootloader.
#ifdef CONFIG_SMP
	call_l	irq_smp_init
#endif
#endif

#ifdef CONFIG_SYS_KERNEL
	# switch different EL entries
	mrs	x0, CurrentEL
	cmp	x0, #CURRENT_EL3
	b.ne	init_lo_el

	ldr	x0, =init_lo_el
	mov	x1, #(PSR_E | PSR_A | PSR_I | PSR_F | PSR_MODE_KERNEL)
	msr	ELR_EL3, x0
	msr	SPSR_EL3, x1
	eret
#endif
init_lo_el:
#if !defined(CONFIG_BOOT_LINUX) && defined(CONFIG_SMP)
	mrs	x0, MPIDR_EL1
	and	x0, x0, #0xFF
	cbnz	x0, _smp_spin
#endif

	bl	__create_page_tables

#ifdef CONFIG_SYS_MONITOR
	# Enable cache and stack alignment check Disable MMU, alignment
	# check, write XN, keep endianness unchanged
	ldr	x0, =(SCTLR_EL3_RES1 | SCTLR_I | SCTLR_C | SCTLR_SA)
#endif

#ifdef CONFIG_MMU
	# Address to jump to after MMU has been enabled
	ldr	x27, =stext
	# return (PIC) address
	adr_l	lr, __enable_mmu
	b	__cpu_setup
#else
	ldr	x27, =stext
	br	x27
#endif
ENDPROC(__start)

ENTRY(stext)
	bl	__linux_boot_el2
#ifdef CONFIG_SMP
	bl	smp_map_init
#endif
	bl	system_init
ENDPIPROC(stext)

#ifdef CONFIG_SMP
ENTRY(smp_hw_cpu_id)
	mrs	x0, MPIDR_EL1
	and	x1, x0, #MPIDR_CPU_MASK
	and	x0, x0, #MPIDR_CLUSTER_MASK
	lsr	x0, x0, #MPIDR_AFF1_SHIFT
	mov	x2, #CPUS_PER_CLUSTER
	madd	x0, x0, x2, x1
	ret
ENDPROC(smp_hw_cpu_id)

ENTRY(_smp_start)
	init_sp

#ifdef CONFIG_MMU
	# Address to jump to after MMU has been enabled
	mov	x27, x4
#ifdef CONFIG_MMU_IDMAP
	ldr	x25, =mmu_pg_dir
#else
	ldr	x25, =mmu_id_map
#endif
	# return (PIC) address
	adr_l	lr, __enable_mmu
	b	__cpu_setup
#else
	mov	x27, x4
	br	x27
#endif
ENDPROC(_smp_start)
#endif

ENTRY(__init_exception_level)
	# switch different EL entries
	mrs	x0, CurrentEL
	cmp	x0, #CURRENT_EL3
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

	call_l	__linux_boot_el3

init_el2:
	# Disable VM and VM traps
	# Lower EL is AARCH64
	ldr	x0, =HCR_RW
	msr	HCR_EL2, x0

init_el1:
	# Enable hardware coherency between cores via
	# CPUECTLR_EL1.SMPEN
	mrs	x0, CPUECTLR_EL1
	orr	x0, x0, #(1 << 6)
	msr	CPUECTLR_EL1, x0
	isb
	ret
ENDPROC(__init_exception_level)

#ifdef CONFIG_BOOT_LINUX
ENTRY(__linux_boot_el3)
	# EL3/EL2 kernel bootloader
	# early timer initialization
	call_l	delay_init

#ifdef CONFIG_SMP
	# Only proceed on boot CPU
	mrs	x0, MPIDR_EL1
	and	x0, x0, #0xFF
	cbnz	x0, _smp_spin
#endif

	# early GICD/GICR initialization
	call_l	irq_init
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
	call_l	console_init

	ldr	x0, =0x88000000
	ldr	x6, =0x80080000
	br	x6
ENDPROC(__linux_boot_el2)
#else
ENTRY(__linux_boot_el3)
	ret
ENDPROC(__linux_boot_el3)
ENTRY(__linux_boot_el2)
	init_sp
	ret
ENDPROC(__linux_boot_el2)
#endif

# Setup the initial page tables. We only setup the barest amount which is
# required to get the kernel running. The following sections are required:
#   - identity mapping to enable the MMU (low address, TTBR0)
#   - first few MB of the kernel linear mapping to jump to once the MMU
#     has been enabled
ENTRY(__create_page_tables)
#ifdef CONFIG_MMU
	adrp	x24, __PHYS_OFFSET
	ldr	x25, =mmu_id_map
	mov	x27, lr

	# Invalidate the idmap and swapper page tables to avoid potential
	# dirty cache lines being evicted.
	mov	x0, x25
	ldr	x1, =BPGT_DIR_SIZE
	call_l	__inval_dcache_area

	# Clear the idmap and boot page tables.
	mov	x0, x25
	add	x6, x25, #BPGT_DIR_SIZE
1:	stp	xzr, xzr, [x0], #16
	stp	xzr, xzr, [x0], #16
	stp	xzr, xzr, [x0], #16
	stp	xzr, xzr, [x0], #16
	cmp	x0, x6
	b.lo	1b

	ldr	x7, =BPGT_MM_MMUFLAGS

#ifdef CONFIG_VMSA_VA_2_RANGES
	# Create the identity mapping.
	mov	x0, x25				// idmap
	adrp	x3, __idmap_text_start		// __pa(__idmap_text_start)

#ifndef CONFIG_VMSA_VA_256TB
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
	cmp	x5, TCR_T0SZ(64 - VA_BITS)	// default T0SZ small enough?
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
#endif

	# Map the sdfirm image (starting with PHYS_OFFSET).
	mov	x0, x25				/* bpgt */
	mov	x5, #PAGE_OFFSET
	create_pgd_entry x0, x5, x3, x6
	ldr	x6, =SDFIRM_END			// __pa(_edata)
	add	x6, x6, #PERCPU_STACKS_SIZE
	mov	x3, x24				// PHYS_OFFSET
	create_block_map x0, x7, x3, x5, x6

#if defined(CONFIG_MMU_IDMAP_DEVICE) && defined(IDMAP_DEV_BASE)
	# Map identity device area for early console
	mov	x0, x25				// idmap
	mov	x5, #IDMAP_DEV_BASE
	locate_pgd_entry x0
	mov	x6, #IDMAP_DEVEND
	mov	x3, x5
	ldr	x7, =BPGT_MM_DEVFLAGS
	create_block_map x0, x7, x3, x5, x6
#endif

	# Since the page tables have been populated with non-cacheable
	# accesses (MMU disabled), invalidate the idmap and swapper page
	# tables again to remove any speculatively loaded cache lines.
	mov	x0, x25
	ldr	x1, =BPGT_DIR_SIZE
	dmb	sy
	call_l	__inval_dcache_area

	mov	lr, x27
#endif
	ret
ENDPROC(__create_page_tables)
	.ltorg

	/* tcr_set_idmap_t0sz - update TCR.T0SZ so that we can load the ID
	 * map
	 */
	.macro	tcr_set_idmap_t0sz, valreg, tmpreg
#if (defined(CONFIG_CPU_64v8_2_LVA) && (VMSA_VA_SIZE_SHIFT != 52)) || \
    (!defined(CONFIG_CPU_64v8_2_LVA) && (VMSA_VA_SIZE_SHIFT != 48))
	ldr_l	\tmpreg, idmap_t0sz
	bfi	\valreg, \tmpreg, #TCR_T0SZ_OFFSET, #TCR_TxSZ_WIDTH
#endif
	.endm

ENTRY(__cpu_setup)
	tlbi	vmalle1				// Invalidate local TLB
	dsb	nsh

	# Enable FP/ASIMD
	mov	x0, #3 << 20
	msr	CPACR_EL1, x0
	# Reset mdscr_el1 and disable
	# access to the DCC from EL0
	mov	x0, #1 << 12
	msr	MDSCR_EL1, x0
	# Memory region attributes for LPAE:
	#   n = AttrIndx[2:0]
	#			n	MAIR
	#   DEVICE_nGnRnE	000	00000000
	#   DEVICE_nGnRE	001	00000100
	#   DEVICE_GRE		010	00001100
	#   NORMAL_NC		011	01000100
	#   NORMAL		100	11111111
	#   NORMAL_WT		101	10111011
	ldr	x5, =MAIR(MAIR_ATTR_DEVICE_nGnRnE, MT_DEVICE_nGnRnE) | \
		     MAIR(MAIR_ATTR_DEVICE_nGnRE, MT_DEVICE_nGnRE) | \
		     MAIR(MAIR_ATTR_DEVICE_GRE, MT_DEVICE_GRE) | \
		     MAIR(MAIR_ATTR_NORMAL_NC, MT_NORMAL_NC) | \
		     MAIR(MAIR_ATTR_NORMAL_WB, MT_NORMAL) | \
		     MAIR(MAIR_ATTR_NORMAL_WT, MT_NORMAL_WT) | \
		     MAIR(MAIR_ATTR_DEVICE_nGRE, MT_DEVICE_nGRE)
	msr	MAIR_EL1, x5
	# Prepare SCTLR
	adr	x5, crval
	ldp	w5, w6, [x5]
	mrs	x0, SCTLR_EL1
	bic	x0, x0, x5			// clear bits
	orr	x0, x0, x6			// set bits
	# Set/prepare TCR and TTBR. We use 512GB (39-bit) address range for
	# both user and kernel.
	ldr	x10, =TCR_TxSZ(64 - VA_BITS) | \
		      TCR_CACHE_FLAGS | TCR_SMP_FLAGS | \
		      TCR_TG_FLAGS | TCR_AS | TCR_TBI0
	tcr_set_idmap_t0sz	x10, x9

	# Read the PARange bits from ID_AA64MMFR0_EL1 and set the IPS bits in
	# TCR_EL1.
	mrs	x9, ID_AA64MMFR0_EL1
	bfi	x10, x9, #32, #3
#ifdef CONFIG_CPU_64v8_1_TTHM
	# Hardware update of the Access and Dirty bits.
	mrs	x9, ID_AA64MMFR1_EL1
	and	x9, x9, #0xf
	cbz	x9, 2f
	cmp	x9, #2
	b.lt	1f
	orr	x10, x10, #TCR_HD		// hardware Dirty flag update
1:	orr	x10, x10, #TCR_HA		// hardware Access flag update
2:
#endif
	msr	TCR_EL1, x10
	ret					// return to head.S
ENDPROC(__cpu_setup)

	/*
	 * We set the desired value explicitly, including those of the
	 * reserved bits. The values of bits EE & E0E were set early in
	 * el2_setup, which are left untouched below.
	 *
	 *                 n n            T
	 *       U E      WT T UD     US IHBS
	 *       CE0      XWHW CZ     ME TEEA S
	 * .... .IEE .... NEAI TE.I ..AD DEN0 ACAM
	 * 0011 0... 1101 ..0. ..0. 10.. .0.. .... < hardware reserved
	 * .... .1.. .... 01.1 11.1 ..01 0.01 1101 < software settings
	 */
	.type	crval, #object
crval:
	.word	0xfcffffff			// clear
	.word	0x34d5d91d			// set

# Enable the MMU.
#  x0  = SCTLR_EL1 value for turning on the MMU.
#  x27 = *virtual* address to jump to upon completion
# Other registers depend on the function called upon completion.
# Checks if the selected granule size is supported by the CPU.If it isn't,
# park the CPU.
	.section	".idmap.text", "ax"
ENTRY(__enable_mmu)
	ldr	x5, =vectors_el3
	msr	VBAR_KERN, x5
	ldr	x26, =empty_zero_page
	msr	TTBR_USER, x26			// load TTBR0
	msr	TTBR_KERN, x25			// load TTBR1
	isb
	msr	SCTLR_KERN, x0
	isb
	# Invalidate the local I-cache so that any instructions fetched
	# speculatively from the PoC are discarded, since they may have
	# been dynamically patched at the PoU.
	ic	iallu
	dsb	nsh
	isb
	br	x27
ENDPROC(__enable_mmu)

# cpu_do_switch_pgd(pgd_phys, tsk)
# Set the translation table base pointer to be pgd_phys.
#  - pgd_phys - physical address of new TTB
ENTRY(cpu_do_switch_pgd)
	mrs		x2, TTBR_USER
	phys_to_ttbr	x3, x0

#if 0
	mmid		x1, x1			// get asid
alternative_if ARM64_HAS_CNP
	cbz		x1, 1f			// skip CNP for reserved ASID
	orr		x3, x3, #TTBR_CNP_BIT
1:
alternative_else_nop_endif
#ifdef CONFIG_CPU_64v8_1_PAN
	bfi		x3, x1, #48, #16	// set the ASID field in TTBR0
#endif
	bfi		x2, x1, #48, #16	// set the ASID
#endif
	msr		TTBR_USER, x2		// in TTBR1 (since TCR.A1 is set)
	isb
	msr		TTBR_KERN, x3		// now update TTBR0
	isb
	ret
ENDPROC(cpu_do_switch_pgd)

.macro	__cpu_set_reserved_ttbr_u, tmp1, tmp2
	adrp		\tmp1, empty_zero_page
	phys_to_ttbr	\tmp2, \tmp1
	offset_ttbr1	\tmp2
	msr		TTBR_USER, \tmp2
	isb
	tlbi		vmalle1
	dsb		nsh
	isb
.endm
ENTRY(idmap_cpu_replace_ttbr1)
	save_and_disable_daif		flags=x2
	__cpu_set_reserved_ttbr_u	x1, x3
	offset_ttbr1			x0
	msr				TTBR_KERN, x0	// now update TTBR0
	isb
	restore_daif			x2
	ret
ENDPROC(idmap_cpu_replace_ttbr1)
