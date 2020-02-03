/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __SBI_TRAP_H__
#define __SBI_TRAP_H__

#ifndef __ASSEMBLY__
#include <sbi/sbi_types.h>
#include <target/task.h>

struct sbi_scratch;

int sbi_trap_redirect(struct pt_regs *regs, struct sbi_scratch *scratch,
		      ulong epc, ulong cause, ulong tval);
void sbi_trap_handler(struct pt_regs *regs, struct sbi_scratch *scratch);
#endif

#endif
