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
 * @(#)mmc.c: multimedia card (MMC) implementation
 * $Id: mmc.c,v 1.1 2019-10-08 18:24:00 zhenglv Exp $
 */

#include <target/mmc.h>

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
	"btst",
	"irq",
	NULL,
	NULL,
	NULL,
	NULL,
	"ina",
};

const char *mmc_phy_cmd_names[64] = {
	"GO_IDLE_STATE",
	"SEND_OP_COND",
	"ALL_SEND_CID",
	"SET_RELATIVE_ADDR",
	"SET_DSR",
	NULL,
	"SWITCH",
	"SELECT_DESELECT_CARD",
	"SEND_EXT_CSD",
	"SEND_CSD",
	"SEND_CID",
	"READ_DAT_UNTIL_STOP",
	"STOP_TRANSMISSION",
	"SEND_STATUS",
	"BUSTEST_R",
	"GO_INACTIVE_STATE",
	"SET_BLOCKLEN",
	"READ_SINGLE_BLOCK",
	"READ_MULTIPLE_BLOCK",
	"BUSTEST_W",
	"WRITE_DATA_UNTIL_STOP",
	NULL,
	NULL,
	"SET_BLOCK_COUNT",
	"WRITE_BLOCK",
	"WRITE_MULTIPLE_BLOCK",
	"PROGRAM_CID",
	"PROGRAM_CSD",
	"SET_WRITE_PROT",
	"CLR_WRITE_PROT",
	"SEND_WRITE_PROT",
	NULL,
	NULL,
	NULL,
	NULL,
	"ERASE_GROUP_START",
	"ERASE_GROUP_END",
	NULL,
	"ERASE",
	"FAST_IO",
	"GO_IRQ_STATE",
	NULL,
	"LOCK_UNLOCK",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"APP_CMD",
	"GEN_CMD",
};
#endif

static void mmc_handle_identify_card(void)
{
	if (mmc_cmd_is(MMC_CMD_NONE))
		mmc_cmd(MMC_CMD_GO_IDLE_STATE);
	else if (mmc_state_is(idle)) {
		mmc_cmd(MMC_CMD_SEND_OP_COND);
	} else if (mmc_state_is(ready))
		mmc_cmd(MMC_CMD_ALL_SEND_CID);
	else if (mmc_state_is(ident))
		mmc_cmd(MMC_CMD_SET_RELATIVE_ADDR);
	else if (mmc_state_is(stby))
		mmc_op_success();
	else if (mmc_state_is(ina))
		mmc_op_failure();
}

void mmc_phy_handle_seq(void)
{
	if (mmc_op_is(MMC_OP_IDENTIFY_CARD))
		mmc_handle_identify_card();
}

static mmc_cid_t mmc_decode_cid(mmc_r2_t raw_cid)
{
	mmc_cid_t cid;

#ifdef CONFIG_MMC_CID_ALL_FIELDS
	cid.crc = raw_cid[0] >> 1;
	cid.mdt = raw_cid[1];
	cid.psn = MAKELONG(MAKEWORD(raw_cid[2], raw_cid[3]),
			   MAKEWORD(raw_cid[4], raw_cid[5]));
	cid.prv = raw_cid[6];
	memcpy(cid.pnm, &raw_cid[7], 6);
#endif
	cid.oid = MAKEWORD(raw_cid[10], raw_cid[11]);
	cid.mid = raw_cid[12];
	return cid;
}

static mmc_csd_t mmc_decode_csd(mmc_r2_t raw_csd)
{
	mmc_csd_t csd;

	csd.csd0 = MAKELONG(MAKEWORD(raw_csd[0], raw_csd[1]),
			    MAKEWORD(raw_csd[2], raw_csd[3]));
	csd.csd21 = MAKELLONG(MAKELONG(MAKEWORD(raw_csd[4],
						raw_csd[5]),
				       MAKEWORD(raw_csd[6],
					        raw_csd[7])),
			      MAKELONG(MAKEWORD(raw_csd[8],
					        raw_csd[9]),
				       MAKEWORD(raw_csd[10],
					        raw_csd[11])));
	csd.csd3 = MAKELONG(MAKEWORD(raw_csd[12], raw_csd[13]),
			    MAKEWORD(raw_csd[14], raw_csd[15]));
	return csd;
}

