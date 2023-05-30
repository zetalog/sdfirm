#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>
#include <target/irq.h>
#include <target/task.h>
#include <asm/asm-offsets.h>

#ifdef CONFIG_TASK
	.macro save_kern_sp
	REG_S	sp, TASK_KERN_SP(tp)
	.endm
	.macro load_kern_sp
	REG_S	sp, TASK_KERN_SP(tp)
	.endm
	.macro save_scratch_sp
	REG_S	sp, TASK_USER_SP(tp)
	.endm
	.macro load_scratch_sp
	REG_L	s0, TASK_USER_SP(tp)
	.endm
#else
	.macro save_kern_sp
	.endm
	.macro load_kern_sp
	.endm
	.macro save_scratch_sp
	REG_S	sp, SCRATCH_SP(tp)
	.endm
	.macro load_scratch_sp
	REG_L	s0, SCRATCH_SP(tp)
	.endm
#endif

#ifdef CONFIG_TASK
ENTRY(__switch_to)
	/* Save context into prev->thread */
	li    a4,  TASK_THREAD_RA
	add   a3, a0, a4
	add   a4, a1, a4
	REG_S ra,  TASK_THREAD_RA_RA(a3)
	REG_S sp,  TASK_THREAD_SP_RA(a3)
	REG_S s0,  TASK_THREAD_S0_RA(a3)
	REG_S s1,  TASK_THREAD_S1_RA(a3)
	REG_S s2,  TASK_THREAD_S2_RA(a3)
	REG_S s3,  TASK_THREAD_S3_RA(a3)
	REG_S s4,  TASK_THREAD_S4_RA(a3)
	REG_S s5,  TASK_THREAD_S5_RA(a3)
	REG_S s6,  TASK_THREAD_S6_RA(a3)
	REG_S s7,  TASK_THREAD_S7_RA(a3)
	REG_S s8,  TASK_THREAD_S8_RA(a3)
	REG_S s9,  TASK_THREAD_S9_RA(a3)
	REG_S s10, TASK_THREAD_S10_RA(a3)
	REG_S s11, TASK_THREAD_S11_RA(a3)
	/* Restore context from next->thread */
	REG_L ra,  TASK_THREAD_RA_RA(a4)
	REG_L sp,  TASK_THREAD_SP_RA(a4)
	REG_L s0,  TASK_THREAD_S0_RA(a4)
	REG_L s1,  TASK_THREAD_S1_RA(a4)
	REG_L s2,  TASK_THREAD_S2_RA(a4)
	REG_L s3,  TASK_THREAD_S3_RA(a4)
	REG_L s4,  TASK_THREAD_S4_RA(a4)
	REG_L s5,  TASK_THREAD_S5_RA(a4)
	REG_L s6,  TASK_THREAD_S6_RA(a4)
	REG_L s7,  TASK_THREAD_S7_RA(a4)
	REG_L s8,  TASK_THREAD_S8_RA(a4)
	REG_L s9,  TASK_THREAD_S9_RA(a4)
	REG_L s10, TASK_THREAD_S10_RA(a4)
	REG_L s11, TASK_THREAD_S11_RA(a4)
	/* Swap the CPU entry around. */
	lw a3, TASK_THREAD_CPU(a0)
	lw a4, TASK_THREAD_CPU(a1)
	sw a3, TASK_THREAD_CPU(a1)
	sw a4, TASK_THREAD_CPU(a0)
	move tp, a1
	ret
ENDPROC(__switch_to)
#endif

	.macro SAVE_ALL

	/* If coming from userspace, preserve the user thread pointer and load
	 * the kernel thread pointer.  If we came from the kernel, sscratch
	 * will contain 0, and we should continue on the current TP.
	 */
	csrrw	tp, CSR_SCRATCH, tp
	bnez	tp, 8888f

	csrr	tp, CSR_SCRATCH
	save_kern_sp
