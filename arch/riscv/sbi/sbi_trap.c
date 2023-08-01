/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>

static void __noreturn sbi_trap_error(const char *msg, int rc,
				      ulong mcause, ulong mtval,
				      struct pt_regs *regs)
{
	__unused uint32_t hartid = sbi_current_hartid();

	sbi_printf("%s: hart%d: %s (error %d)\n", __func__, hartid, msg, rc);
	sbi_printf("%s: hart%d: mcause=0x%" PRILX " mtval=0x%" PRILX "\n",
		   __func__, hartid, mcause, mtval);
	sbi_printf("%s: hart%d: mepc=0x%" PRILX " mstatus=0x%" PRILX "\n",
		   __func__, hartid, regs->epc, regs->status);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "ra", regs->ra, "sp", regs->sp);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "gp", regs->gp, "tp", regs->tp);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "s0", regs->s0, "s1", regs->s1);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "a0", regs->a0, "a1", regs->a1);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "a2", regs->a2, "a3", regs->a3);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "a4", regs->a4, "a5", regs->a5);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "a6", regs->a6, "a7", regs->a7);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "s2", regs->s2, "s3", regs->s3);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "s4", regs->s4, "s5", regs->s5);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "s6", regs->s6, "s7", regs->s7);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "s8", regs->s8, "s9", regs->s9);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "s10", regs->s10, "s11", regs->s11);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "t0", regs->t0, "t1", regs->t1);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "t2", regs->t2, "t3", regs->t3);
	sbi_printf("%s: hart%d: %s=0x%" PRILX " %s=0x%" PRILX "\n", __func__,
		   hartid, "t4", regs->t4, "t5", regs->t5);
	sbi_printf("%s: hart%d: %s=0x%" PRILX "\n", __func__, hartid, "t6",
		   regs->t6);

	sbi_finish_hang();
}

/**
 * Redirect trap to lower privledge mode (S-mode or U-mode)
 *
 * @param regs pointer to register state
 * @param scratch pointer to sbi_scratch of current HART
 * @param epc error PC for lower privledge mode
 * @param cause exception cause for lower privledge mode
 * @param tval trap value for lower privledge mode
 *
 * @return 0 on success and negative error code on failure
 */
#ifdef CONFIG_CPU_S
int sbi_trap_redirect(struct pt_regs *regs, struct sbi_scratch *scratch,
		      ulong epc, ulong cause, ulong tval)
{
	ulong new_status, prev_mode;

	/* Sanity check on previous mode */
	prev_mode = EXTRACT_FIELD(regs->status, SR_MPP);
	if (prev_mode != PRV_S && prev_mode != PRV_U)
		return -ENOTSUP;

	/* Update S-mode exception info */
	csr_write(CSR_STVAL, tval);
	csr_write(CSR_SEPC, epc);
	csr_write(CSR_SCAUSE, cause);

	/* Set MEPC to S-mode exception vector base */
	regs->epc = irq_get_stvec();

	/* Initial value of new MSTATUS */
	new_status = regs->status;

	/* Clear MPP, SPP, SPIE, and SIE */
	new_status &= ~(SR_MPP | SR_SPP | SR_SPIE | SR_SIE);

	/* Set SPP */
	if (prev_mode == PRV_S)
		new_status |= (1UL << SR_SPP_SHIFT);

	/* Set SPIE */
	if (regs->status & SR_SIE)
		new_status |= SR_SPIE;

	/* Set MPP */
	new_status |= (PRV_S << SR_MPP_SHIFT);

	/* Set new value in MSTATUS */
	regs->status = new_status;
	return 0;
}
#else
int sbi_trap_redirect(struct pt_regs *regs, struct sbi_scratch *scratch,
		      ulong epc, ulong cause, ulong tval)
{
	return -ENOTSUP;
}
#endif

void sbi_trap_log(const char *fmt, ...)
{
	va_list arg;

	if (!sbi_trap_log_enabled())
		return;

	va_start(arg, fmt);
	sbi_vprintf(fmt, arg);
	va_end(arg);
}

