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

mmc_rca_t mmc_spi_rca;
spi_t spi_card;

#ifdef CONFIG_MMC_DEBUG
#define MMC_SPI_DEBUG_ENABLE	_BV(0)
#define MMC_SPI_DEBUG_CMD	_BV(1)
#define MMC_SPI_DEBUG_RSP	_BV(2)
#define MMC_SPI_DEBUG_BUSY	_BV(3)
#define MMC_SPI_DEBUG_WAIT	_BV(4)
#define mmc_spi_debug_is_cmd()	(mmc_spi_debug & MMC_SPI_DEBUG_CMD)
#define mmc_spi_debug_is_rsp()	(mmc_spi_debug & MMC_SPI_DEBUG_RSP)
#define mmc_spi_debug_is_busy()	(mmc_spi_debug & MMC_SPI_DEBUG_BUSY)
#define mmc_spi_debug_is_wait()	(mmc_spi_debug & MMC_SPI_DEBUG_WAIT)

static uint8_t mmc_spi_debug;

static void mmc_spi_debug_on(void)
{
	mmc_spi_debug = MMC_SPI_DEBUG_ENABLE;
}

static void mmc_spi_debug_off(void)
{
	if (mmc_spi_debug & MMC_SPI_DEBUG_ENABLE)
		printf("\n");
	mmc_spi_debug = 0;
}

static void mmc_spi_debug_cmd(void)
{
	if (mmc_spi_debug & MMC_SPI_DEBUG_ENABLE) {
		if (!(mmc_spi_debug & MMC_SPI_DEBUG_WAIT))
			mmc_spi_debug |= MMC_SPI_DEBUG_WAIT;
	}
}

static void mmc_spi_debug_busy(uint8_t rx)
{
	if (mmc_spi_debug & MMC_SPI_DEBUG_ENABLE) {
		if (!(mmc_spi_debug & MMC_SPI_DEBUG_BUSY)) {
			printf("  BUSY: %02x", rx);
			mmc_spi_debug |= MMC_SPI_DEBUG_BUSY;
		} else
			printf(" %02x", mmc_slot_ctrl.r1);
	}
}

static void mmc_spi_debug_rsp(void)
{
	if (mmc_spi_debug & MMC_SPI_DEBUG_ENABLE) {
		if (!(mmc_spi_debug & MMC_SPI_DEBUG_RSP))
			printf("\n  RSP  : %02x", mmc_slot_ctrl.r1);
		mmc_spi_debug |= MMC_SPI_DEBUG_RSP;
	}
}

static void mmc_spi_debug_txrx(uint8_t tx, uint8_t rx)
{
	if (mmc_spi_debug & MMC_SPI_DEBUG_ENABLE) {
		if (mmc_spi_debug_is_rsp())
			printf(" %02x", rx);
		else if (!mmc_spi_debug_is_cmd()) {
			printf("  CMD%2d: %02x", (tx & 0x3f), tx);
			mmc_spi_debug |= MMC_SPI_DEBUG_CMD;
		} else if (!mmc_spi_debug_is_wait())
			printf(" %02x", tx);
	}
}
#else
#define mmc_spi_debug_on()		do { } while (0)
#define mmc_spi_debug_off()		do { } while (0)
#define mmc_spi_debug_cmd()		do { } while (0)
#define mmc_spi_debug_busy(rx)		do { } while (0)
#define mmc_spi_debug_rsp()		do { } while (0)
#define mmc_spi_debug_txrx(tx, rx)	do { } while (0)
#endif

uint8_t mmc_spi_txrx(uint8_t tx)
{
	uint8_t rx;

	rx = spi_txrx(tx);
	mmc_spi_debug_txrx(tx, rx);
	return rx;
}

void mmc_spi_select(mmc_rca_t rca)
{
	mmc_spi_rca = rca;
}

uint8_t mmc_spi_dummy(void)
{
	/* Responses are always led by R1, and 0x80 is illegal for R1 */
	return mmc_spi_txrx(0xFF);
}

static void mmc_spi_handle_r1(uint8_t r)
{
	mmc_slot_ctrl.r1 = r;

	if (r & MMC_R1_ERRORS) {
		if (r & MMC_R1_ILLEGAL_COMMAND)
			mmc_cmd_failure(MMC_ERR_ILLEGAL_COMMAND);
		else
			mmc_cmd_failure(MMC_ERR_CARD_LOOSE_BUS);
	} else {
		mmc_cmd_success();
	}
}

