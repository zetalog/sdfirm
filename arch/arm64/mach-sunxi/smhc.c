#include <target/mmc.h>
#include <target/gpio.h>
#include <target/delay.h>

struct sunxi_sd_slot {
	int fatal_err;
};

uint32_t *sunxi_sd_clk_cfg[] = {
	&SUNXI_CCU->sd0_clk_cfg,
	&SUNXI_CCU->sd1_clk_cfg,
	&SUNXI_CCU->sd2_clk_cfg,
	&SUNXI_CCU->sd3_clk_cfg,
};
struct sunxi_sd_slot sunxi_sd_slots[] = {
	{ 0 },
	{ 0 },
	{ 0 },
	{ 0 },
};

static int sunxi_gpio_input(uint32_t pin)
{
	uint32_t dat;
	uint32_t bank = GPIO_BANK(pin);
	uint32_t num = GPIO_NUM(pin);
	struct sunxi_gpio *pio = BANK_TO_GPIO(bank);

	dat = __raw_readl(&pio->dat);
	dat >>= num;
	return dat & 0x1;
}

static int sunxi_sd_set_mod_clk(uint8_t mmc_no, unsigned int hz)
{
	__unused struct sunxi_mmc *mmc = SUNXI_SMHC(mmc_no);
	unsigned int pll, pll_hz, div, n, oclk_dly, sclk_dly;
	bool new_mode = false;
	uint32_t val = 0;

#ifdef CONFIG_SD_SUNXI_NEW_MODE
	if (mmc_no == 2)
		new_mode = true;
#endif

	/*
	 * The MMC clock has an extra /2 post-divider when operating in the new
	 * mode.
	 */
	if (new_mode)
		hz = hz * 2;

	if (hz <= 24000000) {
		pll = CCM_MMC_CTRL_OSCM24;
		pll_hz = 24000000;
	} else {
#ifdef CONFIG_SUN9I
		pll = CCM_MMC_CTRL_PLL_PERIPH0;
		pll_hz = clock_get_pll4_periph0();
#else
		pll = CCM_MMC_CTRL_PLL6;
		pll_hz = clock_get_pll6();
#endif
	}

	div = pll_hz / hz;
	if (pll_hz % hz)
		div++;

	n = 0;
	while (div > 16) {
		n++;
		div = (div + 1) / 2;
	}

	if (n > 3) {
		printf("mmc %u error cannot set clock to %u\n", mmc_no,
		       hz);
		return -1;
	}

	/* determine delays */
	if (hz <= 400000) {
		oclk_dly = 0;
		sclk_dly = 0;
	} else if (hz <= 25000000) {
		oclk_dly = 0;
		sclk_dly = 5;
#ifdef CONFIG_SUN9I
	} else if (hz <= 50000000) {
		oclk_dly = 5;
		sclk_dly = 4;
	} else {
		/* hz > 50000000 */
		oclk_dly = 2;
		sclk_dly = 4;
#else
	} else if (hz <= 50000000) {
		oclk_dly = 3;
		sclk_dly = 4;
	} else {
		/* hz > 50000000 */
		oclk_dly = 1;
		sclk_dly = 4;
#endif
	}

	if (new_mode) {
#ifdef CONFIG_SD_SUNXI_NEW_MODE
		val = CCM_MMC_CTRL_MODE_SEL_NEW;
		__raw_setl(SUNXI_MMC_NTSR_MODE_SEL_NEW,
			   &(mmc->ntsr));
#endif
	} else {
		val = CCM_MMC_CTRL_OCLK_DLY(oclk_dly) |
			CCM_MMC_CTRL_SCLK_DLY(sclk_dly);
	}

	__raw_writel(CCM_MMC_CTRL_ENABLE| pll | CCM_MMC_CTRL_N(n) |
		     CCM_MMC_CTRL_M(div) | val,
		     sunxi_sd_clk_cfg[mmc_no]);

	printf("mmc %u set mod-clk req %u parent %u n %u m %u rate %u\n",
	       mmc_no, hz, pll_hz, 1u << n, div, pll_hz / (1u << n) / div);

	return 0;
}

