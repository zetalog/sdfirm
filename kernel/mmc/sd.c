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
 * @(#)sd.c: secure digital (SD) physical layer implementation
 * $Id: sd.c,v 1.1 2019-10-28 13:44:00 zhenglv Exp $
 */

#include <target/mmc.h>
#include <target/cmdline.h>
#include <target/jiffies.h>

#ifdef CONFIG_SD_SPI
bool sd_spi_mode = true;
#else
bool sd_spi_mode = false;
#endif

#if defined(CONFIG_CONSOLE) && defined(CONFIG_MMC_DEBUG)
const char *mmc_phy_state_names[NR_MMC_STATES] = {
	"idle",
	"ready",
	"ident",
	"stby",
	"tran",
	"data",
	"rcv",
	"prg",
	"dis",
	"ver",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"ina",
};

const char *mmc_phy_cmd_names[64] = {
	"GO_IDLE_STATE",
	NULL,
	"ALL_SEND_CID",
	"SEND_RELATIVE_ADDR",
	"SET_DSR",
	"SDIO reserved",
	"SWITCH_FUNC",
	"SELECT/DESELECT_CARD",
	"SEND_IF_COND",
	"SEND_CSD",
	"SEND_CID",
	"VOLTAGE_SWITCH",
	"STOP_TRANSMISSION",
	"SEND_STATUS/SEND_TASK_STATUS",
	NULL,
	"GO_INACTIVE_STATE",
	"SET_BLOCKLEN",
	"READ_SINGLE_BLOCK",
	"READ_MULTIPLE_BLOCK",
	"SEND_TUNING_BLOCK",
	"SPEED_CLASS_CONTROL",
	NULL,
	NULL,
	"SET_BLOCK_COUNT",
	"WRITE_BLOCK",
	"WRITE_MULTIPLE_BLOCK",
	"Manufacturer reserved",
	"PROGRAM_CSD",
	"SET_WRITE_PROT",
	"CLR_WRITE_PROT",
	"SEND_WRITE_PROT",
	NULL,
	"ERASE_WR_BLK_START",
	"ERASE_WR_BLK_END",
	"Function reserved",
	"Function reserved",
	"Function reserved",
	"Function reserved",
	"ERASE",
	NULL,
	NULL,
	NULL,
	"LOCK_UNLOCK",
	"Q_MANAGEMENT",
	"Q_TASK_INFO_A",
	"Q_TASK_INFO_B",
	"Q_RD_TASK",
	"Q_WR_TASK",
	"READ_EXTRA_SINGLE",
	"WRITE_EXTRA_SINGLE",
	"Function reserved",
	NULL,
	"SDIO reserved",
	"SDIO reserved",
	"SDIO reserved",
	"APP_CMD",
	"GEN_CMD",
	"Function reserved",
	"READ_EXTRA_MULTI | READ_OCR",
	"WRITE_EXTRA_MULTI",
	"Manufacturer reserved",
	"Manufacturer reserved",
	"Manufacturer reserved",
	"Manufacturer reserved",
};

const char *mmc_phy_acmd_names[64] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"SET_BUS_WIDTH",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"SD_STATUS",
	"DPS reserved",
	"DPS reserved",
	"DPS reserved",
	NULL,
	"Security reserved",
	NULL,
	NULL,
	NULL,
	"SEND_NUM_WR_BLOCKS",
	"SET_WR_BLK_ERASE_COUNT",
	NULL,
	"Security reserved",
	"Security reserved",
	NULL,
	"DPS reserved",
	NULL,
	"Security reserved",
	"Security reserved",
	"Security reserved",
	"Security reserved",
	"Security reserved",
	"Security reserved",
	NULL,
	NULL,
	"Security reserved",
	NULL,
	NULL,
	"SD_SEND_OP_COND",
	"SET_CLR_CARD_DETECT",
	"Security reserved",
	"Security reserved",
	"Security reserved",
	"Security reserved",
	"Security reserved",
	"Security reserved",
	"Security reserved",
	NULL,
	"SEND_SCR",
	"Security reserved",
	"Security reserved",
	"Security reserved",
	NULL,
	"Security reserved",
	"Security reserved",
	"Security reserved",
	"Security reserved",
};
#endif

static bool sd_cap_voltage_switch(void)
{
	return false;
}