8888:
	save_scratch_sp
	load_kern_sp
	addi	sp, sp, -(PT_SIZE_ON_STACK)
	REG_S	x1,  PT_RA(sp)
	REG_S	x3,  PT_GP(sp)
	REG_S	x5,  PT_T0(sp)
	REG_S	x6,  PT_T1(sp)
	REG_S	x7,  PT_T2(sp)
	REG_S	x8,  PT_S0(sp)
	REG_S	x9,  PT_S1(sp)
	REG_S	x10, PT_A0(sp)
	REG_S	x11, PT_A1(sp)
	REG_S	x12, PT_A2(sp)
	REG_S	x13, PT_A3(sp)
	REG_S	x14, PT_A4(sp)
	REG_S	x15, PT_A5(sp)
	REG_S	x16, PT_A6(sp)
	REG_S	x17, PT_A7(sp)
	REG_S	x18, PT_S2(sp)
	REG_S	x19, PT_S3(sp)
	REG_S	x20, PT_S4(sp)
	REG_S	x21, PT_S5(sp)
	REG_S	x22, PT_S6(sp)
	REG_S	x23, PT_S7(sp)
	REG_S	x24, PT_S8(sp)
	REG_S	x25, PT_S9(sp)
	REG_S	x26, PT_S10(sp)
	REG_S	x27, PT_S11(sp)
	REG_S	x28, PT_T3(sp)
	REG_S	x29, PT_T4(sp)
	REG_S	x30, PT_T5(sp)
	REG_S	x31, PT_T6(sp)

	load_scratch_sp
#if defined(CONFIG_RISCV_F) && defined(CONFIG_RISCV_EXIT_S)
	/* Disable user-mode memory access as it should only be set in the
	 * actual user copy routines.
	 * Disable the FPU to detect illegal usage of floating point in kernel
	 * space.
	 */
	li	t0, SR_SUM | SR_FS
	csrrc	s1, CSR_STATUS, t0
#else
	csrr	s1, CSR_STATUS
#endif
	csrr	s2, CSR_EPC
	csrr	s3, CSR_TVAL
	csrr	s4, CSR_CAUSE
	csrr	s5, CSR_SCRATCH
	REG_S	s0, PT_SP(sp)
	REG_S	s1, PT_STATUS(sp)
	REG_S	s2, PT_EPC(sp)
	REG_S	s3, PT_BADADDR(sp)
	REG_S	s4, PT_CAUSE(sp)
	REG_S	s5, PT_TP(sp)
	.endm

	.macro RESTORE_ALL
	REG_L	a0, PT_STATUS(sp)
	REG_L	a2, PT_EPC(sp)
	csrw	CSR_STATUS, a0
	csrw	CSR_EPC, a2

	REG_L	x1,  PT_RA(sp)
	REG_L	x3,  PT_GP(sp)
	REG_L	x4,  PT_TP(sp)
	REG_L	x5,  PT_T0(sp)
	REG_L	x6,  PT_T1(sp)
	REG_L	x7,  PT_T2(sp)
	REG_L	x8,  PT_S0(sp)
	REG_L	x9,  PT_S1(sp)
	REG_L	x10, PT_A0(sp)
	REG_L	x11, PT_A1(sp)
	REG_L	x12, PT_A2(sp)
	REG_L	x13, PT_A3(sp)
	REG_L	x14, PT_A4(sp)
	REG_L	x15, PT_A5(sp)
	REG_L	x16, PT_A6(sp)
	REG_L	x17, PT_A7(sp)
	REG_L	x18, PT_S2(sp)
	REG_L	x19, PT_S3(sp)
	REG_L	x20, PT_S4(sp)
	REG_L	x21, PT_S5(sp)
	REG_L	x22, PT_S6(sp)
	REG_L	x23, PT_S7(sp)
	REG_L	x24, PT_S8(sp)
	REG_L	x25, PT_S9(sp)
	REG_L	x26, PT_S10(sp)
	REG_L	x27, PT_S11(sp)
	REG_L	x28, PT_T3(sp)
	REG_L	x29, PT_T4(sp)
	REG_L	x30, PT_T5(sp)
	REG_L	x31, PT_T6(sp)

	REG_L	x2,  PT_SP(sp)
	.endm

	.option norvc