static int sunxi_sd_update_clk(uint8_t mmc_no)
{
	struct sunxi_mmc *mmc = SUNXI_SMHC(mmc_no);
	unsigned int cmd;
	unsigned timeout_msecs = 2000;

	cmd = SUNXI_MMC_CMD_START |
	      SUNXI_MMC_CMD_UPCLK_ONLY |
	      SUNXI_MMC_CMD_WAIT_PRE_OVER;
	__raw_writel(cmd, &mmc->cmd);
	while (__raw_readl(&mmc->cmd) & SUNXI_MMC_CMD_START) {
		if (!timeout_msecs--)
			return -1;
		mdelay(1);
	}

	/* clock update sets various irq status bits, clear these */
	__raw_writel(__raw_readl(&mmc->rint), &mmc->rint);
	return 0;
}

static int sunxi_sd_config_clock(uint8_t mmc_no, uint32_t clock)
{
	struct sunxi_mmc *mmc = SUNXI_SMHC(mmc_no);
	unsigned rval = __raw_readl(&mmc->clkcr);

	/* Disable Clock */
	rval &= ~SUNXI_MMC_CLK_ENABLE;
	__raw_writel(rval, &mmc->clkcr);
	if (sunxi_sd_update_clk(mmc_no))
		return -1;

	/* Set mod_clk to new rate */
	if (sunxi_sd_set_mod_clk(mmc_no, clock))
		return -1;

	/* Clear internal divider */
	rval &= ~SUNXI_MMC_CLK_DIVIDER_MASK;
	__raw_writel(rval, &mmc->clkcr);

	/* Re-enable Clock */
	rval |= SUNXI_MMC_CLK_ENABLE;
	__raw_writel(rval, &mmc->clkcr);
	if (sunxi_sd_update_clk(mmc_no))
		return -1;
	return 0;
}

