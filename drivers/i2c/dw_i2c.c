#include <target/i2c.h>
#include <target/clk.h>
#include <target/console.h>
#include <target/irq.h>
#include <target/panic.h>

#ifdef CONFIG_DW_I2C_DEBUG
#define dw_i2c_dbg(...)		con_dbg(__VA_ARGS__)
#else
#define dw_i2c_dbg(...)		do { } while (0)
#endif

/*
 * This constant is used to calculate when during the clock high phase the
 * data bit shall be read.
 * "This is just an experimental rule: the tHD;STA period turned out to be
 * proportinal to (_HCNT + 3). With this setting, we could meet both tHIGH
 * and tHD;STA timing specs."
 */
#define T_HD_STA_OFFSET			3

struct dw_i2c_speed_config {
	/* SCL high and low period count */
	uint16_t scl_lcnt;
	uint16_t scl_hcnt;
	uint32_t sda_hold;
};

#if NR_DW_I2CS > 1
#define dw_i2c				dw_i2cs[dw_i2cd]

static i2c_t dw_i2cd = 0;
static struct dw_i2c_ctx dw_i2cs[NR_DW_I2CS] = {0};

void dw_i2c_master_select(i2c_t i2c)
{
	BUG_ON(i2c >= NR_DW_I2CS);

	dw_i2cd = i2c;
}
#else
#define dw_i2cd				0
#define dw_i2c_master_select(i2c)	do { } while (0)

static struct dw_i2c_ctx dw_i2c;
#endif

uint32_t dw_i2c_readl(caddr_t reg)
{
	uint32_t val;

	val = __raw_readl(reg);
	dw_i2c_dbg("dw_i2c: R %016lx=%08x\n", reg, val);
	return val;
}

void dw_i2c_writel(uint32_t val, caddr_t reg)
{
	dw_i2c_dbg("dw_i2c: W %016lx=%08x\n", reg, val);
	__raw_writel(val, reg);
}

static __inline uint32_t dw_i2c_get_status(void)
{
	return dw_i2c_readl(IC_STATUS(dw_i2cd));
}

static void dw_i2c_ctrl_abort(void)
{
	dw_i2c_writel(IC_ENABLE_ABORT, IC_ENABLE(dw_i2cd));
	while (dw_i2c_readl(IC_ENABLE(dw_i2cd)) & IC_ENABLE_ABORT);
}

/* Slave configuration */
void dw_i2c_set_address(i2c_addr_t addr, boolean call)
{
	dw_i2c_ctrl_disable();
	dw_i2c_writel((uint32_t)addr, IC_SAR(dw_i2cd));
	dw_i2c_ctrl_enable();
}

#define NANO_TO_KILO		1000000

static uint32_t calc_counts(clk_freq_t ic_clk, uint32_t period_ns)
{
	return DIV_ROUND_UP(ic_clk / 1000 * period_ns, NANO_TO_KILO);
}

struct dw_i2c_mode_info {
	int speed;
	int min_scl_hightime_ns;
	int min_scl_lowtime_ns;
	int def_rise_time_ns;
	int def_fall_time_ns;
};

static const struct dw_i2c_mode_info dw_i2c_ss_info = {
	I2C_SPEED_STANDARD_RATE * 1000,
	MIN_SS_SCL_HIGHTIME,
	MIN_SS_SCL_LOWTIME,
	1000,
	300,
};
static const struct dw_i2c_mode_info dw_i2c_fs_info = {
	I2C_SPEED_FAST_RATE * 1000,
	MIN_FS_SCL_HIGHTIME,
	MIN_FS_SCL_LOWTIME,
	300,
	300,
};
static const struct dw_i2c_mode_info dw_i2c_fps_info = {
	I2C_SPEED_FAST_PLUS_RATE * 1000,
	MIN_FP_SCL_HIGHTIME,
	MIN_FP_SCL_LOWTIME,
	260,
	500,
};
static const struct dw_i2c_mode_info dw_i2c_hs_info = {
	I2C_SPEED_HIGH_RATE,
	MIN_HS_SCL_HIGHTIME,
	MIN_HS_SCL_LOWTIME,
	120,
	120,
};

#ifdef CONFIG_ARCH_HAS_DW_I2C_FREQ_OPTIMIZATION
static void dw_i2c_calc_timing(const struct dw_i2c_mode_info *info,
			       uint32_t spk_cnt, clk_freq_t ic_clk,
			       struct dw_i2c_speed_config *config)
{
	uint32_t fall_cnt, rise_cnt, min_tlow_cnt, min_thigh_cnt;
	uint32_t hcnt, lcnt, period_cnt, cnt;
	uint32_t sda_hold_time_ns, scl_rise_time_ns, scl_fall_time_ns;