#ifdef CONFIG_RISCV_VIRQ
ENTRY(__handle_traps)
#else
#ifdef __VEC_ALIGN
__ENTRY(__vectors, __VEC_ALIGN)
#else
ENTRY(__vectors)
#endif
#endif
	SAVE_ALL

	/* Set the scratch register to 0, so that if a recursive exception
	 * occurs, the vector knows it came from self.
	 */
	csrw	CSR_SCRATCH, zero

	la	ra, ret_from_exception

	/* Distinguish interrupt/exception */
	bge	s4, zero, 1f

#ifndef CONFIG_SYS_NOIRQ
	move	a0, sp /* pt_regs */
	tail	do_riscv_interrupt
#endif

1:
	/* Exceptions run with interrupts enabled or disabled depending on the
	 * state of SR_PIE in CSR_STATUS.
	 */
	andi	t0, s1, SR_PIE
	beqz	t0, 1f
	csrs	CSR_STATUS, SR_IE

1:
	/* Handle exceptions */
	slli	t0, s4, RISCV_LGPTR
	la	t1, __sexctab
	la	t2, __eexctab
	move	a0, sp /* pt_regs */
	add	t0, t1, t0
	/* Check exception array bounds */
	bgeu	t0, t2, 1f
	REG_L	t0, 0(t0)
	jr	t0

1:
	tail	do_trap_unknown

ret_from_exception:
	REG_L	s0, PT_STATUS(sp)
	csrc	CSR_STATUS, SR_IE
	li	t0, SR_PP
	and	s0, s0, t0
	bnez	s0, restore_all

restore_all:
	RESTORE_ALL
#ifdef CONFIG_RISCV_EXIT_M
	mret
#endif
#ifdef CONFIG_RISCV_EXIT_S
	sret
#endif
#ifdef CONFIG_RISCV_VIRQ
END(__handle_traps)
#else
END(__vectors)
#endif

	.section ".rodata"
ENTRY(exc_table)
__sexctab:
	RISCV_PTR do_trap_insn_misaligned
	RISCV_PTR do_trap_insn_fault
	RISCV_PTR do_trap_insn_illegal
	RISCV_PTR do_trap_unknown /* do_trap_breakpoint */
	RISCV_PTR do_trap_load_misaligned
	RISCV_PTR do_trap_load_fault
	RISCV_PTR do_trap_store_misaligned
	RISCV_PTR do_trap_store_fault
	RISCV_PTR do_trap_ecall_u
	RISCV_PTR do_trap_ecall_s
#ifdef CONFIG_RISCV_H
	RISCV_PTR do_trap_ecall_h
#else
	RISCV_PTR do_trap_unknown
#endif
	RISCV_PTR do_trap_ecall_m
	RISCV_PTR do_trap_unknown /* do_page_fault (insn) */
	RISCV_PTR do_trap_unknown /* do_page_fault (load) */
	RISCV_PTR do_trap_unknown
	RISCV_PTR do_trap_unknown /* do_page_fault (store) */
	RISCV_PTR do_trap_unknown
	RISCV_PTR do_trap_unknown
	RISCV_PTR do_trap_unknown
	RISCV_PTR do_trap_unknown
#ifdef CONFIG_RISCV_H
	RISCV_PTR do_guest_page_fault /* insn */
	RISCV_PTR do_guest_page_fault /* load */
	RISCV_PTR do_virtual_insn_illegal
	RISCV_PTR do_guest_page_fault /* store */
#endif
__eexctab:
END(exc_table)

#ifdef CONFIG_RISCV_VIRQ
	.macro	define_virq entry, handler
ENTRY(\entry)
	SAVE_ALL
	csrw	CSR_SCRATCH, zero
	la	ra, 8888f
#ifndef CONFIG_SYS_NOIRQ
	move	a0, sp /* pt_regs */
	tail	\handler
#endif
8888:
	REG_L	s0, PT_STATUS(sp)
	csrc	CSR_STATUS, SR_IE
	li	t0, SR_PP
	and	s0, s0, t0
	bnez	s0, 1f
1:
	RESTORE_ALL
	mret
END(\entry)
	.endm

define_virq __handle_bad_virq _start_hang

#include "asm/mach/virq.h"
#endif
