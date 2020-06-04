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
#include <target/uart.h>
#include <target/irq.h>
#include <target/delay.h>
#include <target/bench.h>

#define VIRT_TEST_ADDR			0x100000
#define VIRT_TEST_FINISHER_FAIL		0x3333
#define VIRT_TEST_FINISHER_PASS		0x5555

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

static u32 virt_pmp_region_count(u32 hartid)
{
	return 1;
}

static int virt_pmp_region_info(u32 hartid, u32 index, ulong *prot, ulong *addr,
				ulong *log2size)
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

#ifdef CONFIG_CONSOLE_OUTPUT_CR
static void append_cr(int c)
{
	if (c == '\n')
		ns16550_con_write('\r');
}
#else
#define append_cr(c)	do { } while (0)
#endif

static void virt_console_putc(char ch)
{
	append_cr(ch);
	ns16550_con_write(ch);
}

static int virt_console_getc(void)
{
	while (!ns16550_con_poll());
	return (int)ns16550_con_read();
}

static int virt_irqchip_init(bool cold_boot)
{
	cpu_t cpu = sbi_processor_id();

	if (cold_boot)
		plic_sbi_init_cold();
	else
		plic_sbi_init_warm(cpu);
	return 0;
}

void virt_ipi_send(u32 target_cpu)
{
	clint_set_ipi(target_cpu);
}

void virt_ipi_sync(u32 target_cpu)
{
	clint_sync_ipi(target_cpu);
}

void virt_ipi_clear(u32 target_cpu)
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

u64 virt_timer_value(void)
{
	return clint_read_mtime();
}

void virt_timer_event_stop(void)
{
	cpu_t cpu = sbi_processor_id();

	clint_unset_mtimecmp(cpu);
}

void virt_timer_event_start(u64 next_event)
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

static int virt_system_down(u32 type)
{
	/* Tell the "finisher" that the simulation
	 * was successful so that QEMU exits
	 */
	__raw_writel(VIRT_TEST_FINISHER_PASS, VIRT_TEST_ADDR);
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
	.ipi_sync		= virt_ipi_sync,
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
