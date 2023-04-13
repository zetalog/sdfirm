#include <target/i2c.h>
#include <target/clk.h>
#include <target/console.h>
#include <target/irq.h>

#if NR_DW_I2CS > 1
#define dw_i2c				dw_i2cs[dw_i2c_did]

static int dw_i2c_did = 0;
static struct dw_i2c_ctx dw_i2cs[NR_DW_I2CS] = {0};

void dw_i2c_master_select(i2c_t i2c)
{
	BUG_ON(i2c >= NR_DW_I2CS);

	dw_i2c_did = i2c;
}
#else
#define dw_i2c_did			0
#define dw_i2c_master_select(i2c)	do { } while (0)

static struct dw_i2c_ctx dw_i2c;
#endif

#ifdef CONFIG_DW_I2C_TEST_IRQ
/* Give RX_FULL interrupt when get 1 or more entry in RX FIFO. */
#ifdef CONFIG_DW_I2C_RX_TL
#undef CONFIG_DW_I2C_RX_TL
#define CONFIG_DW_I2C_RX_TL 0
#endif

static unsigned int irq_test_flag = 0;
static irq_flags_t irq_flags;

/* Interrupts cared about:
 *	- RX_FULL : auto cleared
 */
void dw_i2c_test_irq(irq_t irq)
{
	/* The IRQ handler do not automatically mask irq for hardware, as
	 * there is no return value indicated by the handler.
	 */
	irqc_mask_irq(IRQ_I2C0 + dw_i2c_did);
	irq_test_flag = 1 + dw_i2c_did;
}

static dw_i2c_test_loop(void)
{
	while (irq_test_flag == 0) {
		irq_local_save(irq_flags);
		irq_local_enable();
		irq_local_disable();
		irq_local_restore(irq_flags);
	}
	irq_test_flag = 0;
	irqc_ack_irq(IRQ_I2C0 + dw_i2c_did);
	irqc_unmask_irq(IRQ_I2C0 + dw_i2c_did);
}
#else
#define dw_i2c_test_irq(irq)		do { } while (0)
#define dw_i2c_test_loop()		do { } while (0)
#endif

/* Initialize controller as master device */
void __dw_i2c_master_init(void)
{
	__raw_writel(0, IC_ENABLE(dw_i2c_did));
	__raw_writel(IC_CON_SLAVE_DISABLE | IC_CON_RESTART_EN |
		     IC_CON_SPEED(IC_CON_SPEED_STD) | IC_CON_MASTER_MODE,
		     IC_CON(dw_i2c_did));
	__raw_writel(CONFIG_DW_I2C_RX_TL, IC_RX_TL(dw_i2c_did));
	__raw_writel(CONFIG_DW_I2C_TX_TL, IC_TX_TL(dw_i2c_did));
#ifdef CONFIG_DW_I2C_TEST_IRQ
	__raw_writel(IC_INTR_RX_FULL, IC_INTR_MASK(dw_i2c_did));
#else
	__raw_writel(IC_INTR_ALL, IC_INTR_MASK(dw_i2c_did));
#endif
	__raw_writel(1, IC_ENABLE(dw_i2c_did));

	dw_i2c.state = DW_I2C_DRIVER_INIT;
}

/*
 * Set address when this controller acts as a slave device
 */
void dw_i2c_set_address(i2c_addr_t addr, boolean call)
{
	__raw_writel(0, IC_ENABLE(dw_i2c_did));
	__raw_writel((uint32_t)addr, IC_SAR(dw_i2c_did));
	__raw_writel(1, IC_ENABLE(dw_i2c_did));
}

/*
 * Set frequency by setting speed mode and relative SCL count
 */
