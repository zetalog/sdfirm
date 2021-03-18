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

/* 4.2.3 Card Initialization and Identification Process
 * Application Notes:
 * The host shall set ACMD41 timeout more than 1 second to abort repeat of
 * issueing ACMD41 when the card does not indicate ready. The timeout
 * count starts from the first ACMD41 which is set voltage window in the
 * argument.
 */
#define SD_READY_TIMEOUT_MS		1000
/* 4.6.2.1 Read
 * For a Standard Capacity SD Memory Card, the times after which a timeout
 * condition for read operations occurs are (card independent) either 100
 * times longer than the typical access times for these operations given
 * below or 100ms (the lower of the two). The read access time is defined
 * as the sum of the two times given by the CSD parameters TAAC and NSAC.
 * A High Capacity SD Memory Card and Extended Capacity SD Memory Card
 * indicate TAAC and NSAC as fixed value, the host should use 100ms
 * timeout (mimimum) for single and multiple read operation rather than
 * using TAAC and NSAC.
 */
#define SD_READ_TIMEOUT_MS		100
/* 4.6.2.2 Write
 * For a Standard Capacity SD Memory Card, the times after which a timeout
 * condition for write operations occurs are (card independent) either 100
 * times longer than the typical access times for these operations given
 * below or 250ms (the lower of the two). The R2W_FACTOR field in the CSD
 * is used to calculate the typical block program time obtained by
 * multiplying the read access time by this factor. High Capacity SD
 * Memory Card and Extended SD Memory Card indicate R2W_FACTOR as a fixed
 * value.
 * In case of High Capacity SD Memory Card, maximum length of busy is
 * defined as 250ms for all write operation.
 * Application Notes:
 * The host should use a fixed timeout for write operation rather than
 * using a timeout calculated from the R2W_FACTOR parameter.
 * It is strongly recommended for hosts to implement more than 500ms
 * timeout value even if the card indicates the 250ms maximum busy length.
 * Even if the card supports Speed Class, any multiple block write
 * operation may indicate a busy period of up to a maximum 250ms. The sum
 * of the busy period over an AU is limited by Speed Class.
 * In UHS-II mode, refer to the UHS-II Addendum about host timeout
 * setting.
 */
#define SD_WRITE_TIMEOUT_MS		500

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
static void sd_handle_identify_card(bool is_op)
{
	mmc_card_t cid;

	if (mmc_cmd_is(MMC_CMD_NONE)) {
		mmc_slot_ctrl.start_tick = tick_get_counter();
		mmc_cmd(MMC_CMD_GO_IDLE_STATE);
	} else if (mmc_state_is(idle)) {
		/* TODO: select VHS, timeout */
		mmc_cmd(SD_CMD_SEND_IF_COND);
	} else if (sd_state_is(ver)) {
		if (mmc_slot_ctrl.card_version == SD_PHY_VERSION_UNKNOWN)
			mmc_slot_ctrl.card_version = SD_PHY_VERSION_10;
		if (sd_spi_mode)
			sd_spi_enter_ver();
		else
			sd_enter_ver();
	} else if (mmc_state_is(ready))
		mmc_cmd(MMC_CMD_ALL_SEND_CID);
	else if (mmc_state_is(ident))
		mmc_cmd(SD_CMD_SEND_RELATIVE_ADDR);
	else if (mmc_state_is(stby)) {
		if (is_op)
			mmc_op_success();
		cid = mmc_register_card(mmc_slot_ctrl.rca);
		if (cid != INVALID_MMC_CARD)
			mmc_slot_ctrl.mmc_cid = cid;
	} else if (mmc_state_is(__ina))
		mmc_cmd(MMC_CMD_GO_INACTIVE_STATE);
	else if (mmc_state_is(ina))
		mmc_op_failure();
}

