/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)sdhc.c: secure digital host controller (SDHC) implementation
 * $Id: sdhc.c,v 1.1 2019-10-08 18:46:00 zhenglv Exp $
 */

#include <target/mmc.h>
#include <target/delay.h>
#include <target/jiffies.h>

#define SDHC_TRANSFER_TOUT_US		(2*1000*1000)

#ifdef CONFIG_SDHC_ACCEL
#define sdhc_udelay(us)			do { } while (0)
#else
#define sdhc_udelay(us)			udelay(us)
#endif

#if NR_MMC_SLOTS > 1
struct sdhc_host sdhc_hosts[NR_MMC_SLOTS];
#define sdhc_host_ctrl sdhc_hosts[mmc_sid]

mmc_slot_t sdhc_irq2sid(irq_t irq)
{
	mmc_slot_t slot;

	for (slot = 0; slot < NR_MMC_SLOTS; slot++) {
		if (sdhc_hosts[slot].irq = irq)
			return slot;
	}
	return INVALID_MMC_SLOT;
}
#else
struct sdhc_host sdhc_host_ctrl;
#define sdhc_irq2sid(irq)		mmc_sid
#endif

#define sdhc_spec()			\
	SDHC_SPECIFICATION_VERSION_NUMBER(sdhc_host_ctrl.version)

static void sdhc_handle_irq(irq_t irq);

static void sdhc_transfer_pio(uint32_t *block)
{
	uint8_t type = mmc_get_block_data();
	uint32_t *dat = block;
	uint16_t len = mmc_slot_ctrl.block_len / 4;

	while (len) {
		if (type == MMC_SLOT_BLOCK_READ)
			*dat = __raw_readl(SDHC_BUFFER_DATA_PORT(mmc_sid));
		else
			__raw_writel(*dat, SDHC_BUFFER_DATA_PORT(mmc_sid));
		dat++;
		len--;
	}

#ifdef CONFIG_MMC_DEBUG
	if (mmc_slot_ctrl.block_len < 512) {
		int i;
		con_dbg("sdhc: DATA: \n");
		for (i = 0; i < mmc_slot_ctrl.block_len; i++)
			con_dbg("%02x ", ((uint8_t *)block)[i]);
		con_dbg("\n");
	}
#endif
}

#ifdef CONFIG_SDHC_SDMA
#define sdhc_dma_boundary()		\
	SDHC_SDMA_BUFFER_BOUNDARY(SDHC_DEFAULT_BOUNDARY_ARG)

static void sdhc_dma_config(uint8_t mode)
{
	sdhc_config_dma(mmc_sid, SDHC_SDMA);
}

static bool sdhc_dma_open(void)
{
	uint32_t start_addr;

	/* TODO: DMA remap */
	mmc_slot_ctrl.dat = (uint32_t)mmc_slot_ctrl.block_data;
	__raw_writel(mmc_slot_ctrl.dat, SDHC_SDMA_SYSTEM_ADDRESS(mmc_sid));
}

static void sdhc_dma_close(void)
{
	mmc_slot_ctrl.dat &= ~(SDHC_DEFAULT_BOUNDARY_SIZE - 1);
	mmc_slot_ctrl.dat += SDHC_DEFAULT_BOUNDARY_SIZE;
	__raw_writel(mmc_slot_ctrl.dat, SDHC_SDMA_SYSTEM_ADDRESS(mmc_sid));
	__raw_writel(start_addr, SDHC_SDMA_SYSTEM_ADDRESS(mmc_sid));
}
#else
#define sdhc_dma_boundary()		0
#define sdhc_dma_config(mode)		do { } while (0)
#define sdhc_dma_open()			false
#define sdhc_dma_close()		do { } while (0)
#endif

/* No command will be sent by driver if card is busy, so driver must wait
 * for card ready state.
 * Every time when card is busy after timeout then (last) timeout value will be
 * increased twice but only if it doesn't exceed global defined maximum.
 * Each function call will use last timeout value.
 */
static void sdhc_wait_transfer(void)
{
	while (sdhc_state_present(mmc_sid, SDHC_COMMAND_INHIBIT));
}

