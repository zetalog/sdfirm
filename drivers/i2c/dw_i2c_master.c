#include <target/i2c.h>
#include <target/clk.h>
#include <target/console.h>
#include <target/irq.h>

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
#define dw_i2cd			0
#define dw_i2c_master_select(i2c)	do { } while (0)

static struct dw_i2c_ctx dw_i2c;
#endif

static __inline uint32_t dw_i2c_get_status(void)
{
	return __raw_readl(IC_STATUS(dw_i2cd));
}

static void dw_i2c_ctrl_abort(void)
{
	__raw_writel(IC_ENABLE_ABORT, IC_ENABLE(dw_i2cd));
	while (__raw_readl(IC_ENABLE(dw_i2cd)) & IC_ENABLE_ABORT);
}

/* Slave configuration */
void dw_i2c_set_address(i2c_addr_t addr, boolean call)
{
	dw_i2c_ctrl_disable();
	__raw_writel((uint32_t)addr, IC_SAR(dw_i2cd));
	dw_i2c_ctrl_enable();
}

/* Master configuration */
void dw_i2c_set_frequency(uint16_t khz)
{
	uint32_t cntl;
	uint32_t hcnt, lcnt;
	int i2c_spd;
	int speed = khz * 1000;
	int bus_speed = DW_I2C_FREQ;
#ifndef CONFIG_DW_I2C_USE_COUNTS
	int bus_mhz = bus_speed / 1000 / 1000;
#endif

	if (speed >= I2C_MAX_SPEED)
		i2c_spd = IC_CON_SPEED_MAX;
	else if (speed >= I2C_FAST_SPEED)
		i2c_spd = IC_CON_SPEED_FAST;
	else
		i2c_spd = IC_CON_SPEED_STD;

	dw_i2c_ctrl_disable();
	cntl = __raw_readl(IC_CON(dw_i2cd));

	cntl |= IC_CON_SPEED(i2c_spd);
#ifdef CONFIG_DW_I2C_USE_COUNTS
	switch (i2c_spd) {
	case IC_CON_SPEED_HIGH:
		hcnt = bus_speed / speed / (6+16) * 6 - (1+7);
		lcnt = bus_speed / speed / (6+16) * 16 - 1;
		lcnt = hcnt * 2;
		__raw_writel(hcnt, IC_HS_SCL_HCNT(dw_i2cd));
		__raw_writel(lcnt, IC_HS_SCL_LCNT(dw_i2cd));
		break;
	case IC_CON_SPEED_STD:
		hcnt = bus_speed / speed / (40+47) * 40 - (5+7);
		lcnt = bus_speed / speed / (40+47) * 47 - 1;
		__raw_writel(hcnt, IC_SS_SCL_HCNT(dw_i2cd));
		__raw_writel(lcnt, IC_SS_SCL_LCNT(dw_i2cd));
		break;
	case IC_CON_SPEED_FAST:
	default:
		hcnt = bus_speed / speed / (6+13) * 6 - (5+7);
		lcnt = bus_speed / speed / (6+13) * 13 - 1;
		__raw_writel(hcnt, IC_FS_SCL_HCNT(dw_i2cd));
		__raw_writel(lcnt, IC_FS_SCL_LCNT(dw_i2cd));
		break;
	}
#else
	switch (i2c_spd) {
	case IC_CON_SPEED_HIGH:
		hcnt = (bus_mhz * MIN_HS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_HS_SCL_LOWTIME) / 1000;
		__raw_writel(hcnt, IC_HS_SCL_HCNT(dw_i2cd));
		__raw_writel(lcnt, IC_HS_SCL_LCNT(dw_i2cd));
		break;
	case IC_CON_SPEED_STD:
		hcnt = (bus_mhz * MIN_SS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_SS_SCL_LOWTIME) / 1000;
		__raw_writel(hcnt, IC_SS_SCL_HCNT(dw_i2cd));
		__raw_writel(lcnt, IC_SS_SCL_LCNT(dw_i2cd));
		break;
	case IC_CON_SPEED_FAST:
	default:
		hcnt = (bus_mhz * MIN_FS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_FS_SCL_LOWTIME) / 1000;
		__raw_writel(hcnt, IC_FS_SCL_HCNT(dw_i2cd));
		__raw_writel(lcnt, IC_FS_SCL_LCNT(dw_i2cd));
		break;
	}
#endif
	__raw_writel(cntl, IC_CON(dw_i2cd));
	dw_i2c_ctrl_enable();
}

