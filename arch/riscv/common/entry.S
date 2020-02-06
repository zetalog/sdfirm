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

#ifdef CONFIG_ARCH_HAS_NOVEC
	.macro SAVE_ALL
	.local _restore_kernel_tpsp
	.local _save_context

	/* If coming from userspace, preserve the user thread pointer and load
	 * the kernel thread pointer.  If we came from the kernel, sscratch
	 * will contain 0, and we should continue on the current TP.
	 */
	csrrw	tp, CSR_SCRATCH, tp
	bnez	tp, _save_context

_restore_kernel_tpsp:
	csrr	tp, CSR_SCRATCH
	save_kern_sp
_save_context:
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
	__HEAD

ENTRY(__vectors)
	SAVE_ALL

	/* Set the scratch register to 0, so that if a recursive exception
	 * occurs, the vector knows it came from self.
	 */
	csrw	CSR_SCRATCH, zero

	init_gp

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
END(__vectors)

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
	RISCV_PTR do_trap_unknown
	RISCV_PTR do_trap_ecall_m
	RISCV_PTR do_trap_unknown /* do_page_fault (insn) */
	RISCV_PTR do_trap_unknown /* do_page_fault (load) */
	RISCV_PTR do_trap_unknown
	RISCV_PTR do_trap_unknown /* do_page_fault (store) */
__eexctab:
END(exc_table)
#endif
