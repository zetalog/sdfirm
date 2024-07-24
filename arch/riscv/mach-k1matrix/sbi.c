/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#include <target/sbi.h>
#include <target/fdt.h>
#include <target/uart.h>
#include <target/irq.h>
#include <target/delay.h>
#include <target/bench.h>

static void k1matrix_modify_dt(void *fdt)
{
	fdt_cpu_fixup(fdt);
	fdt_irq_fixup(fdt, "riscv,clint0");
	fdt_irqs_fixup(fdt, "riscv,plic0", PLIC_MAX_CHIPS);
	fdt_fixups(fdt);
}

static int k1matrix_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = sbi_scratch_thishart_arg1_ptr();
	k1matrix_modify_dt(fdt);

	return 0;
}

static uint32_t k1matrix_pmp_region_count(uint32_t hartid)
{
	return 1;
}

static int k1matrix_pmp_region_info(uint32_t hartid, uint32_t index,
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

static int k1matrix_early_init(bool cold_boot)
{
	return 0;
}

#ifdef CONFIG_CONSOLE
#define __K1MATRIX_UART_REG(n, offset)		(__K1MATRIX_UART_BASE + (offset))
#define __UART_RBR(n)				__K1MATRIX_UART_REG(n, 0x00)
#define __UART_THR(n)				__K1MATRIX_UART_REG(n, 0x00)
#define __UART_LSR(n)				__K1MATRIX_UART_REG(n, 0x14)
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
#define __k1matrix_uart_write_poll(n)		(!!(__raw_readl(__UART_LSR(n)) & LSR_TEMT))
#define __k1matrix_uart_write_byte(n, byte)	__raw_writel((byte), __UART_THR(n))
#else
#define __k1matrix_uart_write_poll(n)		true
#define __k1matrix_uart_write_byte(n, byte)	do { } while (0)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define __k1matrix_uart_read_poll(n)		(!!(__raw_readl(__UART_LSR(n)) & LSR_DR))
#define __k1matrix_uart_read_byte(n)		__raw_readl(__UART_RBR(n))
#else
#define __k1matrix_uart_read_poll(n)		true
#define __k1matrix_uart_read_byte(n)		0
#endif

static void k1matrix_console_putc(char ch)
{
	while (!__k1matrix_uart_write_poll(UART_CON_ID));
	__k1matrix_uart_write_byte(UART_CON_ID, ch);
}

static int k1matrix_console_getc(void)
{
	if (!__k1matrix_uart_read_poll(UART_CON_ID))
		return -1;
	return __k1matrix_uart_read_byte(UART_CON_ID);
}

static int k1matrix_irqchip_init(bool cold_boot)
{
	if (cold_boot)
		plic_sbi_init_cold();
	plic_sbi_init_warm(sbi_processor_id());
	return 0;
}

#ifdef CONFIG_SMP
void k1matrix_ipi_send(uint32_t target_cpu)
{
	clint_set_ipi(target_cpu);
}

void k1matrix_ipi_clear(uint32_t target_cpu)
{
	clint_clear_ipi(target_cpu);
}

static int k1matrix_ipi_init(bool cold_boot)
{
	cpu_t cpu = sbi_processor_id();

	if (!cold_boot)
		k1matrix_ipi_clear(cpu);
	return 0;
}
#else
void k1matrix_ipi_send(uint32_t target_cpu)
{
}

void k1matrix_ipi_clear(uint32_t target_cpu)
{
}

static int k1matrix_ipi_init(bool cold_boot)
{
	return 0;
}
#endif

uint64_t k1matrix_timer_value(void)
{
	return clint_read_mtime();
}

void k1matrix_timer_event_stop(void)
{
	__unused cpu_t cpu = sbi_processor_id();

	clint_unset_mtimecmp(cpu);
}

void k1matrix_timer_event_start(uint64_t next_event)
{
	__unused cpu_t cpu = sbi_processor_id();

	clint_set_mtimecmp(cpu, next_event);
}

static int k1matrix_timer_init(bool cold_boot)
{
	if (!cold_boot)
		k1matrix_timer_event_stop();
	return 0;
}

/*
 * Initialize hw performance counters.
 */
static int platform_pmu_init(void)
{
	return fdt_pmu_setup(fdt_get_address());
}

/*
 * Get platform specific mhpmevent value.
 */
static uint64_t platform_pmu_xlate_to_mhpmevent(uint32_t event_idx,
						uint64_t data)
{
	uint64_t evt_val = 0;

	/* 'data' is valid only for raw events and is equal to event selector */
	if (event_idx == SBI_PMU_EVENT_RAW_IDX) {
		evt_val = data;
	} else {
		/*
		 * Follows the SBI specification recommendation
		 * i.e. zero extended event_idx is used as mhpmevent value for
		 * hardware general/cache events if platform does't define one.
		 */
		evt_val = fdt_pmu_get_select_value(event_idx);
		if (!evt_val)
			evt_val = (uint64_t)event_idx;
	}

	return evt_val;
}

static int k1matrix_system_down(uint32_t type)
{
	k1matrix_finish(0);
	return 0;
}

static int k1matrix_system_finish(uint32_t code)
{
	if (code)
		k1matrix_finish(1);
	else
		k1matrix_finish(0);
	return 0;
}

static bool k1matrix_hart_disabled(uint32_t hartid)
{
	return false;
}

const struct sbi_platform_operations platform_ops = {
	.pmp_region_count	= k1matrix_pmp_region_count,
	.pmp_region_info	= k1matrix_pmp_region_info,
	.early_init		= k1matrix_early_init,
	.final_init		= k1matrix_final_init,
	.console_putc		= k1matrix_console_putc,
	.console_getc		= k1matrix_console_getc,
	.irqchip_init		= k1matrix_irqchip_init,
	.ipi_send		= k1matrix_ipi_send,
	.ipi_clear		= k1matrix_ipi_clear,
	.ipi_init		= k1matrix_ipi_init,
	.timer_value		= k1matrix_timer_value,
	.timer_event_stop	= k1matrix_timer_event_stop,
	.timer_event_start	= k1matrix_timer_event_start,
	.timer_init		= k1matrix_timer_init,
	.pmu_init		= platform_pmu_init,
	.pmu_xlate_to_mhpmevent	= platform_pmu_xlate_to_mhpmevent,
	.system_reboot		= k1matrix_system_down,
	.system_shutdown	= k1matrix_system_down,
	.system_finish		= k1matrix_system_finish,
	.hart_disabled		= k1matrix_hart_disabled,
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "SpacemiT K1Matrix Machine",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.disabled_hart_mask	= 0,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