	period_cnt = ic_clk / info->speed;
	scl_rise_time_ns = DW_I2C_SCL_RISE_TIME ?
			   DW_I2C_SCL_RISE_TIME : info->def_rise_time_ns;
	scl_fall_time_ns = DW_I2C_SCL_FALL_TIME ?
			   DW_I2C_SCL_FALL_TIME : info->def_fall_time_ns;
	rise_cnt = calc_counts(ic_clk, scl_rise_time_ns);
	fall_cnt = calc_counts(ic_clk, scl_fall_time_ns);
	min_tlow_cnt = calc_counts(ic_clk, info->min_scl_lowtime_ns);
	min_thigh_cnt = calc_counts(ic_clk, info->min_scl_hightime_ns);
	dw_i2c_dbg("dw_i2c: ic_clk %lld/speed %d, period %d rise %d fall %d tlow %d thigh %d spk %d\n",
		ic_clk, info->speed, period_cnt, rise_cnt, fall_cnt,
		min_tlow_cnt, min_thigh_cnt, spk_cnt);

	/* TODO: Use min_tlow_cnt/min_thigh_cnt */
	cnt = ic_clk / info->speed / 2;
	if (cnt * 2 * info->speed < ic_clk)
		cnt++;
	if ((cnt * 2 - 1) * info->speed < ic_clk) {
		lcnt = cnt;
		hcnt = cnt;
	} else {
		lcnt = cnt;
		hcnt = cnt - 1;
	}
	config->scl_lcnt = lcnt;
	config->scl_hcnt = hcnt;

	sda_hold_time_ns = DW_I2C_SDA_HOLD_TIME ?
			   DW_I2C_SDA_HOLD_TIME : DEFAULT_SDA_HOLD_TIME;
	config->sda_hold = calc_counts(ic_clk, sda_hold_time_ns);
	dw_i2c_dbg("dw_i2c: SCL_HCNT=%d SCL_LCNT=%d SDA_HOLD=%d\n", hcnt, lcnt,
		config->sda_hold);
}
#else
static void dw_i2c_calc_timing(const struct dw_i2c_mode_info *info,
			       uint32_t spk_cnt, clk_freq_t ic_clk,
			       struct dw_i2c_speed_config *config)
{
	uint32_t fall_cnt, rise_cnt, min_tlow_cnt, min_thigh_cnt;
	uint32_t hcnt, lcnt, period_cnt, diff, tot;
	uint32_t sda_hold_time_ns, scl_rise_time_ns, scl_fall_time_ns;

	period_cnt = ic_clk / info->speed;
	scl_rise_time_ns = DW_I2C_SCL_RISE_TIME ?
			   DW_I2C_SCL_RISE_TIME : info->def_rise_time_ns;
	scl_fall_time_ns = DW_I2C_SCL_FALL_TIME ?
			   DW_I2C_SCL_FALL_TIME : info->def_fall_time_ns;
	rise_cnt = calc_counts(ic_clk, scl_rise_time_ns);
	fall_cnt = calc_counts(ic_clk, scl_fall_time_ns);
	min_tlow_cnt = calc_counts(ic_clk, info->min_scl_lowtime_ns);
	min_thigh_cnt = calc_counts(ic_clk, info->min_scl_hightime_ns);
	dw_i2c_dbg("dw_i2c: ic_clk %lld, speed %d, period %d rise %d fall %d tlow %d thigh %d spk %d\n",
		ic_clk, info->speed, period_cnt, rise_cnt, fall_cnt,
		min_tlow_cnt, min_thigh_cnt, spk_cnt);

	/*
	 * Back-solve for hcnt and lcnt according to the following equations:
	 * SCL_High_time = [(HCNT + IC_*_SPKLEN + T_HD_STA_OFFSET) * ic_clk] + SCL_Fall_time
	 * SCL_Low_time = [(LCNT + 1) * ic_clk] - SCL_Fall_time + SCL_Rise_time
	 */
	hcnt = min_thigh_cnt - fall_cnt - T_HD_STA_OFFSET - spk_cnt;
	lcnt = min_tlow_cnt - rise_cnt + fall_cnt - 1;

	if (hcnt < 0 || lcnt < 0) {
		con_err("dw_i2c: bad counts. hcnt = %d lcnt = %d\n", hcnt, lcnt);
		BUG();
	}