static void sunxi_sd_config_gpio(int sdc)
{
	unsigned int pin;
	__unused int pins;

	switch (sdc) {
	case 0:
		/* SDC0: PF0-PF5 */
		for (pin = SUNXI_GPF(0); pin <= SUNXI_GPF(5); pin++) {
			printf("pin %d=%08x\n", pin, SUNXI_GPF_SDC0);
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPF_SDC0);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
		break;

#ifdef CONFIG_SMHC_BOOT_1
	case 1:
		pins = sunxi_name_to_gpio_bank(CONFIG_SMHC_1_PINS);
#if defined(CONFIG_SUN4I) || defined(CONFIG_SUN7I) || \
    defined(CONFIG_SUN8I_R40)
		if (pins == SUNXI_GPIO_H) {
			/* SDC1: PH22-PH-27 */
			for (pin = SUNXI_GPH(22); pin <= SUNXI_GPH(27); pin++) {
				sunxi_gpio_set_cfgpin(pin, SUN4I_GPH_SDC1);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
			}
		} else {
			/* SDC1: PG0-PG5 */
			for (pin = SUNXI_GPG(0); pin <= SUNXI_GPG(5); pin++) {
				sunxi_gpio_set_cfgpin(pin, SUN4I_GPG_SDC1);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
			}
		}
#elif defined(CONFIG_SUN5I)
		/* SDC1: PG3-PG8 */
		for (pin = SUNXI_GPG(3); pin <= SUNXI_GPG(8); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUN5I_GPG_SDC1);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
#elif defined(CONFIG_SUN6I)
		/* SDC1: PG0-PG5 */
		for (pin = SUNXI_GPG(0); pin <= SUNXI_GPG(5); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUN6I_GPG_SDC1);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
#elif defined(CONFIG_SUN8I)
		if (pins == SUNXI_GPIO_D) {
			/* SDC1: PD2-PD7 */
			for (pin = SUNXI_GPD(2); pin <= SUNXI_GPD(7); pin++) {
				sunxi_gpio_set_cfgpin(pin, SUN8I_GPD_SDC1);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
			}
		} else {
			/* SDC1: PG0-PG5 */
			for (pin = SUNXI_GPG(0); pin <= SUNXI_GPG(5); pin++) {
				sunxi_gpio_set_cfgpin(pin, SUN8I_GPG_SDC1);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
			}
		}
#endif
		break;
#endif
#ifdef CONFIG_SMHC_BOOT_2
	case 2:
		pins = sunxi_name_to_gpio_bank(CONFIG_SMHC_2_PINS);
#if defined(CONFIG_SUN4I) || defined(CONFIG_SUN7I)
		/* SDC2: PC6-PC11 */
		for (pin = SUNXI_GPC(6); pin <= SUNXI_GPC(11); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPC_SDC2);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
#elif defined(CONFIG_SUN5I)
		if (pins == SUNXI_GPIO_E) {
			/* SDC2: PE4-PE9 */
			for (pin = SUNXI_GPE(4); pin <= SUNXI_GPD(9); pin++) {
				sunxi_gpio_set_cfgpin(pin, SUN5I_GPE_SDC2);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
			}
		} else {
			/* SDC2: PC6-PC15 */
			for (pin = SUNXI_GPC(6); pin <= SUNXI_GPC(15); pin++) {
				sunxi_gpio_set_cfgpin(pin, SUNXI_GPC_SDC2);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
			}
		}
#elif defined(CONFIG_SUN6I)
		if (pins == SUNXI_GPIO_A) {
			/* SDC2: PA9-PA14 */
			for (pin = SUNXI_GPA(9); pin <= SUNXI_GPA(14); pin++) {
				sunxi_gpio_set_cfgpin(pin, SUN6I_GPA_SDC2);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
			}
		} else {
			/* SDC2: PC6-PC15, PC24 */
			for (pin = SUNXI_GPC(6); pin <= SUNXI_GPC(15); pin++) {
				sunxi_gpio_set_cfgpin(pin, SUNXI_GPC_SDC2);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
			}

			sunxi_gpio_set_cfgpin(SUNXI_GPC(24), SUNXI_GPC_SDC2);
			sunxi_gpio_set_pull(SUNXI_GPC(24), SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(SUNXI_GPC(24), 2);
		}
#elif defined(CONFIG_SUN8I_R40)
		/* SDC2: PC6-PC15, PC24 */
		for (pin = SUNXI_GPC(6); pin <= SUNXI_GPC(15); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPC_SDC2);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}

		sunxi_gpio_set_cfgpin(SUNXI_GPC(24), SUNXI_GPC_SDC2);
		sunxi_gpio_set_pull(SUNXI_GPC(24), SUNXI_GPIO_PULL_UP);
		sunxi_gpio_set_drv(SUNXI_GPC(24), 2);
#elif defined(CONFIG_SUN8I) || defined(CONFIG_SUN50I)
		/* SDC2: PC5-PC6, PC8-PC16 */
		for (pin = SUNXI_GPC(5); pin <= SUNXI_GPC(6); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPC_SDC2);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}

		for (pin = SUNXI_GPC(8); pin <= SUNXI_GPC(16); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPC_SDC2);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
#elif defined(CONFIG_SUN9I)
		/* SDC2: PC6-PC16 */
		for (pin = SUNXI_GPC(6); pin <= SUNXI_GPC(16); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPC_SDC2);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
#endif
		break;
#endif
#ifdef CONFIG_SMHC_BOOT_3
	case 3:
		pins = sunxi_name_to_gpio_bank(CONFIG_SMHC_3_PINS);

#if defined(CONFIG_SUN4I) || defined(CONFIG_SUN7I) || \
    defined(CONFIG_SUN8I_R40)
		/* SDC3: PI4-PI9 */
		for (pin = SUNXI_GPI(4); pin <= SUNXI_GPI(9); pin++) {
			sunxi_gpio_set_cfgpin(pin, SUNXI_GPI_SDC3);
			sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(pin, 2);
		}
#elif defined(CONFIG_SUN6I)
		if (pins == SUNXI_GPIO_A) {
			/* SDC3: PA9-PA14 */
			for (pin = SUNXI_GPA(9); pin <= SUNXI_GPA(14); pin++) {
				sunxi_gpio_set_cfgpin(pin, SUN6I_GPA_SDC3);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
			}
		} else {
			/* SDC3: PC6-PC15, PC24 */
			for (pin = SUNXI_GPC(6); pin <= SUNXI_GPC(15); pin++) {
				sunxi_gpio_set_cfgpin(pin, SUN6I_GPC_SDC3);
				sunxi_gpio_set_pull(pin, SUNXI_GPIO_PULL_UP);
				sunxi_gpio_set_drv(pin, 2);
			}

			sunxi_gpio_set_cfgpin(SUNXI_GPC(24), SUN6I_GPC_SDC3);
			sunxi_gpio_set_pull(SUNXI_GPC(24), SUNXI_GPIO_PULL_UP);
			sunxi_gpio_set_drv(SUNXI_GPC(24), 2);
		}
#endif
		break;
#endif
	default:
		break;
	}
}