static void sd_handle_select_mode(bool is_op)
{
	if (mmc_state_is(stby)) {
		if (sd_spi_mode) {
			mmc_slot_ctrl.cmd = MMC_CMD_SELECT_DESELECT_CARD;
			mmc_event_raise(MMC_EVENT_CMD_SUCCESS);
		} else {
			if (!mmc_slot_ctrl.csd_valid)
				mmc_cmd(MMC_CMD_SEND_CSD);
			else if (!mmc_slot_ctrl.scr_valid)
				mmc_send_acmd(SD_ACMD_SEND_SCR);
			else if (!mmc_width_configured())
				mmc_send_acmd(SD_ACMD_SET_BUS_WIDTH);
			else {
				mmc_config_mode(mmc_slot_ctrl.select_mode);
				mmc_cmd(MMC_CMD_SELECT_DESELECT_CARD);
			}
		}
	} else if (mmc_state_is(tran)) {
		if (is_op)
			mmc_op_success();
	} else if (mmc_state_is(__ina))
		mmc_cmd(MMC_CMD_GO_INACTIVE_STATE);
	else if (mmc_state_is(ina))
		mmc_op_failure();
}

static void sd_handle_select_card(bool is_op)
{
	if (mmc_state_is(tran)) {
		mmc_slot_ctrl.flags |= MMC_SLOT_CARD_SELECT;
		if (is_op)
			mmc_op_success();
	} else if (mmc_state_is(__ina))
		mmc_cmd(MMC_CMD_GO_INACTIVE_STATE);
	else if (mmc_state_is(ina))
		mmc_op_failure();
	else
		sd_handle_select_mode(false);
}

static void sd_handle_deselect_card(bool is_op)
{
	if (mmc_state_is(stby)) {
		mmc_slot_ctrl.flags &= ~MMC_SLOT_CARD_SELECT;
		if (is_op)
			mmc_op_success();
	} else if (mmc_state_is(__ina))
		mmc_cmd(MMC_CMD_GO_INACTIVE_STATE);
	else if (mmc_state_is(ina))
		mmc_op_failure();
	else {
		if (sd_spi_mode) {
			mmc_slot_ctrl.cmd = MMC_CMD_SELECT_DESELECT_CARD;
			mmc_event_raise(MMC_EVENT_CMD_SUCCESS);
		} else
			mmc_cmd(MMC_CMD_SELECT_DESELECT_CARD);
	}
}

static void sd_handle_read_blocks(bool is_op)
{
	if (mmc_state_is(tran)) {
		if (!(mmc_slot_ctrl.flags & MMC_SLOT_TRANS_START)) {
			raise_bits(mmc_slot_ctrl.flags, MMC_SLOT_TRANS_START);
			unraise_bits(mmc_slot_ctrl.flags, MMC_SLOT_TRANS_STOP);
			if (mmc_slot_ctrl.trans_cnt > 1)
				mmc_cmd(MMC_CMD_READ_MULTIPLE_BLOCK);
			else
				mmc_cmd(MMC_CMD_READ_SINGLE_BLOCK);
		} else if (mmc_slot_ctrl.flags & MMC_SLOT_TRANS_STOP) {
			unraise_bits(mmc_slot_ctrl.flags, MMC_SLOT_TRANS_START);
			if (is_op)
				mmc_op_success();
		} else {
			mmc_set_block_data(0);
			mmc_cmd(MMC_CMD_STOP_TRANSMISSION);
		}
	} else if (mmc_state_is(__ina))
		mmc_cmd(MMC_CMD_GO_INACTIVE_STATE);
	else if (mmc_state_is(ina))
		mmc_op_failure();
	else if (mmc_state_is(stby))
		sd_handle_select_card(false);
}

static void sd_handle_write_blocks(bool is_op)
{
	if (mmc_state_is(tran)) {
		if (!(mmc_slot_ctrl.flags & MMC_SLOT_TRANS_START)) {
			raise_bits(mmc_slot_ctrl.flags, MMC_SLOT_TRANS_START);
			unraise_bits(mmc_slot_ctrl.flags, MMC_SLOT_TRANS_STOP);
			if (mmc_slot_ctrl.trans_cnt > 1)
				mmc_cmd(MMC_CMD_WRITE_MULTIPLE_BLOCK);
			else
				mmc_cmd(MMC_CMD_WRITE_BLOCK);
		} else if (mmc_slot_ctrl.flags & MMC_SLOT_TRANS_STOP) {
			unraise_bits(mmc_slot_ctrl.flags, MMC_SLOT_TRANS_START);
			if (is_op)
				mmc_op_success();
		} else {
			mmc_set_block_data(0);
			mmc_cmd(MMC_CMD_STOP_TRANSMISSION);
		}
	} else if (mmc_state_is(__ina))
		mmc_cmd(MMC_CMD_GO_INACTIVE_STATE);
	else if (mmc_state_is(ina))
		mmc_op_failure();
	else if (mmc_state_is(stby))
		sd_handle_select_card(false);
}