void sdhc_send_command(uint8_t cmd, uint32_t arg)
{
	uint8_t type = mmc_get_block_data();
	__unused size_t trans_bytes;
	uint32_t mask, flags, mode;
	uint8_t rsp = mmc_slot_ctrl.rsp;

	sdhc_wait_transfer();
	sdhc_start_transfer();

	mask = SDHC_COMMAND_COMPLETE;
	if (!(rsp & MMC_RSP_PRESENT))
		flags = SDHC_RESPONSE_TYPE_SELECT(SDHC_NO_RESPONSE);
	else if (rsp & MMC_RSP_136)
		flags = SDHC_RESPONSE_TYPE_SELECT(SDHC_RESPONSE_LENGTH_136);
	else if (rsp & MMC_RSP_BUSY) {
		flags = SDHC_RESPONSE_TYPE_SELECT(SDHC_RESPONSE_LENGTH_48_BUSY);
		if (type)
			mask |= SDHC_TRANSFER_COMPLETE;
	} else
		flags = SDHC_RESPONSE_TYPE_SELECT(SDHC_RESPONSE_LENGTH_48);

	if (rsp & MMC_RSP_CRC)
		flags |= SDHC_COMMAND_CRC_CHECK_ENABLE;
	if (rsp & MMC_RSP_OPCODE)
		flags |= SDHC_COMMAND_INDEX_CHECK_ENABLE;

	if (type)
		flags |= SDHC_DATA_PRESENT_SELECT;

	/* Set Transfer mode regarding to data flag */
	if (type) {
		__raw_writeb(0xE, SDHC_TIMEOUT_CONTROL(mmc_sid));
		mode = SDHC_BLOCK_COUNT_ENABLE;
		trans_bytes = mmc_slot_ctrl.block_cnt *
			      mmc_slot_ctrl.block_len;
		if (mmc_slot_ctrl.block_cnt > 1)
			mode |= SDHC_MULTI_SINGLE_BLOCK_SELECT;
		if (type == MMC_SLOT_BLOCK_READ)
			mode |= SDHC_DATA_TRANSFER_DIRECTION_SELECT;

		if (sdhc_dma_open())
			mode |= SDHC_DMA_ENABLE;
#ifdef CONFIG_MMC_DEBUG
		con_dbg("sdhc: BLOCK_SIZE: %04lx\n",
			SDHC_TRANSFER_BLOCK_SIZE(mmc_slot_ctrl.block_len));
		con_dbg("sdhc: BLOCK_COUNT: %04lx\n", mmc_slot_ctrl.block_cnt);
		con_dbg("sdhc: TRANSFER_MODE: %04lx\n", mode);
#endif
		__raw_writew(sdhc_dma_boundary() |
			SDHC_TRANSFER_BLOCK_SIZE(mmc_slot_ctrl.block_len),
			SDHC_BLOCK_SIZE(mmc_sid));
		__raw_writew(mmc_slot_ctrl.block_cnt,
			     SDHC_16BIT_BLOCK_COUNT(mmc_sid));
		__raw_writew(mode, SDHC_TRANSFER_MODE(mmc_sid));
	} else if (rsp & MMC_RSP_BUSY) {
		__raw_writeb(0xE, SDHC_TIMEOUT_CONTROL(mmc_sid));
	}

#ifdef CONFIG_MMC_DEBUG
	con_dbg("sdhc: COMMAND: %04lx\n", SDHC_COMMAND(mmc_sid));
	con_dbg("sdhc: ARGUMENT: %08lx\n", arg);
#endif
	__raw_writel(arg, SDHC_ARGUMENT(mmc_sid));
	__raw_writew(SDHC_CMD(cmd, flags), SDHC_COMMAND(mmc_sid));

	sdhc_host_ctrl.trans = SDHC_TRANS_CMD;
	sdhc_host_ctrl.irq_complete_mask = mask;
}

/* TODO: This function is endianess related. */
static void sdhc_decode_reg(uint8_t *resp, uint8_t size, uint32_t reg)
{
	if (size > 0)
		resp[0] = HIBYTE(HIWORD(reg));
	if (size > 1)
		resp[1] = LOBYTE(HIWORD(reg));
	if (size > 2)
		resp[2] = HIBYTE(LOWORD(reg));
	if (size > 3)
		resp[3] = LOBYTE(LOWORD(reg));
}

