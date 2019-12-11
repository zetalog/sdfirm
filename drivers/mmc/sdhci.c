#include <target/mmc.h>
#include <target/delay.h>
#include <target/jiffies.h>

struct sdhci_host sdhci_hosts[NR_MMC_SLOTS];

struct sdhci_host *mmc2sdhci(void)
{
	if (mmc_sid >= NR_MMC_SLOTS)
		return NULL;
	return sdhci_hosts + mmc_sid;
}

static void sdhci_reset(uint8_t mask)
{
	struct sdhci_host *host = mmc2sdhci();
	unsigned long timeout;

	/* Wait max 100 ms */
	timeout = 100;
	sdhci_writeb(host, mask, SDHCI_SOFTWARE_RESET);
	while (sdhci_readb(host, SDHCI_SOFTWARE_RESET) & mask) {
		if (timeout == 0) {
			printf("%s: Reset 0x%x never completed.\n",
			       __func__, (int)mask);
			return;
		}
		timeout--;
		mdelay(1);
	}
}

static void sdhci_transfer_pio(uint32_t *block)
{
	struct sdhci_host *host = mmc2sdhci();
	uint8_t type = mmc_get_block_data();
	int i;
	uint32_t *offs;

	for (i = 0; i < mmc_slot_ctrl.block_len; i += 4) {
		offs = block + i;
		if (type == MMC_SLOT_BLOCK_READ)
			*offs = sdhci_readl(host, SDHCI_BUFFER);
		else
			sdhci_writel(host, *offs, SDHCI_BUFFER);
	}
}

static void sdhci_transfer_data(void)
{
	unsigned int stat, rdy, mask, timeout, block = 0;
	bool transfer_done = false;
	uint32_t *buf = (uint32_t *)mmc_slot_ctrl.block_data;
#ifdef CONFIG_SDHCI_SDMA
	struct sdhci_host *host = mmc2sdhci();
	uint32_t start_addr = 0;
	unsigned char ctrl;

	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	ctrl &= ~SDHCI_CTRL_DMA_MASK;
	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
#endif

	timeout = 1000000;
	rdy = SDHC_BUFFER_READ_READY | SDHC_BUFFER_WRITE_READY;
	mask = SDHC_BUFFER_READ_ENABLE | SDHC_BUFFER_WRITE_ENABLE;
	do {
		stat = sdhc_irq_status(mmc_sid);
		if (stat & SDHC_ERROR_INTERRUPT) {
			printf("%s: Error detected in status(0x%X)!\n",
			       __func__, stat);
			mmc_cmd_failure(MMC_ERR_CARD_LOOSE_BUS);
			return;
		}
		if (!transfer_done && (stat & rdy)) {
			if (!sdhc_state_present(mmc_sid, mask))
				continue;
			sdhc_clear_irq(mmc_sid, rdy);
			sdhci_transfer_pio(buf);
			block += mmc_slot_ctrl.block_len;
			if (++block >= mmc_slot_ctrl.block_cnt) {
				/* Keep looping until the SDHCI_INT_DATA_END is
				 * cleared, even if we finished sending all the
				 * blocks.
				 */
				transfer_done = true;
				continue;
			}
		}
#ifdef CONFIG_SDHCI_SDMA
		if (!transfer_done && (stat & SDHC_DMA_INTERRUPT)) {
			sdhc_clear_irq(mmc_sid, SDHC_DMA_INTERRUPT);
			start_addr &= ~(SDHCI_DEFAULT_BOUNDARY_SIZE - 1);
			start_addr += SDHCI_DEFAULT_BOUNDARY_SIZE;
			sdhci_writel(host, start_addr, SDHCI_DMA_ADDRESS);
		}
#endif
		if (timeout-- > 0)
			udelay(10);
		else {
			printf("%s: Transfer data timeout\n", __func__);
			mmc_cmd_failure(MMC_ERR_TIMEOUT);
			return;
		}
	} while (!(stat & SDHC_TRANSFER_COMPLETE));
}