void mmc_phy_handle_seq(void)
{
	if (mmc_op_is(MMC_OP_IDENTIFY_CARD))
		sd_handle_identify_card(true);
	if (mmc_op_is(MMC_OP_SELECT_MODE))
		sd_handle_select_mode(true);
	if (mmc_op_is(MMC_OP_SELECT_CARD))
		sd_handle_select_card(true);
	if (mmc_op_is(MMC_OP_DESELECT_CARD))
		sd_handle_deselect_card(true);
	if (mmc_op_is(MMC_OP_READ_BLOCKS))
		sd_handle_read_blocks(true);
	if (mmc_op_is(MMC_OP_WRITE_BLOCKS))
		sd_handle_write_blocks(true);
}

static sd_cid_t sd_decode_cid(mmc_r2_t raw_cid)
{
	sd_cid_t cid;

#ifdef CONFIG_MMC_CID_ALL_FIELDS
	cid.crc = raw_cid[15] >> 1;
	cid.mdt = MAKEWORD(raw_cid[14], raw_cid[13]);
	cid.psn = MAKELONG(MAKEWORD(raw_cid[12], raw_cid[11]),
			   MAKEWORD(raw_cid[10], raw_cid[9]));
	cid.prv = raw_cid[8];
	memcpy(cid.pnm, &raw_cid[3], 5);
#ifdef CONFIG_MMC_DEBUG
	printf("MDT: %d\n", cid.mdt);
	printf("PSN: %d\n", cid.psn);
	printf("PRV: %d\n", cid.prv);
	printf("PNM: %.5s\n", cid.pnm);
#endif
#endif
	cid.oid = MAKEWORD(raw_cid[2], raw_cid[1]);
	cid.mid = raw_cid[0];
#ifdef CONFIG_MMC_DEBUG
	printf("OID: %d\n", cid.oid);
	printf("MID: %d\n", cid.mid);
#endif
	return cid;
}

