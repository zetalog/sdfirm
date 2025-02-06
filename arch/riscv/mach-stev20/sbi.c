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

static char riscv_isa_str[100];
#define FDT_FIXUP_SIZE 100

static void fdt_cpu_riscv_isa_fixup(void *fdt)
{
	int err, cpu_offset, cpus_offset;
	uint32_t hartid;

	strcpy((char *)&riscv_isa_str, "rv64imafdcsu_zicbom_sscofpmf_svpbmt_sstc");

#ifdef CONFIG_RISCV_V
	strcat((char *)&riscv_isa_str, "_v");
#endif

#ifdef CONFIG_RISCV_H
	strcat((char *)&riscv_isa_str, "_h");
#endif

#ifdef CONFIG_RISCV_SMAIA
	strcat((char *)&riscv_isa_str, "_smaia_ssaia_smstateen");
#endif

#ifdef CONFIG_CPU_CVA6
	strcpy((char *)&riscv_isa_str, "rv64imafdcsu_sscofpmf");
#endif

	err = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + FDT_FIXUP_SIZE);
	if (err < 0)
		return;

	cpus_offset = fdt_path_offset(fdt, "/cpus");
	if (cpus_offset < 0)
		return;

	fdt_for_each_subnode(cpu_offset, fdt, cpus_offset) {
		err = fdt_parse_hart_id(fdt, cpu_offset, &hartid);
		if (err)
			continue;

		//FIXME: for k2
		if (hartid >= 8)
			break;

		fdt_setprop_string(fdt, cpu_offset, "riscv,isa",
				   (const char *)&riscv_isa_str);
	}
}

static void k1max_modify_dt_early(void *fdt)
{
	fdt_cpu_riscv_isa_fixup(fdt);
}

static void k1max_modify_dt_late(void *fdt)
{
	fdt_cpu_fixup(fdt);
	fdt_irq_fixup(fdt, "riscv,clint0");
	fdt_irqs_fixup(fdt, "riscv,plic0", PLIC_MAX_CHIPS);
	fdt_fixups(fdt);
}

static int k1max_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = sbi_scratch_thishart_arg1_ptr();
	k1max_modify_dt_late(fdt);

	return 0;
}

static uint32_t k1max_pmp_region_count(uint32_t hartid)
{
	return 1;
}

