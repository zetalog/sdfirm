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

#define DW_MIPI_I3C_I2C_FAST_TLOW_MIN_NS	1300
#define DW_MIPI_I3C_I2C_FAST_PLUS_TLOW_MIN_NS	500
#define DW_MIPI_I3C_I3C_THIGH_MAX_NS		41

#define DW_MIPI_I3C_SCL_RATE_SDR1		8000000
#define DW_MIPI_I3C_SCL_RATE_SDR2		6000000
#define DW_MIPI_I3C_SCL_RATE_SDR3		4000000
#define DW_MIPI_I3C_SCL_RATE_SDR4		2000000

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

#define dw_i3c_ncmds			dw_i3c.ncmds
#define dw_i3c_cmds			dw_i3c.cmds

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

static void dw_mipi_i3c_write_tx_fifo(uint8_t *bytes, uint16_t nbytes)
{
	uint32_t *buf = (uint32_t *)bytes;
	uint16_t i;

	for (i = 0; i < nbytes/4; i++) {
		dw_i3c_writel(*buf, TX_DATA_PORT(dw_i3cd));
		buf++;
	}
	if (nbytes & 3) {
		uint32_t tmp;

		memcpy(&tmp, bytes + (nbytes & ~3), nbytes & 3);
		dw_i3c_writel(tmp, TX_DATA_PORT(dw_i3cd));
	}
}

static void dw_mipi_i3c_begin_xfer(void)
{
	int i;
	struct dw_mipi_i3c_cmd *cmd;

	for (i = 0; i < dw_i3c_ncmds; i++) {
		cmd = &dw_i3c_cmds[i];
		dw_mipi_i3c_write_tx_fifo(cmd->tx_buf, cmd->tx_len);
	}
	dw_i3c_writel_mask(DW_RESP_BUF_THLD(dw_i3c_ncmds),
			   DW_RESP_BUF_THLD(DW_RESP_BUF_THLD_MASK),
			   QUEUE_THLD_CTRL(dw_i3cd));
	for (i = 0; i < dw_i3c_ncmds; i++) {
		cmd = &dw_i3c_cmds[i];
		dw_i3c_writel(cmd->cmd_hi, COMMAND_QUEUE_PORT(dw_i3cd));
		dw_i3c_writel(cmd->cmd_lo, COMMAND_QUEUE_PORT(dw_i3cd));
	}
}

static void dw_mipi_i3c_end_xfer(void)
{
	int i;
	struct dw_mipi_i3c_cmd *cmd;
	uint32_t nresp;
	uint32_t resp;

	nresp = DW_RESP_BUF_BLR(dw_i3c_readl(QUEUE_STATUS_LEVEL(dw_i3cd)));
	for (i = 0; i < nresp; i++) {
	}
}

void dw_mipi_i3c_handle_irq(void)
{
	uint32_t status;

	dw_i3c_readl(INTR_STATUS(dw_i3cd));
	if (dw_i3c_readl(INTR_STATUS_EN(dw_i3cd))) {
		dw_i3c_writel(INTR_ALL, INTR_STATUS(dw_i3cd));
		return;
	}
	dw_mipi_i3c_end_xfer();
}

void dw_mipi_i3c_cfg_i2c_clk(clk_freq_t core_rate)
{
	uint32_t core_period;
	uint16_t lcnt, hcnt;

	core_period = DIV_ROUND_UP(1000000000, core_rate);

	lcnt = DIV_ROUND_UP(DW_MIPI_I3C_I2C_FAST_PLUS_TLOW_MIN_NS,
			    core_period);
	hcnt = DIV_ROUND_UP(core_rate, I3C_SCL_RATE_I2C_FAST_PLUS) - lcnt;

	dw_i3c_writel(DW_I2C_FM_HCNT(hcnt) | DW_I2C_FM_LCNT(lcnt),
		      SCL_I2C_FM_TIMING(dw_i3cd));
	dw_i3c_writel(DW_BUS_FREE_TIME(lcnt), BUS_FREE_AVAIL_TIMING(dw_i3cd));
	dw_i3c_setl(DW_I2C_SLAVE_PRESENT, DEVICE_CTRL(dw_i3cd));
}

