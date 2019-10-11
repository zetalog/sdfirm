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
#define MMC_EVENT_SEND_OCR	_BV(1)
#define MMC_EVENT_SEND_ALL_CID	_BV(2)
#define MMC_EVENT_SEND_RCA	_BV(3)
#define MMC_EVENT_SEND_CID	_BV(4)
#define MMC_EVENT_SEND_CSD	_BV(5)

bh_t mmc_bh = INVALID_BH;

#if NR_IFD_SLOTS > 1
mmc_sid_t mmc_slid = INVALID_MMC_SID;
struct mmc_slot mmc_slots[NR_MMC_SLOTS];
mmc_event_t mmc_slot_events[NR_MMC_SLOTS];
#define mmc_slot_event		mmc_slot_events[mmc_slid]
#else
struct mmc_slot mmc_slot_ctrl;
mmc_event_t mmc_slot_event;
#endif

#define MMC_EVENT_POWER_ON	_BV(0)

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

const char *mmc_cmd_name(uint8_t cmd_index)
{
	return "unk";
}

void mmc_debug(uint8_t tag, uint32_t val)
{
	switch (tag) {
	case MMC_DEBUG_STATE:
		printf("state %s\n", mmc_state_name(val));
		break;
	case MMC_DEBUG_CMD:
		printf("cmd %s\n", mmc_cmd_name(val));
		break;
	default:
		printf("Unknown debug tag 0x%02x\n", tag);
		break;
	}
}
#endif

void mmc_event_raise(mmc_event_t event)
{
	mmc_debug_event(event);
	mmc_slot_event |= event;
	bh_resume(mmc_bh);
}

static mmc_event_t mmc_event_save(void)
{
	mmc_event_t events;
	events = mmc_slot_event;
	mmc_slot_event = 0;
	return events;
}

static void mmc_event_restore(mmc_event_t event)
{
	mmc_slot_event |= event;
}

uint8_t mmc_state_get(void)
{
	return mmc_slot_ctrl.state;
}

void mmc_state_set(uint8_t state)
{
	mmc_debug_state(state);
	mmc_slot_ctrl.state = state;
	//mmc_handle_slot_seq();
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

static void mmc_async_handler(void)
{
	mmc_event_t flags = mmc_event_save();

	if (flags & MMC_EVENT_SEND_OCR) {
		if (mmc_state_is(idle))
			mmc_state_enter(ready);
		unraise_bits(flags, MMC_EVENT_SEND_OCR);
	}
	if (flags & MMC_EVENT_SEND_ALL_CID) {
		if (mmc_state_is(ready))
			mmc_state_enter(ident);
		unraise_bits(flags, MMC_EVENT_SEND_ALL_CID);
	}
	if (flags & MMC_EVENT_SEND_RCA) {
		if (mmc_state_is(ident))
			mmc_state_enter(stby);
		unraise_bits(flags, MMC_EVENT_SEND_RCA);
	}
	mmc_event_restore(flags);
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

void mmc_init(void)
{
	mmc_sid_t sid;
	__unused mmc_sid_t ssid;

	DEVICE_INTF(DEVICE_INTF_MMC);
	mmc_bh = bh_register_handler(mmc_handler);
	mmc_hw_ctrl_init();

	for (sid = 0; sid < NR_MMC_SLOTS; sid++) {
		ssid = mmc_sid_save(sid);
		/* MMC only handles states after powered on */
		mmc_state_enter(idle);
		//mmc_send_ocr();
		mmc_sid_restore(ssid);
	}
}
