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

static void sdhci_cmd_failure(uint32_t stat)
{

	if (stat & (SDHCI_INT_CRC | SDHCI_INT_END_BIT |
		    SDHCI_INT_DATA_CRC | SDHCI_INT_DATA_END_BIT |
		    SDHCI_INT_ERROR)) {
		mmc_cmd_failure(MMC_ERR_CARD_LOOSE_BUS);
		return;
	}
	if (stat & SDHCI_INT_INDEX) {
		mmc_cmd_failure(MMC_ERR_ILLEGAL_COMMAND);
		return;
	}
	if (stat & (SDHCI_INT_TIMEOUT | SDHCI_INT_DATA_TIMEOUT)) {
		mmc_cmd_failure(MMC_ERR_TIMEOUT);
		return;
	}
	if (stat & SDHCI_INT_BUS_POWER) {
		mmc_cmd_failure(MMC_ERR_CARD_NON_COMP_VOLT);
		return;
	}
	mmc_cmd_failure(MMC_ERR_ILLEGAL_COMMAND);
	return;
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

static void sdhci_transfer_data(uint32_t start_addr)
{
	struct sdhci_host *host = mmc2sdhci();
	unsigned int stat, rdy, mask, timeout, block = 0;
	bool transfer_done = false;
	uint32_t *buf = (uint32_t *)mmc_slot_ctrl.block_data;
#ifdef CONFIG_SDHCI_SDMA
	unsigned char ctrl;

	ctrl = sdhci_readb(host, SDHCI_HOST_CONTROL);
	ctrl &= ~SDHCI_CTRL_DMA_MASK;
	sdhci_writeb(host, ctrl, SDHCI_HOST_CONTROL);
#endif

	timeout = 1000000;
	rdy = SDHCI_INT_SPACE_AVAIL | SDHCI_INT_DATA_AVAIL;
	mask = SDHCI_DATA_AVAILABLE | SDHCI_SPACE_AVAILABLE;
	do {
		stat = sdhci_readl(host, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR) {
			printf("%s: Error detected in status(0x%X)!\n",
			       __func__, stat);
			mmc_cmd_failure(MMC_ERR_CARD_LOOSE_BUS);
			return;
		}
		if (!transfer_done && (stat & rdy)) {
			if (!(sdhci_readl(host, SDHCI_PRESENT_STATE) & mask))
				continue;
			sdhci_writel(host, rdy, SDHCI_INT_STATUS);
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
		if (!transfer_done && (stat & SDHCI_INT_DMA_END)) {
			sdhci_writel(host, SDHCI_INT_DMA_END, SDHCI_INT_STATUS);
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
	} while (!(stat & SDHCI_INT_DATA_END));
}

/* No command will be sent by driver if card is busy, so driver must wait
 * for card ready state.
 * Every time when card is busy after timeout then (last) timeout value will be
 * increased twice but only if it doesn't exceed global defined maximum.
 * Each function call will use last timeout value.
 */
#define SDHCI_CMD_MAX_TIMEOUT			3200
#define SDHCI_CMD_DEFAULT_TIMEOUT		100
#define SDHCI_READ_STATUS_TIMEOUT		1000

void sdhci_send_command(uint8_t cmd, uint32_t arg)
{
	struct sdhci_host *host = mmc2sdhci();
	uint8_t type = mmc_get_block_data();
	__unused size_t trans_bytes;
	unsigned int stat = 0;
	uint32_t mask, flags, mode;
	unsigned int time = 0, start_addr = 0;
	tick_t start = tick_get_counter();
	uint8_t rsp = mmc_slot_ctrl.rsp;

	/* Timeout unit - ms */
	static unsigned int cmd_timeout = SDHCI_CMD_DEFAULT_TIMEOUT;

	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);
	mask = SDHCI_CMD_INHIBIT | SDHCI_DATA_INHIBIT;

	/* We shouldn't wait for data inihibit for stop commands, even
	   though they might use busy signaling */
	if (cmd == MMC_CMD_STOP_TRANSMISSION)
		mask &= ~SDHCI_DATA_INHIBIT;

	while (sdhci_readl(host, SDHCI_PRESENT_STATE) & mask) {
		if (time >= cmd_timeout) {
			printf("%s: MMC: %d busy ", __func__, mmc_sid);
			if (2 * cmd_timeout <= SDHCI_CMD_MAX_TIMEOUT) {
				cmd_timeout += cmd_timeout;
				printf("timeout increasing to: %u ms.\n",
				       cmd_timeout);
			} else {
				puts("timeout.\n");
				mmc_cmd_failure(MMC_ERR_TIMEOUT);
				return;
			}
		}
		time++;
		mdelay(1);
	}

	mask = SDHCI_INT_RESPONSE;
	if (!(rsp & MMC_RSP_PRESENT))
		flags = SDHCI_CMD_RESP_NONE;
	else if (rsp & MMC_RSP_136)
		flags = SDHCI_CMD_RESP_LONG;
	else if (rsp & MMC_RSP_BUSY) {
		flags = SDHCI_CMD_RESP_SHORT_BUSY;
		if (type)
			mask |= SDHCI_INT_DATA_END;
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
		sdhci_writeb(host, 0xe, SDHCI_TIMEOUT_CONTROL);
		mode = SDHCI_TRNS_BLK_CNT_EN;
		trans_bytes = mmc_slot_ctrl.block_cnt * mmc_slot_ctrl.block_len;
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
		sdhci_writeb(host, 0xe, SDHCI_TIMEOUT_CONTROL);
	}

	sdhci_writel(host, arg, SDHCI_ARGUMENT);
#ifdef CONFIG_SDHCI_SDMA
	if (type) {
		trans_bytes = ALIGN(trans_bytes, CONFIG_SYS_CACHELINE_SIZE);
		dma_wmb(); /* flush_cache? */
	}
#endif
	sdhci_writew(host, SDHCI_MAKE_CMD(cmd, flags), SDHCI_COMMAND);
	start = tick_get_counter();
	do {
		stat = sdhci_readl(host, SDHCI_INT_STATUS);
		if (stat & SDHCI_INT_ERROR)
			break;

		if ((tick_get_counter() - start) >=
		    SDHCI_READ_STATUS_TIMEOUT) {
			printf("%s: Timeout for status update!\n",
			       __func__);
			mmc_cmd_failure(MMC_ERR_TIMEOUT);
			return;
		}
	} while ((stat & mask) != mask);

	if ((stat & (SDHCI_INT_ERROR | mask)) != mask) {
		sdhci_cmd_failure(stat);
		return;
	}
	sdhci_writel(host, mask, SDHCI_INT_STATUS);
	if (type)
		sdhci_transfer_data(start_addr);
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

	sdhci_writel(host, SDHCI_INT_ALL_MASK, SDHCI_INT_STATUS);

	sdhci_reset(SDHCI_RESET_CMD);
	sdhci_reset(SDHCI_RESET_DATA);
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

bool sdhci_set_clock(uint32_t clock)
{
	struct sdhci_host *host = mmc2sdhci();
	unsigned int div, clk = 0, timeout;
	uint32_t ctrl;

	/* Wait max 20 ms */
	timeout = 200;
	while (sdhci_readl(host, SDHCI_PRESENT_STATE) &
			   (SDHCI_CMD_INHIBIT | SDHCI_DATA_INHIBIT)) {
		if (timeout == 0) {
			printf("%s: Timeout to wait cmd & data inhibit\n",
			       __func__);
			return false;
		}
		timeout--;
		udelay(100);
	}

	sdhci_writew(host, 0, SDHCI_CLOCK_CONTROL);
	if (clock == 0)
		return 0;

	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
		/*
		 * Check if the Host Controller supports Programmable Clock
		 * Mode.
		 */
		if (host->clk_mul) {
			for (div = 1; div <= 1024; div++) {
				if ((host->max_clk / div) <= clock)
					break;
			}

			/*
			 * Set Programmable Clock Mode in the Clock
			 * Control register.
			 */
			clk = SDHCI_PROG_CLOCK_MODE;
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
	} else {
		/* Version 2.00 divisors must be a power of 2. */
		for (div = 1; div < SDHCI_MAX_DIV_SPEC_200; div *= 2) {
			if ((host->max_clk / div) <= clock)
				break;
		}
		div >>= 1;
	}

#if 0
	sdhc_hw_set_clock(div);
#endif

	clk |= (div & SDHCI_DIV_MASK) << SDHCI_DIVIDER_SHIFT;
	clk |= ((div & SDHCI_DIV_HI_MASK) >> SDHCI_DIV_MASK_LEN)
		<< SDHCI_DIVIDER_HI_SHIFT;
	clk |= SDHCI_CLOCK_INT_EN;
	sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);

	/* Wait max 20 ms */
	timeout = 20;
	while (!((clk = sdhci_readw(host, SDHCI_CLOCK_CONTROL))
		& SDHCI_CLOCK_INT_STABLE)) {
		if (timeout == 0) {
			printf("%s: Internal clock never stabilised.\n",
			       __func__);
			return false;
		}
		timeout--;
		mdelay(1);
	}

	clk |= SDHCI_CLOCK_CARD_EN;
	sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);

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

void sdhci_init(uint32_t f_max, uint32_t f_min)
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
	if (caps & SDHCI_CAN_VDD_330)
		mmc_slot_ctrl.host_ocr |= MMC_OCR_32_33 | MMC_OCR_33_34;
	if (caps & SDHCI_CAN_VDD_300)
		mmc_slot_ctrl.host_ocr |= MMC_OCR_29_30 | MMC_OCR_30_31;
	if (caps & SDHCI_CAN_VDD_180)
		mmc_slot_ctrl.host_ocr |= MMC_OCR_170_195;

	mmc_slot_ctrl.host_ocr = SD_OCR_HCS;
	mmc_slot_ctrl.host_scr.bus_widths = 4;
#if 0
	MMC_MODE_HS_52MHz;
#endif

	/* Since Host Controller Version3.0 */
	if (SDHCI_GET_VERSION(host) >= SDHCI_SPEC_300) {
		if (caps & SDHCI_CAN_DO_8BIT)
			mmc_slot_ctrl.host_scr.bus_widths = 8;
	}

	sdhci_reset(SDHCI_RESET_ALL);
	sdhci_set_power(__fls32(mmc_slot_ctrl.host_ocr));
}

void sdhci_start(void)
{
	struct sdhci_host *host = mmc2sdhci();

	/* Enable only interrupts served by the SD controller */
	sdhci_writel(host, SDHCI_INT_DATA_MASK | SDHCI_INT_CMD_MASK,
		     SDHCI_INT_ENABLE);
	/* Mask all sdhci interrupt sources */
	sdhci_writel(host, 0x0, SDHCI_SIGNAL_ENABLE);
}

bool sdhci_detect_card(void)
{
	return true;
}