	/* Now add things back up to ensure the period is hit. If it is
	 * off, split the difference and bias to lcnt for remainder.
	 */
	tot = hcnt + lcnt + T_HD_STA_OFFSET + spk_cnt + rise_cnt + 1;
	if (tot < period_cnt) {
		diff = (period_cnt - tot) / 2;
		hcnt += diff;
		lcnt += diff;
		tot = hcnt + lcnt + T_HD_STA_OFFSET + spk_cnt + rise_cnt + 1;
		lcnt += period_cnt - tot;
	}

	config->scl_lcnt = lcnt;
	config->scl_hcnt = hcnt;

	sda_hold_time_ns = DW_I2C_SDA_HOLD_TIME ?
			   DW_I2C_SDA_HOLD_TIME : DEFAULT_SDA_HOLD_TIME;
	config->sda_hold = calc_counts(ic_clk, sda_hold_time_ns);
	dw_i2c_dbg("dw_i2c: SCL_HCNT=%d SCL_LCNT=%d SDA_HOLD=%d\n", hcnt, lcnt,
		config->sda_hold);
}
#endif

void dw_i2c_set_frequency(uint16_t khz)
{
	uint32_t spklen;
	uint32_t con_speed;
	clk_freq_t bus_speed = DW_I2C_FREQ;
	clk_freq_t i2c_speed = khz * 1000;
	const struct dw_i2c_mode_info *info;
	bool fps = false;
	struct dw_i2c_speed_config config;

	if (i2c_speed >= I2C_MAX_SPEED)
		con_speed = IC_CON_SPEED_MAX;
	else if (i2c_speed >= I2C_FAST_PLUS_SPEED) {
		fps = true;
		con_speed = IC_CON_SPEED_FAST;
	} else if (i2c_speed >= I2C_FAST_SPEED)
		con_speed = IC_CON_SPEED_FAST;
	else
		con_speed = IC_CON_SPEED_STD;

	dw_i2c_ctrl_disable();
	dw_i2c_writel_mask(IC_CON_SPEED(con_speed),
			   IC_CON_SPEED(IC_CON_SPEED_MASK),
			   IC_CON(dw_i2cd));
	switch (con_speed) {
	default:
	case IC_CON_SPEED_STD:
		spklen = dw_i2c_readl(IC_FS_SPKLEN(dw_i2cd));
		info = &dw_i2c_ss_info;
		dw_i2c_calc_timing(info, spklen, bus_speed, &config);
		dw_i2c_writel(config.scl_hcnt, IC_SS_SCL_HCNT(dw_i2cd));
		dw_i2c_writel(config.scl_lcnt, IC_SS_SCL_LCNT(dw_i2cd));
		break;
	case IC_CON_SPEED_FAST:
		spklen = dw_i2c_readl(IC_FS_SPKLEN(dw_i2cd));
		if (fps)
			info = &dw_i2c_fps_info;
		else
			info = &dw_i2c_fs_info;
		dw_i2c_calc_timing(info, spklen, bus_speed, &config);
		dw_i2c_writel(config.scl_hcnt, IC_FS_SCL_HCNT(dw_i2cd));
		dw_i2c_writel(config.scl_lcnt, IC_FS_SCL_LCNT(dw_i2cd));
		break;
	case IC_CON_SPEED_HIGH:
		spklen = dw_i2c_readl(IC_HS_SPKLEN(dw_i2cd));
		info = &dw_i2c_hs_info;
		dw_i2c_calc_timing(info, spklen, bus_speed, &config);
		dw_i2c_writel(config.scl_hcnt, IC_HS_SCL_HCNT(dw_i2cd));
		dw_i2c_writel(config.scl_lcnt, IC_HS_SCL_LCNT(dw_i2cd));
		break;
	}
	if (config.sda_hold)
		dw_i2c_writel(config.sda_hold, IC_SDA_HOLD(dw_i2cd));
	dw_i2c_ctrl_enable();
}

#ifdef CONFIG_DW_I2C_DYNAMIC_TAR_UPDATE
static void dw_i2c_update_target(i2c_addr_t addr)
{
	dw_i2c_writel(addr, IC_TAR(dw_i2cd));
}

