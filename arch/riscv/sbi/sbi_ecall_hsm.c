/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#include <sbi/sbi_hsm.h>
#include <target/sbi.h>

static int sbi_ecall_hsm_handler(unsigned long extid, unsigned long funcid,
				 struct pt_regs *regs, unsigned long *out_val,
				 struct csr_trap_info *out_trap)
{
	int ret = 0;
	int hstate;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	switch (funcid) {
	case SBI_EXT_HSM_HART_START:
		ret = sbi_hsm_hart_start(scratch, regs->a0, regs->a1, regs->a2);
		break;
	case SBI_EXT_HSM_HART_STOP:
		ret = sbi_hsm_hart_stop(scratch, true);
		break;
	case SBI_EXT_HSM_HART_GET_STATUS:
		hstate = sbi_hsm_hart_get_state(scratch, regs->a0);
		ret = sbi_hsm_hart_state_to_status(hstate);
		break;
	default:
		ret = SBI_ENOTSUPP;
	};
	if (ret >= 0) {
		*out_val = ret;
		ret = 0;
	}

	return ret;
}

DEFINE_SBI_ECALL(hsm, SBI_EXT_HSM, SBI_EXT_HSM,
		 NULL, sbi_ecall_hsm_handler);