static void sd_enter_ver(void)
{
	if (!mmc_slot_ctrl.ocr_valid)
		mmc_send_acmd(SD_ACMD_SEND_OP_COND);
	else if (sd_cap_voltage_switch() && !mmc_slot_ctrl.voltage_ready)
		mmc_cmd(SD_CMD_VOLTAGE_SWITCH);
}

/* SD memory card initialization:
 * SD:
 *   CMD0:GO_IDLE_STATE
 *  ->idle
 *   CMD8:SEND_IF_COND
 *  ->ver->
 *   ACMD41:SEND_OP_COND
 *   CMD11:SWITCH_FUNC (S18R S18A)
 *  ->ready
 *   CMD2:ALL_SEND_CID
 *   CMD3:SEND_RELATIVE_ADDR
 *  ->stby
 * SD_SPI:
 *   CMD0:GO_IDLE_STATE
 *  ->idle
 *   CMD8:SEND_IF_COND
 *  ->ver->
 *   [CMD58:READ_OCR]->voltage
 *   ACMD41:SEND_OP_COND
 *   CMD58:READ_OCR->capacity
 *  ->stby
 */
static void sd_handle_identify_card(void)
{
	if (mmc_cmd_is(MMC_CMD_NONE)) {
		mmc_slot_ctrl.start_tick = tick_get_counter();
		mmc_cmd(MMC_CMD_GO_IDLE_STATE);
	} else if (mmc_state_is(idle))
		mmc_cmd(SD_CMD_SEND_IF_COND);
	else if (sd_state_is(ver)) {
		if (sd_spi_mode)
			sd_spi_enter_ver();
		else
			sd_enter_ver();
	} else if (mmc_state_is(ready))
		mmc_cmd(MMC_CMD_ALL_SEND_CID);
	else if (mmc_state_is(ident))
		mmc_cmd(SD_CMD_SEND_RELATIVE_ADDR);
	else if (mmc_state_is(stby)) {
		mmc_op_success();
		mmc_register_card(MMC_RCA(mmc_sid, mmc_slot_ctrl.rca));
	} else if (mmc_state_is(ina))
		mmc_op_failure();
}

static void sd_handle_select_card(void)
{
	if (!mmc_slot_ctrl.csd_valid)
		mmc_cmd(MMC_CMD_SEND_CSD);
	else if (!mmc_slot_ctrl.scr_valid)
		mmc_send_acmd(SD_ACMD_SEND_SCR);
	else if (mmc_state_is(stby))
		mmc_cmd(MMC_CMD_SELECT_DESELECT_CARD);
	else if (mmc_state_is(tran))
		mmc_op_success();
	else if (mmc_state_is(ina))
		mmc_op_failure();
}

static void sd_handle_read_blocks(void)
{
}

void mmc_phy_handle_seq(void)
{
	if (mmc_op_is(MMC_OP_IDENTIFY_CARD))
		sd_handle_identify_card();
	if (mmc_op_is(MMC_OP_SELECT_CARD))
		sd_handle_select_card();
	if (mmc_op_is(MMC_OP_READ_BLOCKS))
		sd_handle_read_blocks();
}

static sd_cid_t sd_decode_cid(mmc_r2_t raw_cid)
{
	sd_cid_t cid;

#ifdef CONFIG_MMC_CID_ALL_FIELDS
	cid.crc = raw_cid[0] >> 1;
	cid.mdt = MAKEWORD(raw_cid[1], raw_cid[2]);
	cid.psn = MAKELONG(MAKEWORD(raw_cid[3], raw_cid[4]),
			   MAKEWORD(raw_cid[5], raw_cid[6]));
	cid.prv = raw_cid[7];
	memcpy(cid.pnm, &raw_cid[8], 5);
#endif
	cid.oid = MAKEWORD(raw_cid[10], raw_cid[11]);
	cid.mid = raw_cid[12];
	return cid;
}

