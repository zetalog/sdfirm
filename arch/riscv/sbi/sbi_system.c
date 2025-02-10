/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#include <target/sbi.h>
#include <sbi/sbi_system.h>

static SBI_LIST_HEAD(reset_devices_list);

int sbi_system_early_init(struct sbi_scratch *scratch, bool cold_boot)
{
	return sbi_platform_early_init(sbi_platform_ptr(scratch), cold_boot);
}

int sbi_system_final_init(struct sbi_scratch *scratch, bool cold_boot)
{
	return sbi_platform_final_init(sbi_platform_ptr(scratch), cold_boot);
}

void __noreturn
sbi_system_reboot(struct sbi_scratch *scratch, uint32_t type)
{
	sbi_platform_system_reboot(sbi_platform_ptr(scratch), type);
	bh_panic();
}

void __noreturn
sbi_system_shutdown(struct sbi_scratch *scratch, uint32_t type)
{
	/* First try the platform-specific method */
	sbi_platform_system_shutdown(sbi_platform_ptr(scratch), type);

	/* If that fails (or is not implemented) send an IPI on every
	 * hart to hang and then hang the current hart */
	sbi_ipi_send_many(sbi_hart_available_mask(), 0,
		          SBI_IPI_EVENT_HALT, NULL);
	bh_panic();
}

void __noreturn
sbi_system_finish(struct sbi_scratch *scratch, uint32_t code)
{
	sbi_platform_system_finish(sbi_platform_ptr(scratch), code);
	sbi_ipi_send_many(sbi_hart_available_mask(), 0,
			  SBI_IPI_EVENT_HALT, NULL);
	bh_panic();
}

void sbi_system_reset_add_device(struct sbi_system_reset_device *dev)
{
	if (!dev || !dev->system_reset_check)
		return;

	sbi_list_add(&(dev->node), &(reset_devices_list));
}

static const struct sbi_system_suspend_device *suspend_dev = NULL;

void sbi_system_suspend_set_device(struct sbi_system_suspend_device *dev)
{
	if (!dev || suspend_dev)
		return;

	suspend_dev = dev;
}