void dw_i2c_set_frequency(uint16_t khz)
{
	uint32_t cntl;
	uint32_t hcnt, lcnt;
	uint32_t ena;
	int i2c_spd;
	int speed = khz * 1000;
	int bus_speed = DW_I2C_FREQ;
#ifndef CONFIG_DW_I2C_USE_COUNT
	int bus_mhz = bus_speed / 1000 / 1000;
#endif

	if (speed >= I2C_MAX_SPEED)
		i2c_spd = IC_CON_SPEED_MAX;
	else if (speed >= I2C_FAST_SPEED)
		i2c_spd = IC_CON_SPEED_FAST;
	else
		i2c_spd = IC_CON_SPEED_STD;

	ena = __raw_readl(IC_ENABLE(dw_i2c_did));
	__raw_writel(0, IC_ENABLE(dw_i2c_did));
	cntl = __raw_readl(IC_CON(dw_i2c_did));

	cntl |= IC_CON_SPEED(i2c_spd);
#ifdef CONFIG_DW_I2C_USE_COUNT
	switch (i2c_spd) {
	case IC_CON_SPEED_HIGH:
		hcnt = bus_speed / speed / (6+16) * 6 - (1+7);
		lcnt = bus_speed / speed / (6+16) * 16 - 1;
		lcnt = hcnt * 2;
		__raw_writel(hcnt, IC_HS_SCL_HCNT(dw_i2c_did));
		__raw_writel(lcnt, IC_HS_SCL_LCNT(dw_i2c_did));
		break;
	case IC_CON_SPEED_STD:
		hcnt = bus_speed / speed / (40+47) * 40 - (5+7);
		lcnt = bus_speed / speed / (40+47) * 47 - 1;
		__raw_writel(hcnt, IC_SS_SCL_HCNT(dw_i2c_did));
		__raw_writel(lcnt, IC_SS_SCL_LCNT(dw_i2c_did));
		break;
	case IC_CON_SPEED_FAST:
	default:
		hcnt = bus_speed / speed / (6+13) * 6 - (5+7);
		lcnt = bus_speed / speed / (6+13) * 13 - 1;
		__raw_writel(hcnt, IC_FS_SCL_HCNT(dw_i2c_did));
		__raw_writel(lcnt, IC_FS_SCL_LCNT(dw_i2c_did));
		break;
	}
#else
	switch (i2c_spd) {
	case IC_CON_SPEED_HIGH:
		hcnt = (bus_mhz * MIN_HS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_HS_SCL_LOWTIME) / 1000;
		__raw_writel(hcnt, IC_HS_SCL_HCNT(dw_i2c_did));
		__raw_writel(lcnt, IC_HS_SCL_LCNT(dw_i2c_did));
		break;
	case IC_CON_SPEED_STD:
		hcnt = (bus_mhz * MIN_SS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_SS_SCL_LOWTIME) / 1000;
		__raw_writel(hcnt, IC_SS_SCL_HCNT(dw_i2c_did));
		__raw_writel(lcnt, IC_SS_SCL_LCNT(dw_i2c_did));
		break;
	case IC_CON_SPEED_FAST:
	default:
		hcnt = (bus_mhz * MIN_FS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_FS_SCL_LOWTIME) / 1000;
		__raw_writel(hcnt, IC_FS_SCL_HCNT(dw_i2c_did));
		__raw_writel(lcnt, IC_FS_SCL_LCNT(dw_i2c_did));
		break;
	}
#endif
	__raw_writel(cntl, IC_CON(dw_i2c_did));

	/* Restore back i2c now speed set */
	if (ena == IC_ENABLE_ENABLE)
		__raw_writel(1, IC_ENABLE(dw_i2c_did));
}

static void i2c_setaddress(unsigned int i2c_addr)
{
	__raw_writel(0, IC_ENABLE(dw_i2c_did));
	__raw_writel(i2c_addr, IC_TAR(dw_i2c_did));
	__raw_writel(1, IC_ENABLE(dw_i2c_did));
}

static void i2c_flush_rxfifo(void)
{
	uint32_t val;

	do {
		val = __raw_readl(IC_STATUS(dw_i2c_did));
	} while (val & IC_STATUS_RFNE);
}

static int i2c_wait_for_bb(void)
{
	uint32_t val;

	do {
		val = __raw_readl(IC_STATUS(dw_i2c_did));
	} while (val & IC_STATUS_MST_ACTIVITY || !(val & IC_STATUS_TFE));
	return 0;
}

#if 0
static int i2c_xfer_init(unsigned char chip, unsigned int addr, int alen)
{
	if (i2c_wait_for_bb())
		return -EBUSY;

	i2c_setaddress(chip);
	while (alen) {
		alen--;
		/* High byte address going out first */
		__raw_writel((addr >> (alen * 8)) & 0xff,
			     IC_DATA_CMD(dw_i2c_did));
	}
	return 0;
}
#endif

