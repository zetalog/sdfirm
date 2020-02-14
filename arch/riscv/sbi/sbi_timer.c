/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>

u64 sbi_timer_value(struct sbi_scratch *scratch)
{
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);

	if (sbi_platform_has_timer_value(plat))
		return sbi_platform_timer_value(plat);
	else
		return rdtime();
}

void sbi_timer_event_stop(struct sbi_scratch *scratch)
{
	sbi_platform_timer_event_stop(sbi_platform_ptr(scratch));
}

void sbi_timer_event_start(struct sbi_scratch *scratch, u64 next_event)
{
	sbi_platform_timer_event_start(sbi_platform_ptr(scratch), next_event);
	csr_clear(CSR_MIP, IR_STI);
	csr_set(CSR_MIE, IR_MTI);
}

void sbi_timer_process(struct sbi_scratch *scratch)
{
	csr_clear(CSR_MIE, IR_MTI);
	csr_set(CSR_MIP, IR_STI);
}

int sbi_timer_init(struct sbi_scratch *scratch, bool cold_boot)
{
	return sbi_platform_timer_init(sbi_platform_ptr(scratch), cold_boot);
}
