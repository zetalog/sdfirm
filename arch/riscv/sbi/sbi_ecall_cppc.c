/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2023 Ventana Micro Systems Inc.
 *
 */

#include <sbi/sbi_cppc.h>
#include <target/sbi.h>

static int sbi_ecall_cppc_handler(unsigned long extid, unsigned long funcid,
				  struct pt_regs *regs,
				//  struct sbi_ecall_return *out)
				unsigned long *out_val,
				struct csr_trap_info *out)
{
	int ret = 0;
	uint64_t temp;

	switch (funcid) {
	case SBI_EXT_CPPC_READ:
		ret = sbi_cppc_read(regs->a0, &temp);
		*out_val = temp;
		break;
	case SBI_EXT_CPPC_READ_HI:
#if __riscv_xlen == 32
		ret = sbi_cppc_read(regs->a0, &temp);
		*out_val = temp >> 32;
#else
		*out_val = 0;
#endif
		break;
	case SBI_EXT_CPPC_WRITE:
#if __riscv_xlen == 32
		temp = regs->a2;
		temp = (temp << 32) | regs->a1;
#else
		temp = regs->a1;
#endif
		ret = sbi_cppc_write(regs->a0, temp);
		break;
	case SBI_EXT_CPPC_PROBE:
		ret = sbi_cppc_probe(regs->a0);
		if (ret >= 0) {
			*out_val = ret;
			ret = 0;
		}
		break;
	default:
		ret = SBI_ENOTSUPP;
	}

	return ret;
}

DEFINE_SBI_ECALL(cppc, SBI_EXT_CPPC, SBI_EXT_CPPC,
		 NULL, sbi_ecall_cppc_handler);
