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
 * @(#)sd_spi.c: secure digital (SD) SPI layer implementation
 * $Id: sd_spi.c,v 1.1 2019-11-01 18:00:00 zhenglv Exp $
 */

#include <target/mmc.h>

static void sd_spi_resp_r1(void)
{
	uint8_t r1;

	mmc_hw_recv_response(&r1, 1);
	if (mmc_slot_ctrl.cmd == MMC_CMD_APP_CMD &&
	    mmc_slot_ctrl.acmd == SD_ACMD_SEND_OP_COND) {
		if (!(r1 & MMC_R1_IN_IDLE_STATE))
			mmc_slot_ctrl.op_cond_sent = true;
	}
	raise_bits(mmc_slot_ctrl.flags, MMC_SLOT_CARD_STATUS_VALID);
}

static bool sd_spi_resp_r1b(void)
{
	sd_spi_resp_r1();
	return !mmc_hw_card_busy();
}

static void sd_spi_resp_r2(void)
{
	uint8_t r2[2];

	mmc_hw_recv_response(r2, 2);
}

static bool sd_spi_resp_r3(void)
{
	uint8_t r3[5];

	mmc_hw_recv_response(r3, 5);
	mmc_slot_ctrl.card_ocr = sd_decode_ocr(r3);
	if (!(mmc_slot_ctrl.card_ocr & MMC_OCR_BUSY)) {
		mmc_rsp_failure(MMC_ERR_CARD_IS_BUSY);
		return false;
	}
	mmc_slot_ctrl.ocr_valid = true;
	return true;
}

static bool sd_spi_resp_r7(void)
{
	uint8_t r7[5];

	mmc_hw_recv_response(r7, 5);
	if (r7[0] != SD_CHECK_PATTERN) {
		mmc_rsp_failure(MMC_ERR_CHECK_PATTERN);
		return false;
	}
	if (r7[1] != sd_get_vhs()) {
		mmc_rsp_failure(MMC_ERR_CARD_NON_COMP_VOLT);
		return false;
	}
	return true;
}

void sd_spi_recv_rsp(void)
{
	switch (mmc_slot_ctrl.rsp) {
	case MMC_R1b:
		if (!sd_spi_resp_r1b())
			return;
		break;
	case MMC_R1:
		sd_spi_resp_r1();
		break;
	case MMC_R2:
		sd_spi_resp_r2();
		break;
	case MMC_R3:
		if (!sd_spi_resp_r3())
			return;
		break;
	case SD_R7:
		if (!sd_spi_resp_r7())
			return;
		break;
	}
	mmc_rsp_success();
}

void sd_spi_send_acmd(void)
{
	uint32_t arg = 0;

	switch (mmc_slot_ctrl.acmd) {
	case SD_ACMD_SD_STATUS:
		mmc_slot_ctrl.rsp = MMC_R2;
		/* 512-bits SD status */
		MMC_BLOCK(READ, 64, 1);
		break;
	case SD_ACMD_SEND_NUM_WR_BLOCKS:
		mmc_slot_ctrl.rsp = MMC_R1;
		break;
	case SD_ACMD_SET_WR_BLK_ERASE_COUNT:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = sd_wr_blk_erase_count(mmc_slot_ctrl.wr_blk_erase_count);
		break;
	case SD_ACMD_SEND_OP_COND:
		mmc_slot_ctrl.rsp = MMC_R1;
#if 0
		arg = SD_OCR_HCS;
#else
		arg = mmc_slot_ctrl.host_ocr;
#endif
		break;
	case SD_ACMD_SET_CLR_CARD_DETECT:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.flags & MMC_SLOT_CARD_DETECT ? 1 : 0;
		break;
	case SD_ACMD_SEND_SCR:
		mmc_slot_ctrl.rsp = MMC_R1;
		/* 64-bits SCR register */
		MMC_BLOCK(READ, 8, 1);
		break;
	default:
		break;
	}
	mmc_hw_send_command(mmc_slot_ctrl.acmd, arg);
}

void sd_spi_send_cmd(void)
{
	uint32_t arg = 0;

	switch (mmc_slot_ctrl.cmd) {
	case MMC_CMD_GO_IDLE_STATE:
		mmc_slot_ctrl.rsp = MMC_R1;
		break;
	case MMC_CMD_SEND_OP_COND:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = SD_OCR_HCS;
		break;
	case SD_CMD_SWITCH_FUNC:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.func;
		break;
	case SD_CMD_SEND_IF_COND:
		mmc_slot_ctrl.rsp = SD_R7;
		arg = sd_encode_if_cond();
		break;
	case MMC_CMD_SEND_CSD:
	case MMC_CMD_SEND_CID:
		mmc_slot_ctrl.rsp = MMC_R1;
		break;
	case MMC_CMD_STOP_TRANSMISSION:
		mmc_slot_ctrl.rsp = MMC_R1b;
		mmc_wait_busy();
		break;
	case MMC_CMD_SEND_STATUS:
		mmc_slot_ctrl.rsp = MMC_R2;
		break;
	case MMC_CMD_SET_BLOCKLEN:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.block_len;
		break;
	case MMC_CMD_READ_SINGLE_BLOCK:
	case MMC_CMD_READ_MULTIPLE_BLOCK:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.address;
		break;
	case MMC_CMD_WRITE_BLOCK:
	case MMC_CMD_WRITE_MULTIPLE_BLOCK:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.address;
		break;
	case MMC_CMD_PROGRAM_CSD:
		mmc_slot_ctrl.rsp = MMC_R1;
		break;
#ifdef SD_CLASS6
	case MMC_CMD_SET_WRITE_PROT:
	case MMC_CMD_CLR_WRITE_PROT:
		mmc_slot_ctrl.rsp = MMC_R1b;
		mmc_wait_busy();
		arg = mmc_slot_ctrl.address;
		break;
	case MMC_CMD_SEND_WRITE_PROT:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.address;
		break;
#endif
	case SD_CMD_ERASE_WR_BLK_START:
	case SD_CMD_ERASE_WR_BLK_END:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.address;
		break;
	case MMC_CMD_ERASE:
		mmc_slot_ctrl.rsp = MMC_R1b;
		mmc_wait_busy();
		break;
#ifdef SD_CLASS7
	case MMC_CMD_LOCK_UNLOCK:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = 0;
		break;
#endif
	case MMC_CMD_APP_CMD:
		if (mmc_slot_ctrl.flags & MMC_SLOT_WAIT_APP_ACMD) {
			sd_spi_send_acmd();
			return;
		}
		mmc_slot_ctrl.rsp = MMC_R1;
		break;
	case MMC_CMD_GEN_CMD:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.flags & MMC_SLOT_GEN_CMD_RDWR ? 1 : 0;
		break;
	case SD_CMD_READ_OCR:
		mmc_slot_ctrl.rsp = MMC_R3;
		break;
	case MMC_CMD_CRC_ON_OFF:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.flags & MMC_SLOT_SPI_CRC_ONOFF ? 1 : 0;
		break;
	default:
		break;
	}
	mmc_hw_send_command(mmc_slot_ctrl.cmd, arg);
}

void sd_spi_enter_ver(void)
{
	if (!mmc_slot_ctrl.op_cond_sent)
		mmc_send_acmd(SD_ACMD_SEND_OP_COND);
	else if (!mmc_slot_ctrl.ocr_valid)
		mmc_cmd(SD_CMD_READ_OCR);
}

void sd_spi_handle_ver(void)
{
	if (mmc_slot_ctrl.op_cond_sent && mmc_slot_ctrl.ocr_valid)
		mmc_state_enter(stby);
	else
		sd_state_enter(ver);
}