static int sunxi_sd_getcd_gpio(int sdc)
{
        switch (sdc) {
        case 0: return sunxi_name_to_gpio("PF6");
        case 1: return sunxi_name_to_gpio("");
        case 2: return sunxi_name_to_gpio("");
        case 3: return sunxi_name_to_gpio("");
        }
        return -EINVAL;
}

int sunxi_sd_detect_card(uint8_t mmc_no)
{
	int cd_pin;

	cd_pin = sunxi_sd_getcd_gpio(mmc_no);
	if (cd_pin < 0)
		return 1;
	return !sunxi_gpio_input(cd_pin);
}

void sunxi_sd_set_clock(uint8_t mmc_no, uint32_t clock)
{
	if (clock > SUNXI_SD_CLK_MAX)
		clock = SUNXI_SD_CLK_MAX;
	if (clock < SUNXI_SD_CLK_MIN)
		clock = SUNXI_SD_CLK_MIN;
	sunxi_sd_config_clock(mmc_no, clock);
}

void sunxi_sd_set_width(uint8_t mmc_no, uint8_t width)
{
	struct sunxi_mmc *mmc = SUNXI_SMHC(mmc_no);

	/* Change bus width */
	if (width == 8)
		__raw_writel(0x2, &mmc->width);
	else if (width == 4)
		__raw_writel(0x1, &mmc->width);
	else
		__raw_writel(0x0, &mmc->width);
}

static int sunxi_sd_rint_wait(uint8_t mmc_no,
			      timeout_t timeout_msecs, uint32_t done_bit,
			      const char *what)
{
	struct sunxi_mmc *mmc = SUNXI_SMHC(mmc_no);
	uint32_t status;

	do {
		status = __raw_readl(&mmc->rint);
		if (!timeout_msecs-- ||
		    (status & SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT)) {
			printf("%s timeout %x\n", what,
			       status & SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT);
			return -ETIME;
		}
		mdelay(1);
	} while (!(status & done_bit));
	return 0;
}

static int sunxi_sd_transfer_data(void)
{
	uint8_t mmc_no = mmc_sid;
	struct sunxi_mmc *mmc = SUNXI_SMHC(mmc_no);
	uint8_t type = mmc_get_block_data();
	uint32_t *buff = mmc_slot_ctrl.block_data;
	size_t byte_cnt = 
		mmc_slot_ctrl.block_len * mmc_slot_ctrl.block_cnt;
	unsigned i;
	unsigned timeout_usecs = (byte_cnt >> 8) * 1000;
	uint32_t status_bit;

	if (timeout_usecs < 2000000)
		timeout_usecs = 2000000;

	if (type == MMC_SLOT_BLOCK_READ)
		status_bit = SUNXI_MMC_STATUS_FIFO_EMPTY;
	else
		status_bit = SUNXI_MMC_STATUS_FIFO_FULL;

	/* Always read / write data through the CPU */
	__raw_setl(SUNXI_MMC_GCTRL_ACCESS_BY_AHB, &mmc->gctrl);

	for (i = 0; i < (byte_cnt >> 2); i++) {
		while (__raw_readl(&mmc->status) & status_bit) {
			if (!timeout_usecs--)
				return -ETIME;
			udelay(1);
		}

		if (type == MMC_SLOT_BLOCK_READ)
			buff[i] = __raw_readl(&mmc->fifo);
		else
			__raw_writel(buff[i], &mmc->fifo);
	}
	return 0;
}