/* No command will be sent by driver if card is busy, so driver must wait
 * for card ready state.
 * Every time when card is busy after timeout then (last) timeout value will be
 * increased twice but only if it doesn't exceed global defined maximum.
 * Each function call will use last timeout value.
 */
static void sdhc_wait_transfer(void)
{
	while (sdhc_state_present(mmc_sic, SDHC_COMMAND_INHIBIT));
}

void sdhci_send_command(uint8_t cmd, uint32_t arg)
{
	struct sdhci_host *host = mmc2sdhci();
	uint8_t type = mmc_get_block_data();
	__unused size_t trans_bytes;
	uint32_t mask, flags, mode;
	uint8_t rsp = mmc_slot_ctrl.rsp;

	sdhc_wait_transfer();
	sdhci_start_transfer();

	mask = SDHC_COMMAND_COMPLETE;
	if (!(rsp & MMC_RSP_PRESENT))
		flags = SDHCI_CMD_RESP_NONE;
	else if (rsp & MMC_RSP_136)
		flags = SDHCI_CMD_RESP_LONG;
	else if (rsp & MMC_RSP_BUSY) {
		flags = SDHCI_CMD_RESP_SHORT_BUSY;
		if (type)
			mask |= SDHC_TRANSFER_COMPLETE;
	} else
		flags = SDHCI_CMD_RESP_SHORT;

	if (rsp & MMC_RSP_CRC)
		flags |= SDHCI_CMD_CRC;
	if (rsp & MMC_RSP_OPCODE)
		flags |= SDHCI_CMD_INDEX;

	if (type)
		flags |= SDHCI_CMD_DATA;

	/* Set Transfer mode regarding to data flag */
	if (type) {
		__raw_writeb(0xE, SDHC_TIMEOUT_CONTROL(mmc_sid));
		mode = SDHCI_TRNS_BLK_CNT_EN;
		trans_bytes = mmc_slot_ctrl.block_cnt *
			      mmc_slot_ctrl.block_len;
		if (mmc_slot_ctrl.block_cnt > 1)
			mode |= SDHCI_TRNS_MULTI;
		if (type == MMC_SLOT_BLOCK_READ)
			mode |= SDHCI_TRNS_READ;

#ifdef CONFIG_SDHCI_SDMA
		/* TODO: Bounce buffer */
		start_addr = (uint32_t)mmc_slot_ctrl->block_data;
		sdhci_writel(host, start_addr, SDHCI_DMA_ADDRESS);
		mode |= SDHCI_TRNS_DMA;
#endif
		sdhci_writew(host,
			     SDHCI_MAKE_BLKSZ(SDHCI_DEFAULT_BOUNDARY_ARG,
			     mmc_slot_ctrl.block_len),
			     SDHCI_BLOCK_SIZE);
		sdhci_writew(host, mmc_slot_ctrl.block_cnt,
			     SDHCI_BLOCK_COUNT);
		sdhci_writew(host, mode, SDHCI_TRANSFER_MODE);
	} else if (rsp & MMC_RSP_BUSY) {
		__raw_writeb(0xE, SDHC_TIMEOUT_CONTROL(mmc_sid));
	}

	sdhci_writel(host, arg, SDHCI_ARGUMENT);
	sdhci_writew(host, SDHCI_MAKE_CMD(cmd, flags), SDHCI_COMMAND);
}

static void sdhci_decode_reg(uint8_t *resp, uint32_t reg)
{
	resp[3] = HIBYTE(HIWORD(reg));
	resp[2] = LOBYTE(HIWORD(reg));
	resp[1] = HIBYTE(LOWORD(reg));
	resp[0] = LOBYTE(LOWORD(reg));
}

