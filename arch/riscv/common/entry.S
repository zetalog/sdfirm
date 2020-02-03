#include <target/init.h>
#include <target/arch.h>
#include <target/paging.h>
#include <target/irq.h>
#include <target/task.h>
#include <asm/asm-offsets.h>

#ifdef CONFIG_ARCH_HAS_NOVEC
#define BAD_TRAP_VECTOR			0
#define TRAP_FROM_MACHINE_MODE_VECTOR	13

	.data
	.align 6
trap_table:
	.word bad_trap
	.word pmp_trap
	.word illegal_insn_trap
	.word bad_trap
	.word misaligned_load_trap
	.word pmp_trap
	.word misaligned_store_trap
	.word pmp_trap
	.word bad_trap
	.word mcall_trap
	.word bad_trap
#ifdef BBL_BOOT_MACHINE
	.word mcall_trap
#else
	.word bad_trap
#endif /* BBL_BOOT_MACHINE */
	.word bad_trap
	.word __trap_from_machine_mode
	.word bad_trap
	.word bad_trap

	.option norvc
	__HEAD
.global __vectors
__vectors:
	csrrw	sp, CSR_SCRATCH, sp
	beqz	sp, .Ltrap_from_machine_mode

	REG_S	a0, PT_A0(sp)
	REG_S	a1, PT_A1(sp)

	csrr	a1, CSR_CAUSE
	bgez	a1, .Lhandle_trap_in_machine_mode

	# This is an interrupt.  Discard the mcause MSB and decode the rest.
	move	a0, sp /* pt_regs */
#ifndef CONFIG_SYS_NOIRQ
	tail	irqc_hw_handle_irq
#endif

	sll	a1, a1, 1

	# Is it a machine timer interrupt?
	li	a0, IRQ_TIMER * 2
	bne	a0, a1, 1f

	# Yes.  Simply clear MTIE and raise STIP.
	li	a0, IE_MTIE
	csrc	mie, a0
	li	a0, IE_STIE
	csrs	mip, a0

.Lmret:
	# Go back whence we came.
	REG_L	a0, PT_A0(sp)
	REG_L	a1, PT_A1(sp)
	csrrw	sp, CSR_SCRATCH, sp
	mret

1:
	# Is it an IPI?
	li	a0, IRQ_SOFT * 2
	bne	a0, a1, .Lbad_trap

	# Yes.  First, clear the MIPI bit.
	REG_L	a0, MENTRY_IPI_OFFSET(sp)
	sw	x0, (a0)
	fence

	# Now, decode the cause(s).
#ifdef CONFIG_RISCV_A
	addi	a0, sp, MENTRY_IPI_PENDING_OFFSET
	amoswap.w	a0, x0, (a0)
#else
	lw	a0, MENTRY_IPI_PENDING_OFFSET(sp)
	sw	x0, MENTRY_IPI_PENDING_OFFSET(sp)
#endif
	and	a1, a0, IPI_SOFT
	beqz	a1, 1f
	csrs	CSR_IP, SR_IE
1:
	andi	a1, a0, IPI_FENCE_I
	beqz	a1, 1f
	fence.i
1:
	andi	a1, a0, IPI_SFENCE_VMA
	beqz	a1, 1f
	sfence.vma
1:
	andi	a1, a0, IPI_HALT
	beqz	a1, 1f
	wfi
	j	1b
1:
	j	.Lmret

.Lhandle_trap_in_machine_mode:
	# Preserve the registers.  Compute the address of the trap handler.
	REG_S	ra, PT_RA(sp)
	REG_S	gp, PT_GP(sp)
	REG_S	tp, PT_TP(sp)
	REG_S	t0, PT_T0(sp)