static mmc_csd_t sd_decode_csd(mmc_r2_t raw_csd)
{
	uint32_t csd0, csd1, csd2, csd3;
	mmc_csd_t csd;
	uint32_t csize, cmult;

	csd0 = MAKELONG(MAKEWORD(raw_csd[12], raw_csd[13]),
			MAKEWORD(raw_csd[14], raw_csd[15]));
	csd1 = MAKELONG(MAKEWORD(raw_csd[8], raw_csd[9]),
			MAKEWORD(raw_csd[10], raw_csd[11]));
	csd2 = MAKELONG(MAKEWORD(raw_csd[4], raw_csd[5]),
			MAKEWORD(raw_csd[6], raw_csd[7]));
	csd3 = MAKELONG(MAKEWORD(raw_csd[0], raw_csd[1]),
			MAKEWORD(raw_csd[2], raw_csd[3]));

	csd.csd_structure = MMC_CSD3_CSD_STRUCTURE(csd3);
	csd.tran_speed = MMC_CSD3_TRAN_SPEED(csd3);
	csd.read_bl_len = MMC_CSD2_READ_BL_LEN(csd2);
	csd.read_bl_partial = !!(MMC_CSD2_READ_BL_PARTIAL & csd2);
	csd.read_blk_misalign = !!(MMC_CSD2_READ_BLK_MISALIGN & csd2);
	csd.write_bl_len = MMC_CSD0_WRITE_BL_LEN(csd0);
	csd.write_bl_partial = !!(MMC_CSD0_WRITE_BL_PARTIAL & csd0);
	csd.write_blk_misalign = !!(MMC_CSD2_WRITE_BLK_MISALIGN & csd2);
#ifdef CONFIG_MMC_DEBUG
	if (csd.read_bl_len != csd.write_bl_len)
		printf("Mismatched BL_LEN: READ - %d, WRITE - %d\n",
		       csd.read_bl_len, csd.write_bl_len);
#endif
	csd.dsr_imp = !!(MMC_CSD2_DSR_IMP & csd2);
	if (mmc_slot_ctrl.card_ocr & SD_OCR_HCS) {
		csize = SD_CSD20_2_C_SIZE(csd2) << 16 |
			SD_CSD20_1_C_SIZE(csd1);
		cmult = 8;
	} else {
		csize = MMC_CSD2_C_SIZE(csd2) << 2 |
			MMC_CSD1_C_SIZE(csd1);
		cmult = MMC_CSD1_C_SIZE_MULT(csd1);
	}
	csd.capacity = (csize + 1) << (cmult + 2);
	return csd;
}

sd_scr_t sd_decode_scr(void)
{
	sd_scr_t scr;
	uint32_t scr1;
	uint8_t *buf = (uint8_t *)mmc_slot_ctrl.block_data;
	uint8_t sd_specx;

	scr1 = MAKELONG(MAKEWORD(buf[0], buf[1]),
			MAKEWORD(buf[2], buf[3]));
	scr.scr_structure = SD_SCR1_SCR_STRUCTURE(scr1);
	sd_specx = SD_SCR1_SD_SPECX(scr1);
	if (sd_specx)
		scr.version = 4 + sd_specx;
	else if (scr1 & SD_SCR1_SD_SPEC4)
		scr.version = 4;
	else if (scr1 & SD_SCR1_SD_SPEC3)
		scr.version = 3;
	else
		scr.version = SD_SCR1_SD_SPEC(scr1);
	scr.data_stat_after_erase = !!(SD_SCR1_DATA_STAT_AFTER_ERASE & scr1);
	scr.sd_security = SD_SCR1_SD_SECURITY(scr1);
	scr.ex_security = SD_SCR1_EX_SECURITY(scr1);
	scr.bus_widths = 1 << SD_SCR1_SD_BUS_WIDTHS(scr1);
	scr.cmd_support = SD_SCR1_CMD_SUPPORT(scr1);
	return scr;
}

uint32_t sd_decode_ocr(mmc_r3_t raw_ocr)
{
	uint32_t ocr;

	ocr = MAKELONG(MAKEWORD(raw_ocr[0], raw_ocr[1]),
		       MAKEWORD(raw_ocr[2], raw_ocr[3]));
	return ocr;
}

static uint32_t sd_encode_speed(sd_speed_t speed)
{
	return SD_SPEED_CLASS(speed.speed_class) |
	       SD_SPEED_CONTROL(speed.speed_control);
}

uint8_t sd_get_vhs(void)
{
	if (mmc_slot_ctrl.host_ocr & MMC_OCR_HIGH_VOLTAGE)
		return 1;
	return 0;
}

