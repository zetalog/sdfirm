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

static int sbi_ecall_time_handler(unsigned long extid, unsigned long funcid,
				  struct pt_regs *regs,
				  unsigned long *out_val,
				  struct csr_trap_info *out_trap)
{
	int ret = 0;

	if (funcid == SBI_EXT_TIME_SET_TIMER) {
#if __riscv_xlen == 32
		sbi_timer_event_start(sbi_scratch_thishart_ptr(),
				     (((uint64_t)regs->a1 << 32) | (uint64_t)regs->a0));
#else
		sbi_timer_event_start(sbi_scratch_thishart_ptr(),
				      (uint64_t)regs->a0);
#endif
	} else
		ret = SBI_ENOTSUPP;

	return ret;
}

static struct sbi_ecall_extension ecall_time;

static int sbi_ecall_time_register_extensions(void)
{
	return sbi_ecall_register_extension(&ecall_time);
}

DEFINE_SBI_ECALL(ecall_time,
		 SBI_EXT_TIME,
		 SBI_EXT_TIME,
		 sbi_ecall_time_register_extensions,
		 NULL,
		 sbi_ecall_time_handler);