static int i2c_rx_finish(void)
{
	uint32_t val;

	while (1) {
		val = __raw_readl(IC_RAW_INTR_STAT(dw_i2c_did));
		if (val & IC_INTR_STOP_DET) {
			val = __raw_readl(IC_CLR_STOP_DET(dw_i2c_did));
			break;
		}
	}

	if (i2c_wait_for_bb())
		return -EBUSY;
	i2c_flush_rxfifo();
	return 0;
}

static int i2c_tx_finish(void)
{
	uint32_t val;

	while (1) {
		val = __raw_readl(IC_STATUS(dw_i2c_did));
		if (val & IC_STATUS_TFE)
			break;

		val = __raw_readl(IC_RAW_INTR_STAT(dw_i2c_did));
		if (val & IC_INTR_STOP_DET) {
			val = __raw_readl(IC_CLR_STOP_DET(dw_i2c_did));
			break;
		}
	}

	if (i2c_wait_for_bb())
		return -EBUSY;
	return 0;
}

/*
 * Simple read and write for some bytes.
 */
int dw_i2c_read_bytes(uint8_t dev, uint8_t *buffer,
		      int len, unsigned int stop)
{
	unsigned int active = 0;
	uint32_t val;
	int orig_len = len;

	if (i2c_wait_for_bb())
		return -EBUSY;
	i2c_setaddress(dev);

	while (len) {
		if (!active) {
			/*
			 * Avoid writing to ic_cmd_data multiple times
			 * in case this loop spins too quickly and the
			 * ic_status RFNE bit isn't set after the first
			 * write. Subsequent writes to ic_cmd_data can
			 * trigger spurious i2c transfer.
			 */
			val = IC_DATA_CMD_CMD;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1 && stop != 0)
				val |= IC_DATA_CMD_STOP;
#endif
			__raw_writel(val, IC_DATA_CMD(dw_i2c_did));
			active = 1;
		}

		val = __raw_readl(IC_STATUS(dw_i2c_did));
		if (val & IC_STATUS_RFNE) {
			val = __raw_readl(IC_DATA_CMD(dw_i2c_did));
			*buffer++ = (uint8_t)val;
			len--;
			active = 0;
		}
		/* TODO Timeout. */
	}

	return i2c_rx_finish() == 0 ? orig_len - len : -1;
}

int dw_i2c_write_bytes(uint8_t dev, uint8_t *buffer,
		       int len, unsigned int stop)
{
	uint32_t val;
	int orig_len = len;

	if (i2c_wait_for_bb())
		return -EBUSY;
	i2c_setaddress(dev);

	while (len) {
		val = __raw_readl(IC_STATUS(dw_i2c_did));
		if (val & IC_STATUS_TFNF) {
			val = *buffer;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1 && stop != 0) {
				val = *buffer | IC_DATA_CMD_STOP;
			}
#endif
			__raw_writel(val, IC_DATA_CMD(dw_i2c_did));
			buffer++;
			len--;
		}
		/* TODO Timeout. */
	}

	return i2c_tx_finish() == 0 ? orig_len - len : -1;
}

/*
 * Do nothing at I2C bus here because START will be issued automatically
 * by the controller.
 * When operating as an I2C master, putting data into the transmit FIFO
 * causes the DW_apb_i2c to generate a START condition on the I2C bus.
 */
