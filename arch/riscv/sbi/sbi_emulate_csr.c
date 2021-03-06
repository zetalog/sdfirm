/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>

#ifndef CONFIG_ARCH_HAS_SBI_TIMER
#warning "ARCH_HAS_SBI_TIMER is not defined, using pseudo timer"
uint64_t sbi_timer;

uint64_t sbi_timer_value(struct sbi_scratch *scratch)
{
	return sbi_timer++;
}
#endif

int sbi_emulate_csr_read(int csr_num, struct pt_regs *regs,
			 struct sbi_scratch *scratch, ulong *csr_val)
{
	int ret = 0;
	ulong cen = -1UL;
	ulong prev_mode = EXTRACT_FIELD(regs->status, SR_MPP);

#ifdef CONFIG_CPU_S
	if (prev_mode == PRV_U)
		cen = csr_read(CSR_SCOUNTEREN);
#endif

	switch (csr_num) {
	case CSR_CYCLE:
		if (!((cen >> (CSR_CYCLE - CSR_CYCLE)) & 1))
			return -1;
		sbi_trap_log("CSR_CYCLE read\n");
		*csr_val = csr_read(CSR_MCYCLE);
		break;
	case CSR_TIME:
		if (!((cen >> (CSR_TIME - CSR_CYCLE)) & 1))
			return -1;
		sbi_trap_log("CSR_TIME read\n");
		*csr_val = sbi_timer_value(scratch);
		break;
	case CSR_INSTRET:
		if (!((cen >> (CSR_INSTRET - CSR_CYCLE)) & 1))
			return -1;
		sbi_trap_log("CSR_INSTRET read\n");
		*csr_val = csr_read(CSR_MINSTRET);
		break;
	case CSR_MHPMCOUNTER(3):
		if (!((cen >> (3 + CSR_MHPMCOUNTER(3) - CSR_MHPMCOUNTER(3))) & 1))
			return -1;
		*csr_val = csr_read(CSR_MHPMCOUNTER(3));
		break;
	case CSR_MHPMCOUNTER(4):
		if (!((cen >> (3 + CSR_MHPMCOUNTER(4) - CSR_MHPMCOUNTER(3))) & 1))
			return -1;
		*csr_val = csr_read(CSR_MHPMCOUNTER(4));
		break;
#if __riscv_xlen == 32
	case CSR_CYCLEH:
		if (!((cen >> (CSR_CYCLE - CSR_CYCLE)) & 1))
			return -1;
		*csr_val = csr_read(CSR_MCYCLEH);
		break;
	case CSR_TIMEH:
		if (!((cen >> (CSR_TIME - CSR_CYCLE)) & 1))
			return -1;
		*csr_val = sbi_timer_value(scratch) >> 32;
		break;
	case CSR_INSTRETH:
		if (!((cen >> (CSR_INSTRET - CSR_CYCLE)) & 1))
			return -1;
		*csr_val = csr_read(CSR_MINSTRETH);
		break;
	case CSR_MHPMCOUNTERH(3):
		if (!((cen >> (3 + CSR_MHPMCOUNTER(3) - CSR_MHPMCOUNTER(3))) & 1))
			return -1;
		*csr_val = csr_read(CSR_MHPMCOUNTERH(3));
		break;
	case CSR_MHPMCOUNTERH(4):
		if (!((cen >> (3 + CSR_MHPMCOUNTER(4) - CSR_MHPMCOUNTER(3))) & 1))
			return -1;
		*csr_val = csr_read(CSR_MHPMCOUNTERH(4));
		break;
#endif
	case CSR_MHPMEVENT(3):
		*csr_val = csr_read(CSR_MHPMEVENT(3));
		break;
	case CSR_MHPMEVENT(4):
		*csr_val = csr_read(CSR_MHPMEVENT(4));
		break;
	case CSR_MHARTID:
		*csr_val = csr_read(CSR_MHARTID);
		break;
#ifdef CONFIG_SBI_CSR_MIMPID
	case CSR_MIMPID:
		*csr_val = csr_read(CSR_MIMPID);
		break;
#endif
	default:
		ret = -ENOTSUP;
		break;
	};
	if (ret)
		sbi_printf("%s: hartid%d: invalid csr_num=0x%x\n",
			   __func__, sbi_current_hartid(), csr_num);
	return ret;
}

int sbi_emulate_csr_write(int csr_num, struct pt_regs *regs,
			  struct sbi_scratch *scratch, ulong csr_val)
{
	int ret = 0;
	/*ulong prev_mode = EXTRACT_FIELD(regs->status, SR_MPP);*/

	switch (csr_num) {
	case CSR_CYCLE:
		csr_write(CSR_MCYCLE, csr_val);
		break;
	case CSR_INSTRET:
		csr_write(CSR_MINSTRET, csr_val);
		break;
	case CSR_MHPMCOUNTER(3):
		csr_write(CSR_MHPMCOUNTER(3), csr_val);
		break;
	case CSR_MHPMCOUNTER(4):
		csr_write(CSR_MHPMCOUNTER(4), csr_val);
		break;
#if __riscv_xlen == 32
	case CSR_CYCLEH:
		csr_write(CSR_MCYCLEH, csr_val);
		break;
	case CSR_INSTRETH:
		csr_write(CSR_MINSTRETH, csr_val);
		break;
	case CSR_MHPMCOUNTERH(3):
		csr_write(CSR_MHPMCOUNTERH(3), csr_val);
		break;
	case CSR_MHPMCOUNTERH(4):
		csr_write(CSR_MHPMCOUNTERH(4), csr_val);
		break;
#endif
	case CSR_MHPMEVENT(3):
		csr_write(CSR_MHPMEVENT(3), csr_val);
		break;
	case CSR_MHPMEVENT(4):
		csr_write(CSR_MHPMEVENT(4), csr_val);
		break;
	default:
		ret = -ENOTSUP;
		break;
	};
	if (ret)
		sbi_printf("%s: hartid%d: invalid csr_num=0x%x\n",
			   __func__, sbi_current_hartid(), csr_num);
	return ret;
}