uint32_t sd_encode_if_cond(void)
{
	return MAKELONG(MAKEWORD(SD_CHECK_PATTERN,
			MAKEBYTE(sd_get_vhs(), 0)), 0);
}

void sd_state_enter_idle(void)
{
	if (SD_PHY_VERSION >= 2 &&
	    mmc_slot_ctrl.card_version == SD_PHY_VERSION_UNKNOWN)
		mmc_state_enter(idle);
	else
		sd_state_enter(ver);
}

static void sd_handle_ver(void)
{
	if (mmc_slot_ctrl.ocr_valid &&
	    (!sd_cap_voltage_switch() || mmc_slot_ctrl.voltage_ready))
		mmc_state_enter(ready);
	else
		sd_state_enter(ver);
}

/* This function implements the state digrams in the following sections:
 * 7.2 Card Identification Mode
 * 7.3 Interrupt Mode
 * 7.4 Data Transfer Mode
 */
void mmc_phy_handle_stm(void)
{
	mmc_event_t flags;

	flags = mmc_event_save();
	if (flags & MMC_EVENT_CARD_INSERT) {
		mmc_identify_card();
		unraise_bits(flags, MMC_EVENT_CARD_INSERT);
	} else if (flags & MMC_EVENT_CARD_BUSY &&
		   mmc_slot_ctrl.flags & MMC_SLOT_CARD_IS_BUSY &&
		   mmc_slot_ctrl.rsp & MMC_RSP_BUSY) {
		mmc_hw_card_busy();
		unraise_bits(flags, MMC_EVENT_CARD_BUSY);
	} else if (flags & MMC_EVENT_CMD_SUCCESS &&
		   mmc_slot_ctrl.flags & MMC_SLOT_WAIT_APP_CMD &&
		   mmc_cmd_is(MMC_CMD_APP_CMD)) {
		mmc_app_cmd_complete();
		unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
	} else if (flags & MMC_EVENT_POWER_ON) {
		sd_state_enter_idle();
		unraise_bits(flags, MMC_EVENT_POWER_ON);
	} else if (flags & MMC_EVENT_CMD_SUCCESS &&
		   mmc_cmd_is(MMC_CMD_GO_INACTIVE_STATE) &&
		   mmc_state_ge(stby)) {
		mmc_state_enter(ina);
		unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
	} else if (flags & MMC_EVENT_CMD_SUCCESS &&
		   mmc_cmd_is(MMC_CMD_GO_IDLE_STATE) &&
		   !mmc_state_is(ina)) {
		sd_state_enter_idle();
		unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
	} else if (flags & MMC_EVENT_CMD_SUCCESS &&
		   mmc_state_ge(stby) &&
		   (mmc_cmd_is(MMC_CMD_SEND_STATUS) ||
		    mmc_cmd_is_app())) {
		/* No state transition in data transfer mode */
		unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
	/* identification mode */
	} else if (mmc_state_is(idle)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			sd_state_enter(ver);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_CMD_FAILURE) {
			if (mmc_err_is(MMC_ERR_ILLEGAL_COMMAND))
				sd_state_enter(ver);
			if (mmc_err_is(MMC_ERR_CHECK_PATTERN))
				mmc_state_enter(ina);
			if (mmc_err_is(MMC_ERR_CARD_NON_COMP_VOLT))
				mmc_state_enter(ina);
			unraise_bits(flags, MMC_EVENT_CMD_FAILURE);
		}
	} else if (sd_state_is(ver)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (sd_spi_mode)
				sd_spi_handle_ver();
			else
				sd_handle_ver();
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_CMD_FAILURE) {
			/* OCR_BUSY */
			if (mmc_err_is(MMC_ERR_CARD_IS_BUSY)) {
				if (HZ > (tick_get_counter() -
					  mmc_slot_ctrl.start_tick))
					sd_state_enter(ver);
				else
					mmc_state_enter(ina);
			}
			if (mmc_err_is(MMC_ERR_HOST_OMIT_VOLT))
				sd_state_enter(ver);
			unraise_bits(flags, MMC_EVENT_CMD_FAILURE);
		}
	} else if (mmc_state_is(ready)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_ALL_SEND_CID))
				mmc_state_enter(ident);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_CMD_FAILURE) {
			if (mmc_err_is(MMC_ERR_CARD_LOOSE_BUS));
			unraise_bits(flags, MMC_EVENT_CMD_FAILURE);
		}
	} else if (mmc_state_is(ident)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(SD_CMD_SEND_RELATIVE_ADDR))
				mmc_state_enter(stby);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_CMD_FAILURE) {
			unraise_bits(flags, MMC_EVENT_CMD_FAILURE);
		}
	/* data transfer mode */
	} else if (mmc_state_is(stby)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_SEND_CSD));
			if (mmc_cmd_is(MMC_CMD_SEND_CID));
			if (mmc_cmd_is(MMC_CMD_SELECT_DESELECT_CARD))
				mmc_state_enter(tran);
			else
				mmc_state_enter(stby);

			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_CMD_FAILURE) {
			unraise_bits(flags, MMC_EVENT_CMD_FAILURE);
		}
	} else if (mmc_state_is(tran)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_SELECT_DESELECT_CARD))
				mmc_state_enter(stby);
			else
				mmc_state_enter(tran);

			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_CMD_FAILURE) {
			unraise_bits(flags, MMC_EVENT_CMD_FAILURE);
		}
	}
	mmc_event_restore(flags);
}

