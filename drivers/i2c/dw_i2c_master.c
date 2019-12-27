#include <target/i2c.h>
#include <driver/dw_i2c.h>

#define DW_I2C_DEBUG
#ifdef DW_I2C_DEBUG
#include <target/console.h>
#endif

enum dw_i2c_driver_state {
	DW_I2C_DRIVER_INIT = 0,
	DW_I2C_DRIVER_START,
	DW_I2C_DRIVER_ADDRESS,
	DW_I2C_DRIVER_TRANS,
	DW_I2C_DRIVER_STOP,
#ifdef CONFIG_I2C_DEVICE_ID
	DW_I2C_DRIVER_DEVID_START,
	DW_I2C_DRIVER_DEVID_TAR,
#endif

	DW_I2C_DRIVER_INVALID
};

struct dw_i2c_private {
	caddr_t base;
	uint8_t addr_mode;
	int state;
};

static struct dw_i2c_private dw_i2c0 = {
	.base = DW_I2C_BASE(0),
	.addr_mode = 0,
	.state = DW_I2C_DRIVER_INVALID,
};

static struct dw_i2c_private *dw_i2c_pri = &dw_i2c0;

/*
 * Initialize controller as master device
 */
void i2c_hw_ctrl_init(void)
{
	caddr_t base = dw_i2c_pri->base;
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	__raw_writel(0, base + IC_ENABLE);
	__raw_writel(IC_CON_SD /* IC_SLAVE_DISABLE */
				 | IC_CON_RE /* IC_RESTART_EN */
				 /* IC_10BITADDR_MASTER = 0 */
				 /* IC_10BITADDR_SLAVE = 0 */
				 | IC_CON_SPD_SS /* SEEP = Stand */
				 | IC_CON_MM /* IC_MASTER_MODE */
				 , base + IC_CON);
	__raw_writel(0, base + IC_RX_TL); /* 0 sets the threshold for 1 entry */
	__raw_writel(1, base + IC_TX_TL); /* 1 sets the threshold for 1 entry */
	__raw_writel(IC_INTR_ALL, base + IC_INTR_MASK);
	__raw_writel(1, base + IC_ENABLE);
	dw_i2c_pri->state = DW_I2C_DRIVER_INIT;
	return;
}

/*
 * Set address when this controller acts as a slave device
 */
void i2c_hw_set_address(i2c_addr_t addr, boolean call)
{
	caddr_t base = dw_i2c_pri->base;
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s. addr = 0x%x\n", __func__, addr);
#endif
	__raw_writel(0, base + IC_ENABLE);
	__raw_writel((uint32_t)addr, base + IC_SAR);
	__raw_writel(1, base + IC_ENABLE);
	return;
}

/*
 * Set frequency by setting speed mode and relative SCL count
 */
#ifndef CONFIG_DW_I2C_CLK
#define BUS_CLK (125000 * 1000)
#else
#define BUS_CLK (CONFIG_DW_I2C_CLK * 1000)
#endif
void i2c_hw_set_frequency(uint16_t khz)
{
	caddr_t base = dw_i2c_pri->base;
	uint32_t cntl;
	uint32_t hcnt, lcnt;
	uint32_t ena;
	int i2c_spd;
	int speed = khz * 1000;
	int bus_mhz = DW_I2C_FREQ;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s. khz = %d\n", __func__, khz);
#endif
	if (speed >= I2C_MAX_SPEED)
		i2c_spd = IC_SPEED_MODE_MAX;
	else if (speed >= I2C_FAST_SPEED)
		i2c_spd = IC_SPEED_MODE_FAST;
	else
		i2c_spd = IC_SPEED_MODE_STANDARD;

	ena = __raw_readl(base + IC_ENABLE);
	__raw_writel(0, base + IC_ENABLE);

	cntl = __raw_readl(base + IC_CON) & (~IC_CON_SPD_MSK);

	switch (i2c_spd) {
	case IC_SPEED_MODE_MAX:
		cntl |= IC_CON_SPD_HS;
		hcnt = (bus_mhz * MIN_HS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_HS_SCL_LOWTIME) / 1000;
		__raw_writel(hcnt, base + IC_HS_SCL_HCNT);
		__raw_writel(lcnt, base + IC_HS_SCL_LCNT);
		break;

	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;
		hcnt = (bus_mhz * MIN_SS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_SS_SCL_LOWTIME) / 1000;
		__raw_writel(hcnt, base + IC_SS_SCL_HCNT);
		__raw_writel(lcnt, base + IC_SS_SCL_LCNT);
		break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
		hcnt = (bus_mhz * MIN_FS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_FS_SCL_LOWTIME) / 1000;
		__raw_writel(hcnt, base + IC_FS_SCL_HCNT);
		__raw_writel(lcnt, base + IC_FS_SCL_LCNT);
		break;
	}

	__raw_writel(cntl, base + IC_CON);

	/* Restore back i2c now speed set */
	if (ena == IC_ENABLE_0B)
		__raw_writel(1, base + IC_ENABLE);

	return;
}

/*
 * Do nothing at I2C bus here because START will be issued automatically
 * by the controller.
 * When operating as an I2C master, putting data into the transmit FIFO
 * causes the DW_apb_i2c to generate a START condition on the I2C bus.
 */
void i2c_hw_start_condition(void)
{
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	dw_i2c_pri->state = DW_I2C_DRIVER_START;
	return;
}