void sdhci_recv_response(uint8_t *resp, uint8_t size)
{
	struct sdhci_host *host = mmc2sdhci();
	uint8_t rsp = mmc_slot_ctrl.rsp;
	int i;
	uint32_t reg;

	if (rsp & MMC_RSP_136) {
		/* CRC is stripped so we need to do some shifting. */
		for (i = 0; i < 4; i++) {
			reg = sdhci_readl(host,
					  SDHCI_RESPONSE + (3-i)*4) << 8;
			if (i != 3)
				reg |= sdhci_readb(host,
						   SDHCI_RESPONSE + (3-i)*4-1);
			sdhci_decode_reg(resp + i, reg);
		}
	} else {
		reg = sdhci_readl(host, SDHCI_RESPONSE);
		sdhci_decode_reg(resp, reg);
	}
	sdhci_stop_transfer();
}

static void sdhci_set_power(uint8_t power)
{
	struct sdhci_host *host = mmc2sdhci();
	uint8_t pwr = 0;

	if (power != (uint8_t)-1) {
		switch (1 << power) {
		case MMC_OCR_170_195:
			pwr = SDHCI_POWER_180;
			break;
		case MMC_OCR_29_30:
		case MMC_OCR_30_31:
			pwr = SDHCI_POWER_300;
			break;
		case MMC_OCR_32_33:
		case MMC_OCR_33_34:
			pwr = SDHCI_POWER_330;
			break;
		}
	}

	if (pwr == 0) {
		sdhci_writeb(host, 0, SDHCI_POWER_CONTROL);
		return;
	}

	pwr |= SDHCI_POWER_ON;
	sdhci_writeb(host, pwr, SDHCI_POWER_CONTROL);
}

void sdhc_start_clock(void)
{
	sdhc_set_clock(mmc_sid, SDHC_CLOCK_ENABLE);
}

void sdhc_stop_clock(void)
{
	sdhc_wait_transfer();
	sdhc_clear_clock(mmc_sid, SDHC_CLOCK_ENABLE);
}

static bool sdhc_clock_stabilised(void)
{
	tick_t timeout = SDHC_INTERNAL_CLOCK_STABLE_TOUT_MS;

	do {
		if (__raw_readw(SDHC_CLOCK_CONTROL(mmc_sid)) &
		    SDHC_INTERNAL_CLOCK_STABLE)
			break;
		if (timeout == 0) {
			printf("Internal clock not stabilised.\n");
			return false;
		}
		timeout--;
		mdelay(1);
	} while (1);
	return true;
}

#ifdef CONFIG_SDHC_SPEC_4_10
static void sdhc_enable_pll(void)
{
	sdhc_set_clock(mmc_sid, SDHC_PLL_ENABLE);
	if (!sdhc_clock_stabilised())
		return;
}
static void sdhc_disable_pll(void)
{
	sdhc_clear_clock(mmc_sid, SDHC_PLL_ENABLE);
}
#else
#define sdhc_enable_pll()	do { } while (0)
#define sdhc_disable_pll()	do { } while (0)
#endif

bool sdhci_set_clock(uint32_t clock)
{
	struct sdhci_host *host = mmc2sdhci();
	uint32_t div, clk = 0;
	uint32_t ctrl;

	sdhc_stop_clock();
	sdhc_disable_pll();

	/* 1. calculate a divisor */
	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
		/* Host Controller supports Programmable Clock Mode? */
		if (host->clk_mul) {
			for (div = 1; div <= 1024; div++) {
				if ((host->max_clk / div) <= clock)
					break;
			}

			clk = SDHC_CLOCK_GENERATOR_SELECT;
			div--;
		} else {
			/* Version 3.00 divisors must be a multiple of 2. */
			if (host->max_clk <= clock) {
				div = 1;
			} else {
				for (div = 2;
				     div < SDHCI_MAX_DIV_SPEC_300;
				     div += 2) {
					if ((host->max_clk / div) <= clock)
						break;
				}
			}
			div >>= 1;
		}
		clk |= SDHC_10BIT_DIVIDED_CLOCK(div);
	} else {
		/* Version 2.00 divisors must be a power of 2. */
		for (div = 1; div < SDHCI_MAX_DIV_SPEC_200; div *= 2) {
			if ((host->max_clk / div) <= clock)
				break;
		}
		div >>= 1;
		clk |= SDHC_8BIT_DIVIDED_CLOCK(div);
	}

	/* 2. set SDCLK/RCLK Frequency Select
	 *    Preset Value Enable?
	 */
	__raw_writew(clk, SDHC_CLOCK_CONTROL(mmc_sid));

	/* 3. set Internal Clock Enable */
	sdhc_set_clock(mmc_sid, SDHC_INTERNAL_CLOCK_ENABLE);

	/* 4. check Internal Clock Stable */
	if (!sdhc_clock_stabilised())
		return false;