static uint32_t mmc_decode_ocr(mmc_r3_t raw_ocr)
{
	uint32_t ocr;

	ocr = MAKELONG(MAKEWORD(raw_ocr[0], raw_ocr[1]),
		       MAKEWORD(raw_ocr[2], raw_ocr[3]));
	return ocr;
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
	if (flags & MMC_EVENT_POWER_ON) {
		mmc_state_enter(idle);
		unraise_bits(flags, MMC_EVENT_POWER_ON);
	} else if (flags & MMC_EVENT_CMD_SUCCESS &&
		   mmc_cmd_is(MMC_CMD_GO_INACTIVE_STATE) &&
		   mmc_state_ge(stby)) {
		mmc_state_enter(ina);
		unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
	} else if (flags & MMC_EVENT_CMD_SUCCESS &&
		   mmc_cmd_is(MMC_CMD_GO_IDLE_STATE) &&
		   !mmc_state_is(ina)) {
		mmc_state_enter(idle);
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
			if (mmc_cmd_is(MMC_CMD_GO_IDLE_STATE));
			if (mmc_cmd_is(MMC_CMD_SEND_OP_COND))
				mmc_state_enter(ready);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_CMD_FAILURE) {
			if (mmc_err_is(MMC_ERR_CARD_IS_BUSY));
			if (mmc_err_is(MMC_ERR_HOST_OMIT_VOLT));
			if (mmc_err_is(MMC_ERR_CARD_NON_COMP_VOLT))
				mmc_state_enter(ina);
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
			if (mmc_cmd_is(MMC_CMD_SET_RELATIVE_ADDR))
				mmc_state_enter(stby);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
	/* irq mode */
	} else if (mmc_state_is(irq)) {
		if (flags & MMC_EVENT_NO_IRQ) {
			mmc_state_enter(stby);
			unraise_bits(flags, MMC_EVENT_NO_IRQ);
		}
		if (flags & MMC_EVENT_CARD_IRQ) {
			mmc_state_enter(stby);
			unraise_bits(flags, MMC_EVENT_CARD_IRQ);
		}
	/* data transfer mode */
	} else if (mmc_state_is(stby)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_SET_DSR));
			if (mmc_cmd_is(MMC_CMD_SEND_CSD));
			if (mmc_cmd_is(MMC_CMD_SEND_CID));
			if (mmc_cmd_is(MMC_CMD_SELECT_DESELECT_CARD))
				mmc_state_enter(tran);

			if (mmc_cmd_is_fio());
			if (mmc_cmd_is_irq())
				mmc_state_enter(irq);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
	} else if (mmc_state_is(tran)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_SET_BLOCK_COUNT));
			if (mmc_cmd_is(MMC_CMD_SELECT_DESELECT_CARD))
				mmc_state_enter(stby);
			if (mmc_cmd_is(MMC_CMD_BUSTEST_W))
				mmc_state_enter(btst);
			if (mmc_cmd_is(MMC_CMD_SWITCH))
				mmc_state_enter(prg);
			if (mmc_cmd_is(MMC_CMD_SEND_EXT_CSD))
				mmc_state_enter(data);

			if (mmc_cmd_is_erase_group());
			if (mmc_cmd_is_block_len());
			if (mmc_cmd_is_write_prot_bit() ||
			    mmc_cmd_is_erase())
				mmc_state_enter(prg);
			if (mmc_cmd_is_read_stream() ||
			    mmc_cmd_is_read_block() ||
			    mmc_cmd_is_write_prot() ||
			    mmc_cmd_is_gen_r())
				mmc_state_enter(data);
			if (mmc_cmd_is_write_stream() ||
			    mmc_cmd_is_write_block() ||
			    mmc_cmd_is_program() ||
			    mmc_cmd_is_lock() ||
			    mmc_cmd_is_gen_w())
				mmc_state_enter(rcv);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
	} else if (mmc_state_is(data)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_SELECT_DESELECT_CARD))
				mmc_state_enter(stby);
			if (mmc_cmd_is(MMC_CMD_STOP_TRANSMISSION))
				mmc_state_enter(tran);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_OP_COMPLETE) {
			mmc_state_enter(tran);
			unraise_bits(flags, MMC_EVENT_OP_COMPLETE);
		}
	} else if (mmc_state_is(rcv)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_STOP_TRANSMISSION))
				mmc_state_enter(prg);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_TRANS_END) {
			mmc_state_enter(prg);
			unraise_bits(flags, MMC_EVENT_TRANS_END);
		}
	} else if (mmc_state_is(prg)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_SELECT_DESELECT_CARD))
				mmc_state_enter(dis);

			if (mmc_cmd_is_write_block())
				mmc_state_enter(rcv);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_OP_COMPLETE) {
			mmc_state_enter(tran);
			unraise_bits(flags, MMC_EVENT_OP_COMPLETE);
		}
	} else if (mmc_state_is(dis)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_SELECT_DESELECT_CARD))
				mmc_state_enter(prg);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_OP_COMPLETE) {
			mmc_state_enter(stby);
			unraise_bits(flags, MMC_EVENT_OP_COMPLETE);
		}
	} else if (mmc_state_is(btst)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_BUSTEST_R))
				mmc_state_enter(tran);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
	}
	mmc_event_restore(flags);
}