1:	auipc	t0, %pcrel_hi(trap_table)	# t0 <- %hi(trap_table)
	REG_S	t1, PT_T1(sp)
	sll	t1, a1, 2			# t1 <- mcause << 2
	REG_S	t2, PT_T2(sp)
	add	t1, t0, t1			# t1 <- %hi(trap_table)[mcause]
	REG_S	s0, PT_S0(sp)
	LWU	t1, %pcrel_lo(1b)(t1)		# t1 <- trap_table[mcause]
	REG_S	s1, PT_S1(sp)
	mv	a0, sp				# a0 <- regs
	REG_S	a2, PT_A2(sp)
	csrr	a2, CSR_EPC			# a2 <- mepc
	REG_S	a3, PT_A3(sp)
	csrrw	t0, CSR_SCRATCH, x0		# t0 <- user sp
	REG_S	a4, PT_A4(sp)
	REG_S	a5, PT_A5(sp)
	REG_S	a6, PT_A6(sp)
	REG_S	a7, PT_A7(sp)
	REG_S	s2, PT_S2(sp)
	REG_S	s3, PT_S3(sp)
	REG_S	s4, PT_S4(sp)
	REG_S	s5, PT_S5(sp)
	REG_S	s6, PT_S6(sp)
	REG_S	s7, PT_S7(sp)
	REG_S	s8, PT_S8(sp)
	REG_S	s9, PT_S9(sp)
	REG_S	s10, PT_S10(sp)
	REG_S	s11, PT_S11(sp)
	REG_S	t3, PT_T3(sp)
	REG_S	t4, PT_T4(sp)
	REG_S	t5, PT_T5(sp)
	REG_S	t6, PT_T6(sp)
	REG_S	t0, PT_T0(sp)		# sp

#ifndef __riscv_flen
	# Move the emulated FCSR from x0's save slot into tp.
	lw	tp, (sp)
#endif
	REG_S	x0, (sp)		# Zero x0's save slot.
	# Invoke the handler.
	jalr	t1
#ifndef __riscv_flen
	# Move the emulated FCSR from tp into x0's save slot.
	sw	tp, (sp)
#endif

restore_scratch:
	# Restore mscratch, so future traps will know they didn't come
	# from M-mode.
	csrw	CSR_SCRATCH, sp

restore_regs:
	# Restore all of the registers.
	REG_L	ra, PT_RA(sp)
	REG_L	gp, PT_GP(sp)
	REG_L	tp, PT_TP(sp)
	REG_L	t0, PT_T0(sp)
	REG_L	t1, PT_T1(sp)
	REG_L	t2, PT_T2(sp)
	REG_L	s0, PT_S0(sp)
	REG_L	s1, PT_S1(sp)
	REG_L	a0, PT_A0(sp)
	REG_L	a1, PT_A1(sp)
	REG_L	a2, PT_A2(sp)
	REG_L	a3, PT_A3(sp)
	REG_L	a4, PT_A4(sp)
	REG_L	a5, PT_A5(sp)
	REG_L	a6, PT_A6(sp)
	REG_L	a7, PT_A7(sp)
	REG_L	s2, PT_S2(sp)
	REG_L	s3, PT_S3(sp)
	REG_L	s4, PT_S4(sp)
	REG_L	s5, PT_S5(sp)
	REG_L	s6, PT_S6(sp)
	REG_L	s7, PT_S7(sp)
	REG_L	s8, PT_S8(sp)
	REG_L	s9, PT_S9(sp)
	REG_L	s10, PT_S10(sp)
	REG_L	s11, PT_S11(sp)
	REG_L	t3, PT_T3(sp)
	REG_L	t4, PT_T4(sp)
	REG_L	t5, PT_T5(sp)
	REG_L	t6, PT_T6(sp)
	REG_L	sp, PT_SP(sp)
	mret

.globl __redirect_trap
__redirect_trap:
	# reset sp to top of M-mode stack
	li	t0, MACHINE_STACK_SIZE
	add	sp, sp, t0
	neg	t0, t0
	and	sp, sp, t0
	addi	sp, sp, -MENTRY_FRAME_SIZE
	j	restore_scratch

.Ltrap_from_machine_mode:
	csrr	sp, CSR_SCRATCH
	addi	sp, sp, -INTEGER_CONTEXT_SIZE
	REG_S	a0, PT_A0(sp)
	REG_S	a1, PT_A1(sp)
	li	a1, TRAP_FROM_MACHINE_MODE_VECTOR
	j	.Lhandle_trap_in_machine_mode

.Lbad_trap:
	li	a1, BAD_TRAP_VECTOR
	j	.Lhandle_trap_in_machine_mode

__trap_from_machine_mode:
	jal	trap_from_machine_mode
	j	restore_regs
#endif
