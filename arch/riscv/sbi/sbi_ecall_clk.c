/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Atish Patra <atish.patra@wdc.com>
 */

 #include <target/sbi.h>

 static int sbi_ecall_clk_handler(unsigned long extid, unsigned long funcid,
				   struct pt_regs *regs,
				   unsigned long *out_val,
				   struct csr_trap_info *out_trap)
 {
	int ret = 0;
 
	switch (funcid)
	{
	case SBI_EXT_CLK_ENABLE:
		sbi_trap_log("%d: ECALL_ENABLE\n");
		sbi_clock_enable(regs->a0);
		ret = 0;
		break;
	case SBI_EXT_CLK_DISABLE:
		sbi_trap_log("%d: ECALL_DISENABLE\n");
		sbi_clock_disable(regs->a0);
		ret = 0;
		break;
	case SBI_EXT_CLK_SET_FREQUENCY:
		sbi_trap_log("%d: ECALL_SET_CLK_FREQ\n");
		sbi_clock_set_freq(regs->a0, regs->a1);
		ret = 0;
		break;
	case SBI_EXT_CLK_GET_FREQUENCY:
		sbi_trap_log("%d: ECALL_GET_CLK_FREQ\n");
		sbi_clock_set_freq(regs->a0, regs->a1);
		ret = 0;
		break;
	default:
		break;
	}
		 ret = SBI_ENOTSUPP;
 
	 return ret;
 }
 
 DEFINE_SBI_ECALL(clock, SBI_EXT_CLK, SBI_EXT_CLK,
		  NULL, sbi_ecall_clk_handler);
 