void sd_resp_r0(void)
{
	mmc_hw_recv_response(NULL, 0);
}

void sd_resp_r1(void)
{
	mmc_r1_t r1;

	mmc_hw_recv_response(r1, 4);
	raise_bits(mmc_slot_ctrl.flags, MMC_SLOT_CARD_STATUS_VALID);
	mmc_slot_ctrl.csr = MAKELONG(MAKEWORD(r1[0], r1[1]),
				     MAKEWORD(r1[2], r1[3]));
	if (mmc_slot_ctrl.cmd == MMC_CMD_APP_CMD) {
		switch (mmc_slot_ctrl.acmd) {
		case SD_ACMD_SD_STATUS:
			/* TODO: parse SD_STATUS */
			break;
		case SD_ACMD_SEND_SCR:
			mmc_slot_ctrl.host_scr = sd_decode_scr();
			mmc_slot_ctrl.scr_valid = true;
			break;
		}
	} else {
	}
}

void sd_resp_r1b(void)
{
	sd_resp_r1();
	mmc_hw_card_busy();
}

void sd_resp_r2(void)
{
	mmc_r2_t r2;

	mmc_hw_recv_response(r2, 16);
	if (mmc_slot_ctrl.cmd == MMC_CMD_SEND_CSD) {
		mmc_slot_ctrl.csd = sd_decode_csd(r2);
		mmc_slot_ctrl.csd_valid = true;
	} else
		mmc_slot_ctrl.cid = sd_decode_cid(r2);
}

bool sd_resp_r3(void)
{
	mmc_r3_t r3;

	mmc_hw_recv_response(r3, 4);
	mmc_slot_ctrl.card_ocr = sd_decode_ocr(r3);
	if (!(mmc_slot_ctrl.card_ocr & MMC_OCR_BUSY)) {
		mmc_rsp_failure(MMC_ERR_CARD_IS_BUSY);
		return false;
	}
	mmc_slot_ctrl.ocr_valid = true;
	return true;
}

void sd_encode_current_state(uint8_t state)
{
	mmc_slot_ctrl.csr &= ~MMC_DET_CURRENT_STATE_MASK;
	mmc_slot_ctrl.csr |= MMC_CURRENT_STATE(state);
}

void sd_resp_r6(void)
{
	sd_r6_t r6;
	uint16_t cs;

#define SD_R6_COM_CRC_ERROR	_BV(15)
#define SD_R6_ILLEGAL_COMMAND	_BV(14)
#define SD_R6_ERROR		_BV(13)

	mmc_hw_recv_response(r6, 4);

	/* Decode RCA */
	mmc_slot_ctrl.rca = MAKEWORD(r6[2], r6[3]);
	/* Decode card status */
	cs = MAKEWORD(r6[0], r6[1]);
	if (cs & SD_R6_COM_CRC_ERROR)
		raise_bits(mmc_slot_ctrl.csr, MMC_DET_COM_CRC_ERROR);
	else
		unraise_bits(mmc_slot_ctrl.csr, MMC_DET_COM_CRC_ERROR);
	if (cs & SD_R6_ILLEGAL_COMMAND)
		raise_bits(mmc_slot_ctrl.csr, MMC_DET_ILLEGAL_COMMAND);
	else
		unraise_bits(mmc_slot_ctrl.csr, MMC_DET_ILLEGAL_COMMAND);
	if (cs & SD_R6_ERROR)
		raise_bits(mmc_slot_ctrl.csr, MMC_DET_ERROR);
	else
		unraise_bits(mmc_slot_ctrl.csr, MMC_DET_ERROR);
	sd_encode_current_state(mmc_current_state(cs));
}

