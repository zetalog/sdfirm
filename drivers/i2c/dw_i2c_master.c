#include <target/clk.h>
#include <target/i2c.h>
#include <target/console.h>
#include <driver/dw_i2c.h>

#define DW_I2C_DEBUG

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

static struct dw_i2c_private dw_i2c_masters[CONFIG_I2C_MAX_MASTERS] = {0};

static struct dw_i2c_private *dw_i2c_pri = NULL;

#ifdef CONFIG_DW_I2C_TEST_IRQ
/* Give RX_FULL interrupt when get 1 or more entry in RX FIFO. */
#ifdef CONFIG_DW_I2C_RX_TL
#undef CONFIG_DW_I2C_RX_TL
#define CONFIG_DW_I2C_RX_TL 0
#endif
#include <target/irq.h>
static unsigned int irq_test_master_num = 0;
static unsigned int irq_test_flag = 0;
static irq_flags_t irq_flags;
/* Interrupts cared about:
 *	- RX_FULL : auto cleared
 */
void dw_i2c_irq_handler(void)
{
	/* The IRQ handler do not automatically mask irq for hardware, as
	 * there is no return value indicated by the handler.
	 */
	irqc_mask_irq(IRQ_I2C0 + irq_test_master_num);
	irq_test_flag = 1 + irq_test_master_num;
}
#endif

void i2c_hw_init(void)
{
	int i;
	for (i = 0; i < CONFIG_I2C_MAX_MASTERS; i++) {
		dw_i2c_masters[i].base = DW_I2C_BASE(i);
		dw_i2c_masters[i].addr_mode = 0;
		dw_i2c_masters[i].state = DW_I2C_DRIVER_INVALID;
	}
#ifdef CONFIG_DW_I2C_TEST_IRQ
	for (i = 0; i < CONFIG_I2C_MAX_MASTERS; i++) {
		irqc_configure_irq(IRQ_I2C0 + i, 0, IRQ_LEVEL_TRIGGERED);
	}
	for (i = 0; i < CONFIG_I2C_MAX_MASTERS; i++) {
		irq_register_vector(IRQ_I2C0 + i, dw_i2c_irq_handler);
	}
	for (i = 0; i < CONFIG_I2C_MAX_MASTERS; i++) {
		irqc_enable_irq(IRQ_I2C0 + i);
	}
#endif
#ifdef CONFIG_ARCH_DPU
	clk_enable(srst_i2c0);
	clk_enable(srst_i2c1);
	clk_enable(srst_i2c2);
#endif
}

int i2c_hw_master_select_by_num(unsigned int num)
{
	if (num >= CONFIG_I2C_MAX_MASTERS) {
		con_printf("Error: Invalid I2C num = %d\n", num);
		return -1;
	}
	dw_i2c_pri = dw_i2c_masters + num;
#ifdef CONFIG_DW_I2C_TEST_IRQ
	irq_test_master_num = num;
#endif
	return 0;
}

/*
 * Initialize controller as master device
 */
