#include <target/tsc.h>

.globl __psci_handler

__psci_handler:
	# PSCI call is Fast Call(atomic), so mask DAIF
	mrs	x15, DAIF
	stp	x15, xzr, [sp, #-16]!
	ldr	x15, =0x3C0
	msr	DAIF, x15
	# SMC convention, x18 ~ x30 should be saved by callee
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	mrs	x15, elr_el3
	stp	x18, x15, [sp, #-16]!

	bl	psci_cmd

	# restore registers
	ldp	x18, x15, [sp], #16
	msr	elr_el3, x15
	ldp	x19, x20, [sp], #16
	ldp	x21, x22, [sp], #16
	ldp	x23, x24, [sp], #16
	ldp	x25, x26, [sp], #16
	ldp	x27, x28, [sp], #16
	ldp	x29, x30, [sp], #16
	# restore DAIF
	ldp	x15, xzr, [sp], #16
	msr	DAIF, x15
	eret

.globl jump_to_kernel

# primary x0: dtb x1: 0 x2: 0 x3: 0 x4: kernel entry
# secondary x0: context x1: 0 x2: 0 x3: 0 x4: kernel entry 
jump_to_kernel:
	mov     x5, #0x30
        orr     x5, x5, #(1 << 10) 
        orr     x5, x5, #(1 << 8) 
	orr	x5, x5, #(1 << 0)
        msr     scr_el3, x5

	msr	cptr_el3, xzr
	mov	x5, #(3 << 12 | 0x3ff)
	msr	cptr_el2, x5

	ldr	x5, =(TSC_FREQ * 1000)
	msr	cntfrq_el0, x5
	msr     cntvoff_el2, xzr

	stp	x0, x4, [sp, #-16]!
	bl	irq_init
	ldp	x0, x4, [sp], #16

	msr	sctlr_el2, xzr

	ldr	x5, =(9 | (1 << 6) | (1 << 7) | (1 << 8) | (1 << 9))
	msr	spsr_el3, x5

	mov	x1, xzr
	mov	x2, xzr
	mov	x3, xzr
	msr	elr_el3, x4
	eret