bool sd_resp_r7(void)
{
	sd_r7_t r7;

	mmc_hw_recv_response(r7, 4);
	if (r7[0] != SD_CHECK_PATTERN) {
		mmc_rsp_failure(MMC_ERR_CHECK_PATTERN);
		return false;
	}
	if (r7[1] != sd_get_vhs()) {
		mmc_rsp_failure(MMC_ERR_CARD_NON_COMP_VOLT);
		return false;
	}
	mmc_slot_ctrl.card_version = SD_PHY_VERSION_20;
	return true;
}

void sd_recv_rsp(void)
{
	switch (mmc_slot_ctrl.rsp) {
	case MMC_R0:
		sd_resp_r0();
		break;
	case MMC_R1:
		sd_resp_r1();
		break;
	case MMC_R1b:
		sd_resp_r1b();
		break;
	case MMC_R2:
		sd_resp_r2();
		break;
	case MMC_R3:
		if (!sd_resp_r3())
			return;
		break;
	case SD_R6:
		sd_resp_r6();
		break;
	case SD_R7:
		if (!sd_resp_r7())
			return;
		break;
	}
	mmc_rsp_success();
}

void mmc_phy_recv_rsp(void)
{
	if (sd_spi_mode)
		sd_spi_recv_rsp();
	else
		sd_recv_rsp();
}

