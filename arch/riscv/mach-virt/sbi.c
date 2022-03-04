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
#include <target/fdt.h>
#include <target/uart.h>
#include <target/irq.h>
#include <target/delay.h>
#include <target/bench.h>

static void virt_modify_dt(void *fdt)
{
#if 0
	plic_fdt_fixup(fdt, "riscv,plic0");
#endif
}

static int virt_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = sbi_scratch_thishart_arg1_ptr();
	virt_modify_dt(fdt);

	return 0;
}

static uint32_t virt_pmp_region_count(uint32_t hartid)
{
	return 1;
}

static int virt_pmp_region_info(uint32_t hartid, uint32_t index,
				ulong *prot, ulong *addr, ulong *log2size)
{
	int ret = 0;

	switch (index) {
	case 0:
		*prot	  = PMP_R | PMP_W | PMP_X;
		*addr	  = 0;
		*log2size = __riscv_xlen;
		break;
	default:
		ret = -1;
		break;
	};

	return ret;
}

#define __UART_REG(n, offset)	(__VIRT_UART_BASE + (offset))
#define __UART_RBR(n)		__UART_REG(n, 0)
#define __UART_LSR(n)		__UART_REG(n, 5)
#define __UART_THR(n)		__UART_RBR(n)

#define __ns16550_con_write(c)								\
	do {										\
		while ((uart_reg_read(__UART_LSR(UART_CON_ID)) & UART_LSR_THRE) == 0);	\
		uart_reg_write(c, __UART_THR(UART_CON_ID));				\
	} while (0)
#define __ns16550_con_read()		uart_reg_read(__UART_RBR(UART_CON_ID))
#define __ns16550_con_poll()		(uart_reg_read(__UART_LSR(UART_CON_ID)) & UART_LSR_DR)

static void virt_console_putc(char ch)
{
	__ns16550_con_write(ch);
}

static int virt_console_getc(void)
{
	if (!__ns16550_con_poll())
		return -1;
	return (int)__ns16550_con_read();
}

static int virt_irqchip_init(bool cold_boot)
{
	if (cold_boot)
		plic_sbi_init_cold();
	plic_sbi_init_warm(sbi_processor_id());
	return 0;
}

void virt_ipi_send(uint32_t target_cpu)
{
	clint_set_ipi(target_cpu);
}

void virt_ipi_clear(uint32_t target_cpu)
{
	clint_clear_ipi(target_cpu);
}

static int virt_ipi_init(bool cold_boot)
{
	cpu_t cpu = sbi_processor_id();

	if (!cold_boot)
		virt_ipi_clear(cpu);
	return 0;
}

uint64_t virt_timer_value(void)
{
	return clint_read_mtime();
}

void virt_timer_event_stop(void)
{
	cpu_t cpu = sbi_processor_id();

	clint_unset_mtimecmp(cpu);
}

void virt_timer_event_start(uint64_t next_event)
{
	cpu_t cpu = sbi_processor_id();

	clint_set_mtimecmp(cpu, next_event);
}

static int virt_timer_init(bool cold_boot)
{
	if (!cold_boot)
		virt_timer_event_stop();
	return 0;
}

static int virt_system_down(uint32_t type)
{
	/* Tell the "finisher" that the simulation
	 * was successful so that QEMU exits
	 */
	virt_finish_pass();
	return 0;
}

const struct sbi_platform_operations platform_ops = {
	.pmp_region_count	= virt_pmp_region_count,
	.pmp_region_info	= virt_pmp_region_info,
	.final_init		= virt_final_init,
	.console_putc		= virt_console_putc,
	.console_getc		= virt_console_getc,
	.irqchip_init		= virt_irqchip_init,
	.ipi_send		= virt_ipi_send,
	.ipi_clear		= virt_ipi_clear,
	.ipi_init		= virt_ipi_init,
	.timer_value		= virt_timer_value,
	.timer_event_stop	= virt_timer_event_stop,
	.timer_event_start	= virt_timer_event_start,
	.timer_init		= virt_timer_init,
	.system_reboot		= virt_system_down,
	.system_shutdown	= virt_system_down
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "QEMU Virt Machine",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.disabled_hart_mask	= 0,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
