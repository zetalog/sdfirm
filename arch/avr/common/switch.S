#include <target/config.h>
#include "macros.inc"
#include <asm/asm-offsets.h>


;arch_hw_switch_to(prev, next)
;		r25/r24	r23/r22

	.global	__arch_hw_switch_to
	.func	__arch_hw_switch_to
__arch_hw_switch_to:
	push r1
	push r0
	in   r0, _SFR_IO_ADDR(SREG)
	push r0				;*sp-- = SREG
	push r29
	push r28
	push r17
	push r16
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push r7
	push r6
	push r5
	push r4
	push r3
	push r2				;*sp-- = r2
	movw r30, r24			;z = &task_entries
	adiw r30, TASK_ENTRY_SP		;z = &(task_entries[prev].sp)
	in   r1,  _SFR_IO_ADDR(SPH)
	in   r0,  _SFR_IO_ADDR(SPL)
	std  z+1, r1
	st   z,   r0
	sts  task_current+1, r23
	sts  task_current,   r22	;task_current = next
	movw r30, r22
	adiw r30, TASK_ENTRY_SP
	ldd  r1,  z+1
	ld   r0,  z
	out  _SFR_IO_ADDR(SPH), r1
	out  _SFR_IO_ADDR(SPL), r0	;sp = task_entries[next].sp
	pop  r2				;r2 = *(sp++)
	pop  r3
	pop  r4
	pop  r5
	pop  r6
	pop  r7
	pop  r8
	pop  r9
	pop  r10
	pop  r11
	pop  r12
	pop  r13
	pop  r14
	pop  r15
	pop  r16
	pop  r17
	pop  r28
	pop  r29
	pop  r0
	out  _SFR_IO_ADDR(SREG), r0	;SREG = *(sp++)
	pop  r0
	pop  r1
	ret				;PC = *(sp++)
	.endfunc


;arch_hw_init_task(task, call, priv)
;		r25/r24	r23/r22	r21/r20
	
	.global	__arch_hw_init_task
	.func	__arch_hw_init_task
__arch_hw_init_task:
	push r29
	push r28
	push r17
	push r16
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push r7
	push r6
	push r5
	push r4
	push r3
	push r2
	push r1
	push r0
	movw r30, r24			;z = &task_entries[pid]
	adiw r30, TASK_ENTRY_SP		;z = &(task_entries[pid].sp)
	ldd  r1,  z+1
	ld   r0,  z			;z = task_entries[pid].sp
	movw r30, r0
	st   z,   r21
	st   -z,  r20			;*z-- = priv			
	sbiw r30, 1
	st   z,   r22
	st   -z,  r23			;*z-- = call
	sbiw r30, 1
	ldi  r19, 0x80			;0x80 (Interrupt Enable)
	st   z,   r19			
	sbiw r30, 1			;*z-- = 0x80;
	ldi  r19, 0x14			;r0~r17,r28,r29
	eor  r0,  r0
__save_reg:	
	st   z,   r0
	sbiw r30, 1
	subi r19, 1
	cpi  r19, 0
	BRNE __save_reg			;*z-- = 0
	movw r18, r30			;sp
	movw r30, r24
	adiw r30, TASK_ENTRY_SP
	movw r0,  r18
	std  z+1, r1
	st   z,   r0
	pop  r0
	pop  r1
	pop  r2
	pop  r3
	pop  r4
	pop  r5
	pop  r6
	pop  r7
	pop  r8
	pop  r9
	pop  r10
	pop  r11
	pop  r12
	pop  r13
	pop  r14
	pop  r15
	pop  r16
	pop  r17
	pop  r28
	pop  r29
	ret
	.endfunc