void sd_send_acmd(void)
{
	uint32_t arg = 0;

	switch (mmc_slot_ctrl.acmd) {
	case SD_ACMD_SET_BUS_WIDTH:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = sd_bus_width(mmc_slot_ctrl.bus_width);
		break;
	case SD_ACMD_SD_STATUS:
		mmc_slot_ctrl.rsp = MMC_R1;
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
		mmc_slot_ctrl.rsp = MMC_R3;
		arg = mmc_slot_ctrl.host_ocr;
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

void sd_send_cmd(void)
{
	uint32_t arg = 0;

	switch (mmc_slot_ctrl.cmd) {
	case MMC_CMD_GO_IDLE_STATE:
		mmc_slot_ctrl.rsp = MMC_R0;
		break;
	case MMC_CMD_ALL_SEND_CID:
		mmc_slot_ctrl.rsp = MMC_R2;
		break;
	case SD_CMD_SEND_RELATIVE_ADDR:
		mmc_slot_ctrl.rsp = SD_R6;
		break;
	case MMC_CMD_SET_DSR:
		mmc_slot_ctrl.rsp = MMC_R0;
		arg = MAKELONG(0, mmc_slot_ctrl.dsr);
		break;
	case MMC_CMD_SELECT_DESELECT_CARD:
		mmc_slot_ctrl.rsp = MMC_R1b;
		mmc_wait_busy();
		arg = MAKELONG(0, mmc_slot_ctrl.rca);
		break;
	case SD_CMD_SEND_IF_COND:
		mmc_slot_ctrl.rsp = SD_R7;
		arg = sd_encode_if_cond();
		break;
	case MMC_CMD_SEND_CSD:
	case MMC_CMD_SEND_CID:
		mmc_slot_ctrl.rsp = MMC_R2;
		arg = MAKELONG(0, mmc_slot_ctrl.rca);
		break;
	case SD_CMD_VOLTAGE_SWITCH:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = 0;
		break;
	case MMC_CMD_STOP_TRANSMISSION:
		mmc_slot_ctrl.rsp = MMC_R1b;
		mmc_wait_busy();
		break;
	case MMC_CMD_SEND_STATUS:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = MAKELONG(0, mmc_slot_ctrl.rca);
		break;
	case MMC_CMD_GO_INACTIVE_STATE:
		mmc_slot_ctrl.rsp = MMC_R0;
		arg = MAKELONG(0, mmc_slot_ctrl.rca);
		break;
#if defined(SD_CLASS2) || defined(SD_CLASS4) || defined(SD_CLASS7)
	case MMC_CMD_SET_BLOCKLEN:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.block_len;
		break;
#endif
#ifdef SD_CLASS2
	case MMC_CMD_READ_SINGLE_BLOCK:
	case MMC_CMD_READ_MULTIPLE_BLOCK:
	case SD_CMD_SEND_TUNING_BLOCK:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.address;
		break;
#endif
#if defined(SD_CLASS2) || defined(SD_CLASS4)
	case SD_CMD_SPEED_CLASS_CONTROL:
		mmc_slot_ctrl.rsp = MMC_R1b;
		mmc_wait_busy();
		arg = sd_encode_speed(mmc_slot_ctrl.speed);
		break;
	case MMC_CMD_SET_BLOCK_COUNT:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = MAKELONG(mmc_slot_ctrl.block_cnt, 0);
		break;
#endif
#ifdef SD_CLASS4
	case MMC_CMD_WRITE_BLOCK:
	case MMC_CMD_WRITE_MULTIPLE_BLOCK:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.address;
		break;
	case MMC_CMD_PROGRAM_CSD:
		mmc_slot_ctrl.rsp = MMC_R1;
		break;
#endif
#ifdef SD_CLASS6
	case MMC_CMD_SET_WRITE_PROT:
	case MMC_CMD_CLR_WRITE_PROT:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.address;
		break;
	case MMC_CMD_SEND_WRITE_PROT:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.address;
		break;
#endif
#ifdef SD_CLASS5
	case SD_CMD_ERASE_WR_BLK_START:
	case SD_CMD_ERASE_WR_BLK_END:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.address;
		break;
	case MMC_CMD_ERASE:
		mmc_slot_ctrl.rsp = MMC_R1b;
		mmc_wait_busy();
		arg = mmc_slot_ctrl.func;
		break;
#endif
#ifdef SD_CLASS7
	case MMC_CMD_LOCK_UNLOCK:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = 0;
		break;
#endif
#ifdef SD_CLASS8
	case MMC_CMD_APP_CMD:
		if (mmc_slot_ctrl.flags & MMC_SLOT_WAIT_APP_ACMD) {
			sd_send_acmd();
			return;
		}
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = MAKELONG(0, mmc_slot_ctrl.rca);
		break;
	case MMC_CMD_GEN_CMD:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.flags & MMC_SLOT_GEN_CMD_RDWR ? 1 : 0;
		break;
#endif
#ifdef SD_CLASS10
	case SD_CMD_SWITCH_FUNC:
		mmc_slot_ctrl.rsp = MMC_R1;
		/* 512 bits status */
		MMC_BLOCK(READ, 64, 1);
		arg = mmc_slot_ctrl.func;
		break;
#endif
#ifdef SD_CLASS11
	case SD_CMD_READ_EXTRA_SINGLE:
	case SD_CMD_WRITE_EXTRA_SINGLE:
	case SD_CMD_READ_EXTRA_MULTI:
	case SD_CMD_WRITE_EXTRA_MULTI:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.func;
		break;
#endif
#ifdef SD_CLASS1
	case SD_CMD_Q_MANAGEMENT:
		mmc_slot_ctrl.rsp = MMC_R1b;
		mmc_wait_busy();
		arg = mmc_slot_ctrl.func;
		break;
	case SD_CMD_Q_TASK_INFO_A:
	case SD_CMD_Q_TASK_INFO_B:
	case SD_CMD_Q_RD_TASK:
	case SD_CMD_Q_WR_TASK:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = mmc_slot_ctrl.func;
		break;
#endif
	default:
		/* stuff bits */
		break;
	}
	mmc_hw_send_command(mmc_slot_ctrl.cmd, arg);
}

void mmc_phy_send_cmd(void)
{
	if (sd_spi_mode)
		sd_spi_send_cmd();
	else
		sd_send_cmd();
}

void mmc_phy_reset_slot(void)
{
	mmc_slot_ctrl.card_version = SD_PHY_VERSION_UNKNOWN;
}