static void dw_i2c_set_target(i2c_addr_t addr)
{
	dw_i2c_ctrl_disable();
	dw_i2c_update_target(addr);
	dw_i2c_ctrl_enable();
}
#else
static void dw_i2c_update_target(i2c_addr_t addr)
{
	dw_i2c_ctrl_disable();
	dw_i2c_writel(addr, IC_TAR(dw_i2cd));
	dw_i2c_ctrl_enable();
}

#define dw_i2c_set_target(addr)		dw_i2c_update_target(addr)
#endif

static void dw_i2c_flush_rxfifo(void)
{
	while (dw_i2c_get_status() & IC_STATUS_RFNE);
}

/* Do nothing at I2C bus here because START will be issued automatically
 * by the controller.
 * When operating as an I2C master, putting data into the transmit FIFO
 * causes the DW_apb_i2c to generate a START condition on the I2C bus.
 */
void dw_i2c_start_condition(bool sr)
{
	if (sr) {
		dw_i2c_stop_condition();
	}
	i2c_set_status(I2C_STATUS_START);
	dw_i2c_dbg("dw_i2c: DW_I2C_DRIVER_START\n");
	dw_i2c.state = DW_I2C_DRIVER_START;
}

/* Do nothing I2C bus here because STOP will be issued automatically by the
 * controller.
 * If the IC_EMPTYFIFO_HOLD_MASTER_EN parameter is set to 0, allowing the
 * transmit FIFO to empty causes the DW_apb_i2c to generate a STOP
 * condition on the I2C bus.
 * If the IC_EMPTYFIFO_HOLD_MASTER_EN parameter is set to 1, then writing
 * a 1 to IC_DATA_CMD[9] causes the DW_apb_i2c to generate a STOP
 * condition on the I2C bus; a STOP condition is not issued if this bit is
 * not set, even if the transmit FIFO is empty.
 */
void dw_i2c_stop_condition(void)
{
	dw_i2c_dbg("dw_i2c: DW_I2C_DRIVER_STOP\n");
	dw_i2c.state = DW_I2C_DRIVER_STOP;
}

#ifdef CONFIG_I2C_DEVICE_ID
bool dw_i2c_device_id(uint8_t byte)
{
	uint32_t val;

	/* Special case: to probe device ID.
	 *   START +
	 *   1st byte: I2C_ADDR_DEVID << 7 | I2C_MODE_TX
	 *   2nd byte: target_address << 7 | I2C_DIR
	 *   3rd byte: target_address << 7 | I2C_MODE_RX
	 */
	switch (dw_i2c.state) {
	case DW_I2C_DRIVER_START:
		/* Process 1st byte: Detect this case. */
		if (byte == (I2C_ADDR_DEVID << 7 | I2C_MODE_TX)) {
			dw_i2c_dbg("dw_i2c: DW_I2C_DRIVER_DEVID_START\n");
			dw_i2c.state = DW_I2C_DRIVER_DEVID_START;
			return true;
		}
		break;
	case DW_I2C_DRIVER_DEVID_START:
		/* Process 2nd byte:
		 * Write 1 to IC_TAR[13] and IC_TAR[11] (along with target
		 * address) to enable a Device ID read between disable and
		 * enable.
		 */
		val = dw_i2c.addr_mode >> 1;
		val |= IC_TAR_DEVICE_ID;
		dw_i2c_update_target(val);
		dw_i2c_dbg("dw_i2c: DW_I2C_DRIVER_DEVID_TAR\n");
		dw_i2c.state = DW_I2C_DRIVER_DEVID_TAR;
		return true;
	case DW_I2C_DRIVER_DEVID_TAR:
		/* Process 3rd byte:
		 * Push 3 read commands into IC_DATA_CMD.
		 */
		dw_i2c_dbg("dw_i2c: DW_I2C_DRIVER_DATA\n");
		dw_i2c.state = DW_I2C_DRIVER_DATA;
		return true;
	default:
		break;
	}
	return false;
}
#else
#define dw_i2c_device_id(byte)		false
#endif

void dw_i2c_submit_write(uint8_t byte)
{
	uint32_t val = byte;

	if ((i2c_rxsubmit == 0) && i2c_prev_byte()) {
		val |= IC_DATA_CMD_STOP;
	}
	if (i2c_first_byte()) {
		dw_i2c_setl(IC_INTR_TX, IC_INTR_MASK(dw_i2cd));
		val |= IC_DATA_CMD_RESTART;
	} else {
		dw_i2c_setl(IC_INTR_IDL, IC_INTR_MASK(dw_i2cd));
	}
	dw_i2c.last_tx_byte = byte;
	while (!(dw_i2c_get_status() & IC_STATUS_TFNF));
	dw_i2c_writel(val, IC_DATA_CMD(dw_i2cd));
}