#ifdef CONFIG_DW_I2C_DYNAMIC_TAR_UPDATE
static void dw_i2c_update_target(i2c_addr_t addr)
{
	__raw_writel(addr, IC_TAR(dw_i2cd));
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
	__raw_writel(addr, IC_TAR(dw_i2cd));
	dw_i2c_ctrl_enable();
}

#define dw_i2c_set_target(addr)		dw_i2c_update_target(addr)
#endif

static void dw_i2c_flush_rxfifo(void)
{
	while (dw_i2c_get_status() & IC_STATUS_RFNE);
}

static int i2c_tx_finish(void)
{
	uint32_t val;

	while (1) {
		val = dw_i2c_get_status();
		if (val & IC_STATUS_TFE)
			break;

		val = __raw_readl(IC_RAW_INTR_STAT(dw_i2cd));
		if (val & IC_INTR_STOP_DET) {
			val = __raw_readl(IC_CLR_STOP_DET(dw_i2cd));
			break;
		}
	}
	return 0;
}

static void dw_i2c_translate_status(void)
{
	uint32_t irqs = __raw_readl(IC_RAW_INTR_STAT(dw_i2cd));
	uint32_t abrt;

	con_dbg("dw_i2c: irqs=%08x\n", irqs);

	if (irqs & IC_INTR_TX_ABRT) {
		abrt = __raw_readl(IC_TX_ABRT_SOURCE(dw_i2cd));
		if (abrt & IC_TX_ABRT_SOURCE_NOACK)
			i2c_set_status(I2C_STATUS_NACK);
		else
			i2c_set_status(I2C_STATUS_ARBI);
	} else if (irqs & IC_INTR_STOP_DET)
		i2c_set_status(I2C_STATUS_STOP);
	else if (irqs & IC_INTR_ACTIVITY)
		i2c_set_status(I2C_STATUS_ACK);
	else if (irqs & IC_INTR_START_DET ||
		 irqs & IC_INTR_RESTART_DET) {
		if (i2c_bus_mode() != I2C_MODE_MASTER)
			i2c_set_status(I2C_STATUS_ARBI);
		else
			i2c_set_status(I2C_STATUS_START);
	} else if (irqs & IC_INTR_TX_EMPTY ||
		   irqs & IC_INTR_RX_FULL) {
		i2c_set_status(I2C_STATUS_ACK);
	} else if (irqs & IC_INTR_TX_OVER ||
		   irqs & IC_INTR_RX_UNDER) {
		i2c_set_status(I2C_STATUS_NACK);
	} else
		i2c_set_status(I2C_STATUS_ACK);
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
	uint32_t val = dw_i2c.last_tx_byte;

#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
	val |= IC_DATA_CMD_STOP;
#endif
	__raw_writel(val, IC_DATA_CMD(dw_i2cd));
	i2c_tx_finish();
	dw_i2c_translate_status();
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
		dw_i2c.state = DW_I2C_DRIVER_DEVID_TAR;
		return true;
	case DW_I2C_DRIVER_DEVID_TAR:
		/* Process 3rd byte:
		 * Push 3 read commands into IC_DATA_CMD.
		 */
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

void dw_i2c_write_byte(uint8_t byte)
{
	uint32_t val;

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
		val = dw_i2c.addr_mode >> 1;
		dw_i2c_set_target(val);
		if (dw_i2c.addr_mode & I2C_DIR_MASK) {
			val = IC_DATA_CMD_CMD;
			__raw_writel(val, IC_DATA_CMD(dw_i2cd));
			dw_i2c.state = DW_I2C_DRIVER_DATA;
		} else
			dw_i2c.state = DW_I2C_DRIVER_TX;
		dw_i2c_translate_status();
		return;
	}

	if (dw_i2c.state == DW_I2C_DRIVER_TX) {
		dw_i2c.last_tx_byte = byte;
		dw_i2c.state = DW_I2C_DRIVER_DATA;
		dw_i2c_translate_status();
		return;
	}

	val = (uint32_t)dw_i2c.last_tx_byte;
	dw_i2c.last_tx_byte = byte;
	while (!(dw_i2c_get_status() & IC_STATUS_TFNF));
	__raw_writel(val, IC_DATA_CMD(dw_i2cd));
	dw_i2c_translate_status();
}

