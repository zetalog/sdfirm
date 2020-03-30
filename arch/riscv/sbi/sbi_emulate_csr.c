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
u64 sbi_timer;

u64 sbi_timer_value(struct sbi_scratch *scratch)
{
	return sbi_timer++;
}
#endif

int sbi_emulate_csr_read(int csr_num, u32 hartid, ulong mstatus,
			 struct sbi_scratch *scratch, ulong *csr_val)
{
	ulong cen = -1UL;

#ifndef CONFIG_ARCH_HAS_NOSEE
	if (EXTRACT_FIELD(mstatus, SR_MPP) == PRV_U)
		cen = csr_read(CSR_SCOUNTEREN);
#endif

	switch (csr_num) {
	case CSR_CYCLE:
		if (!((cen >> (CSR_CYCLE - CSR_CYCLE)) & 1))
			return -1;
		*csr_val = csr_read(CSR_MCYCLE);
		break;
	case CSR_TIME:
		if (!((cen >> (CSR_TIME - CSR_CYCLE)) & 1))
			return -1;
		*csr_val = sbi_timer_value(scratch);
		break;
	case CSR_INSTRET:
		if (!((cen >> (CSR_INSTRET - CSR_CYCLE)) & 1))
			return -1;
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
		*csr_val = hartid;
		break;
#ifdef CONFIG_SBI_CSR_MIMPID
	case CSR_MIMPID:
		*csr_val = csr_read(CSR_MIMPID);
		break;
#endif
	default:
		sbi_printf("%s: hartid%d: invalid csr_num=0x%x\n", __func__,
			   hartid, csr_num);
		return -ENOTSUP;
	};
	return 0;
}

int sbi_emulate_csr_write(int csr_num, u32 hartid, ulong mstatus,
			  struct sbi_scratch *scratch, ulong csr_val)
{
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
		sbi_printf("%s: hartid%d: invalid csr_num=0x%x\n", __func__,
			   hartid, csr_num);
		return -ENOTSUP;
	};
	return 0;
}
