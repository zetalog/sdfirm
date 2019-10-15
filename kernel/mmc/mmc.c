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
#include <target/devid.h>
#include <target/bh.h>
#include <target/panic.h>
#include <stdio.h>

#define MMC_EVENT_POWER_ON	_BV(0)
#define MMC_EVENT_CMD_SUCCESS	_BV(1)
#define MMC_EVENT_CMD_FAILURE	_BV(2)
#define MMC_EVENT_CARD_IRQ	_BV(3)
#define MMC_EVENT_NO_IRQ	_BV(4)
#define MMC_EVENT_OP_COMPLETE	_BV(5)
#define MMC_EVENT_TRANS_END	_BV(6)

bh_t mmc_bh = INVALID_BH;

#if NR_IFD_SLOTS > 1
mmc_sid_t mmc_slid = INVALID_MMC_SID;
struct mmc_slot mmc_slots[NR_MMC_SLOTS];
#else
struct mmc_slot mmc_slot_ctrl;
#endif

#if defined(CONFIG_CONSOLE) && defined(CONFIG_MMC_DEBUG)
const char *mmc_state_names[] = {
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
	"ina",
	"irq",
};

const char *mmc_state_name(uint32_t val)
{
	if (val >= ARRAY_SIZE(mmc_state_names))
		return "unk";
	return mmc_state_names[val];
}