void mmc_resp_r0(void)
{
	mmc_hw_recv_response(NULL, 0);
}

void mmc_resp_r1(void)
{
	mmc_r1_t r1;

	mmc_hw_recv_response(r1, 4);
	raise_bits(mmc_slot_ctrl.flags, MMC_SLOT_CARD_STATUS_VALID);
	mmc_slot_ctrl.csr = MAKELONG(MAKEWORD(r1[0], r1[1]),
				     MAKEWORD(r1[2], r1[3]));
}

void mmc_resp_r1b(void)
{
	mmc_resp_r1();
	if (mmc_hw_card_busy())
		raise_bits(mmc_slot_ctrl.flags, MMC_SLOT_CARD_IS_BUSY);
	else
		unraise_bits(mmc_slot_ctrl.flags, MMC_SLOT_CARD_IS_BUSY);
}

void mmc_resp_r2(void)
{
	mmc_r2_t r2;

	mmc_hw_recv_response(r2, 16);
	if (mmc_slot_ctrl.cmd == MMC_CMD_SEND_CSD)
		mmc_slot_ctrl.csd = mmc_decode_csd(r2);
	else
		mmc_slot_ctrl.cid = mmc_decode_cid(r2);
}

void mmc_resp_r3(void)
{
	mmc_r3_t r3;

	mmc_hw_recv_response(r3, 4);
	mmc_slot_ctrl.ocr = mmc_decode_ocr(r3);
}

#ifdef MMC_CLASS9
void mmc_resp_r4(void)
{
}

void mmc_resp_r5(void)
{
}
#endif

void mmc_phy_recv_rsp(void)
{
	switch (mmc_slot_ctrl.cmd) {
	case MMC_CMD_SET_RELATIVE_ADDR:
	case MMC_CMD_SEND_EXT_CSD:
	case MMC_CMD_SEND_STATUS:
	case MMC_CMD_BUSTEST_R:
	case MMC_CMD_BUSTEST_W:
#ifdef MMC_CLASS1
	case MMC_CMD_READ_DAT_UNTIL_STOP:
#endif
#if defined(MMC_CLASS2) || defined(MMC_CLASS4)
	case MMC_CMD_SET_BLOCKLEN:
#endif
#ifdef MMC_CLASS2
	case MMC_CMD_READ_SINGLE_BLOCK:
	case MMC_CMD_READ_MULTIPLE_BLOCK:
#endif
#ifdef MMC_CLASS3
	case MMC_CMD_WRITE_DAT_UNTIL_STOP:
#endif
	case MMC_CMD_SET_BLOCK_COUNT:
#ifdef MMC_CLASS4
	case MMC_CMD_WRITE_BLOCK:
	case MMC_CMD_WRITE_MULTIPLE_BLOCK:
	case MMC_CMD_PROGRAM_CID:
	case MMC_CMD_PROGRAM_CSD:
#endif
#ifdef MMC_CLASS6
	case MMC_CMD_SEND_WRITE_PROT:
#endif
#ifdef MMC_CLASS5
	case MMC_CMD_ERASE_GROUP_START:
	case MMC_CMD_ERASE_GROUP_END:
#endif
#ifdef MMC_CLASS8
	case MMC_CMD_APP_CMD:
#endif
		mmc_resp_r1();
		break;
	case MMC_CMD_SWITCH:
	case MMC_CMD_SELECT_DESELECT_CARD:
	case MMC_CMD_STOP_TRANSMISSION:
#ifdef MMC_CLASS6
	case MMC_CMD_SET_WRITE_PROT:
	case MMC_CMD_CLR_WRITE_PROT:
#endif
#ifdef MMC_CLASS5
	case MMC_CMD_ERASE:
#endif
#ifdef MMC_CLASS7
	case MMC_CMD_LOCK_UNLOCK:
#endif
#ifdef MMC_CLASS8
	case MMC_CMD_GEN_CMD:
#endif
		mmc_resp_r1b();
		break;
	case MMC_CMD_ALL_SEND_CID:
	case MMC_CMD_SEND_CID:
	case MMC_CMD_SEND_CSD:
		mmc_resp_r2();
		break;
	case MMC_CMD_SEND_OP_COND:
		mmc_resp_r3();
		break;
#ifdef MMC_CLASS9
	case MMC_CMD_FAST_IO:
		mmc_resp_r4();
		break;
	case MMC_CMD_GO_IRQ_STATE:
		mmc_resp_r5();
		break;
#endif
	default:
		mmc_resp_r0();
		break;
	}
	mmc_rsp_success();
}

