#include <target/i3c.h>
#include <target/clk.h>
#include <target/console.h>
#include <target/irq.h>
#include <target/panic.h>

#ifdef CONFIG_DW_MIPI_I2C_DEBUG
#define dw_i3c_dbg(...)		con_dbg(__VA_ARGS__)
#else
#define dw_i3c_dbg(...)		do { } while (0)
#endif

#if NR_DW_MIPI_I3CS > 1
#define dw_i3c				dw_i3cs[dw_i3cd]

static i3c_t dw_i3cd = 0;
static struct dw_mipi_i3c_ctx dw_i3cs[NR_DW_MIPI_I3CS] = {0};

void dw_mipi_i3c_master_select(i3c_t i3c)
{
	BUG_ON(i3c >= NR_DW_MIPI_I3CS);

	dw_i3cd = i3c;
}
#else
#define dw_i3cd				0
#define dw_mipi_i3c_master_select(i3c)	do { } while (0)

static struct dw_mipi_i3c_ctx dw_i3c;
#endif

#ifndef SYS_REALTIME
static void dw_mipi_i3c_irq_handler(irq_t irq)
{
	dw_mipi_i3c_master_select(irq - IRQ_I3C0);
	dw_mipi_i3c_handle_irq();
	irqc_ack_irq(irq);
}

void dw_mipi_i3c_irq_init(void)
{
	irqc_configure_irq(IRQ_I3C0 + dw_i3cd, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_I3C0 + dw_i3cd, dw_mipi_i3c_irq_handler);
	irqc_enable_irq(IRQ_I3C0 + dw_i3cd);
}
#endif

uint32_t dw_i3c_readl(caddr_t reg)
{
	uint32_t val;

	val = __raw_readl(reg);
	dw_i3c_dbg("dw_i3c: R %016lx=%08x\n", reg, val);
	return val;
}

void dw_i3c_writel(uint32_t val, caddr_t reg)
{
	dw_i3c_dbg("dw_i3c: W %016lx=%08x\n", reg, val);
	__raw_writel(val, reg);
}

void dw_mipi_i3c_transfer_reset(void)
{
}

void dw_mipi_i3c_handle_irq(void)
{
}

void dw_mipi_i3c_ctrl_init(void)
{
}
