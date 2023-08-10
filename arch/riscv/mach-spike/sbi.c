#include <target/sbi.h>
#include <target/fdt.h>
#include <target/uart.h>
#include <target/irq.h>
#include <target/delay.h>

static void spike_modify_dt(void *fdt)
{
	fdt_cpu_fixup(fdt);
	fdt_irq_fixup(fdt, "riscv,clint0");
	fdt_fixups(fdt);
}

static int spike_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = sbi_scratch_thishart_arg1_ptr();
	spike_modify_dt(fdt);
	return 0;
}

static uint32_t spike_pmp_region_count(uint32_t hartid)
{
	return 1;
}

static int spike_pmp_region_info(uint32_t hartid, uint32_t index,
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

#define __UART_REG(n, offset)	(__UART_BASE + (offset))
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

#ifdef CONFIG_SPIKE_8250
static void spike_console_putc(char ch)
{
	__ns16550_con_write(ch);
}

static int spike_console_getc(void)
{
	if (!__ns16550_con_poll())
		return -1;
	return (int)__ns16550_con_read();
}
#else
static void spike_console_putc(char ch)
{
	htif_putc(ch);
}

static int spike_console_getc(void)
{
	return htif_getc();
}
#endif

#ifdef CONFIG_SPIKE_PLIC
static int spike_irqchip_init(bool cold_boot)
{
	if (cold_boot)
		plic_sbi_init_cold();
	plic_sbi_init_warm(sbi_processor_id());
	return 0;
}
#else
static int spike_irqchip_init(bool cold_boot)
{
	return 0;
}
#endif

#define SPIKE_HART_STACK_SIZE			8192
#define SPIKE_CLINT_ADDR			0x2000000

static uint32_t clint_ipi_hart_count;
static volatile void *clint_ipi_base;
static volatile uint32_t *clint_ipi;

static void spike_ipi_send(uint32_t target_hart)
{
	if (clint_ipi_hart_count <= target_hart)
		return;

	/* Set CLINT IPI */
	__raw_writel(1, (caddr_t)&clint_ipi[target_hart]);
}

static void spike_ipi_clear(uint32_t target_hart)
{
	if (clint_ipi_hart_count <= target_hart)
		return;

	/* Clear CLINT IPI */
	__raw_writel(0, (caddr_t)&clint_ipi[target_hart]);
}

static int clint_cold_ipi_init(unsigned long base, uint32_t hart_count)
{
	/* Figure-out CLINT IPI register address */
	clint_ipi_hart_count = hart_count;
	clint_ipi_base	     = (void *)base;
	clint_ipi	     = (uint32_t *)clint_ipi_base;

	return 0;
}

static void clint_ipi_clear(uint32_t target_hart)
{
	if (clint_ipi_hart_count <= target_hart)
		return;

	/* Clear CLINT IPI */
	__raw_writel(0, (caddr_t)&clint_ipi[target_hart]);
}

static int clint_warm_ipi_init(void)
{
	uint32_t hartid = sbi_current_hartid();

	if (!clint_ipi_base)
		return -1;

	/* Clear CLINT IPI */
	clint_ipi_clear(hartid);

	return 0;
}

static int spike_ipi_init(bool cold_boot)
{
	if (cold_boot) {
		int ret = clint_cold_ipi_init(SPIKE_CLINT_ADDR,
					  MAX_CPU_NUM);
		if (ret)
			return ret;
	}

	return clint_warm_ipi_init();
}

uint64_t spike_timer_value(void)
{
	return clint_read_mtime();
}

void spike_timer_event_stop(void)
{
	cpu_t cpu = sbi_processor_id();

	clint_unset_mtimecmp(cpu);
}

void spike_timer_event_start(uint64_t next_event)
{
	cpu_t cpu = sbi_processor_id();

	clint_set_mtimecmp(cpu, next_event);
}

static int spike_timer_init(bool cold_boot)
{
	if (!cold_boot)
		spike_timer_event_stop();
	return 0;
}

static int spike_system_down(uint32_t type)
{
	htif_poweroff();
	return 0;
}

const struct sbi_platform_operations platform_ops = {
	.pmp_region_count	= spike_pmp_region_count,
	.pmp_region_info	= spike_pmp_region_info,
	.final_init		= spike_final_init,
	.console_putc		= spike_console_putc,
	.console_getc		= spike_console_getc,
	.irqchip_init		= spike_irqchip_init,
	.ipi_send		= spike_ipi_send,
	.ipi_clear		= spike_ipi_clear,
	.ipi_init		= spike_ipi_init,
	.timer_value		= spike_timer_value,
	.timer_event_stop	= spike_timer_event_stop,
	.timer_event_start	= spike_timer_event_start,
	.timer_init		= spike_timer_init,
	.system_reboot		= spike_system_down,
	.system_shutdown	= spike_system_down
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x01),
	.name			= "RISC-V ISA simulator (spike)",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count		= MAX_CPU_NUM,
	.disabled_hart_mask	= ~HART_ALL,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
