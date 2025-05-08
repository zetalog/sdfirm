/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <asm/aplic.h>
#include <asm/io.h>
#include <target/smp.h>
#include <target/irq.h>
#include <target/sbi.h>
#include <target/fdt.h>
#include <target/fdt_irqchip.h>

static int irqchip_aplic_warm_init(void)
{
	/* Nothing to do here. */
	return 0;
}

static struct aplic_data _aplic_data;

static int irqchip_aplic_cold_init(void *fdt, int nodeoff,
				  const struct fdt_match *match)
{
	int rc;
	struct aplic_data *pd;

	pd = &_aplic_data;

	rc = fdt_parse_aplic_node(fdt, nodeoff, pd);
	if (rc)
		goto fail_free_data;

	rc = aplic_cold_irqchip_init(pd);
	if (rc)
		goto fail_free_data;

	return 0;

fail_free_data:
	return rc;
}

static const struct fdt_match irqchip_aplic_match[] = {
	{ .compatible = "riscv,aplic" },
	{ },
};

struct fdt_irqchip fdt_irqchip_aplic = {
	.match_table = irqchip_aplic_match,
	.cold_init = irqchip_aplic_cold_init,
	.warm_init = irqchip_aplic_warm_init,
	.exit = NULL,
};
#if 0
int aplic_sbi_init_cold(void)
{
	void *fdt = sbi_scratch_thishart_arg1_ptr();
	int nodeoff = fdt_node_offset_by_compatible(fdt, 0,
                                                irqchip_aplic_match[0].compatible);
	return irqchip_aplic_cold_init(fdt, nodeoff, fdt_irqchip_aplic.match_table);
}
#endif