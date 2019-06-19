#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <target/arch.h>
#include <target/smp.h>
#include <asm/assembler.h>
#include <asm/asm-offsets.h>

#define ARM_ROM_OFFSET	0x850

	__HEAD

#ifdef CONFIG_BOOT_ROM
.org 0
ENTRY(stext)
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

ENTRY(stext)
#endif
	# zero init BSS region
	ldr	x0, =__bss_start
	ldr	x1, =__bss_stop
	mov	x2, xzr
bss_init_loop:
	stp	x2, x2, [x0], #16
	cmp	x0, x1
	blt	bss_init_loop

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

	# switch different EL entries
	mrs	x0, CurrentEL
	cmp	x0, #0xc
	b.ne	stext_el1

stext_el3:
#ifdef CONFIG_SYS_MONITOR
	# Enable cache and stack alignment check
	# Disable MMU, alignment check, write XN
	# Keep endianness unchanged
	ldr	x0, =(SCTLR_EL3_RES1 | SCTLR_I | SCTLR_C | SCTLR_SA)
	msr	SCTLR_EL3, x0
	# Make sure we throw away anything fetched before the
	# MMU/Caches were in a known state
	dsb	sy
	isb

	ldr	x0, =vectors_el3
	msr	VBAR_EL3, x0

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

	# Enable hardware coherency between cores via
	# CPUECTLR_EL1.SMPEN
        mrs	x0, S3_1_c15_c2_1
        orr	x0, x0, #(1 << 6)
        msr	S3_1_c15_c2_1, x0
        isb

	# No feature trap
	msr	CPTR_EL3, xzr

stext_el2:
	# Disable VM and VM traps
	# Lower EL is AARCH64
	ldr	x0, =HCR_RW
	msr	HCR_EL2, x0

	msr	SCTLR_EL2, xzr

#ifdef CONFIG_BOOT_LINUX
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
#endif

	# Linux requirement - exit to EL2h
#if defined(CONFIG_BOOT_LINUX) || defined(CONFIG_SYS_HYPERVISOR)
#define SPSR_MODE_KERNEL	SPSR_MODE_EL2h
#else
#define SPSR_MODE_KERNEL	SPSR_MODE_EL1h
#endif

#ifdef CONFIG_SYS_KERNEL
	ldr	x0, =stext_el1
	mov	x1, #(SPSR_E | SPSR_A | SPSR_I | SPSR_F | SPSR_MODE_KERNEL)
	msr	ELR_EL3, x0
	msr	SPSR_EL3, x1
	eret
#endif

stext_el1:
#ifdef CONFIG_BOOT_ROM
	mov	x0, xzr
	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
#endif

#ifdef CONFIG_BOOT_LINUX
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
#else
	bl	system_init
#endif
ENDPIPROC(stext)