#ifdef CONFIG_SDHC_SPEC_4_10
	/* 5. set PLL Enable */
	sdhc_enable_pll();
#endif

	sdhc_start_clock();

	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	if (clock > 26000000)
		ctrl |= SDHCI_CTRL_HISPD;
	else
		ctrl &= ~SDHCI_CTRL_HISPD;
	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
	return true;
}

void sdhci_set_width(uint8_t width)
{
	struct sdhci_host *host = mmc2sdhci();
	uint32_t ctrl;

	/* Set bus width */
	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	if (width == 8) {
		ctrl &= ~SDHCI_CTRL_4BITBUS;
		if ((SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300))
			ctrl |= SDHCI_CTRL_8BITBUS;
	} else {
		if ((SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300))
			ctrl &= ~SDHCI_CTRL_8BITBUS;
		if (width == 4)
			ctrl |= SDHCI_CTRL_4BITBUS;
		else
			ctrl &= ~SDHCI_CTRL_4BITBUS;
	}
	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
}

void sdhci_init(uint32_t f_min, uint32_t f_max)
{
	struct sdhci_host *host = mmc2sdhci();
	uint32_t caps, caps_1;

	caps = sdhci_readl(host, SDHCI_CAPABILITIES);

#ifdef CONFIG_SDHCI_SDMA
	BUG_ON(!(caps & SDHCI_CAN_DO_SDMA));
#endif
	host->version = sdhci_readw(host, SDHCI_HOST_VERSION);

	/* Check whether the clock multiplier is supported or not */
	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
		caps_1 = sdhci_readl(host, SDHCI_CAPABILITIES_1);
		host->clk_mul = (caps_1 & SDHCI_CLOCK_MUL_MASK) >>
				SDHCI_CLOCK_MUL_SHIFT;
	}

	if (host->max_clk == 0) {
		if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300)
			host->max_clk = (caps & SDHCI_CLOCK_V3_BASE_MASK) >>
					SDHCI_CLOCK_BASE_SHIFT;
		else
			host->max_clk = (caps & SDHCI_CLOCK_BASE_MASK) >>
					SDHCI_CLOCK_BASE_SHIFT;
		host->max_clk *= 1000000;
		if (host->clk_mul)
			host->max_clk *= host->clk_mul;
	}
	BUG_ON(host->max_clk == 0);
	if (f_max && (f_max < host->max_clk))
		mmc_slot_ctrl.f_max = f_max;
	else
		mmc_slot_ctrl.f_max = host->max_clk;
	if (f_min)
		mmc_slot_ctrl.f_min = f_min;
	else {
		if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300)
			mmc_slot_ctrl.f_min =
				mmc_slot_ctrl.f_max / SDHCI_MAX_DIV_SPEC_300;
		else
			mmc_slot_ctrl.f_min =
				mmc_slot_ctrl.f_max / SDHCI_MAX_DIV_SPEC_200;
	}
	printf("clock: %dHz ~ %dHz\n",
	       mmc_slot_ctrl.f_min, mmc_slot_ctrl.f_max);

	mmc_slot_ctrl.host_ocr = SD_OCR_HCS;
	if (caps & SDHCI_CAN_VDD_330)
		mmc_slot_ctrl.host_ocr |= (MMC_OCR_32_33 | MMC_OCR_33_34);
	if (caps & SDHCI_CAN_VDD_300)
		mmc_slot_ctrl.host_ocr |= (MMC_OCR_29_30 | MMC_OCR_30_31);
	if (caps & SDHCI_CAN_VDD_180)
		mmc_slot_ctrl.host_ocr |= MMC_OCR_170_195;

	mmc_slot_ctrl.host_scr.bus_widths = 4;