uint8_t dw_i2c_read_byte(void)
{
	uint8_t byte;
	uint32_t val = IC_DATA_CMD_CMD;

#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
	if (i2c_last_byte())
		val |= IC_DATA_CMD_STOP;
#endif
	__raw_writel(val, IC_DATA_CMD(dw_i2cd));
	while (!(dw_i2c_get_status() & IC_STATUS_RFNE));
	dw_i2c_translate_status();
	byte = (uint8_t)__raw_readl(IC_DATA_CMD(dw_i2cd));
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
	dw_i2c.state = DW_I2C_DRIVER_INIT;
}

void dw_i2c_handle_irq(void)
{
	uint32_t status;
	uint32_t enable;
	i2c_addr_t abrt_src;

	enable = __raw_readl(IC_ENABLE(dw_i2cd));
	status = __raw_readl(IC_RAW_INTR_STAT(dw_i2cd));
	if (!enable || !(status & ~IC_INTR_ACTIVITY))
		return;

	if (status & IC_INTR_RX_UNDER)
		__raw_readl(IC_CLR_RX_UNDER(dw_i2cd));
	if (status & IC_INTR_RX_OVER)
		__raw_readl(IC_CLR_RX_OVER(dw_i2cd));
	if (status & IC_INTR_TX_OVER)
		__raw_readl(IC_CLR_TX_OVER(dw_i2cd));
	if (status & IC_INTR_RD_REQ)
		__raw_readl(IC_CLR_RD_REQ(dw_i2cd));
	if (status & IC_INTR_TX_ABRT) {
		abrt_src = (i2c_addr_t)__raw_readl(IC_TX_ABRT_SOURCE(dw_i2cd));
		__raw_readl(IC_CLR_TX_ABRT(dw_i2cd));
		i2c_master_abort(abrt_src);
	}
	if (status & IC_INTR_RX_DONE)
		__raw_readl(IC_CLR_RX_DONE(dw_i2cd));
	if (status & IC_INTR_ACTIVITY)
		__raw_readl(IC_CLR_ACTIVITY(dw_i2cd));
	if (status & IC_INTR_STOP_DET &&
	    (i2c_last_byte() || (status & IC_INTR_RX_FULL))) {
		__raw_readl(IC_CLR_STOP_DET(dw_i2cd));
		i2c_master_stop();
	}
	if (status & IC_INTR_START_DET) {
		__raw_readl(IC_CLR_START_DET(dw_i2cd));
		i2c_master_start();
	}
	if (status & IC_INTR_GEN_CALL)
		__raw_readl(IC_CLR_GEN_CALL(dw_i2cd));
}

#ifndef SYS_REALTIME
static void dw_i2c_irq_handler(irq_t irq)
{
	dw_i2c_master_select(irq - IRQ_I2C0);
	dw_i2c_handle_irq();
}

void dw_i2c_irq_init(void)
{
	irqc_configure_irq(IRQ_I2C0 + dw_i2cd, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_I2C0 + dw_i2cd, dw_i2c_irq_handler);
	irqc_enable_irq(IRQ_I2C0 + dw_i2cd);
}
#endif

void dw_i2c_read_bytes(uint8_t *buf, i2c_len_t len)
{
}

void dw_i2c_write_bytes(uint8_t *buf, i2c_len_t len)
{
}

/* Initialize controller as master device */
void __dw_i2c_master_init(void)
{
	dw_i2c_ctrl_disable();
	__raw_writel(IC_CON_SLAVE_DISABLE | IC_CON_RESTART_EN |
		     IC_CON_SPEED(IC_CON_SPEED_STD) | IC_CON_MASTER_MODE,
		     IC_CON(dw_i2cd));
	__raw_writel(DW_I2C_RX_FIFO_SIZE - 1, IC_RX_TL(dw_i2cd));
	__raw_writel(0, IC_TX_TL(dw_i2cd));
	__raw_writel(IC_INTR_ALL, IC_INTR_MASK(dw_i2cd));
	dw_i2c_ctrl_disable();

	dw_i2c.state = DW_I2C_DRIVER_INIT;
}

void dw_i2c_master_init(void)
{
	dw_i2c.addr_mode = 0;
	dw_i2c.state = DW_I2C_DRIVER_INVALID;
	__dw_i2c_master_init();
}