void dw_i2c_start_condition(void)
{
	dw_i2c.state = DW_I2C_DRIVER_START;
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
void dw_i2c_stop_condition(void)
{
	dw_i2c.state = DW_I2C_DRIVER_STOP;
}

void dw_i2c_write_byte(uint8_t byte)
{
	uint32_t val;

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
		dw_i2c.state == DW_I2C_DRIVER_START) {
		dw_i2c.state = DW_I2C_DRIVER_DEVID_START;
		return;
	}

	/* Process 2nd byte:
	 * Write 1 to IC_TAR[13] and IC_TAR[11] (along with target address)
	 * to enable a Device ID read between disable and enable.
	 */
	if (dw_i2c.state == DW_I2C_DRIVER_DEVID_START) {
		val = dw_i2c.addr_mode >> 1;
		val |= IC_TAR_DEVICE_ID;
		__raw_writel(0, IC_ENABLE(dw_i2c_did));
		__raw_writel(val, IC_TAR(dw_i2c_did));
		__raw_writel(1, IC_ENABLE(dw_i2c_did));
		dw_i2c.state = DW_I2C_DRIVER_DEVID_TAR;
		return;
	}
	/* Process 3rd byte:
	 * Push 3 read commands into IC_DATA_CMD.
	 */
	if (dw_i2c.state == DW_I2C_DRIVER_DEVID_TAR) {
		dw_i2c.state = DW_I2C_DRIVER_TRANS;
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
	 * If dynamically updating is not supported, the controller
	 * should be disabled first.
	 */
	if (dw_i2c.state == DW_I2C_DRIVER_START) {
		dw_i2c.addr_mode = byte;
		val = dw_i2c.addr_mode >> 1;
#ifndef CONFIG_DW_I2C_DYNAMIC_TAR_UPDATE
		__raw_writel(0, IC_ENABLE(dw_i2c_did));
#endif
		__raw_writel(val, IC_TAR(dw_i2c_did));
#ifndef CONFIG_DW_I2C_DYNAMIC_TAR_UPDATE
		__raw_writel(1, IC_ENABLE(dw_i2c_did));
#endif
		if (dw_i2c.addr_mode & I2C_DIR_MASK) { /* Read */
			/* XXX No STOP for Read. */
			val = IC_DATA_CMD_CMD;
			__raw_writel(val, IC_DATA_CMD(dw_i2c_did));
		}
		dw_i2c.state = DW_I2C_DRIVER_TRANS;
		return;
	}

	if (dw_i2c.state != DW_I2C_DRIVER_TRANS) {
		dw_i2c.state = DW_I2C_DRIVER_INVALID;
		return;
	}

	/* The following bytes are data for Tx. */
	val = (uint32_t)byte;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
	val |= IC_DATA_CMD_STOP;
#endif
	__raw_writel(val, IC_DATA_CMD(dw_i2c_did));
	/* When IC_TX_TL = 1 (set in _init()), TX_EMPTY will be reported as
	 * soon as this command is sent. */
	do {
		val = __raw_readl(IC_INTR_STAT(dw_i2c_did));
	} while (!(val & IC_INTR_TX_EMPTY));
}

uint8_t dw_i2c_read_byte(void)
{
	uint32_t val;

	if (dw_i2c.state != DW_I2C_DRIVER_TRANS) {
		dw_i2c.state = DW_I2C_DRIVER_INVALID;
		return 0xFF; // XXX Invalid value
	}
	/* When IC_RX_TL = 0 (set in _init()), RX_FULL will be reported as
	 * soon as one byte is received. */
	// TODO: Detect timeout
	do {
		val = __raw_readl(IC_INTR_STAT(dw_i2c_did));
	} while (!(val & IC_INTR_RX_FULL));
	val = __raw_readl(IC_DATA_CMD(dw_i2c_did));
	return (uint8_t)val;
}

/*
 * Abort transfer if any, and get ready for new start
 */
void dw_i2c_transfer_reset(void)
{
	uint32_t val;

	if (dw_i2c.state == DW_I2C_DRIVER_TRANS) {
		__raw_writel(IC_ENABLE_ABORT, IC_ENABLE(dw_i2c_did));
		do {
			val = __raw_readl(IC_INTR_STAT(dw_i2c_did));
		} while (!(val & IC_INTR_TX_ABRT));
	}

	/* Note: Do not identify the source as ABRT_USER_ABRT */

	/* Reset internal status of driver */
	dw_i2c.addr_mode = 0;
	dw_i2c.state = DW_I2C_DRIVER_INIT;
}

#ifndef CONFIG_SYS_NOIRQ
void dw_i2c_irq_handler(irq_t irq)
{
	dw_i2c_test_irq(irq);
}

void dw_i2c_irq_init(void)
{
	irqc_configure_irq(IRQ_I2C0 + dw_i2c_did, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_I2C0 + dw_i2c_did, dw_i2c_irq_handler);
	irqc_enable_irq(IRQ_I2C0 + dw_i2c_did);
}
#else
void dw_i2c_irq_init(void)
{
}
#endif

void dw_i2c_master_init(void)
{
	dw_i2c.addr_mode = 0;
	dw_i2c.state = DW_I2C_DRIVER_INVALID;
	__dw_i2c_master_init();
	dw_i2c_irq_init();
}
