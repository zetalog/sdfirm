/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Atish Patra <atish.patra@wdc.com>
 */

#if 0
#include <libfdt.h>
#include <fdt.h>
#endif
#include <target/sbi.h>
#include <target/uart.h>
#include <target/irq.h>
#include <target/delay.h>
#include <target/console.h>

/**
 * The FU540 SoC has 5 HARTs but HART ID 0 doesn't have S mode. enable only
 * HARTs 1 to 4.
 */
#ifdef CONFIG_UNLEASHED_E51
#define FU540_ENABLED_HART_MASK			(1 << 0)
#endif /* CONFIG_UNLEASHED_E51 */
#ifdef CONFIG_UNLEASHED_U54
#define FU540_ENABLED_HART_MASK			CPU_ALL
#endif /* CONFIG_UNLEASHED_U54 */
#define FU540_HART_ID_DISABLED			~(FU540_ENABLED_HART_MASK)

static void fu540_modify_dt(void *fdt)
{
#if 0
	u32 i, size;
	int chosen_offset, err;
	int cpu_offset;
	char cpu_node[32] = "";
	const char *mmu_type;

	for (i = 0; i < FU540_HART_COUNT; i++) {
		sbi_sprintf(cpu_node, "/cpus/cpu@%d", i);
		cpu_offset = fdt_path_offset(fdt, cpu_node);
		mmu_type   = fdt_getprop(fdt, cpu_offset, "mmu-type", NULL);
		if (mmu_type && (!strcmp(mmu_type, "riscv,sv39") ||
				 !strcmp(mmu_type, "riscv,sv48")))
			continue;
		else
			fdt_setprop_string(fdt, cpu_offset, "status", "masked");
		memset(cpu_node, 0, sizeof(cpu_node));
	}
	size = fdt_totalsize(fdt);
	err  = fdt_open_into(fdt, fdt, size + 256);
	if (err < 0)
		sbi_printf(
			"Device Tree can't be expanded to accmodate new node");

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	fdt_setprop_string(fdt, chosen_offset, "stdout-path",
			   "/soc/serial@10010000:115200");

	plic_fdt_fixup(fdt, "riscv,plic0");
#endif
}

static int fu540_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = sbi_scratch_thishart_arg1_ptr();
	fu540_modify_dt(fdt);

	return 0;
}

#ifdef CONFIG_UNLEASHED_U54
static u32 fu540_pmp_region_count(u32 hartid)
{
	return 1;
}

static int fu540_pmp_region_info(u32 hartid, u32 index, ulong *prot,
				 ulong *addr, ulong *log2size)
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
#else
static u32 fu540_pmp_region_count(u32 hartid)
{
	return 0;
}

static int fu540_pmp_region_info(u32 hartid, u32 index, ulong *prot,
				 ulong *addr, ulong *log2size)
{
	return 0;
}
#endif

#define __UART_REG(n, offset)	(__SIFIVE_UART_BASE(n) + (offset))
#define __UART_TXDATA(n)	__UART_REG(n, 0x00)
#define __UART_RXDATA(n)	__UART_REG(n, 0x04)
#ifdef CONFIG_SIFIVE_UART_STATUS
#define __UART_TXSTAT(n)	__UART_REG(n, 0x02)
#define __UART_RXSTAT(n)	__UART_REG(n, 0x06)
#endif

#define __sifive_uart_write_poll(n)				\
	(!(__raw_readw(__UART_TXSTAT(n)) & UART_FULL))
#define __sifive_uart_read_poll(n)				\
	(!(__raw_readw(__UART_RXSTAT(n)) & UART_EMPTY))
#define __sifive_uart_write_byte(n, byte)			\
	__raw_writeb(byte, __UART_TXDATA(n))
#define __sifive_uart_read_byte(n)				\
	__raw_readb(__UART_RXDATA(n))

#define __fu540_console_putc(byte)				\
	do {							\
		while (!sifive_uart_write_poll(UART_CON_ID));	\
		sifive_uart_write_byte(UART_CON_ID, byte);	\
	} while (0)

#ifdef CONFIG_CONSOLE_OUTPUT_CR
static void append_cr(int c)
{
	if (c == '\n')
		__fu540_console_putc('\r');
}
#else
#define append_cr(c)	do { } while (0)
#endif

static void fu540_console_putc(char ch)
{
	append_cr(ch);
	__fu540_console_putc(ch);
}

static int fu540_console_getc(void)
{
	while (!__sifive_uart_read_poll(UART_CON_ID));
	return (int)__sifive_uart_read_byte(UART_CON_ID);
}

static int fu540_irqchip_init(bool cold_boot)
{
	cpu_t cpu = sbi_processor_id();

	if (cold_boot)
		plic_sbi_init_cold();
	else
		plic_sbi_init_warm(cpu);
	return 0;
}

void fu540_ipi_send(u32 target_cpu)
{
	clint_set_ipi(target_cpu);
}

void fu540_ipi_sync(u32 target_cpu)
{
	clint_sync_ipi(target_cpu);
}

void fu540_ipi_clear(u32 target_cpu)
{
	clint_clear_ipi(target_cpu);
}

static int fu540_ipi_init(bool cold_boot)
{
	cpu_t cpu = sbi_processor_id();

	if (!cold_boot)
		fu540_ipi_clear(cpu);
	return 0;
}

u64 fu540_timer_value(void)
{
	return clint_read_mtime();
}

void fu540_timer_event_stop(void)
{
	cpu_t cpu = sbi_processor_id();

	clint_unset_mtimecmp(cpu);
}

void fu540_timer_event_start(u64 next_event)
{
	cpu_t cpu = sbi_processor_id();

	clint_set_mtimecmp(cpu, next_event);
}

static int fu540_timer_init(bool cold_boot)
{
	if (!cold_boot)
		fu540_timer_event_stop();
	return 0;
}

static int fu540_system_down(u32 type)
{
	/* For now nothing to do. */
	return 0;
}

const struct sbi_platform_operations platform_ops = {
	.pmp_region_count	= fu540_pmp_region_count,
	.pmp_region_info	= fu540_pmp_region_info,
	.final_init		= fu540_final_init,
	.console_putc		= fu540_console_putc,
	.console_getc		= fu540_console_getc,
	.irqchip_init		= fu540_irqchip_init,
	.ipi_send		= fu540_ipi_send,
	.ipi_sync		= fu540_ipi_sync,
	.ipi_clear		= fu540_ipi_clear,
	.ipi_init		= fu540_ipi_init,
	.timer_value		= fu540_timer_value,
	.timer_event_stop	= fu540_timer_event_stop,
	.timer_event_start	= fu540_timer_event_start,
	.timer_init		= fu540_timer_init,
	.system_reboot		= fu540_system_down,
	.system_shutdown	= fu540_system_down
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "SiFive Freedom U540",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.disabled_hart_mask	= FU540_HART_ID_DISABLED,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