static mmc_csd_t sd_decode_csd(mmc_r2_t raw_csd)
{
	uint32_t csd0, csd1, csd2, csd3;
	mmc_csd_t csd;
	uint32_t csize, cmult;

	csd0 = MAKELONG(MAKEWORD(raw_csd[15], raw_csd[14]),
			MAKEWORD(raw_csd[13], raw_csd[12]));
	csd1 = MAKELONG(MAKEWORD(raw_csd[11], raw_csd[10]),
			MAKEWORD(raw_csd[9], raw_csd[8]));
	csd2 = MAKELONG(MAKEWORD(raw_csd[7], raw_csd[6]),
			MAKEWORD(raw_csd[5], raw_csd[4]));
	csd3 = MAKELONG(MAKEWORD(raw_csd[3], raw_csd[2]),
			MAKEWORD(raw_csd[1], raw_csd[0]));

	csd.csd_structure = MMC_CSD3_CSD_STRUCTURE(csd3);
	csd.tran_speed = MMC_CSD3_TRAN_SPEED(csd3);
	csd.read_bl_len = MMC_CSD2_READ_BL_LEN(csd2);
	csd.read_bl_partial = !!(MMC_CSD2_READ_BL_PARTIAL & csd2);
	csd.read_blk_misalign = !!(MMC_CSD2_READ_BLK_MISALIGN & csd2);
	csd.write_bl_len = MMC_CSD0_WRITE_BL_LEN(csd0);
	csd.write_bl_partial = !!(MMC_CSD0_WRITE_BL_PARTIAL & csd0);
	csd.write_blk_misalign = !!(MMC_CSD2_WRITE_BLK_MISALIGN & csd2);
	if (csd.read_bl_len != csd.write_bl_len)
		printf("Mismatched BL_LEN: READ - %d, WRITE - %d\n",
		       csd.read_bl_len, csd.write_bl_len);
	csd.dsr_imp = !!(MMC_CSD2_DSR_IMP & csd2);
	if (csd.csd_structure == SD_CSD3_CSD_VERSION_2_0) {
		if (csd.read_bl_len != 9) {
			printf("Mismatched BL_LEN: FIXED(9) - %d\n",
			       csd.read_bl_len);
			csd.read_bl_len = csd.write_bl_len = 9;
		}
		csize = SD_CSD20_2_C_SIZE(csd2) << 16 |
			SD_CSD20_1_C_SIZE(csd1);
		cmult = 8;
	} else {
		csize = MMC_CSD2_C_SIZE(csd2) << 2 |
			MMC_CSD1_C_SIZE(csd1);
		cmult = MMC_CSD1_C_SIZE_MULT(csd1);
	}
	csd.capacity = (csize + 1) << (cmult + 2);
#ifdef CONFIG_MMC_DEBUG
	printf("CSD_STRUCTURE: %d\n", csd.csd_structure);
	printf("TRAN_SPEED: %d\n", csd.tran_speed);
	printf("Block number: %d\n", csd.capacity);
	printf("Block length: %d\n", _BV(csd.read_bl_len));
#endif
	mmc_slot_ctrl.capacity_len = _BV(csd.read_bl_len);
	mmc_slot_ctrl.capacity_cnt = csd.capacity;
	mmc_slot_ctrl.high_capacity = !!(mmc_slot_ctrl.card_ocr & SD_OCR_CCS);
	return csd;
}

sd_scr_t sd_decode_scr(void)
{
	sd_scr_t scr;
	__unused uint32_t scr0;
	uint32_t scr1;
	uint8_t *buf = (uint8_t *)mmc_slot_ctrl.block_data;
	uint8_t sd_specx;

	scr0 = MAKELONG(MAKEWORD(buf[7], buf[6]),
			MAKEWORD(buf[5], buf[4]));
	scr1 = MAKELONG(MAKEWORD(buf[3], buf[2]),
			MAKEWORD(buf[1], buf[0]));
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
	scr.bus_widths = SD_SCR1_SD_BUS_WIDTHS(scr1);
	scr.cmd_support = SD_SCR1_CMD_SUPPORT(scr1);
#ifdef CONFIG_MMC_DEBUG
	printf("SCR_STRUCTURE: %d\n", scr.scr_structure);
	printf("Version: %d\n", scr.version);
	printf("BUS_WIDTHS: 0x%02x\n", scr.bus_widths);
	printf("CMD_SUPPORT: 0x%02x\n", scr.cmd_support);
#endif
	return scr;
}

