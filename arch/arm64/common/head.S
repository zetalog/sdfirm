#include <target/config.h>
#include <target/linkage.h>
#include <target/init.h>
#include <asm/assembler.h>
#include <asm/asm-offsets.h>
#include <asm/reg.h>

	__HEAD

#ifdef CONFIG_XIP
	.fill 0x850
#endif

ENTRY(stext)
	# Enable cache and stack alignment check
	# Disable MMU, alignment check, write XN
	# Keep endianness unchanged
	ldr	x0, =(SCTLR_EL3_RES1 | SCTLR_I | SCTLR_C | SCTLR_SA)
	msr	SCTLR_EL3, x0
	# Make sure we throw away anything fetched before the
	# MMU/Caches were in a known state
	dsb	sy
	isb

	# Do not trap WFE/WFI/SMC/HVC
	# Route SError and External Aborts, IRQ, FIQ
	# Permit non-secure secure instruction fetch
	# Lower EL is AARCH64 and non-secure
	ldr	x0, =(SCR_EL3_RES1 | SCR_NS | SCR_RW | \
		      SCR_EA | SCR_IRQ| SCR_FIQ | \
		      SCR_ST | SCR_RW | SCR_NS)
	msr	SCR_EL3, x0

	# No feature trap
	msr	CPTR_EL3, xzr

	# Disable VM and VM traps
	# Lower EL is AARCH64
	ldr	x0, =HCR_RW
	msr	HCR_EL2, x0

	ldr	x0, =vectors
	msr	VBAR_EL3, x0

	/* zero init BSS region */
	ldr	x0, =__bss_start
	ldr	x1, =__bss_stop
	mov	x2, xzr
bss_init_loop:
	stp	x2, x2, [x0], #16
	cmp	x0, x1
	blt	bss_init_loop

	ldr	x0, =RAMEND
	mov	sp, x0
	bl	system_init
ENDPIPROC(stext)

ENTRY(vectors)
	b		__bad_interrupt		// reset, PIT
	b		__bad_interrupt		// undefined instruction
#ifdef CONFIG_ARM_MONITOR
	b		__bad_interrupt		// SMC
#else
	b		__bad_interrupt		// SVC
#endif
	b		__bad_interrupt		// prefetch abort
	b		__bad_interrupt		// data abort
	b		__bad_interrupt
	b		__bad_interrupt		// IRQ
	b		__bad_interrupt		// FIQ
ENDPIPROC(vectors)

ENTRY(__bad_interrupt)
	b	.
ENDPIPROC(__bad_interrupt)