/**
 * Handle trap/interrupt
 *
 * This function is called by firmware linked to OpenSBI
 * library for handling trap/interrupt. It expects the
 * following:
 * 1. The 'mscratch' CSR is pointing to sbi_scratch of current HART
 * 2. The 'mcause' CSR is having exception/interrupt cause
 * 3. The 'mtval' CSR is having additional trap information
 * 4. Stack pointer (SP) is setup for current HART
 * 5. Interrupts are disabled in MSTATUS CSR
 *
 * @param regs pointer to register state
 * @param scratch pointer to sbi_scratch of current HART
 */
void sbi_trap_handler(struct pt_regs *regs, struct sbi_scratch *scratch)
{
	int rc = -ENOTSUP;
	const char *msg = "trap handler failed";
	uint32_t hartid = sbi_current_hartid();
	ulong mcause = csr_read(CSR_MCAUSE);
	ulong mtval = csr_read(CSR_MTVAL);
	struct unpriv_trap *uptrap;
	ulong irq;

	if (mcause & (1UL << (__riscv_xlen - 1))) {
		irq = mcause & (~(1UL << (__riscv_xlen - 1)));

		switch (irq) {
		case IRQ_M_TIMER:
			sbi_trap_log("%d: IRQ_M_TIMER\n", hartid);
			sbi_timer_process(scratch);
			break;
		case IRQ_M_SOFT:
			sbi_trap_log("%d: IRQ_M_SOFT\n", hartid);
			sbi_ipi_process(scratch);
			break;
		default:
			if (sbi_platform_process_irq(sbi_platform_ptr(scratch),
						     irq) != 0) {
				sbi_trap_log("Unknown extarnal IRQ\n");
				msg = "unhandled external interrupt";
				goto trap_error;
			}
			break;
		};
		return;
	}

	switch (mcause) {
	case EXC_INSN_ILLEGAL:
		sbi_trap_log("EXC_INSN_ILLEGAL\n");
		rc  = sbi_illegal_insn_handler(hartid, mcause, regs, scratch);
		msg = "illegal instruction handler failed";
		break;
	case EXC_LOAD_MISALIGNED:
		sbi_trap_log("EXC_LOAD_MISALIGNED\n");
		rc = sbi_misaligned_load_handler(hartid, mcause, regs, scratch);
		msg = "misaligned load handler failed";
		break;
	case EXC_STORE_MISALIGNED:
		sbi_trap_log("EXC_STORE_MISALIGNED\n");
		rc  = sbi_misaligned_store_handler(hartid, mcause, regs,
						   scratch);
		msg = "misaligned store handler failed";
		break;
	case EXC_ECALL_H:
	case EXC_ECALL_S:
		rc  = sbi_ecall_handler(regs);
		msg = "ecall handler failed";
		break;
	case EXC_LOAD_ACCESS:
	case EXC_STORE_ACCESS:
	case EXC_LOAD_PAGE_FAULT:
	case EXC_STORE_PAGE_FAULT:
		uptrap = sbi_hart_get_trap_info(scratch);
		if ((regs->status & SR_MPRV) && uptrap) {
			sbi_trap_log("EXC_x_ACCESS/EXC_x_PAGE_FAULT\n");
			rc = 0;
			regs->epc += uptrap->ilen;
			uptrap->cause = mcause;
			uptrap->tval = mtval;
		} else {
			sbi_trap_log("Redirected page/access exception %d\n", mcause);
			rc = sbi_trap_redirect(regs, scratch, regs->epc,
					       mcause, mtval);
		}
		msg = "page/access fault handler failed";
		break;
	default:
		sbi_trap_log("Redirected unhandled exception %d\n", mcause);
		/* If the trap came from S or U mode, redirect it there */
		rc = sbi_trap_redirect(regs, scratch, regs->epc, mcause, mtval);
		break;
	};

trap_error:
	if (rc)
		sbi_trap_error(msg, rc, mcause, mtval, regs);
}
