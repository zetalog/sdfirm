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
#include <target/cmdline.h>

int sbi_system_early_init(struct sbi_scratch *scratch, bool cold_boot)
{
	return sbi_platform_early_init(sbi_platform_ptr(scratch), cold_boot);
}

int sbi_system_final_init(struct sbi_scratch *scratch, bool cold_boot)
{
	return sbi_platform_final_init(sbi_platform_ptr(scratch), cold_boot);
}

void __attribute__((noreturn))
sbi_system_reboot(struct sbi_scratch *scratch, u32 type)

{
	sbi_platform_system_reboot(sbi_platform_ptr(scratch), type);
	hart_hang();
}

void __attribute__((noreturn))
sbi_system_shutdown(struct sbi_scratch *scratch, u32 type)
{
	/* First try the platform-specific method */
	sbi_platform_system_shutdown(sbi_platform_ptr(scratch), type);

	/* If that fails (or is not implemented) send an IPI on every
	 * hart to hang and then hang the current hart */
	sbi_ipi_send_many(scratch, NULL, NULL, SBI_IPI_EVENT_HALT, NULL);
	hart_hang();
}

static int do_sbi_shutdown(int argc, char *argv[])
{
	sbi_shutdown();
	return 0;
}

static int do_sbi(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "shutdown") == 0)
		return do_sbi_shutdown(argc, argv);
	return 0;
}

DEFINE_COMMAND(sbi, do_sbi, "Supervisor binary interface commands",
	"shutdown\n"
	"    - shutdown system\n"
);