#if 0
	MMC_MODE_HS_52MHz;
#endif

	/* Since Host Controller Version3.0 */
	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
		if (caps & SDHCI_CAN_DO_8BIT)
			mmc_slot_ctrl.host_scr.bus_widths = 8;
	}

	/* Mask all SDHC IRQ sources, let sdhci_irq_init() enables the
	 * required IRQ sources.
	 */
	sdhc_mask_all_irqs(mmc_sid);

	sdhci_reset(SDHCI_RESET_ALL);
	sdhci_set_power(__fls32(
			MMC_OCR_VOLTAGE_RANGE(mmc_slot_ctrl.host_ocr)));
}

void sdhc_handle_irq(void)
{
	uint32_t irqs = sdhc_irq_status(mmc_sid);
	uint8_t type = mmc_get_block_data();

	if (irqs & SDHC_CARD_DETECTION_MASK) {
		if (irqs & SDHC_CARD_INSERTION) {
			sdhc_clear_irq(mmc_sid, SDHC_CARD_INSERTION);
			if (sdhc_state_present(mmc_sid,
					       SDHC_CARD_INSERTED))
				mmc_event_raise(MMC_EVENT_CARD_INSERT);
		}
		if (irqs & SDHC_CARD_REMOVAL) {
			sdhc_clear_irq(mmc_sid, SDHC_CARD_REMOVAL);
			if (!sdhc_state_present(mmc_sid,
						SDHC_CARD_INSERTED))
				mmc_event_raise(MMC_EVENT_CARD_REMOVE);
		}
	}
	/* Handle cmd/data IRQs */
	if (irqs & SDHC_ERROR_INTERRUPT_MASK) {
		if (irqs & SDHC_COMMAND_INDEX_ERROR)
			mmc_cmd_failure(MMC_ERR_ILLEGAL_COMMAND);
		else if (irqs & SDHC_CURRENT_LIMIT_ERROR)
			mmc_cmd_failure(MMC_ERR_CARD_NON_COMP_VOLT);
		else if (irqs & SDHC_TRANSFER_TIMEOUT)
			mmc_cmd_failure(MMC_ERR_TIMEOUT);
		else if (irqs & SDHC_TRANSFER_FAILURE)
			mmc_cmd_failure(MMC_ERR_CARD_LOOSE_BUS);
		else
			mmc_cmd_failure(MMC_ERR_CARD_LOOSE_BUS);
		sdhc_clear_irq(mmc_sid, SDHC_ERROR_INTERRUPT_MASK);
		return;
	}
	if (irqs & SDHC_COMMAND_COMPLETE) {
		if (type)
			sdhci_transfer_data();
		mmc_cmd_success();
	}
}

void sdhc_irq_handler(void)
{
	mmc_slot_t slot;
	__unused mmc_slot_t sslot;

	sslot = mmc_slot_save(slot);
	for (slot = 0; slot < NR_MMC_SLOTS; slot++) {
		sslot = mmc_slot_save(slot);
		sdhc_handle_irq();
		mmc_slot_restore(sslot);
	}
}

#ifdef SYS_REALTIME
void sdhci_irq_poll(void)
{
	sdhc_irq_handler();
}
#else
void sdhci_irq_init(void)
{
}
#endif

void sdhci_detect_card(void)
{
	sdhc_enable_irq(mmc_sid, SDHC_CARD_DETECTION_MASK);
}

void sdhci_start_transfer(void)
{
	sdhc_clear_all_irqs(mmc_sid);
	sdhc_enable_irq(mmc_sid, SDHC_COMMAND_MASK | SDHC_TRANSFER_MASK);
}

void sdhci_stop_transfer(void)
{
	sdhc_disable_irq(mmc_sid, SDHC_COMMAND_MASK | SDHC_TRANSFER_MASK);
	sdhc_clear_all_irqs(mmc_sid);
	sdhci_reset(SDHCI_RESET_CMD);
	sdhci_reset(SDHCI_RESET_DATA);
}