void sdhc_recv_response(uint8_t *resp, uint8_t size)
{
	uint8_t type = mmc_get_block_data();
	uint8_t rsp = mmc_slot_ctrl.rsp;
	int i;
	uint32_t reg;
	uint8_t len;

	if (rsp & MMC_RSP_136) {
		/* CRC is stripped so we need to do some shifting. */
		len = 0;
		for (i = 0; i < 4; i++) {
			reg = __raw_readl(
				SDHC_RESPONSE32(mmc_sid, 3-i)) << 8;
			if (i != 3)
				reg |= __raw_readb(
					SDHC_RESPONSE8(mmc_sid, (3-i)*4-1));
			sdhc_decode_reg(resp + len,
					size >= len ? 4 : 0, reg);
			len += 4;
		}
	} else {
		reg = __raw_readl(SDHC_RESPONSE32(mmc_sid, 0));
		sdhc_decode_reg(resp, size, reg);
	}
	if (type) {
		sdhc_host_ctrl.trans = SDHC_TRANS_DAT;
		sdhc_host_ctrl.irq_complete_mask =
			SDHC_BUFFER_READ_READY | SDHC_BUFFER_WRITE_READY;
	} else
		sdhc_stop_transfer();

#ifdef CONFIG_MMC_DEBUG
	if (size > 0) {
		con_dbg("sdhc: RESPONSE: \n");
		for (i = 0; i < size; i++)
			con_dbg("%02x ", resp[i]);
		con_dbg("\n");
	}
#endif
}

void sdhc_tran_data(uint8_t *dat, uint32_t len, uint16_t cnt)
{
	sdhc_dma_config(SDHC_SDMA);
}

bool sdhc_card_busy(void)
{
	return false;
}

static void sdhc_set_power(uint8_t power)
{
	if (power >= MMC_OCR_MAX_VOLTAGES) {
		sdhc_power_off(mmc_sid);
		return;
	}

	switch (1 << power) {
	case MMC_OCR_170_195:
		sdhc_power_on_vdd1(mmc_sid, SDHC_SD_BUS_POWER_180);
		sdhc_power_on_vdd2(mmc_sid, SDHC_SD_BUS_POWER_180);
		break;
	case MMC_OCR_29_30:
	case MMC_OCR_30_31:
		sdhc_power_on_vdd1(mmc_sid, SDHC_SD_BUS_POWER_300);
		break;
	case MMC_OCR_32_33:
	case MMC_OCR_33_34:
		sdhc_power_on_vdd1(mmc_sid, SDHC_SD_BUS_POWER_330);
		break;
	default:
		sdhc_power_off(mmc_sid);
		break;
	}
}

void sdhc_start_clock(void)
{
	sdhc_set_clock_step(mmc_sid, SDHC_CLOCK_ENABLE);
}

void sdhc_stop_clock(void)
{
	sdhc_wait_transfer();
	sdhc_clear_clock_step(mmc_sid, SDHC_CLOCK_ENABLE);
}

static void sdhc_clock_stabilise(void)
{
	while (!(__raw_readw(SDHC_CLOCK_CONTROL(mmc_sid)) &
	       SDHC_INTERNAL_CLOCK_STABLE));
}

static void sdhc_enable_clock(void)
{
	sdhc_set_clock_step(mmc_sid, SDHC_INTERNAL_CLOCK_ENABLE);
	sdhc_clock_stabilise();
}

#ifdef CONFIG_SDHC_SPEC_410
static void sdhc_enable_pll(void)
{
	sdhc_set_clock_step(mmc_sid, SDHC_PLL_ENABLE);
	sdhc_clock_stabilise();
}
static void sdhc_disable_pll(void)
{
	sdhc_clear_clock_step(mmc_sid, SDHC_PLL_ENABLE);
}
#else
#define sdhc_enable_pll()	do { } while (0)
#define sdhc_disable_pll()	do { } while (0)
#endif