uint32_t sd_decode_ocr(mmc_r3_t raw_ocr)
{
	uint32_t ocr;

	ocr = MAKELONG(MAKEWORD(raw_ocr[3], raw_ocr[2]),
		       MAKEWORD(raw_ocr[1], raw_ocr[0]));
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

void sd_state_enter_ina(void)
{
	if (sd_spi_mode)
		mmc_state_enter(ina);
	else
		mmc_state_enter(__ina);
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
		unraise_bits(flags, MMC_EVENT_CARD_BUSY);
		if (!mmc_hw_card_busy())
			mmc_rsp_success();
		else
			mmc_event_raise(MMC_EVENT_CARD_BUSY);
	} else if (flags & MMC_EVENT_CMD_SUCCESS &&
		   mmc_slot_ctrl.flags & MMC_SLOT_WAIT_APP_CMD &&
		   mmc_cmd_is_app()) {
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
		mmc_state_set(mmc_state_get());
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
				sd_state_enter_ina();
			/* If card cannot operate under supplied
			 * voltage, card doesn't respond and return to
			 * "idle" state.
			 */
			if (mmc_err_is(MMC_ERR_CARD_NON_COMP_VOLT) ||
			    mmc_err_is(MMC_ERR_TIMEOUT)) {
#if 0
				if (!mmc_next_vhs())
					sd_state_enter_ina();
				else
#endif
					mmc_state_enter(idle);
			}
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
					sd_state_enter_ina();
			}
			if (mmc_err_is(MMC_ERR_HOST_OMIT_VOLT))
				sd_state_enter(ver);
			/* No response, must be a multimedia card */
			if (mmc_err_is(MMC_ERR_TIMEOUT))
				sd_state_enter_ina();
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
			if (mmc_cmd_is(MMC_CMD_SEND_CSD))
				mmc_state_enter(stby);
			else if (mmc_cmd_is(MMC_CMD_SEND_CID))
				mmc_state_enter(stby);
			else if (mmc_cmd_is(MMC_CMD_SELECT_DESELECT_CARD))
				mmc_state_enter(tran);
			else
				mmc_state_enter(stby);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_CMD_FAILURE) {
			mmc_state_enter(__ina);
			unraise_bits(flags, MMC_EVENT_CMD_FAILURE);
		}
	} else if (mmc_state_is(tran)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_SELECT_DESELECT_CARD))
				mmc_state_enter(stby);
			else {
				if (mmc_cmd_is(MMC_CMD_STOP_TRANSMISSION))
					raise_bits(mmc_slot_ctrl.flags,
						   MMC_SLOT_TRANS_STOP);
				mmc_state_enter(tran);
			}
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_CMD_FAILURE) {
			if (mmc_cmd_is_read_block() ||
			    mmc_cmd_is_write_block())
				mmc_state_enter(__ina);
			else
				mmc_state_enter(tran);
			unraise_bits(flags, MMC_EVENT_CMD_FAILURE);
		}
	}
	mmc_event_restore(flags);
}

static void sd_recv_acmd(void)
{
	uint32_t arg;

	switch (mmc_slot_ctrl.acmd) {
	case SD_ACMD_SET_BUS_WIDTH:
		mmc_config_width(mmc_bus_width());
		break;
	case SD_ACMD_SD_STATUS:
		/* TODO: parse SD_STATUS */
		break;
	case SD_ACMD_SEND_SCR:
		mmc_slot_ctrl.host_scr = sd_decode_scr();
		mmc_slot_ctrl.scr_valid = true;
		if (mmc_slot_ctrl.host_scr.bus_widths &
		    SD_BUS_WIDTH_4BIT_SUPPORT)
			arg = MMC_MODE_4BIT;
		else
			arg = MMC_MODE_1BIT;
		/* Switch to F_PP */
		if (mmc_slot_ctrl.mode == MMC_IDENT)
			mmc_select_modes(MMC_LEGACY, arg);
		break;
	}
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
	mmc_slot_ctrl.csr = MAKELONG(MAKEWORD(r1[3], r1[2]),
				     MAKEWORD(r1[1], r1[0]));
	if (mmc_cmd_is_app() && !mmc_get_block_data() &&
	    mmc_slot_ctrl.flags & MMC_SLOT_WAIT_APP_ACMD)
		sd_recv_acmd();
}

bool sd_resp_r1b(void)
{
	sd_resp_r1();
	return !mmc_hw_card_busy();
}