void mmc_hw_send_command(uint8_t cmd, uint32_t arg)
{
	uint8_t mmc_no = mmc_sid;
	struct sunxi_mmc *mmc = SUNXI_SMHC(mmc_no);
	uint8_t resp = mmc_slot_ctrl.rsp;
	uint8_t type = mmc_get_block_data();
	size_t bytecnt = 
		mmc_slot_ctrl.block_len * mmc_slot_ctrl.block_cnt;
	unsigned int timeout_msecs;
	uint32_t status;
	uint32_t cmdval = SUNXI_MMC_CMD_START;
	int error = 0;

#ifdef CONFIG_MMC_STO
	BUG_ON(cmd == MMC_CMD_STOP_TRANSMISSION);
#endif

	if (!cmd)
		cmdval |= SUNXI_MMC_CMD_SEND_INIT_SEQ;
	if (resp & MMC_RSP_PRESENT)
		cmdval |= SUNXI_MMC_CMD_RESP_EXPIRE;
	if (resp & MMC_RSP_136)
		cmdval |= SUNXI_MMC_CMD_LONG_RESPONSE;
	if (resp & MMC_RSP_CRC)
		cmdval |= SUNXI_MMC_CMD_CHK_RESPONSE_CRC;

	if (type) {
#if 0
		BUG_ON(!IS_ALIGNED(mmc_slot_ctrl.block, 4));
#endif

		cmdval |= SUNXI_MMC_CMD_DATA_EXPIRE|SUNXI_MMC_CMD_WAIT_PRE_OVER;
		if (type == MMC_SLOT_BLOCK_WRITE)
			cmdval |= SUNXI_MMC_CMD_WRITE;
		if (mmc_slot_ctrl.block_cnt > 1)
			cmdval |= SUNXI_MMC_CMD_AUTO_STOP;
		__raw_writel(mmc_slot_ctrl.block_len, &mmc->blksz);
		__raw_writel(bytecnt, &mmc->bytecnt);
	}

	printf("mmc %d, cmd %d(0x%08x), arg 0x%08x\n", mmc_no,
	       cmd, cmdval | cmd, arg);
	__raw_writel(arg, &mmc->arg);
	__raw_writel(cmdval | cmd, &mmc->cmd);

	/* Transfer data and check status:
	 * STATREG[2] : FIFO empty
	 * STATREG[3] : FIFO full
	 */
	if (type) {
		int ret = 0;

		ret = sunxi_sd_transfer_data();
		if (ret) {
			error = __raw_readl(&mmc->rint) &
				SUNXI_MMC_RINT_INTERRUPT_ERROR_BIT;
			error = -ETIME;
			goto out;
		}
	}

	error = sunxi_sd_rint_wait(mmc_no, 1000,
				   SUNXI_MMC_RINT_COMMAND_DONE, "cmd");
	if (error)
		goto out;

	if (type) {
		timeout_msecs = 120;
		error = sunxi_sd_rint_wait(mmc_no, timeout_msecs,
					   mmc_slot_ctrl.block_cnt > 1 ?
					   SUNXI_MMC_RINT_AUTO_COMMAND_DONE :
					   SUNXI_MMC_RINT_DATA_OVER,
					   "data");
		if (error)
			goto out;
	}

	if (resp & MMC_RSP_BUSY) {
		timeout_msecs = 2000;
		do {
			status = __raw_readl(&mmc->status);
			if (!timeout_msecs--) {
				error = -ETIME;
				goto out;
			}
			mdelay(1);
		} while (status & SUNXI_MMC_STATUS_CARD_DATA_BUSY);
	}

	mmc_cmd_success();
	return;

out:
	mmc_cmd_failure(MMC_ERR_TIMEOUT);
	__raw_writel(SUNXI_MMC_GCTRL_RESET, &mmc->gctrl);
	sunxi_sd_update_clk(mmc_no);
	__raw_writel(0xffffffff, &mmc->rint);
	__raw_writel(__raw_readl(&mmc->gctrl) | SUNXI_MMC_GCTRL_FIFO_RESET,
		     &mmc->gctrl);
}

