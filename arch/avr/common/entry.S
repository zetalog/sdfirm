#include <target/config.h>
#include "macros.inc"

#ifndef CONFIG_CC_ISR_VECTOR
	.macro 	real_vector nr=1
	.if (\nr < _VECTORS_CUNT)
	.global __do_common_intr\nr
__do_common_intr\nr:
	; XXX: Use the register is not operated in prologue.
	; Here we choose r16.
	ldi	r16, \nr
	XJMP	__irq_prologue
	.endif
	.endm

	.global __do_common_intr
	.func	__do_common_intr
__do_common_intr:
	real_vector	1
	real_vector	2
	real_vector	3
	real_vector	4
	real_vector	5
	real_vector	6
	real_vector	7
	real_vector	8
	real_vector	9
	real_vector	10
	real_vector	11
	real_vector	12
	real_vector	13
	real_vector	14
	real_vector	15
	real_vector	16
	real_vector	17
	real_vector	18
	real_vector	19
	real_vector	20
	real_vector	21
	real_vector	22
	real_vector	23
	real_vector	24
	real_vector	25
	real_vector	26
	real_vector	27
	real_vector	28
	real_vector	29
	real_vector	30
	real_vector	31
	real_vector	32
	real_vector	33
	real_vector	34
	real_vector	35
	real_vector	36
	real_vector	37
	real_vector	38
	real_vector	39
	real_vector	40

__irq_prologue:
	push	r1
	push	r0
	in	r0, _SFR_IO_ADDR(SREG)
	push	r0
#ifdef RAMPZ
	in	r0, _SFR_IO_ADDR(RAMPZ)
	push	r0
#endif
	eor	r1, r1
	push	r18
	push	r19
	push	r20
	push	r21
	push	r22
	push	r23
	push	r24
	push	r25
	push	r26
	push	r27
	push	r30
	push	r31
#ifdef CONFIG_FRAME_POINTER
	push	r29
	push	r28
	in	r28, _SFR_IO_ADDR(SPL)
	in	r29, _SFR_IO_ADDR(SPH)
#endif

	; store irq_nr(r24) as the parameter and then call do_IRQ(nr)
	mov	r24, r16
	XCALL	do_IRQ

__irq_epilogue:
#ifdef CONFIG_FRAME_POINTER
	pop	r28
	pop	r29
#endif
	pop	r31
	pop	r30
	pop	r27
	pop	r26
	pop	r25
	pop	r24
	pop	r23
	pop	r22
	pop	r21
	pop	r20
	pop	r19
	pop	r18
#ifdef RAMPZ
	pop	r0
	out	_SFR_IO_ADDR(RAMPZ), r0
#endif
	pop	r0
	out	_SFR_IO_ADDR(SREG), r0
	pop	r0
	pop	r1

	reti
	.endfunc
#endif