static int k1max_pmp_region_info(uint32_t hartid, uint32_t index,
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

static int k1max_early_init(bool cold_boot)
{
	struct csr_trap_info trap = {0};
	
	csr_read_allowed(CSR_TCMCFG, (unsigned long)&trap);
	if (!trap.cause) {
		csr_write(CSR_TCMCFG, 1);
	}

	if (cold_boot) {
		void *fdt = sbi_scratch_thishart_arg1_ptr();
		k1max_modify_dt_early(fdt);
	}

	return 0;
}

#ifdef CONFIG_DW_UART
#ifdef CONFIG_CONSOLE
#define __K1MAX_UART_REG(n, offset)		(__K1MAX_UART_BASE + (offset))
#define __UART_RBR(n)				__K1MAX_UART_REG(n, 0x00)
#define __UART_THR(n)				__K1MAX_UART_REG(n, 0x00)
#define __UART_LSR(n)				__K1MAX_UART_REG(n, 0x14)
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
#define __k1max_uart_write_poll(n)		(!!(__raw_readl(__UART_LSR(n)) & LSR_TEMT))
#define __k1max_uart_write_byte(n, byte)	__raw_writel((byte), __UART_THR(n))
#else
#define __k1max_uart_write_poll(n)		true
#define __k1max_uart_write_byte(n, byte)	do { } while (0)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define __k1max_uart_read_poll(n)		(!!(__raw_readl(__UART_LSR(n)) & LSR_DR))
#define __k1max_uart_read_byte(n)		__raw_readl(__UART_RBR(n))
#else
#define __k1max_uart_read_poll(n)		true
#define __k1max_uart_read_byte(n)		0
#endif
#endif /* CONFIG_DW_UART */

#ifdef CONFIG_NS16550
#ifdef CONFIG_CONSOLE
#define __K1MAX_UART_REG2(n, offset)		(UART_BASE(n) + ((offset) << 2))
#define __UART_RBR(n)				__K1MAX_UART_REG2(n, 0)
#define __UART_LSR(n)				__K1MAX_UART_REG2(n, 5)
#define __UART_THR(n)				__UART_RBR(n)
#define __uart_reg_write(v, reg)		__raw_writeb(v, reg)
#define __uart_reg_read(reg)			__raw_readb(reg)
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
#define __k1max_uart_write_poll(n)		(!!(__uart_reg_read(__UART_LSR(n)) & UART_LSR_TEMT))
#define __k1max_uart_write_byte(n, byte)	__uart_reg_write((byte), __UART_THR(n))
#else
#define __k1max_uart_write_poll(n)		true
#define __k1max_uart_write_byte(n, byte)	do { } while (0)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define __k1max_uart_read_poll(n)		(!!(__uart_reg_read(__UART_LSR(n)) & UART_LSR_DR))
#define __k1max_uart_read_byte(n)		__uart_reg_read(__UART_RBR(n))
#else
#define __k1max_uart_read_poll(n)		true
#define __k1max_uart_read_byte(n)		0
#endif
#endif

static void k1max_console_putc(char ch)
{
	while (!__k1max_uart_write_poll(UART_CON_ID));
	__k1max_uart_write_byte(UART_CON_ID, ch);
}

static int k1max_console_getc(void)
{
	if (!__k1max_uart_read_poll(UART_CON_ID))
		return -1;
	return __k1max_uart_read_byte(UART_CON_ID);
}

static int k1max_irqchip_init(bool cold_boot)
{
#ifdef CONFIG_RISCV_SMAIA
	imsic_sbi_init_cold();
	aplic_sbi_init_cold();
#else
	if (cold_boot)
		plic_sbi_init_cold();
	plic_sbi_init_warm(sbi_processor_id());
#endif
	return 0;
}

#ifdef CONFIG_SMP
void k1max_ipi_send(uint32_t target_cpu)
{
	clint_set_ipi(target_cpu);
}

void k1max_ipi_clear(uint32_t target_cpu)
{
	clint_clear_ipi(target_cpu);
}

static int k1max_ipi_init(bool cold_boot)
{
	cpu_t cpu = sbi_processor_id();

	if (!cold_boot)
		k1max_ipi_clear(cpu);
	return 0;
}
#else
void k1max_ipi_send(uint32_t target_cpu)
{
}

void k1max_ipi_clear(uint32_t target_cpu)
{
}

static int k1max_ipi_init(bool cold_boot)
{
	return 0;
}
#endif

uint64_t k1max_timer_value(void)
{
	return clint_read_mtime();
}

void k1max_timer_event_stop(void)
{
	cpu_t cpu = sbi_processor_id();

	clint_unset_mtimecmp(cpu);
}

void k1max_timer_event_start(uint64_t next_event)
{
	cpu_t cpu = sbi_processor_id();

	clint_set_mtimecmp(cpu, next_event);
}

static int k1max_timer_init(bool cold_boot)
{
	if (!cold_boot)
		k1max_timer_event_stop();
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
static uint64_t platform_pmu_xlate_to_mhpmevent(uint32_t event_idx, uint64_t data)
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

static int k1max_system_down(uint32_t type)
{
	/* Tell the "finisher" that the simulation
	 * was successful so that QEMU exits
	 */
	return 0;
}

static int k1max_system_finish(uint32_t code)
{
	if (code)
		k1max_finish(1);
	else
		k1max_finish(0);
	return 0;
}

static bool k1max_hart_disabled(uint32_t hartid)
{
	return ~BOOT_MASK & CPU_TO_MASK(hartid);
}

const struct sbi_platform_operations platform_ops = {
	.pmp_region_count	= k1max_pmp_region_count,
	.pmp_region_info	= k1max_pmp_region_info,
	.early_init		= k1max_early_init,
	.final_init		= k1max_final_init,
	.console_putc		= k1max_console_putc,
	.console_getc		= k1max_console_getc,
	.irqchip_init		= k1max_irqchip_init,
	.ipi_send		= k1max_ipi_send,
	.ipi_clear		= k1max_ipi_clear,
	.ipi_init		= k1max_ipi_init,
	.timer_value		= k1max_timer_value,
	.timer_event_stop	= k1max_timer_event_stop,
	.timer_event_start	= k1max_timer_event_start,
	.timer_init		= k1max_timer_init,
	.pmu_init		= platform_pmu_init,
	.pmu_xlate_to_mhpmevent	= platform_pmu_xlate_to_mhpmevent,
	.system_reboot		= k1max_system_down,
	.system_shutdown	= k1max_system_down,
	.system_finish		= k1max_system_finish,
	.hart_disabled		= k1max_hart_disabled,
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "Spacemit K1MAX Machine",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.disabled_hart_mask	= 0,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