bool sunxi_sd_card_busy(uint8_t mmc_no)
{
	struct sunxi_mmc *mmc = SUNXI_SMHC(mmc_no);
	uint32_t status;

	status = __raw_readl(&mmc->status);
	return !!(status & SUNXI_MMC_STATUS_CARD_DATA_BUSY);
}

void sunxi_sd_decode_reg(uint8_t *resp, uint32_t *addr)
{
	uint32_t tmp = __raw_readl(addr);

	resp[3] = HIBYTE(HIWORD(tmp));
	resp[2] = LOBYTE(HIWORD(tmp));
	resp[1] = HIBYTE(LOWORD(tmp));
	resp[0] = LOBYTE(LOWORD(tmp));
}

void mmc_hw_recv_response(uint8_t *resp, uint8_t size)
{
	uint8_t mmc_no = mmc_sid;
	struct sunxi_mmc *mmc = SUNXI_SMHC(mmc_no);
	uint8_t rsp = mmc_slot_ctrl.rsp;

	if (rsp & MMC_RSP_136) {
		sunxi_sd_decode_reg(resp, &mmc->resp3);
		sunxi_sd_decode_reg(resp + 4, &mmc->resp2);
		sunxi_sd_decode_reg(resp + 8, &mmc->resp1);
		sunxi_sd_decode_reg(resp + 12, &mmc->resp0);
		printf("mmc resp 0x%02x %02x %02x %02x\n"
		       "         0x%02x %02x %02x %02x\n"
		       "         0x%02x %02x %02x %02x\n"
		       "         0x%02x %02x %02x %02x\n",
		       resp[15], resp[14], resp[13], resp[12],
		       resp[11], resp[10], resp[9], resp[8],
		       resp[7], resp[6], resp[5], resp[4],
		       resp[3], resp[2], resp[1], resp[0]);
	} else {
		sunxi_sd_decode_reg(resp, &mmc->resp0);
		printf("mmc resp 0x%02x %02x %02x %02x\n",
		       resp[3], resp[2], resp[1], resp[0]);
	}
	__raw_writel(0xffffffff, &mmc->rint);
	__raw_writel(__raw_readl(&mmc->gctrl) | SUNXI_MMC_GCTRL_FIFO_RESET,
		     &mmc->gctrl);
}

void sunxi_sd_ctrl_init(uint8_t mmc_no)
{
        struct sunxi_ccm_reg *ccm = SUNXI_CCU;
	struct sunxi_mmc *mmc = SUNXI_SMHC(mmc_no);
	int cd_pin;
	__unused mmc_slot_t sslot;

	sunxi_sd_config_gpio(mmc_no);

	/* mmc_resource_init */
	cd_pin = sunxi_sd_getcd_gpio(mmc_no);
	if (cd_pin >= 0) {
		sunxi_gpio_set_pull(cd_pin, SUNXI_GPIO_PULL_UP);
		sunxi_gpio_set_cfgpin(cd_pin, SUNXI_GPIO_INPUT);
	}

	/* sunxi_mmc_init */
	__raw_setl(1 << AHB_GATE_OFFSET_MMC(mmc_no),
		   &ccm->ahb_gate0);
	__raw_setl(1 << AHB_RESET_OFFSET_MMC(mmc_no),
		   &ccm->ahb_reset0_cfg);
	sunxi_sd_set_mod_clk(mmc_no, 24000000);
	printf("SD detect: %s\n",
	       sunxi_sd_detect_card(mmc_no) ? "TRUE" : "FALSE");
	/* Reset controller */
	__raw_writel(SUNXI_MMC_GCTRL_RESET, &mmc->gctrl);
	mdelay(1);
	sunxi_sd_set_width(mmc_no, 1);
	sunxi_sd_set_clock(mmc_no, 1);

	sslot = mmc_slot_save(mmc_no);
	mmc_slot_ctrl.host_ocr = SD_OCR_HCS | MMC_OCR_32_33 | MMC_OCR_33_34;
	mmc_slot_restore(sslot);
	mdelay(1);
}