void dw_i2c_commit_write(void)
{
	if (i2c_last_byte())
		dw_i2c_clearl(IC_INTR_TX, IC_INTR_MASK(dw_i2cd));
}

void dw_i2c_write_byte(uint8_t byte)
{
	if (dw_i2c_device_id(byte))
		return;

	/* Normal case:
	 *   START +
	 *   1st byte: target_address << 7 | I2C_DIR
	 *   following: data
	 */
	/* The 1st byte after START is target address and direction. 
	 * If dynamically updating is not supported, the controller
	 * should be disabled first.
	 */
	if (dw_i2c.state == DW_I2C_DRIVER_START) {
		dw_i2c.addr_mode = byte;
		dw_i2c_set_target(i2c_addr(dw_i2c.addr_mode));
		dw_i2c_dbg("dw_i2c: DW_I2C_DRIVER_DATA\n");
		dw_i2c.state = DW_I2C_DRIVER_DATA;
		i2c_set_status(I2C_STATUS_ACK);
		return;
	}

	i2c_sync_status();
	dw_i2c.status = I2C_STATUS_IDLE;
	dw_i2c_submit_write(byte);
	while (dw_i2c.status == I2C_STATUS_IDLE)
		bh_sync();
	dw_i2c_commit_write();
	i2c_set_status(dw_i2c.status);
}

void dw_i2c_submit_read(void)
{
	uint32_t val = IC_DATA_CMD_CMD;

	if (i2c_first_byte()) {
		dw_i2c_setl(IC_INTR_RX, IC_INTR_MASK(dw_i2cd));
		val |= IC_DATA_CMD_RESTART;
	}
	dw_i2c_writel(val, IC_DATA_CMD(dw_i2cd));
}

uint8_t dw_i2c_commit_read(void)
{
	uint8_t byte;

	while (!(dw_i2c_get_status() & IC_STATUS_RFNE));
	byte = (uint8_t)dw_i2c_readl(IC_DATA_CMD(dw_i2cd));
	if (i2c_last_byte())
		dw_i2c_clearl(IC_INTR_RX, IC_INTR_MASK(dw_i2cd));
	return byte;
}

uint8_t dw_i2c_read_byte(void)
{
	uint8_t byte;

	i2c_sync_status();
	dw_i2c.status = I2C_STATUS_IDLE;
	dw_i2c_submit_read();
	while (dw_i2c.status == I2C_STATUS_IDLE)
		bh_sync();
	byte = dw_i2c_commit_read();
	i2c_set_status(dw_i2c.status);
	return byte;
}

/*
 * Abort transfer if any, and get ready for new start
 */
void dw_i2c_transfer_reset(void)
{
	dw_i2c_ctrl_abort();
	dw_i2c_flush_rxfifo();

	/* Note: Do not identify the source as ABRT_USER_ABRT */

	/* Reset internal status of driver */
	dw_i2c.addr_mode = 0;
	dw_i2c_dbg("dw_i2c: DW_I2C_DRIVER_INIT\n");
	dw_i2c.state = DW_I2C_DRIVER_INIT;
}