void dw_mipi_i3c_cfg_i3c_clk(i3c_bus_t bus, clk_freq_t core_rate)
{
	uint32_t core_period;
	uint8_t hcnt, lcnt;
	uint32_t scl_timing;

	core_period = DIV_ROUND_UP(1000000000, core_rate);

	hcnt = DIV_ROUND_UP(DW_MIPI_I3C_I3C_THIGH_MAX_NS, core_period) - 1;
	if (hcnt < DW_I3C_PP_CNT_MIN)
		hcnt = DW_I3C_PP_CNT_MIN;
	lcnt = DIV_ROUND_UP(core_rate, I3C_SCL_RATE_I3C_TYP);
	if (lcnt < DW_I3C_PP_CNT_MIN)
		lcnt = DW_I3C_PP_CNT_MIN;

	dw_i3c_writel(DW_I3C_PP_HCNT(hcnt) | DW_I3C_PP_LCNT(lcnt),
		      SCL_I3C_PP_TIMING(dw_i3cd));
	if (bus == I3C_BUS_PURE)
		dw_i3c_writel(DW_BUS_FREE_TIME(lcnt),
			      BUS_FREE_AVAIL_TIMING(dw_i3cd));

	lcnt = max((uint8_t)DIV_ROUND_UP(I3C_I3C_TLOW_OD_MIN_NS, core_period),
		   lcnt);

	dw_i3c_writel(DW_I3C_OD_HCNT(hcnt) | DW_I3C_OD_LCNT(lcnt),
		      SCL_I3C_OD_TIMING(dw_i3cd));

	lcnt = DIV_ROUND_UP(core_rate, DW_MIPI_I3C_SCL_RATE_SDR1) - hcnt;
	scl_timing = DW_I3C_EXT_LCNT_1(lcnt);
	lcnt = DIV_ROUND_UP(core_rate, DW_MIPI_I3C_SCL_RATE_SDR2) - hcnt;
	scl_timing |= DW_I3C_EXT_LCNT_2(lcnt);
	lcnt = DIV_ROUND_UP(core_rate, DW_MIPI_I3C_SCL_RATE_SDR3) - hcnt;
	scl_timing |= DW_I3C_EXT_LCNT_3(lcnt);
	lcnt = DIV_ROUND_UP(core_rate, DW_MIPI_I3C_SCL_RATE_SDR4) - hcnt;
	scl_timing |= DW_I3C_EXT_LCNT_4(lcnt);
	dw_i3c_writel(scl_timing, SCL_EXT_LCNT_TIMING(dw_i3cd));
}

static void dw_mipi_i3c_ccc_set(struct i3c_ccc *ccc)
{
}

static void dw_mipi_i3c_ccc_get(struct i3c_ccc *ccc)
{
}

void dw_mipi_i3c_submit_ccc(struct i3c_ccc *ccc)
{
	if (ccc->rnw)
		dw_mipi_i3c_ccc_get(ccc);
	else
		dw_mipi_i3c_ccc_set(ccc);
}

static void dw_mipi_i3c_ibi_init(void)
{
	dw_i3c_writel_mask(DW_IBI_STATUS_THLD(1) |
			   DW_IBI_DATA_THLD(31) |
			   DW_RESP_BUF_THLD(0),
			   DW_IBI_STATUS_THLD(DW_IBI_STATUS_THLD_MASK) |
			   DW_IBI_DATA_THLD(DW_IBI_DATA_THLD_MASK) |
			   DW_RESP_BUF_THLD(DW_RESP_BUF_THLD_MASK),
			   QUEUE_THLD_CTRL(dw_i3cd));
	dw_i3c_writel_mask(DW_RX_BUF_THLD(0),
			   DW_RX_BUF_THLD(DW_RX_BUF_THLD_MASK),
			   DATA_BUFFER_THLD_CTRL(dw_i3cd));
	dw_mipi_i3c_disable_all_irqs(dw_i3cd);
	dw_mipi_i3c_enable_mst_irqs(dw_i3cd);
#ifdef CONFIG_DW_MIPI_I3C_SECONDARY_MASTER
	dw_i3c_writel(IBI_REQ_REJECT_ALL, IBI_MR_REQ_REJECT(dw_i3cd));
#endif
	dw_i3c_writel(IBI_REQ_REJECT_ALL, IBI_SIR_REQ_REJECT(dw_i3cd));
}

void dw_mipi_i3c_ctrl_init(i3c_bus_t bus, clk_freq_t core_rate)
{
	i3c_addr_t addr;

	switch (bus) {
	case I3C_BUS_MIXED_FAST:
	case I3C_BUS_MIXED_LIMITED:
		dw_mipi_i3c_cfg_i2c_clk(core_rate);
		break;
	case I3C_BUS_PURE:
		dw_mipi_i3c_cfg_i3c_clk(bus, core_rate);
		break;
	default:
		BUG();
		break;
	}

	addr = i3c_get_free_addr(0);
	BUG_ON(addr == INVALID_I3C_ADDR);
	i3c_dev_addr = addr;
	dw_mipi_i3c_config_da(dw_i3cd, addr);
	dw_mipi_i3c_ibi_init();
	dw_mipi_i3c_dev_enable(dw_i3cd);
}