void mmc_spi_send(uint8_t cmd, uint32_t arg)
{
	uint8_t crc = 0;
	uint16_t n;
	uint8_t r;

	spi_select_device(spi_card);

	mmc_spi_dummy();
	mmc_spi_debug_on();
	mmc_spi_txrx(MMC_SPI_CMD(cmd));
	crc = mmc_crc7_update(crc, MMC_SPI_CMD(cmd));
	mmc_spi_txrx(arg >> 24);
	crc = mmc_crc7_update(crc, arg >> 24);
	mmc_spi_txrx(arg >> 16);
	crc = mmc_crc7_update(crc, arg >> 16);
	mmc_spi_txrx(arg >> 8);
	crc = mmc_crc7_update(crc, arg >> 8);
	mmc_spi_txrx(arg);
	crc = mmc_crc7_update(crc, arg >> 0);
	mmc_spi_txrx(crc | 0x01);
	mmc_spi_debug_cmd();

	n = 1000;
	do {
		r = mmc_spi_dummy();
		if (!(r & 0x80)) {
			mmc_spi_handle_r1(r);
			return;
		}
	} while (--n > 0);
	mmc_cmd_failure(MMC_ERR_CARD_LOOSE_BUS);
}

bool mmc_spi_busy(void)
{
	uint8_t r;

	if (mmc_slot_ctrl.rsp & MMC_RSP_BUSY) {
		r = mmc_spi_dummy();
		if (r == 0) {
			mmc_spi_debug_busy(r);
			mmc_event_raise(MMC_EVENT_CARD_BUSY);
			return true;
		} else
			unraise_bits(mmc_slot_ctrl.flags,
				     MMC_SLOT_CARD_IS_BUSY);
	}
	return false;
}

void mmc_spi_recv(uint8_t *resp, uint16_t len)
{
	uint8_t r;

	mmc_spi_debug_rsp();
	resp[len-1] = mmc_slot_ctrl.r1;
	len--;
	while (len) {
		r = mmc_spi_dummy();
		resp[len-1] = r;
		len--;
	}
	mmc_spi_debug_off();

	mmc_spi_dummy();
	spi_deselect_device();
}

static uint16_t mmc_spi_crc16_update(uint16_t crc, uint8_t data)
{
	/* CRC polynomial 0x11021 */
	crc = (uint8_t)(crc >> 8) | (crc << 8);
	crc ^= data;
	crc ^= (uint8_t)(crc >> 4) & 0xf;
	crc ^= crc << 12;
	crc ^= (crc & 0xff) << 5;
	return crc;
}

void mmc_spi_tran(uint8_t *dat, uint32_t len, uint16_t cnt)
{
	volatile uint8_t *p = dat;
	uint16_t crc, crc_exp;
	uint32_t i;
	uint8_t x;

	if (mmc_slot_ctrl.flags & MMC_SLOT_BLOCK_READ) {
		do {
			crc = 0;
			i = len;
			while (mmc_spi_dummy() != SD_DATA_TOKEN);
			do {
				x = mmc_spi_dummy();
				*p++ = x;
				crc = mmc_spi_crc16_update(crc, x);
			} while (--i > 0);
			crc_exp = ((uint16_t)mmc_spi_dummy() << 8);
			crc_exp |= mmc_spi_dummy();
			if (crc != crc_exp) {
				mmc_dat_failure(MMC_ERR_COM_CRC_ERROR);
				return;
			}
		} while (--cnt > 0);
	}
	mmc_dat_success();
}

static void mmc_spi_mode(void)
{
	int i;

	/* Switch SD mode to SPI mode:
	 * Set SD card pin DI high for 74+ cycles
	 * SD CMD pin is the same pin as SPI DI, so CMD = 0xff means assert
	 * DI high for 8 cycles.
	 */
	spi_select_device(spi_card);
	for (i = 10; i > 0; i--)
		mmc_spi_dummy();
	spi_deselect_device();
}

struct spi_device spid_mmc_spi = {
	.mode = SPI_MODE(SPI_MODE_0) | SPI_ORDER(SPI_MSB),
	.max_freq_khz = 20000,
	.chip = 0,
};

void mmc_spi_init(void)
{
	mmc_hw_spi_init();
	spi_card = spi_register_device(&spid_mmc_spi);
	mmc_spi_mode();
}