void dw_i2c_handle_irq(void)
{
	uint32_t status;
	uint32_t enable;
	i2c_addr_t abrt_src;

	enable = __raw_readl(IC_ENABLE(dw_i2cd));
	status = __raw_readl(IC_INTR_STAT(dw_i2cd));
	if (!enable || !(status & ~IC_INTR_ACTIVITY))
		return;

	if (status & IC_INTR_RX_UNDER) {
		dw_i2c_dbg("dw_i2c: INTR_RX_UNDER\n");
		__raw_readl(IC_CLR_RX_UNDER(dw_i2cd));
	}
	if (status & IC_INTR_RX_OVER) {
		dw_i2c_dbg("dw_i2c: INTR_RX_OVER\n");
		__raw_readl(IC_CLR_RX_OVER(dw_i2cd));
	}
	if (status & IC_INTR_RX_FULL) {
		dw_i2c_dbg("dw_i2c: INTR_RX_FULL\n");
		dw_i2c.status = I2C_STATUS_ACK;
	}
	if (status & IC_INTR_TX_OVER) {
		dw_i2c_dbg("dw_i2c: INTR_TX_OVER\n");
		__raw_readl(IC_CLR_TX_OVER(dw_i2cd));
	}
	if (status & IC_INTR_TX_EMPTY) {
		dw_i2c_dbg("dw_i2c: INTR_TX_EMPTY\n");
		dw_i2c.status = I2C_STATUS_ACK;
		dw_i2c_clearl(IC_INTR_IDL, IC_INTR_MASK(dw_i2cd));
	}
	if (status & IC_INTR_RD_REQ) {
		dw_i2c_dbg("dw_i2c: INTR_RD_REQ\n");
		__raw_readl(IC_CLR_RD_REQ(dw_i2cd));
	}
	if (status & IC_INTR_TX_ABRT) {
		dw_i2c_dbg("dw_i2c: INTR_TX_ABRT\n");
		abrt_src = (i2c_addr_t)__raw_readl(IC_TX_ABRT_SOURCE(dw_i2cd));
		__raw_readl(IC_CLR_TX_ABRT(dw_i2cd));
		if (abrt_src & IC_TX_ABRT_SOURCE_NOACK)
			dw_i2c.status = I2C_STATUS_NACK;
		else
			dw_i2c.status = I2C_STATUS_ARBI;
	}
	if (status & IC_INTR_RX_DONE) {
		dw_i2c_dbg("dw_i2c: INTR_RX_DONE\n");
		__raw_readl(IC_CLR_RX_DONE(dw_i2cd));
	}
	if (status & IC_INTR_ACTIVITY) {
		dw_i2c_dbg("dw_i2c: INTR_ACTIVITY\n");
		__raw_readl(IC_CLR_ACTIVITY(dw_i2cd));
	}
	if (status & IC_INTR_STOP_DET) {
		dw_i2c_dbg("dw_i2c: INTR_STOP_DET\n");
		__raw_readl(IC_CLR_STOP_DET(dw_i2cd));
		/* dw_i2c.status = I2C_STATUS_STOP; */
	}
	if (status & IC_INTR_START_DET) {
		dw_i2c_dbg("dw_i2c: INTR_START_DET\n");
		__raw_readl(IC_CLR_START_DET(dw_i2cd));
	}
	if (status & IC_INTR_GEN_CALL) {
		dw_i2c_dbg("dw_i2c: INTR_GEN_CALL\n");
		__raw_readl(IC_CLR_GEN_CALL(dw_i2cd));
	}
#ifdef CONFIG_DW_I2C_SLV_RESTART_DET
	if (status & IC_INTR_RESTART_DET) {
		dw_i2c_dbg("dw_i2c: INTR_RESTART_DET\n");
	}
#endif
	if (status & IC_INTR_MASTER_ON_HOLD) {
		dw_i2c_dbg("dw_i2c: INTR_MASTER_ON_HOLD\n");
	}
#ifdef CONFIG_DW_I2C_BUS_CLEAR
	if (status & IC_INTR_SCL_STUCK_AT_LOW) {
		dw_i2c_dbg("dw_i2c: INTR_SCL_STU_AT_LOW\n");
	}
#endif
}

#ifndef SYS_REALTIME
static void dw_i2c_irq_handler(irq_t irq)
{
	dw_i2c_master_select(irq - IRQ_I2C0);
	dw_i2c_handle_irq();
	irqc_ack_irq(irq);
}

void dw_i2c_irq_init(void)
{
	irqc_configure_irq(IRQ_I2C0 + dw_i2cd, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_I2C0 + dw_i2cd, dw_i2c_irq_handler);
	irqc_enable_irq(IRQ_I2C0 + dw_i2cd);
}
#endif

/* Initialize controller as master device */
void __dw_i2c_master_init(void)
{
	dw_i2c_ctrl_disable();
	dw_i2c_writel(IC_CON_DEFAULT, IC_CON(dw_i2cd));
	dw_i2c_writel(0, IC_RX_TL(dw_i2cd));
	dw_i2c_writel(DW_I2C_TX_FIFO_SIZE - 1, IC_TX_TL(dw_i2cd));
	dw_i2c_writel(IC_INTR_DEF, IC_INTR_MASK(dw_i2cd));
	dw_i2c_ctrl_disable();

	dw_i2c_dbg("dw_i2c: DW_I2C_DRIVER_INIT\n");
	dw_i2c.state = DW_I2C_DRIVER_INIT;
}

void dw_i2c_master_init(void)
{
	dw_i2c.addr_mode = 0;
	dw_i2c_dbg("dw_i2c: DW_I2C_DRIVER_INVALID\n");
	dw_i2c.state = DW_I2C_DRIVER_INVALID;
	__dw_i2c_master_init();
}
