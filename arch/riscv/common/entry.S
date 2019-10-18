#include <target/config.h>
#include <target/compiler.h>
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
	csrrw	sp, mscratch, sp
	beqz	sp, .Ltrap_from_machine_mode

	REG_S	a0, 10*SZREG(sp)
	REG_S	a1, 11*SZREG(sp)

	csrr	a1, mcause
	bgez	a1, .Lhandle_trap_in_machine_mode

	# This is an interrupt.  Discard the mcause MSB and decode the rest.
	move	a0, sp /* pt_regs */
	tail	irqc_hw_handle_irq

	sll	a1, a1, 1

	# Is it a machine timer interrupt?
	li	a0, IRQ_M_TIMER * 2
	bne	a0, a1, 1f

	# Yes.  Simply clear MTIE and raise STIP.
	li	a0, IE_MTIE
	csrc	mie, a0
	li	a0, IE_STIE
	csrs	mip, a0

.Lmret:
	# Go back whence we came.
	REG_L	a0, 10*SZREG(sp)
	REG_L	a1, 11*SZREG(sp)
	csrrw	sp, mscratch, sp
	mret

1:
	# Is it an IPI?
	li	a0, IRQ_M_SOFT * 2
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
	csrs	mip, IE_SSIE
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
	REG_S	ra, 1*SZREG(sp)
	REG_S	gp, 3*SZREG(sp)
	REG_S	tp, 4*SZREG(sp)
	REG_S	t0, 5*SZREG(sp)
1:	auipc	t0, %pcrel_hi(trap_table)	# t0 <- %hi(trap_table)
	REG_S	t1, 6*SZREG(sp)
	sll	t1, a1, 2			# t1 <- mcause << 2
	REG_S	t2, 7*SZREG(sp)
	add	t1, t0, t1			# t1 <- %hi(trap_table)[mcause]
	REG_S	s0, 8*SZREG(sp)
	LWU	t1, %pcrel_lo(1b)(t1)		# t1 <- trap_table[mcause]
	REG_S	s1, 9*SZREG(sp)
	mv	a0, sp				# a0 <- regs
	REG_S	a2, 12*SZREG(sp)
	csrr	a2, mepc			# a2 <- mepc
	REG_S	a3, 13*SZREG(sp)
	csrrw	t0, mscratch, x0		# t0 <- user sp
	REG_S	a4, 14*SZREG(sp)
	REG_S	a5, 15*SZREG(sp)
	REG_S	a6, 16*SZREG(sp)
	REG_S	a7, 17*SZREG(sp)
	REG_S	s2, 18*SZREG(sp)
	REG_S	s3, 19*SZREG(sp)
	REG_S	s4, 20*SZREG(sp)
	REG_S	s5, 21*SZREG(sp)
	REG_S	s6, 22*SZREG(sp)
	REG_S	s7, 23*SZREG(sp)
	REG_S	s8, 24*SZREG(sp)
	REG_S	s9, 25*SZREG(sp)
	REG_S	s10, 26*SZREG(sp)
	REG_S	s11, 27*SZREG(sp)
	REG_S	t3, 28*SZREG(sp)
	REG_S	t4, 29*SZREG(sp)
	REG_S	t5, 30*SZREG(sp)
	REG_S	t6, 31*SZREG(sp)
	REG_S	t0, 2*SZREG(sp)		# sp

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

restore_mscratch:
	# Restore mscratch, so future traps will know they didn't come
	# from M-mode.
	csrw	mscratch, sp

restore_regs:
	# Restore all of the registers.
	REG_L	ra, 1*SZREG(sp)
	REG_L	gp, 3*SZREG(sp)
	REG_L	tp, 4*SZREG(sp)
	REG_L	t0, 5*SZREG(sp)
	REG_L	t1, 6*SZREG(sp)
	REG_L	t2, 7*SZREG(sp)
	REG_L	s0, 8*SZREG(sp)
	REG_L	s1, 9*SZREG(sp)
	REG_L	a0, 10*SZREG(sp)
	REG_L	a1, 11*SZREG(sp)
	REG_L	a2, 12*SZREG(sp)
	REG_L	a3, 13*SZREG(sp)
	REG_L	a4, 14*SZREG(sp)
	REG_L	a5, 15*SZREG(sp)
	REG_L	a6, 16*SZREG(sp)
	REG_L	a7, 17*SZREG(sp)
	REG_L	s2, 18*SZREG(sp)
	REG_L	s3, 19*SZREG(sp)
	REG_L	s4, 20*SZREG(sp)
	REG_L	s5, 21*SZREG(sp)
	REG_L	s6, 22*SZREG(sp)
	REG_L	s7, 23*SZREG(sp)
	REG_L	s8, 24*SZREG(sp)
	REG_L	s9, 25*SZREG(sp)
	REG_L	s10, 26*SZREG(sp)
	REG_L	s11, 27*SZREG(sp)
	REG_L	t3, 28*SZREG(sp)
	REG_L	t4, 29*SZREG(sp)
	REG_L	t5, 30*SZREG(sp)
	REG_L	t6, 31*SZREG(sp)
	REG_L	sp, 2*SZREG(sp)
	mret

.Ltrap_from_machine_mode:
	csrr	sp, mscratch
	addi	sp, sp, -INTEGER_CONTEXT_SIZE
	REG_S	a0, 10*SZREG(sp)
	REG_S	a1, 11*SZREG(sp)
	li	a1, TRAP_FROM_MACHINE_MODE_VECTOR
	j	.Lhandle_trap_in_machine_mode

.Lbad_trap:
	li	a1, BAD_TRAP_VECTOR
	j	.Lhandle_trap_in_machine_mode

__trap_from_machine_mode:
	jal	trap_from_machine_mode
	j	restore_regs
#endif