bool sdhc_set_clock(uint32_t clock)
{
	uint32_t div, clk = 0;

	sdhc_stop_clock();
	sdhc_disable_pll();

	/* 1. calculate a divisor */
	if (sdhc_spec() >= SDHC_SPEC_300) {
		/* Host Controller supports Programmable Clock Mode? */
		if (sdhc_host_ctrl.clk_mul) {
			for (div = 1;
			     div <= SDHC_10BIT_PROGRAMMABLE_CLOCK_MAX_DIV;
			     div++) {
				if ((sdhc_host_ctrl.max_clk / div) <= clock)
					break;
			}

			clk = SDHC_CLOCK_GENERATOR_SELECT;
			div--;
		} else {
			/* Version 3.00 divisors must be a multiple of 2. */
			if (sdhc_host_ctrl.max_clk <= clock) {
				div = 0;
			} else {
				for (div = 2;
				     div <= SDHC_10BIT_DIVIDED_CLOCK_MAX_DIV;
				     div += 2) {
					if ((sdhc_host_ctrl.max_clk / div) <=
					    clock)
						break;
				}
				div >>= 1;
			}
		}
		clk |= SDHC_10BIT_DIVIDED_CLOCK(div);
	} else {
		/* Version 2.00 divisors must be a power of 2. */
		for (div = 1; div <= SDHC_8BIT_DIVIDED_CLOCK_MAX_DIV;
		     div <<= 1) {
			if ((sdhc_host_ctrl.max_clk / div) <= clock)
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
	sdhc_enable_clock();

	/* 4. set PLL Enable */
	sdhc_enable_pll();

	sdhc_start_clock();

	if (clock > 26000000)
		sdhc_enable_high_speed(mmc_sid);
	else
		sdhc_disable_high_speed(mmc_sid);
	return true;
}

void sdhc_set_width(uint8_t width)
{
	uint32_t ctrl;

	/* Set bus width */
	ctrl = __raw_readb(SDHC_HOST_CONTROL_1(mmc_sid));
	if (width == 8) {
		ctrl &= ~SDHC_DATA_TRANSFER_WIDTH;
		if ((sdhc_spec() >= SDHC_SPEC_300))
			ctrl |= SDHC_EXTENDED_DATA_TRANSFER_WIDTH;
	} else {
		if ((sdhc_spec() >= SDHC_SPEC_300))
			ctrl &= ~SDHC_EXTENDED_DATA_TRANSFER_WIDTH;
		if (width == 4)
			ctrl |= SDHC_DATA_TRANSFER_WIDTH;
		else
			ctrl &= ~SDHC_DATA_TRANSFER_WIDTH;
	}
	__raw_writeb(ctrl, SDHC_HOST_CONTROL_1(mmc_sid));
}

void sdhc_init(uint32_t f_min, uint32_t f_max, irq_t irq)
{
	uint32_t caps_0, caps_1;

	caps_0 = __raw_readl(SDHC_CAPABILITIES_0(mmc_sid));

#ifdef CONFIG_SDHC_SDMA
	BUG_ON(!(caps_0 & SDHC_CAP_SDMA_SUPPORT));
#endif
	sdhc_host_ctrl.version =
		__raw_readw(SDHC_HOST_CONTROLLER_VERSION(mmc_sid));

	/* Check whether the clock multiplier is supported or not */
	if (sdhc_spec() >= SDHC_SPEC_300) {
		caps_1 = __raw_readl(SDHC_CAPABILITIES_1(mmc_sid));
		sdhc_host_ctrl.clk_mul = SDHC_CAP_CLOCK_MULTIPLIER(caps_1);
	} else
		sdhc_host_ctrl.clk_mul = 0;

	if (sdhc_spec() >= SDHC_SPEC_300)
		sdhc_host_ctrl.max_clk =
			SDHC_CAP_8BIT_BASE_CLOCK_FREQUENCY(caps_0);
	else
		sdhc_host_ctrl.max_clk =
			SDHC_CAP_6BIT_BASE_CLOCK_FREQUENCY(caps_0);
	sdhc_host_ctrl.max_clk *= 1000000;
	if (sdhc_host_ctrl.clk_mul)
		sdhc_host_ctrl.max_clk *= sdhc_host_ctrl.clk_mul;

	BUG_ON(sdhc_host_ctrl.max_clk == 0);
	if (f_max && (f_max < sdhc_host_ctrl.max_clk))
		mmc_slot_ctrl.f_max = f_max;
	else
		mmc_slot_ctrl.f_max = sdhc_host_ctrl.max_clk;
	if (f_min)
		mmc_slot_ctrl.f_min = f_min;
	else {
		if (sdhc_spec() >= SDHC_SPEC_300)
			mmc_slot_ctrl.f_min = mmc_slot_ctrl.f_max /
				SDHC_10BIT_DIVIDED_CLOCK_MAX_DIV;
		else
			mmc_slot_ctrl.f_min = mmc_slot_ctrl.f_max /
				SDHC_8BIT_DIVIDED_CLOCK_MAX_DIV;
	}
#ifdef CONFIG_MMC_DEBUG
	con_dbg("sdhc: clock: %dHz ~ %dHz\n",
		mmc_slot_ctrl.f_min, mmc_slot_ctrl.f_max);
#endif

	mmc_slot_ctrl.host_ocr = SD_OCR_HCS;
	if (caps_0 & SDHC_CAP_VOLTAGE_SUPPORT_330)
		mmc_slot_ctrl.host_ocr |= (MMC_OCR_32_33 | MMC_OCR_33_34);
	if (caps_0 & SDHC_CAP_VOLTAGE_SUPPORT_300)
		mmc_slot_ctrl.host_ocr |= (MMC_OCR_29_30 | MMC_OCR_30_31);
	if (caps_0 & SDHC_CAP_VOLTAGE_SUPPORT_180)
		mmc_slot_ctrl.host_ocr |= MMC_OCR_170_195;

	mmc_slot_ctrl.host_scr.bus_widths = 4;
#if 0
	MMC_MODE_HS_52MHz;
#endif

	/* Since Host Controller Version3.0 */
	if (sdhc_spec() >= SDHC_SPEC_300) {
		if (caps_0 & SDHC_8BIT_SUPPORT_FOR_EMBEDDED_DEVICE)
			mmc_slot_ctrl.host_scr.bus_widths = 8;
	}

	/* Mask all SDHC IRQ sources, let sdhc_irq_init() enables the
	 * required IRQ sources.
	 */
	sdhc_mask_all_irqs(mmc_sid);

	sdhc_software_reset(mmc_sid, SDHC_SOFTWARE_RESET_FOR_ALL);
	sdhc_set_power(__fls32(
			MMC_OCR_VOLTAGE_RANGE(mmc_slot_ctrl.host_ocr)));

	sdhc_host_ctrl.irq = irq;
	sdhc_host_ctrl.trans = SDHC_TRANS_NON;
}

void sdhc_err_failure(uint8_t err)
{
	if (sdhc_host_ctrl.trans == SDHC_TRANS_DAT)
		mmc_dat_failure(MMC_ERR_CARD_LOOSE_BUS);
	else if (sdhc_host_ctrl.trans == SDHC_TRANS_CMD)
		mmc_cmd_failure(err);
}

#ifdef SYS_REALTIME
static void sdhc_irq_handler(void)
{
	mmc_slot_t slot;
	__unused mmc_slot_t sslot;

	sslot = mmc_slot_save(slot);
	for (slot = 0; slot < NR_MMC_SLOTS; slot++) {
		sslot = mmc_slot_save(slot);
		sdhc_handle_irq(sdhc_host_ctrl.irq);
		mmc_slot_restore(sslot);
	}
}

void sdhc_irq_poll(void)
{
	sdhc_irq_handler();
}

#define sdhc_irq_ack()		do { } while (0)
#else
void sdhc_irq_init(void)
{
	irq_t irq = sdhc_host_ctrl.irq;

	irqc_configure_irq(irq, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(irq, sdhc_handle_irq);
	irqc_enable_irq(irq);
}

#define sdhc_irq_ack()		irqc_ack_irq(sdhc_host_ctrl.irq);
#endif

static void sdhc_handle_irq(irq_t irq)
{
	__unused mmc_slot_t slot = sdhc_irq2sid(irq);
	__unused mmc_slot_t sslot;
	uint32_t irqs;
	uint32_t mask;
	uint32_t *buf;

	sslot = mmc_slot_save(slot);
	irqs = sdhc_irq_status(mmc_sid);

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

	/* Handle error IRQs */
	if (irqs & SDHC_ERROR_INTERRUPT_MASK) {
		printf("sdhc: Error detected in status(0x%X)!\n", irqs);
		if (irqs & SDHC_COMMAND_INDEX_ERROR)
			sdhc_err_failure(MMC_ERR_ILLEGAL_COMMAND);
		else if (irqs & (SDHC_ERR_COMMAND_CRC_ERROR |
				 SDHC_ERR_DATA_CRC_ERROR))
			sdhc_err_failure(MMC_ERR_COM_CRC_ERROR);
		else if (irqs & (SDHC_ERR_COMMAND_END_BIT_ERROR |
				 SDHC_ERR_DATA_END_BIT_ERROR))
			sdhc_err_failure(MMC_ERR_CHECK_PATTERN);
#if 0
		else
			sdhc_err_failure(MMC_ERR_TIMEOUT);
#endif
		else if (irqs & SDHC_CURRENT_LIMIT_ERROR)
			sdhc_err_failure(MMC_ERR_CARD_NON_COMP_VOLT);
		else if (irqs & SDHC_TRANSFER_TIMEOUT)
			sdhc_err_failure(MMC_ERR_TIMEOUT);
		else if (irqs & SDHC_TRANSFER_FAILURE)
			sdhc_err_failure(MMC_ERR_CARD_LOOSE_BUS);
		else
			sdhc_err_failure(MMC_ERR_CARD_LOOSE_BUS);
		sdhc_clear_irq(mmc_sid, SDHC_ERROR_INTERRUPT_MASK);
		sdhc_stop_transfer();
		goto exit_irq;
	}
	/* Handle cmd line IRQs */
	if (sdhc_host_ctrl.trans == SDHC_TRANS_CMD) {
		mask = irqs & sdhc_host_ctrl.irq_complete_mask;
		if (mask) {
			sdhc_clear_irq(mmc_sid, mask);
			unraise_bits(sdhc_host_ctrl.irq_complete_mask, mask);
		}
		if (!sdhc_host_ctrl.irq_complete_mask) {
			mmc_cmd_success();
			goto exit_irq;
		}
	}
	/* Handle dat line IRQs */
	if (sdhc_host_ctrl.trans == SDHC_TRANS_DAT) {
		mask = SDHC_BUFFER_READ_ENABLE | SDHC_BUFFER_WRITE_ENABLE;
		buf = (uint32_t *)mmc_slot_ctrl.block_data;
		if (sdhc_state_present(mmc_sid, mask) &&
		    irqs & sdhc_host_ctrl.irq_complete_mask) {
			sdhc_clear_irq(mmc_sid,
				       sdhc_host_ctrl.irq_complete_mask);
			sdhc_transfer_pio(buf);
			if (mmc_blk_success())
				sdhc_stop_transfer();
			goto exit_irq;
		}
	}

exit_irq:
	sdhc_irq_ack();
	mmc_slot_restore(sslot);
}

void sdhc_detect_card(void)
{
	if (sdhc_state_present(mmc_sid, SDHC_CARD_INSERTED))
		mmc_event_raise(MMC_EVENT_CARD_INSERT);
	sdhc_enable_irq(mmc_sid, SDHC_CARD_DETECTION_MASK);
}

void sdhc_start_transfer(void)
{
	sdhc_host_ctrl.trans = SDHC_TRANS_NON;
	sdhc_clear_all_irqs(mmc_sid);
	sdhc_enable_irq(mmc_sid, SDHC_COMMAND_MASK | SDHC_TRANSFER_MASK);
}

void sdhc_stop_transfer(void)
{
	sdhc_disable_irq(mmc_sid, SDHC_COMMAND_MASK | SDHC_TRANSFER_MASK);
	sdhc_clear_all_irqs(mmc_sid);
	sdhc_host_ctrl.trans = SDHC_TRANS_NON;
	sdhc_software_reset(mmc_sid, SDHC_SOFTWARE_RESET_FOR_CMD_LINE);
	sdhc_software_reset(mmc_sid, SDHC_SOFTWARE_RESET_FOR_DAT_LINE);
}
