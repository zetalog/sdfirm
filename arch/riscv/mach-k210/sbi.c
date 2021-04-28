/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Damien Le Moal <damien.lemoal@wdc.com>
 */

#include <target/sbi.h>
#include <target/fdt.h>
#include <target/uart.h>
#include <target/irq.h>
#include <target/delay.h>

static int k210_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = sbi_scratch_thishart_arg1_ptr();
	fdt_cpu_fixup(fdt);
	fdt_irq_fixup(fdt, "riscv,clint0");
	fdt_irq_fixup(fdt, "riscv,plic0");
	fdt_fixups(fdt);

	return 0;
}

static void k210_console_putc(char ch)
{
	putchar(ch);
}

static int k210_console_getc(void)
{
	return getchar();
}

static int k210_irqchip_init(bool cold_boot)
{
	if (cold_boot)
		plic_sbi_init_cold();
	else
		plic_sbi_init_warm(sbi_processor_id());
	return 0;
}

void k210_ipi_send(uint32_t target_cpu)
{
	clint_set_ipi(target_cpu);
}

void k210_ipi_sync(uint32_t target_cpu)
{
	clint_sync_ipi(target_cpu);
}

void k210_ipi_clear(uint32_t target_cpu)
{
	clint_clear_ipi(target_cpu);
}

static int k210_ipi_init(bool cold_boot)
{
	cpu_t cpu = sbi_processor_id();

	if (!cold_boot)
		k210_ipi_clear(cpu);
	return 0;
}

uint64_t k210_timer_value(void)
{
	return clint_read_mtime();
}

void k210_timer_event_stop(void)
{
	cpu_t cpu = sbi_processor_id();

	clint_unset_mtimecmp(cpu);
}

void k210_timer_event_start(uint64_t next_event)
{
	cpu_t cpu = sbi_processor_id();

	clint_set_mtimecmp(cpu, next_event);
}

static int k210_timer_init(bool cold_boot)
{
	if (!cold_boot)
		k210_timer_event_stop();
	return 0;
}

static int k210_system_reboot(uint32_t type)
{
	return 0;
}

static int k210_system_shutdown(uint32_t type)
{
	return 0;
}

const struct sbi_platform_operations platform_ops = {
	.final_init = k210_final_init,

	.console_putc = k210_console_putc,
	.console_getc = k210_console_getc,

	.irqchip_init = k210_irqchip_init,

	.ipi_init  = k210_ipi_init,
	.ipi_send  = k210_ipi_send,
	.ipi_sync  = k210_ipi_sync,
	.ipi_clear = k210_ipi_clear,

	.timer_init	   = k210_timer_init,
	.timer_value	   = k210_timer_value,
	.timer_event_stop  = k210_timer_event_stop,
	.timer_event_start = k210_timer_event_start,

	.system_reboot	 = k210_system_reboot,
	.system_shutdown = k210_system_shutdown
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version   	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "Kendryte K210",
	.features		= SBI_PLATFORM_HAS_TIMER_VALUE,
	.disabled_hart_mask	= 0,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