const char *mmc_cmd_names[] = {
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

const char *mmc_cmd_name(uint8_t cmd_index)
{
	if (cmd_index >= ARRAY_SIZE(mmc_cmd_names))
		return "UNDEF";
	if (mmc_cmd_names[cmd_index])
		return mmc_cmd_names[cmd_index];
	else
		return "NULL";
}

const char *mmc_event_names[] = {
	"POWER_ON",
	"CMD_SUCCESS",
	"CMD_FAILURE",
	"CARD_IRQ",
	"NO_IRQ",
	"OP_COMPLETE",
	"TRANS_END",
};

const char *mmc_event_name(mmc_event_t event)
{
	int i, nr_evts = 0;

	for (i = 0; i < ARRAY_SIZE(mmc_event_names); i++) {
		if (_BV(i) & event) {
			nr_evts++;
			return mmc_event_names[i];
		}
	}
	return "NONE";
}

const char *mmc_op_names[] = {
	"NO_OP",
	"IDENTIFY_CARD",
};

const char *mmc_op_name(uint8_t op)
{
	if (op >= ARRAY_SIZE(mmc_op_names))
		return "NO-OP";
	return mmc_op_names[op];
}

const char *mmc_error_names[] = {
	"no error",
	"card is busy",
	"host omits voltage range",
	"card is with non compatible voltage range",
	"card looses bus",
};

const char *mmc_error_name(uint8_t error)
{
	if (error >= ARRAY_SIZE(mmc_error_names))
		return "no error";
	return mmc_error_names[error];
}

void mmc_debug(uint8_t tag, uint32_t val)
{
	switch (tag) {
	case MMC_DEBUG_STATE:
		printf("state %s\n", mmc_state_name(val));
		break;
	case MMC_DEBUG_EVENT:
		printf("event %s\n", mmc_event_name(val));
		break;
	case MMC_DEBUG_ERROR:
		printf("error %s\n", mmc_error_name(val));
		break;
	case MMC_DEBUG_CMD:
		printf("cmd %s\n", mmc_cmd_name(val));
		break;
	case MMC_DEBUG_OP:
		printf("op %s\n", mmc_op_name(val));
		break;
	default:
		printf("Unknown debug tag 0x%02x\n", tag);
		break;
	}
}
#endif

void mmc_handle_slot_seq(void)
{
	if (mmc_op_is(MMC_OP_IDENTIFY_CARD)) {
	}
}

void mmc_event_raise(mmc_event_t event)
{
	mmc_debug_event(event);
	mmc_slot_ctrl.event |= event;
	bh_resume(mmc_bh);
}

static mmc_event_t mmc_event_save(void)
{
	mmc_event_t events;
	events = mmc_slot_ctrl.event;
	mmc_slot_ctrl.event = 0;
	return events;
}

static void mmc_event_restore(mmc_event_t event)
{
	mmc_slot_ctrl.event |= event;
}

uint8_t mmc_state_get(void)
{
	return mmc_slot_ctrl.state;
}

void mmc_state_set(uint8_t state)
{
	mmc_debug_state(state);
	mmc_slot_ctrl.state = state;
	mmc_handle_slot_seq();
}

#if NR_MMC_SLOTS > 1
void mmc_sid_restore(mmc_sid_t sid)
{
	mmc_slid = sid;
	mmc_hw_slot_select(sid);
}

mmc_sid_t mmc_sid_save(mmc_sid_t sid)
{
	mmc_sid_t o_sid = mmc_slid;
	mmc_sid_restore(sid);
	return o_sid;
}
#endif

mmc_cid_t mmc_decode_cid(mmc_raw_cid_t raw_cid)
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

mmc_csd_t mmc_decode_csd(mmc_raw_csd_t raw_csd)
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

/* This function implements the state digrams in the following sections:
 * 7.2 Card Identification Mode
 * 7.3 Interrupt Mode
 * 7.4 Data Transfer Mode
 */
static void mmc_handle_slot_state(void)
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
			if (mmc_cmd_is_fio());
			if (mmc_cmd_is_irq())
				mmc_state_enter(irq);
			if (mmc_cmd_is(MMC_CMD_SELECT_CARD))
				mmc_state_enter(tran);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
	} else if (mmc_state_is(tran)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is_grp_se());
			if (mmc_cmd_is(MMC_CMD_SET_BLOCK_COUNT));
			if (mmc_cmd_is_blk_len());
			if (mmc_cmd_is(MMC_CMD_SELECT_CARD))
				mmc_state_enter(stby);
			if (mmc_cmd_is(MMC_CMD_SWITCH) ||
			    mmc_cmd_is_prot_sc() ||
			    mmc_cmd_is_grp())
				mmc_state_enter(prg);
			if (mmc_cmd_is(MMC_CMD_SEND_EXT_CSD) ||
			    mmc_cmd_is_rstr() || mmc_cmd_is_rblk() ||
			    mmc_cmd_is_wprot() || mmc_cmd_is_gen())
				mmc_state_enter(data);
			if (mmc_cmd_is_wstr() || mmc_cmd_is_wblk() ||
			    mmc_cmd_is_prg() || mmc_cmd_is_lck() ||
			    mmc_cmd_is_gen())
				mmc_state_enter(rcv);
			if (mmc_cmd_is(MMC_CMD_BUSTEST_W))
				mmc_state_enter(btst);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
	} else if (mmc_state_is(data)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_SELECT_CARD))
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
			if (mmc_cmd_is(MMC_CMD_SELECT_CARD))
				mmc_state_enter(dis);
			if (mmc_cmd_is_wblk())
				mmc_state_enter(rcv);
			unraise_bits(flags, MMC_EVENT_CMD_SUCCESS);
		}
		if (flags & MMC_EVENT_OP_COMPLETE) {
			mmc_state_enter(tran);
			unraise_bits(flags, MMC_EVENT_OP_COMPLETE);
		}
	} else if (mmc_state_is(dis)) {
		if (flags & MMC_EVENT_CMD_SUCCESS) {
			if (mmc_cmd_is(MMC_CMD_SELECT_CARD))
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

static void mmc_async_handler(void)
{
	mmc_sid_t sid;
	__unused mmc_sid_t ssid;

	for (sid = 0; sid < NR_MMC_SLOTS; sid++) {
		ssid = mmc_sid_save(sid);
		mmc_handle_slot_state();
		mmc_sid_restore(ssid);
	}
}

static void mmc_handler(uint8_t event)
{
	if (event != BH_POLLIRQ) {
		switch (event) {
		case BH_WAKEUP:
			mmc_async_handler();
			break;
		default:
			BUG();
		}
	}
}

void mmc_cmd_success(void)
{
	mmc_event_raise(MMC_EVENT_CMD_SUCCESS);
}

void mmc_cmd_failure(uint8_t err)
{
	mmc_slot_ctrl.err = err;
	mmc_debug_error(err);
	mmc_event_raise(MMC_EVENT_CMD_FAILURE);
}

void mmc_send_cmd(uint8_t cmd)
{
	mmc_slot_ctrl.cmd = cmd;
	mmc_slot_ctrl.err = MMC_ERR_NO_ERROR;
	mmc_debug_cmd(cmd);
	mmc_hw_send_command(cmd);
}

int mmc_start_op(uint8_t op, mmc_cmpl_cb cb)
{
	if (mmc_slot_ctrl.op != MMC_OP_NO_OP)
		return -EBUSY;

	mmc_slot_ctrl.op = op;
	mmc_slot_ctrl.op_cb = cb;
	mmc_slot_ctrl.cmd = MMC_CMD_INVALID;
	mmc_debug_op(op);

	switch (op) {
	case MMC_OP_IDENTIFY_CARD:
		mmc_event_raise(MMC_EVENT_POWER_ON);
		break;
	}
	return 0;
}

void mmc_reset_slot(void)
{
	mmc_slot_ctrl.op = MMC_OP_NO_OP;
	mmc_slot_ctrl.state = MMC_STATE_idle;
	mmc_slot_ctrl.event = 0;
	mmc_identify_card();
}

void mmc_init(void)
{
	mmc_sid_t sid;
	__unused mmc_sid_t ssid;

	DEVICE_INTF(DEVICE_INTF_MMC);
	mmc_bh = bh_register_handler(mmc_handler);
	mmc_hw_ctrl_init();

	for (sid = 0; sid < NR_MMC_SLOTS; sid++) {
		ssid = mmc_sid_save(sid);
		mmc_reset_slot();
		mmc_sid_restore(ssid);
	}
}