void sd_resp_r2(void)
{
	mmc_r2_t r2;

	mmc_hw_recv_response(r2, 16);
	if (mmc_slot_ctrl.cmd == MMC_CMD_SEND_CSD) {
		mmc_slot_ctrl.csd = sd_decode_csd(r2);
		mmc_slot_ctrl.csd_valid = true;
		if (mmc_slot_ctrl.csd.tran_speed == 0x5A)
			mmc_slot_ctrl.default_speed = SD_FREQ_HS;
		else /* 0x32 */
			mmc_slot_ctrl.default_speed = SD_FREQ_PP;
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
	mmc_slot_ctrl.rca = MAKEWORD(r6[1], r6[0]);
	/* Decode card status */
	cs = MAKEWORD(r6[3], r6[2]);
#ifdef CONFIG_MMC_DEBUG
	printf("RCA: %d\n", mmc_slot_ctrl.rca);
	printf("Card status: 0x%04x\n", cs);
#endif
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
	if (r7[3] != SD_CHECK_PATTERN) {
		mmc_rsp_failure(MMC_ERR_CHECK_PATTERN);
		return false;
	}
	if (r7[2] != sd_get_vhs()) {
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
		if (!sd_resp_r1b())
			return;
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

static void sd_set_block_data(bool trans)
{
	if (trans && mmc_slot_ctrl.trans_buf)
		mmc_slot_ctrl.block_data = mmc_slot_ctrl.trans_buf;
	else {
		/* Note: register width shouldn't exceed MMC_DEF_BL_LEN */
		mmc_slot_ctrl.block_data = mmc_slot_ctrl.dat;
	}
}

void sd_send_acmd(void)
{
	uint32_t arg = 0;
	uint8_t width;

	switch (mmc_slot_ctrl.acmd) {
	case SD_ACMD_SET_BUS_WIDTH:
		mmc_slot_ctrl.rsp = MMC_R1;
		if (mmc_bus_width() == 4)
			width = SD_BUS_WIDTH_4BIT;
		else
			width = SD_BUS_WIDTH_1BIT;
		arg = SD_BUS_WIDTH(width);
		break;
	case SD_ACMD_SD_STATUS:
		mmc_slot_ctrl.rsp = MMC_R1;
		/* 512-bits SD status */
		MMC_BLOCK(READ, SD_SD_STATUS_DATA_LEN, 1);
		sd_set_block_data(false);
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
		MMC_BLOCK(READ, SD_SCR_DATA_LEN, 1);
		sd_set_block_data(false);
		break;
	default:
		break;
	}
	mmc_hw_send_command(mmc_slot_ctrl.acmd, arg);
}

static uint32_t sd_block_address(void)
{
	uint32_t address;

	if (mmc_slot_ctrl.high_capacity)
		address = mmc_slot_ctrl.address / mmc_slot_ctrl.capacity_len;
	else
		address = mmc_slot_ctrl.address;
#ifdef CONFIG_MMC_DEBUG
	printf("ARGUMENT(address): 0x%08lx\n", address);
#endif
	return address;
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
		/* TODO: Fix capacity_len/capacity_cnt */
		break;
#endif
#ifdef SD_CLASS2
	case MMC_CMD_READ_SINGLE_BLOCK:
		MMC_BLOCK(READ, mmc_slot_ctrl.trans_len, 1);
		sd_set_block_data(true);
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = sd_block_address();
		break;
	case MMC_CMD_READ_MULTIPLE_BLOCK:
		MMC_BLOCK(READ, mmc_slot_ctrl.trans_len,
			  mmc_slot_ctrl.trans_cnt);
		sd_set_block_data(true);
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = sd_block_address();
		break;
	case SD_CMD_SEND_TUNING_BLOCK:
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = 0;
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
		MMC_BLOCK(WRITE, mmc_slot_ctrl.trans_len, 1);
		sd_set_block_data(true);
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = sd_block_address();
		break;
	case MMC_CMD_WRITE_MULTIPLE_BLOCK:
		MMC_BLOCK(WRITE, mmc_slot_ctrl.trans_len,
			  mmc_slot_ctrl.trans_cnt);
		sd_set_block_data(true);
		mmc_slot_ctrl.rsp = MMC_R1;
		arg = sd_block_address();
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
		arg = sd_block_address();
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

void mmc_phy_start_dat(void)
{
	mmc_hw_tran_data(mmc_slot_ctrl.block_data,
			 mmc_slot_ctrl.block_len,
			 mmc_slot_ctrl.block_cnt);
}

void mmc_phy_stop_dat(void)
{
	if (mmc_cmd_is_app() && mmc_get_block_data())
		sd_recv_acmd();
}

void mmc_phy_reset_slot(void)
{
	mmc_slot_ctrl.card_version = SD_PHY_VERSION_UNKNOWN;
}
