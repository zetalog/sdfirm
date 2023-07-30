/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>

uint64_t sbi_timer_value(struct sbi_scratch *scratch)
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

#ifdef CONFIG_RISCV_SSTC
void sbi_timer_event_start(struct sbi_scratch *scratch, uint64_t next_event)
{
	/**
	 * Update the stimecmp directly if available. This allows
	 * the older software to leverage sstc extension on newer hardware.
	 */
	if (sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC)) {
#if __riscv_xlen == 32
		csr_write(CSR_STIMECMP, next_event & 0xFFFFFFFF);
		csr_write(CSR_STIMECMPH, next_event >> 32);
#else
		csr_write(CSR_STIMECMP, next_event);
#endif
	} else {
		sbi_platform_timer_event_start(sbi_platform_ptr(scratch), next_event);
                csr_clear(CSR_MIP, IR_STI);
        }
	csr_set(CSR_MIE, IR_MTI);
}

void sbi_timer_process(struct sbi_scratch *scratch)
{
	csr_clear(CSR_MIE, IR_MTI);
	/*
	 * If sstc extension is available, supervisor can receive the timer
	 * directly without M-mode come in between. This function should
	 * only invoked if M-mode programs the timer for its own purpose.
	 */
	if (!sbi_hart_has_extension(sbi_scratch_thishart_ptr(), SBI_HART_EXT_SSTC))
		csr_set(CSR_MIP, IR_STI);
}
#else
void sbi_timer_event_start(struct sbi_scratch *scratch, uint64_t next_event)
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
#endif

int sbi_timer_init(struct sbi_scratch *scratch, bool cold_boot)
{
	return sbi_platform_timer_init(sbi_platform_ptr(scratch), cold_boot);
}