/*
 * Do nothing I2C bus here because STOP will be issued automatically by the
 * controller.
 * If the IC_EMPTYFIFO_HOLD_MASTER_EN parameteris set to 0, allowing the
 * transmit FIFO to empty causes the DW_apb_i2c to generate a STOP
 * condition on the I2C bus.
 * If IC_EMPTYFIFO_HOLD_MASTER_EN is set to 1, then writing a 1 to
 * IC_DATA_CMD[9] causes the DW_apb_i2c to generate a STOP condition on the
 * I2C bus; a STOP condition is not issued if this bit is not set, even if
 * the transmit FIFO is empty.
 */
void i2c_hw_stop_condition(void)
{
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	dw_i2c_pri->state = DW_I2C_DRIVER_STOP;
	return;
}

void i2c_hw_write_byte(uint8_t byte)
{
	caddr_t base = dw_i2c_pri->base;
	uint32_t val;
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif

#ifdef CONFIG_I2C_DEVICE_ID
	/*
	 * Special case: to probe device ID.
	 *   START +
	 *   1st byte: I2C_ADDR_DEVID << 7 | I2C_MODE_TX
	 *   2nd byte: target_address << 7 | I2C_DIR
	 *   3rd byte: target_address << 7 | I2C_MODE_RX
	 */
	/* Process 1st byte: Detect this case. */
	if (byte == (I2C_ADDR_DEVID << 7 | I2C_MODE_TX) &&
		dw_i2c_pri->state == DW_I2C_DRIVER_START) {
		dw_i2c_pri->state = DW_I2C_DRIVER_DEVID_START;
		return;
	}

	/* Process 2nd byte:
	   Write 1 to IC_TAR[13] and IC_TAR[11] (along with target address) to
	   enable a Device ID read between disable and enable. */
	if (dw_i2c_pri->state == DW_I2C_DRIVER_DEVID_START) {
		val = dw_i2c_pri->addr_mode >> 1;
		val |= TAR_DEVID;
		__raw_writel(0, base + IC_ENABLE);
		__raw_writel(val, base + IC_TAR);
		__raw_writel(1, base + IC_ENABLE);
		dw_i2c_pri->state = DW_I2C_DRIVER_DEVID_TAR;
		return;
	}
	/* Process 3rd byte:
	   Push 3 read commands into IC_DATA_CMD. */
	if (dw_i2c_pri->state == DW_I2C_DRIVER_DEVID_TAR) {
		dw_i2c_pri->state = DW_I2C_DRIVER_TRANS;
		return;
	}
#endif

	/*
	 * Normal case:
	 *   START +
	 *   1st byte: target_address << 7 | I2C_DIR
	 *   following: data
	 */
	/* The 1st byte after START is target address and direction. 
	   If dynamically updating is not supported, the controller
	   should be disabled first. */
	if (dw_i2c_pri->state == DW_I2C_DRIVER_START) {
		dw_i2c_pri->addr_mode = byte;
		val = dw_i2c_pri->addr_mode >> 1;
#ifndef CONFIG_DW_I2C_DYNAMIC_TAR_UPDATE
		__raw_writel(0, base + IC_ENABLE);
#endif
		__raw_writel(val, base + IC_TAR);
#ifndef CONFIG_DW_I2C_DYNAMIC_TAR_UPDATE
		__raw_writel(1, base + IC_ENABLE);
#endif
		if (dw_i2c_pri->addr_mode & I2C_DIR_MASK) { /* Read */
			/* XXX No STOP for Read. */
			val = IC_CMD;
			__raw_writel(val, base + IC_DATA_CMD);
		}
		dw_i2c_pri->state = DW_I2C_DRIVER_TRANS;
		return;
	}

	if (dw_i2c_pri->state != DW_I2C_DRIVER_TRANS) {
#ifdef DW_I2C_DEBUG
		con_printf("Debug: Error state = %d in %s\n",
			  		__func__, dw_i2c_pri->state);
#endif
		dw_i2c_pri->state = DW_I2C_DRIVER_INVALID;
		return;
	}

	/* The following bytes are data for Tx. */
	val = (uint32_t)byte;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
	val |= IC_STOP;
#endif
	__raw_writel(val, base + IC_DATA_CMD);
	/* When IC_TX_TL = 1 (set in _init()), TX_EMPTY will be reported as
	 * soon as this command is sent. */
	do {
		val = __raw_readl(base + IC_INTR_STAT);
	} while (!(val & IC_TX_EMPTY));

	return;
}

uint8_t i2c_hw_read_byte(void)
{
	caddr_t base = dw_i2c_pri->base;
	uint32_t val;
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	if (dw_i2c_pri->state != DW_I2C_DRIVER_TRANS) {
#ifdef DW_I2C_DEBUG
		con_printf("Debug: Error state = %d in %s\n",
					__func__, dw_i2c_pri->state);
#endif
		dw_i2c_pri->state = DW_I2C_DRIVER_INVALID;
		return 0xFF; // XXX Invalid value
	}
	/* When IC_RX_TL = 0 (set in _init()), RX_FULL will be reported as
	 * soon as one byte is received. */
	// TODO: Detect timeout
	do {
		val = __raw_readl(base + IC_INTR_STAT);
	} while (!(val & IC_RX_FULL));
	val = __raw_readl(base + IC_DATA_CMD);
	return (uint8_t)val;
}

/*
 * Abort transfer if any, and get ready for new start
 */
void i2c_hw_transfer_reset(void)
{
	caddr_t base = dw_i2c_pri->base;
	uint32_t val;
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	__raw_writel(IC_ENABLE_ABRT, base + IC_ENABLE);
	do {
		val = __raw_readl(base + IC_INTR_STAT);
	} while (!(val & IC_TX_ABRT));

	/* Note: Do not identify the source as ABRT_USER_ABRT */

	/* Reset internal status of driver */
	dw_i2c_pri->addr_mode = 0;
	dw_i2c_pri->state = DW_I2C_DRIVER_INIT;
	return;
}
