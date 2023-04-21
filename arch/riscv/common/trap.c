#include <target/init.h>
#include <target/irq.h>
#include <target/panic.h>
#include <target/arch.h>

#if __riscv_xlen == 64
#define REG_FMT		"%16lx"
#endif
#if __riscv_xlen == 32
#define REG_FMT		"%8lx"
#endif

#define user_mode(regs)		(((regs)->status & SR_PP) == 0)

void show_regs(struct pt_regs *regs)
{
	printf(" cpu: %d\n", smp_processor_id());
	printf(" epc: " REG_FMT " ra : " REG_FMT " sp : " REG_FMT "\n",
	       regs->epc, regs->ra, regs->sp);
	printf(" gp : " REG_FMT " tp : " REG_FMT " t0 : " REG_FMT "\n",
	       regs->gp, regs->tp, regs->t0);
	printf(" t1 : " REG_FMT " t2 : " REG_FMT " s0 : " REG_FMT "\n",
	       regs->t1, regs->t2, regs->s0);
	printf(" s1 : " REG_FMT " a0 : " REG_FMT " a1 : " REG_FMT "\n",
	       regs->s1, regs->a0, regs->a1);
	printf(" a2 : " REG_FMT " a3 : " REG_FMT " a4 : " REG_FMT "\n",
	       regs->a2, regs->a3, regs->a4);
	printf(" a5 : " REG_FMT " a6 : " REG_FMT " a7 : " REG_FMT "\n",
	       regs->a5, regs->a6, regs->a7);
	printf(" s2 : " REG_FMT " s3 : " REG_FMT " s4 : " REG_FMT "\n",
	       regs->s2, regs->s3, regs->s4);
	printf(" s5 : " REG_FMT " s6 : " REG_FMT " s7 : " REG_FMT "\n",
	       regs->s5, regs->s6, regs->s7);
	printf(" s8 : " REG_FMT " s9 : " REG_FMT " s10: " REG_FMT "\n",
	       regs->s8, regs->s9, regs->s10);
	printf(" s11: " REG_FMT " t3 : " REG_FMT " t4 : " REG_FMT "\n",
	       regs->s11, regs->t3, regs->t4);
	printf(" t5 : " REG_FMT " t6 : " REG_FMT "\n",
	       regs->t5, regs->t6);

	printf("status: " REG_FMT " badaddr: " REG_FMT " cause: " REG_FMT "\n",
	       regs->status, regs->badaddr, regs->cause);
}

static void do_trap_error(struct pt_regs *regs, unsigned long addr,
			  const char *str)
{
	if (user_mode(regs))
		printf("Unhandled exception at " REG_FMT".\n", addr);
	else
		printf("%s at " REG_FMT ".\n", str, addr);
	show_regs(regs);
	board_finish(1);
	bh_panic();
}

#define DO_ERROR_INFO(name, str)			\
asmlinkage void name(struct pt_regs *regs)		\
{							\
	do_trap_error(regs, regs->epc, "Oops - " str);	\
}

DO_ERROR_INFO(do_trap_unknown, "unknown exception");
DO_ERROR_INFO(do_trap_insn_misaligned, "instruction address misaligned");
DO_ERROR_INFO(do_trap_insn_fault, "instruction access fault");
DO_ERROR_INFO(do_trap_insn_illegal, "illegal instruction");
DO_ERROR_INFO(do_trap_load_misaligned, "load address misaligned");
DO_ERROR_INFO(do_trap_load_fault, "load access fault");
DO_ERROR_INFO(do_trap_store_misaligned, "store (or AMO) address misaligned");
DO_ERROR_INFO(do_trap_store_fault, "store (or AMO) access fault");
DO_ERROR_INFO(do_trap_ecall_u, "environment call from U-mode");
DO_ERROR_INFO(do_trap_ecall_s, "environment call from S-mode");
DO_ERROR_INFO(do_trap_ecall_m, "environment call from M-mode");

asmlinkage void __vectors(void);

__init void trap_init(void)
{
	csr_write(CSR_SCRATCH, 0);
	irq_set_tvec(&__vectors);
	csr_write(CSR_IE, -1);
}
