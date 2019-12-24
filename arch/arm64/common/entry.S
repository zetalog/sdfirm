#include <target/config.h>
#include <target/linkage.h>
#include <asm/asm-offsets.h>

/*
 * Branch according to exception level
 */
.macro switch_el, xreg, el3_label, el2_label, el1_label
	mrs	\xreg, CurrentEL
	cmp	\xreg, 0xc
	b.eq	\el3_label
	cmp	\xreg, 0x8
	b.eq	\el2_label
	cmp	\xreg, 0x4
	b.eq	\el1_label
.endm

/*
 * Enter Exception.
 * This will save the processor state that is ELR/X0~X30
 * to the stack frame.
 */
.macro exception_entry
	stp	x29, x30, [sp, #-16]!
	stp	x27, x28, [sp, #-16]!
	stp	x25, x26, [sp, #-16]!
	stp	x23, x24, [sp, #-16]!
	stp	x21, x22, [sp, #-16]!
	stp	x19, x20, [sp, #-16]!
	stp	x17, x18, [sp, #-16]!
	stp	x15, x16, [sp, #-16]!
	stp	x13, x14, [sp, #-16]!
	stp	x11, x12, [sp, #-16]!
	stp	x9, x10, [sp, #-16]!
	stp	x7, x8, [sp, #-16]!
	stp	x5, x6, [sp, #-16]!
	stp	x3, x4, [sp, #-16]!
	stp	x1, x2, [sp, #-16]!
.endm

_bad_mode:
	/* Could be running at EL3/EL2/EL1 */
	switch_el x11, 3f, 2f, 1f
3:
	mrs	x1, esr_el3
	mrs	x3, elr_el3
	mrs	x4, spsr_el3
	b	0f
2:
	mrs	x1, esr_el2
	mrs	x3, elr_el2
	mrs	x4, spsr_el2
	b	0f
1:
	mrs	x1, esr_el1
	mrs	x3, elr_el1
	mrs	x4, spsr_el1
0:
	stp	x3, x0, [sp, #-16]!
	stp	x0, x4, [sp, #-16]!
	mov	x0, sp
	bl	bad_mode
	b	.

_do_irq:
	/* Could be running at EL3/EL2/EL1 */
	switch_el x11, 3f, 2f, 1f
3:
	mrs	x1, esr_el3
	mrs	x3, elr_el3
	mrs	x4, spsr_el3
	b	0f
2:
	mrs	x1, esr_el2
	mrs	x3, elr_el2
	mrs	x4, spsr_el2
	b	0f
1:
	mrs	x1, esr_el1
	mrs	x3, elr_el1
	mrs	x4, spsr_el1
0:
	stp	x3, x0, [sp, #-16]!
	stp	x0, x4, [sp, #-16]!
	mov	x0, sp
#ifdef CONFIG_ARCH_HAS_IRQC
	bl	irqc_hw_handle_irq
#endif
	b	exception_exit

exception_exit:
	ldp	x0, x4, [sp],#16
	ldp	x3, x0, [sp],#16
	switch_el x11, 3f, 2f, 1f
3:
	msr	spsr_el3, x4
	msr	elr_el3, x3
	b	0f
2:
	msr	spsr_el2, x4
	msr	elr_el2,  x3
	b	0f
1:
	msr	spsr_el1, x4
	msr	elr_el1,  x3
0:
	ldp	x1, x2, [sp],#16
	ldp	x3, x4, [sp],#16
	ldp	x5, x6, [sp],#16
	ldp	x7, x8, [sp],#16
	ldp	x9, x10, [sp],#16
	ldp	x11, x12, [sp],#16
	ldp	x13, x14, [sp],#16
	ldp	x15, x16, [sp],#16
	ldp	x17, x18, [sp],#16
	ldp	x19, x20, [sp],#16
	ldp	x21, x22, [sp],#16
	ldp	x23, x24, [sp],#16
	ldp	x25, x26, [sp],#16
	ldp	x27, x28, [sp],#16
	ldp	x29, x30, [sp],#16
	eret

/* reason: SYNC=0 IRQ=1 FIQ=2 SError=3 */
.macro ventry, label, reason
.align 7, 0
	exception_entry
	mov	x2, #\reason
	b	\label
.endm

.align 11
ENTRY(vectors_el3)
	/* -----------------------------------------------------------
	 * Current EL with SP_EL0 : 0x0 - 0x200
	 * ----------------------------------------------------------- */
sync_sp_el0:
	ventry _bad_mode, 0
irq_sp_el0:
	ventry _bad_mode, 1
fiq_sp_el0:
	ventry _bad_mode, 2
serror_sp_el0:
	ventry _bad_mode, 3
	/* -----------------------------------------------------------
	 * Current EL with SP_ELx: 0x200 - 0x400
	 * ----------------------------------------------------------- */
sync_sp_elx:
	ventry _bad_mode, 0
irq_sp_elx:
	ventry _do_irq, 1
fiq_sp_elx:
	ventry _do_irq, 2
serror_sp_elx:
	ventry _bad_mode, 3
	/* -----------------------------------------------------------
	 * Lower EL using AArch64 : 0x400 - 0x600
	 * ----------------------------------------------------------- */
sync_aarch64:
	ventry _bad_mode, 0
irq_aarch64:
	ventry _bad_mode, 1
fiq_aarch64:
	ventry _bad_mode, 2
serror_aarch64:
	ventry _bad_mode, 3
	/* -----------------------------------------------------------
	 * Lower EL using AArch32 : 0x600 - 0x800
	 * ----------------------------------------------------------- */
sync_aarch32:
	ventry _bad_mode, 0
irq_aarch32:
	ventry _bad_mode, 1
fiq_aarch32:
	ventry _bad_mode, 2
serror_aarch32:
	ventry _bad_mode, 3
END(vectors_el3)

.align 11
ENTRY(vectors_el1)
END(vectors_el1)

ENTRY(__bad_interrupt)
	b	.
ENDPIPROC(__bad_interrupt)
