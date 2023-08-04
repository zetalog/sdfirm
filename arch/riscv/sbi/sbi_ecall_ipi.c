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

static int sbi_ecall_ipi_handler(unsigned long extid, unsigned long funcid,
				 struct pt_regs *regs,
				 unsigned long *out_val,
				 struct csr_trap_info *out_trap)
{
	int ret = 0;

	if (funcid == SBI_EXT_IPI_SEND_IPI)
		ret = sbi_ipi_send_many(regs->a0, regs->a1,
					SBI_IPI_EVENT_SOFT, NULL);
	else
		ret = SBI_ENOTSUPP;

	return ret;
}

DEFINE_SBI_ECALL(ipi, SBI_EXT_IPI, SBI_EXT_IPI,
		 NULL, sbi_ecall_ipi_handler);
