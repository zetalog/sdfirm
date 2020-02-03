/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Damien Le Moal <damien.lemoal@wdc.com>
 */

#include <target/sbi.h>
#include <target/uart.h>
#include <target/irq.h>
#include <target/delay.h>

#define K210_HART_COUNT		2
#define K210_HART_STACK_SIZE	4096

static int k210_console_init(void)
{
	uart_hw_con_init();
	return 0;
}

static void k210_console_putc(char c)
{
	uart_hw_con_write(c);
}

static int k210_console_getc(void)
{
	while (!uart_hw_con_poll());
	return (int)uart_hw_con_read();
}

static int k210_irqchip_init(bool cold_boot)
{
	cpu_t cpu = sbi_current_hartid();

	if (cold_boot)
		plic_sbi_init_cold();
	else
		plic_sbi_init_warm(cpu);
	return 0;
}

void k210_ipi_send(u32 target_hart)
{
	clint_set_ipi(target_hart);
}

void k210_ipi_sync(u32 target_hart)
{
	clint_sync_ipi(target_hart);
}

void k210_ipi_clear(u32 target_hart)
{
	clint_clear_ipi(target_hart);
}

static int k210_ipi_init(bool cold_boot)
{
	cpu_t cpu = sbi_current_hartid();

	if (!cold_boot) {
		k210_ipi_clear(cpu);
	}
	return 0;
}

u64 k210_timer_value(void)
{
	return clint_read_mtime();
}

void k210_timer_event_stop(void)
{
	cpu_t cpu = sbi_current_hartid();

	clint_unset_mtimecmp(cpu);
}

void k210_timer_event_start(u64 next_event)
{
	cpu_t cpu = sbi_current_hartid();

	clint_set_mtimecmp(cpu, next_event);
}

static int k210_timer_init(bool cold_boot)
{
	if (!cold_boot)
		k210_timer_event_stop();
	return 0;
}

static int k210_system_reboot(u32 type)
{
	return 0;
}

static int k210_system_shutdown(u32 type)
{
	return 0;
}

const struct sbi_platform_operations platform_ops = {
	.console_init	= k210_console_init,
	.console_putc	= k210_console_putc,
	.console_getc	= k210_console_getc,

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
	.hart_count		= K210_HART_COUNT,
	.hart_stack_size	= K210_HART_STACK_SIZE,
	.disabled_hart_mask	= 0,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