void mmc_phy_send_cmd(void)
{
	uint32_t arg = 0;

	switch (mmc_slot_ctrl.cmd) {
	case MMC_CMD_SEND_OP_COND:
		arg = mmc_slot_ctrl.ocr;
		break;
	case MMC_CMD_SET_RELATIVE_ADDR:
	case MMC_CMD_SELECT_DESELECT_CARD:
	case MMC_CMD_SEND_CSD:
	case MMC_CMD_SEND_CID:
	case MMC_CMD_SEND_STATUS:
	case MMC_CMD_GO_INACTIVE_STATE:
#ifdef MMC_CLASS8
	case MMC_CMD_APP_CMD:
#endif
		arg = MAKELONG(0, mmc_rca);
		break;
	case MMC_CMD_SET_DSR:
		arg = MAKELONG(0, mmc_slot_ctrl.dsr);
		break;
	case MMC_CMD_SWITCH:
		/* card mode */
		break;
#if defined(MMC_CLASS2) || defined(MMC_CLASS4)
	case MMC_CMD_SET_BLOCKLEN:
		arg = mmc_slot_ctrl.block_len;
		break;
#endif
	case MMC_CMD_SET_BLOCK_COUNT:
		arg = MAKELONG(mmc_slot_ctrl.block_cnt, 0);
		break;
#ifdef MMC_CLASS1
	case MMC_CMD_READ_DAT_UNTIL_STOP:
#endif
#ifdef MMC_CLASS2
	case MMC_CMD_READ_SINGLE_BLOCK:
	case MMC_CMD_READ_MULTIPLE_BLOCK:
#endif
#ifdef MMC_CLASS3
	case MMC_CMD_WRITE_DAT_UNTIL_STOP:
#endif
#ifdef MMC_CLASS4
	case MMC_CMD_WRITE_BLOCK:
	case MMC_CMD_WRITE_MULTIPLE_BLOCK:
#endif
#ifdef MMC_CLASS5
	case MMC_CMD_ERASE_GROUP_START:
	case MMC_CMD_ERASE_GROUP_END:
#endif
#ifdef MMC_CLASS6
	case MMC_CMD_SET_WRITE_PROT:
	case MMC_CMD_CLR_WRITE_PROT:
	case MMC_CMD_SEND_WRITE_PROT:
#endif
#if defined(MMC_CLASS1) || defined(MMC_CLASS2) || defined(MMC_CLASS3) || \
    defined(MMC_CLASS4) || defined(MMC_CLASS5) || defined(MMC_CLASS6)
		arg = mmc_slot_ctrl.address;
		break;
#endif
#ifdef MMC_CLASS9
	case MMC_CMD_FAST_IO:
		arg = MAKELONG(MAKEWORD(mmc_slot_ctrl.reg_data,
					mmc_slot_ctrl.reg_addr),
			       mmc_rca);
		break;
#endif
#ifdef MMC_CLASS8
	case MMC_CMD_GEN_CMD:
		arg = mmc_slot_ctrl.flags & MMC_SLOT_GEN_CMD_RDWR ? 1 : 0;
		break;
#endif
	}
	mmc_hw_send_command(mmc_slot_ctrl.cmd, arg);
}
