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
 * @(#)sdcard.c: FU540 (unleashed) QSPI SDCard adapter implementation
 * $Id: sdcard.c,v 1.1 2019-10-23 10:21:00 zhenglv Exp $
 */

#include <target/sd.h>
#include <target/spi.h>
#include <target/delay.h>

#if 0
/* Read operation conditions register (OCR) to check for availability of
 * block addressing mode.
 */
#if 0
static int sd_cmd58(void)
{
	int rc;

	rc = (sd_cmd(SD_CMD_READ_EXTRA_MULTI, 0) != 0x00);
	rc |= ((sd_dummy() & 0x80) != 0x80); /* Power up status */
	sd_dummy(); /* Supported voltages */
	sd_dummy(); /* Supported voltages */
	sd_dummy(); /* Supported voltages */
	sd_cmd_end();
	return rc;
}
#else
static int sd_cmd58(void)
{
	/* HACK: Disabled due to bugs. It is not strictly necessary to
	 * issue this command if we only support SD cards that support
	 * SDHC mode.
	 */
	return 0;
}
#endif
#endif

void mmc_hw_spi_init(void)
{
	board_init_clock();
	mdelay(1);
	sifive_qspi_chip_mode(SPI_FLASH_ID, QSPI_MODE_OFF);
	sifive_qspi_chip_select(SPI_FLASH_ID, 0);
}

#define APP_STATE_IDLE		0x00
#define APP_STATE_CMD8		0x01
#define APP_STATE_STBY		0x02
#define APP_VOLTAGE		UL(0x000001AA)

uint8_t app_state = APP_STATE_IDLE;

static int sd_cmd(uint8_t cmd, uint32_t arg)
{
	return __mmc_spi_send(cmd, arg);
}

static uint8_t sd_dummy(void)
{
	return spi_txrx(0xFF);
}

/* Check for SD version and supported voltages */
static int sd_cmd8(void)
{
	/* Check for high capacity cards
	 * Fail if card does not support SDHC
	 */
	int rc;
	uint8_t r;

	rc = (sd_cmd(SD_CMD_SEND_IF_COND, 0x000001AA) != SD_RESPONSE_IDLE);
	r = sd_dummy(); /* command version; reserved */
	/* NOTE: Should be R3 response while in SiFive sample,
	 * 4 bytes are checked..
	 */
	r = sd_dummy(); /* reserved */
	r = sd_dummy();
	rc |= ((r & 0xF) != 0x1); /* voltage */
	r = sd_dummy();
	rc |= (r != 0xAA); /* check pattern */
	mmc_spi_recv(NULL, 0);
	return rc;
}

/* Send app command. Used as prefix to app commands (ACMD) */
static void sd_cmd55(void)
{
	sd_cmd(SD_CMD_APP_CMD, 0);
	mmc_spi_recv(NULL, 0);
}

/* Start SDC initialization process */
static int sd_acmd41(void)
{
	uint8_t r;

	do {
		sd_cmd55();
		r = sd_cmd(SD_ACMD_SEND_OP_COND, 0x40000000); /* HCS = 1 */
		mmc_spi_recv(NULL, 0);
	} while (r == SD_RESPONSE_IDLE);
	return (r != 0x00);
}

void mmc_hw_spi_reset(void)
{
	if (app_state == APP_STATE_IDLE) {
		if (sd_cmd8())
			mmc_cmd_failure(MMC_ERR_CARD_NON_COMP_VOLT);
		else {
			app_state = APP_STATE_CMD8;
			mmc_cmd_success();
		}
	} else if (app_state == APP_STATE_CMD8) {
		if (sd_acmd41())
			mmc_cmd_failure(MMC_ERR_CARD_NON_COMP_VOLT);
		else {
			app_state = APP_STATE_STBY;
			mmc_spi_reset_success();
		}
	}
}