void i2c_hw_ctrl_init(void)
{
	caddr_t base = dw_i2c_pri->base;

	uint32_t val;
	uint32_t offset;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif

	offset = IC_ENABLE;
	val = 0;
#ifdef DW_I2C_DEBUG
	con_printf("w 0x%2x 0x%x _ENABLE\n", offset, val);
#endif
	__raw_writel(val, base + offset);

#ifdef DW_I2C_DEBUG
	offset = IC_CON; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _CON\n", offset, val);
	offset = IC_RX_TL; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _RX_TL\n", offset, val);
	offset = IC_TX_TL; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _TX_TL\n", offset, val);
	offset = IC_INTR_MASK; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _INTR_MASK\n", offset, val);
	offset = IC_SS_SCL_HCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _SS_SCL_HCNT\n", offset, val);
	offset = IC_SS_SCL_LCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _SS_SCL_LCNT\n", offset, val);
	offset = IC_HS_SCL_HCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _HS_SCL_HCNT\n", offset, val);
	offset = IC_HS_SCL_LCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _HS_SCL_LCNT\n", offset, val);
	offset = IC_FS_SCL_HCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _FS_SCL_HCNT\n", offset, val);
	offset = IC_FS_SCL_LCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _FS_SCL_LCNT\n", offset, val);
#endif

	offset = IC_CON;
	val = IC_CON_SD /* IC_SLAVE_DISABLE */
		| IC_CON_RE /* IC_RESTART_EN */
		/* IC_10BITADDR_MASTER = 0 */
		/* IC_10BITADDR_SLAVE = 0 */
		| IC_CON_SPD_SS /* SEEP = Stand */
		| IC_CON_MM /* IC_MASTER_MODE */
		;
#ifdef DW_I2C_DEBUG
	con_printf("w 0x%2x 0x%x _CON\n", offset, val);
#endif
	__raw_writel(val, base + offset);

	offset = IC_RX_TL;
	val = CONFIG_DW_I2C_RX_TL;
#ifdef DW_I2C_DEBUG
	con_printf("w 0x%2x 0x%x _RX_TL\n", offset, val);
#endif
	__raw_writel(val, base + offset);

	offset = IC_TX_TL;
	val = CONFIG_DW_I2C_TX_TL;
#ifdef DW_I2C_DEBUG
	con_printf("w 0x%2x 0x%x _TX_TL\n", offset, val);
#endif
	__raw_writel(val, base + offset);

	offset = IC_INTR_MASK;
#ifdef CONFIG_DW_I2C_TEST_IRQ
	val = IC_RX_FULL;
#else
	val = IC_INTR_ALL;
#endif
#ifdef DW_I2C_DEBUG
	con_printf("w 0x%2x 0x%x _INTR_MASK\n", offset, val);
#endif
	__raw_writel(val, base + offset);

	offset = IC_ENABLE;
	val = 1;
#ifdef DW_I2C_DEBUG
	con_printf("w 0x%2x 0x%x _ENABLE\n", offset, val);
#endif
	__raw_writel(val, base + offset);

	dw_i2c_pri->state = DW_I2C_DRIVER_INIT;
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
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
void i2c_hw_set_frequency(uint16_t khz)
{
	caddr_t base = dw_i2c_pri->base;
	uint32_t cntl;
	uint32_t hcnt, lcnt;
	uint32_t ena;
	int i2c_spd;
	int speed = khz * 1000;
	int bus_speed = DW_I2C_FREQ;
#ifndef CONFIG_DW_I2C_USE_COUNT
	int bus_mhz = bus_speed / 1000 / 1000;
#endif

	uint32_t val;
	uint32_t offset;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s. bus speed = %ld Hz, i2c seep = %d kHz\n", __func__, bus_speed, khz);
#endif
	if (speed >= I2C_MAX_SPEED)
		i2c_spd = IC_SPEED_MODE_MAX;
	else if (speed >= I2C_FAST_SPEED)
		i2c_spd = IC_SPEED_MODE_FAST;
	else
		i2c_spd = IC_SPEED_MODE_STANDARD;

	offset = IC_ENABLE;
	val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
	con_printf("r 0x%2x 0x%x _ENABLE\n", offset, val);
#endif
	ena = val;

	val = 0;
#ifdef DW_I2C_DEBUG
	con_printf("w 0x%2x 0x%x _ENABLE\n", offset, val);
#endif
	__raw_writel(val, base + offset);

	offset = IC_CON;
	val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
	con_printf("r 0x%2x 0x%x _CON\n", offset, val);
#endif
	cntl = val;

#ifdef CONFIG_DW_I2C_USE_COUNT
	switch (i2c_spd) {
	case IC_SPEED_MODE_HIGH:
		cntl |= IC_CON_SPD_HS;
		hcnt = bus_speed / speed / (6+16) * 6 - (1+7);
		lcnt = bus_speed / speed / (6+16) * 16 - 1;
		lcnt = hcnt * 2;
#ifdef DW_I2C_DEBUG
		offset = IC_HS_SCL_HCNT; val = hcnt;
		con_printf("w 0x%2x 0x%x _HS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_HS_SCL_HCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_HS_SCL_LCNT; val = lcnt;
		con_printf("w 0x%2x 0x%x _HS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_HS_SCL_LCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_HS_SCL_HCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _HS_SCL_HCNT\n", offset, val);
		offset = IC_HS_SCL_LCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _HS_SCL_LCNT\n", offset, val);
#endif
		break;

	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;
		hcnt = bus_speed / speed / (40+47) * 40 - (5+7);
		lcnt = bus_speed / speed / (40+47) * 47 - 1;
#ifdef DW_I2C_DEBUG
		offset = IC_SS_SCL_HCNT; val = hcnt;
		con_printf("w 0x%2x 0x%x _SS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_SS_SCL_HCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_SS_SCL_LCNT; val = lcnt;
		con_printf("w 0x%2x 0x%x _SS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_SS_SCL_LCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_SS_SCL_HCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _SS_SCL_HCNT\n", offset, val);
		offset = IC_SS_SCL_LCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _SS_SCL_LCNT\n", offset, val);
#endif
		break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
		hcnt = bus_speed / speed / (6+13) * 6 - (5+7);
		lcnt = bus_speed / speed / (6+13) * 13 - 1;
#ifdef DW_I2C_DEBUG
		offset = IC_FS_SCL_HCNT; val = hcnt;
		con_printf("w 0x%2x 0x%x _FS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_FS_SCL_HCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_FS_SCL_LCNT; val = lcnt;
		con_printf("w 0x%2x 0x%x _FS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_FS_SCL_LCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_FS_SCL_HCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _FS_SCL_HCNT\n", offset, val);
		offset = IC_FS_SCL_LCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _FS_SCL_LCNT\n", offset, val);
#endif
		break;
	}
#else
	switch (i2c_spd) {
	case IC_SPEED_MODE_HIGH:
		cntl |= IC_CON_SPD_HS;
		hcnt = (bus_mhz * MIN_HS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_HS_SCL_LOWTIME) / 1000;
#ifdef DW_I2C_DEBUG
		offset = IC_HS_SCL_HCNT; val = hcnt;
		con_printf("w 0x%2x 0x%x _HS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_HS_SCL_HCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_HS_SCL_LCNT; val = lcnt;
		con_printf("w 0x%2x 0x%x _HS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_HS_SCL_LCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_HS_SCL_HCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _HS_SCL_HCNT\n", offset, val);
		offset = IC_HS_SCL_LCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _HS_SCL_LCNT\n", offset, val);
#endif
		break;

	case IC_SPEED_MODE_STANDARD:
		cntl |= IC_CON_SPD_SS;
		hcnt = (bus_mhz * MIN_SS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_SS_SCL_LOWTIME) / 1000;
#ifdef DW_I2C_DEBUG
		offset = IC_SS_SCL_HCNT; val = hcnt;
		con_printf("w 0x%2x 0x%x _SS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_SS_SCL_HCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_SS_SCL_LCNT; val = lcnt;
		con_printf("w 0x%2x 0x%x _SS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_SS_SCL_LCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_SS_SCL_HCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _SS_SCL_HCNT\n", offset, val);
		offset = IC_SS_SCL_LCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _SS_SCL_LCNT\n", offset, val);
#endif
		break;

	case IC_SPEED_MODE_FAST:
	default:
		cntl |= IC_CON_SPD_FS;
		hcnt = (bus_mhz * MIN_FS_SCL_HIGHTIME) / 1000;
		lcnt = (bus_mhz * MIN_FS_SCL_LOWTIME) / 1000;
#ifdef DW_I2C_DEBUG
		offset = IC_FS_SCL_HCNT; val = hcnt;
		con_printf("w 0x%2x 0x%x _FS_SCL_HCNT\n", offset, val);
#endif
		__raw_writel(hcnt, base + IC_FS_SCL_HCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_FS_SCL_LCNT; val = lcnt;
		con_printf("w 0x%2x 0x%x _FS_SCL_LCNT\n", offset, val);
#endif
		__raw_writel(lcnt, base + IC_FS_SCL_LCNT);
#ifdef DW_I2C_DEBUG
		offset = IC_FS_SCL_HCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _FS_SCL_HCNT\n", offset, val);
		offset = IC_FS_SCL_LCNT; val = __raw_readl(base + offset); con_printf("r 0x%2x 0x%x _FS_SCL_LCNT\n", offset, val);
#endif
		break;
	}
#endif

	offset = IC_CON;
	val = cntl;
#ifdef DW_I2C_DEBUG
	con_printf("w 0x%2x 0x%x _CON\n", offset, val);
#endif
	__raw_writel(val, base + offset);

	/* Restore back i2c now speed set */
	if (ena == IC_ENABLE_0B) {
		offset = IC_ENABLE;
		val = 1;
#ifdef DW_I2C_DEBUG
		con_printf("w 0x%2x 0x%x _ENABLE\n", offset, val);
#endif
		__raw_writel(val, base + offset);
	}

	return;
}

static void i2c_setaddress(caddr_t base, unsigned int i2c_addr)
{
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	__raw_writel(0, base + IC_ENABLE);
	__raw_writel(i2c_addr, base + IC_TAR);
	__raw_writel(1, base + IC_ENABLE);
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
}
static void i2c_flush_rxfifo(caddr_t base)
{
	uint32_t val;
	uint32_t offset;
	offset  = IC_STATUS;
	do {
		val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
		con_printf("r 0x%2x 0x%x _STATUS\n", offset, val);
#endif
	} while (val & IC_STATUS_RFNE);
}

static int i2c_wait_for_bb(caddr_t base)
{
	uint32_t val;
	uint32_t offset;
	offset  = IC_STATUS;
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	do {
		val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
		con_printf("r 0x%2x 0x%x _STATUS\n", offset, val);
#endif
	} while(val & IC_STATUS_MA || !(val & IC_STATUS_TFE));
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
	return 0;
}

static int i2c_xfer_init(caddr_t base, unsigned char chip, unsigned int addr,
			 int alen)
{
	uint32_t val;
	uint32_t offset;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	if (i2c_wait_for_bb(base))
		return 1;

	i2c_setaddress(base, chip);
	offset = IC_DATA_CMD;
	while (alen) {
		alen--;
		/* high byte address going out first */
		val = (addr >> (alen * 8)) & 0xff;
#ifdef DW_I2C_DEBUG
		con_printf("w 0x%2x 0x%x _DATA_CMD\n", offset, val);
#endif
		__raw_writel(val, base + offset);
	}
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
	return 0;
}

static int i2c_rx_finish(caddr_t base)
{
	uint32_t val;
	uint32_t offset;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	while (1) {
		offset = IC_RAW_INTR_STAT;
		val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
		con_printf("r 0x%2x 0x%x _RAW_INTR_STAT\n", offset, val);
#endif
		if (val & IC_STOP_DET) {
			offset = IC_CLR_STOP_DET;
			val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
			con_printf("r 0x%2x 0x%x _CLR_STOP_DET\n", offset, val);
#endif
			break;
		}
	}

	if (i2c_wait_for_bb(base)) {
		printf("Timed out waiting for bus\n");
		return 1;
	}

	i2c_flush_rxfifo(base);

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
	return 0;
}

static int i2c_tx_finish(caddr_t base)
{
	uint32_t val;
	uint32_t offset;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif

	while (1) {
		offset = IC_STATUS;
		val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
		con_printf("r 0x%2x 0x%x _STATUS\n", offset, val);
#endif
		if (val & IC_STATUS_TFE) {
			break;
		}

		offset = IC_RAW_INTR_STAT;
		val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
		con_printf("r 0x%2x 0x%x _RAW_INTR_STAT\n", offset, val);
#endif
		if (val & IC_STOP_DET) {
			offset = IC_CLR_STOP_DET;
			val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
			con_printf("r 0x%2x 0x%x _CLR_STOP_DET\n", offset, val);
#endif
			break;
		}
	}

	if (i2c_wait_for_bb(base)) {
		printf("Timed out waiting for bus\n");
		return 1;
	}

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
	return 0;
}

/*
 * Read from i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Read from i2c memory.
 */
int i2c_hw_read_mem(uint8_t dev, unsigned int addr,
			 int alen, uint8_t *buffer, int len)
{
	caddr_t base = dw_i2c_pri->base;
	unsigned int active = 0;
	uint32_t val;
	uint32_t offset;
	int ret;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif

	ret = i2c_xfer_init(base, dev, addr, alen);
	if (ret != 0) {
		con_printf("Error: Failed i2c_xfer_init. ret = %d\n", ret);
		return 1;
	}

	while (len) {
		if (!active) {
			/*
			 * Avoid writing to ic_cmd_data multiple times
			 * in case this loop spins too quickly and the
			 * ic_status RFNE bit isn't set after the first
			 * write. Subsequent writes to ic_cmd_data can
			 * trigger spurious i2c transfer.
			 */
			offset = IC_DATA_CMD;
			val = IC_CMD;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1)
				val |= IC_STOP;
#endif
#ifdef DW_I2C_DEBUG
			con_printf("w 0x%2x 0x%x _DATA_CMD\n", offset, val);
#endif
			__raw_writel(val, base + offset);
			active = 1;
		}

		offset = IC_STATUS;
		val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
		con_printf("r 0x%2x 0x%x _STATUS\n", offset, val);
#endif
		if (val & IC_STATUS_RFNE) {
			offset = IC_DATA_CMD;
			val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
			con_printf("r 0x%2x 0x%x _DATA_CMD\n", offset, val);
#endif
			*buffer++ = (uint8_t)val;
			len--;
			active = 0;
		}
	}

	ret = i2c_rx_finish(base);
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
	return ret;
}

/*
 * Write to i2c memory
 * @chip:	target i2c address
 * @addr:	address to read from
 * @alen:
 * @buffer:	buffer for read data
 * @len:	no of bytes to be read
 *
 * Write to i2c memory.
 */
int i2c_hw_write_mem(uint8_t dev, unsigned int addr,
			  int alen, uint8_t *buffer, int len)
{
	caddr_t base = dw_i2c_pri->base;
	//int nb = len;
	uint32_t val;
	uint32_t offset;
	int ret;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif

	ret = i2c_xfer_init(base, dev, addr, alen);
	if (ret != 0) {
		con_printf("Error: Failed i2c_xfer_init. ret = %d\n", ret);
		return 1;
	}

	while (len) {
		offset = IC_STATUS;
		val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
		con_printf("r 0x%2x 0x%x _STATUS\n", offset, val);
#endif
		if (val & IC_STATUS_TFNF) {
			offset = IC_DATA_CMD;
			val = *buffer;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1) {
				val = *buffer | IC_STOP;
			}
#endif
#ifdef DW_I2C_DEBUG
			con_printf("w 0x%2x 0x%x _DATA_CMD\n", offset, val);
#endif
			__raw_writel(val, base + offset);
			buffer++;
			len--;
		}
	}

	ret = i2c_tx_finish(base);
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
	return ret;
}

/*
 * Simple read and write for some bytes.
 */
int i2c_hw_read_bytes(uint8_t dev, uint8_t *buffer, int len, unsigned int stop)
{
	caddr_t base = dw_i2c_pri->base;
	unsigned int active = 0;
	uint32_t val;
	uint32_t offset;
	int orig_len = len;
	int ret;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	if (i2c_wait_for_bb(base))
		return -1;
	i2c_setaddress(base, dev);

	while (len) {
		if (!active) {
			/*
			 * Avoid writing to ic_cmd_data multiple times
			 * in case this loop spins too quickly and the
			 * ic_status RFNE bit isn't set after the first
			 * write. Subsequent writes to ic_cmd_data can
			 * trigger spurious i2c transfer.
			 */
			offset = IC_DATA_CMD;
			val = IC_CMD;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1 && stop != 0)
				val |= IC_STOP;
#endif
#ifdef DW_I2C_DEBUG
			con_printf("w 0x%2x 0x%x _DATA_CMD\n", offset, val);
#endif
			__raw_writel(val, base + offset);
			active = 1;
		}

		offset = IC_STATUS;
		val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
		con_printf("r 0x%2x 0x%x _STATUS\n", offset, val);
#endif
		if (val & IC_STATUS_RFNE) {
			offset = IC_DATA_CMD;
			val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
			con_printf("r 0x%2x 0x%x _DATA_CMD\n", offset, val);
#endif
			*buffer++ = (uint8_t)val;
			len--;
			active = 0;
		}
		/* TODO Timeout. */
	}

	ret = i2c_rx_finish(base);
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
	if (ret != 0) {
		return -1;
	}

	return (orig_len - len);
}

int i2c_hw_write_bytes(uint8_t dev, uint8_t *buffer, int len, unsigned int stop)
{
	caddr_t base = dw_i2c_pri->base;
	uint32_t val;
	uint32_t offset;
	int orig_len = len;
	int ret;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif
	if (i2c_wait_for_bb(base))
		return -1;
	i2c_setaddress(base, dev);

	while (len) {
		offset = IC_STATUS;
		val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
		con_printf("r 0x%2x 0x%x _STATUS\n", offset, val);
#endif
		if (val & IC_STATUS_TFNF) {
			offset = IC_DATA_CMD;
			val = *buffer;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1 && stop != 0) {
				val = *buffer | IC_STOP;
			}
#endif
#ifdef DW_I2C_DEBUG
			con_printf("w 0x%2x 0x%x _DATA_CMD\n", offset, val);
#endif
			__raw_writel(val, base + offset);
			buffer++;
			len--;
		}
		/* TODO Timeout. */
	}

	ret = i2c_tx_finish(base);
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
	if (ret != 0) {
		return -1;
	}

	return (orig_len - len);
}

/*
 * DW I2C VIP operations:
 * - write: 1 byte I2C CMD + 1 byte address + n bytes data
 * - read:  1 byte I2C CMD + n bytes data (from slave)
 */
int i2c_hw_write_vip(uint8_t dev, unsigned int addr,
			  uint8_t *buffer, int len)
{
	return i2c_hw_write_mem(dev, addr, 1, buffer, len);
}

int i2c_hw_read_vip(uint8_t dev, uint8_t *buffer, int len)
{
	caddr_t base = dw_i2c_pri->base;
	unsigned int active = 0;
	uint32_t val;
	uint32_t offset;
	int ret;

#ifdef DW_I2C_DEBUG
	con_printf("Debug: Enter %s\n", __func__);
#endif

	if (i2c_wait_for_bb(base))
		return 1;

	i2c_setaddress(base, dev);

	while (len) {
		if (!active) {
			/*
			 * Avoid writing to ic_cmd_data multiple times
			 * in case this loop spins too quickly and the
			 * ic_status RFNE bit isn't set after the first
			 * write. Subsequent writes to ic_cmd_data can
			 * trigger spurious i2c transfer.
			 */
			offset = IC_DATA_CMD;
			val = IC_CMD;
#ifdef CONFIG_DW_I2C_EMPTYFIFO_HOLD_MASTER
			if (len == 1)
				val |= IC_STOP;
#endif
#ifdef DW_I2C_DEBUG
			con_printf("w 0x%2x 0x%x _DATA_CMD\n", offset, val);
#endif
			__raw_writel(val, base + offset);
			active = 1;
		}

#ifdef CONFIG_DW_I2C_TEST_IRQ
		while (irq_test_flag == 0) {
			irq_local_save(irq_flags);
			irq_local_enable();
			irq_local_disable();
			irq_local_restore(irq_flags);
		}
		irq_test_flag = 0;
		irqc_ack_irq(IRQ_I2C0 + irq_test_master_num);
		irqc_unmask_irq(IRQ_I2C0 + irq_test_master_num);
#endif

		offset = IC_STATUS;
		val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
		con_printf("r 0x%2x 0x%x _STATUS\n", offset, val);
#endif
		if (val & IC_STATUS_RFNE) {
			offset = IC_DATA_CMD;
			val = __raw_readl(base + offset);
#ifdef DW_I2C_DEBUG
			con_printf("r 0x%2x 0x%x _DATA_CMD\n", offset, val);
#endif
			*buffer++ = (uint8_t)val;
			len--;
			active = 0;
		}
	}

	ret = i2c_rx_finish(base);
#ifdef DW_I2C_DEBUG
	con_printf("Debug: Exit %s\n", __func__);
#endif
	return ret;
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

void i2c_hw_master_select(i2c_t i2c)
{
}
