/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2020 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __FDT_IRQCHIP_H__
#define __FDT_IRQCHIP_H__

#include <target/types.h>

struct fdt_irqchip {
	const struct fdt_match *match_table;
	int (*cold_init)(void *fdt, int nodeoff, const struct fdt_match *match);
	int (*warm_init)(void);
	void (*exit)(void);
};

void fdt_irqchip_exit(void);

int fdt_irqchip_init(bool cold_boot);

#endif
