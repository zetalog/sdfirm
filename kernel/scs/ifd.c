/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2009
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
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
 * @(#)ifd.c: ISO/IEC 7816-3 interface device functions
 * $Id: ifd.c,v 1.190 2011-11-11 10:50:06 zhenglv Exp $
 */

/* TODO:
 *   1. warm reset
 *   2. proprietary PPS algorithm
 */
/* We will never support following features:
 * 1. character level exchage:
 *    USB should be fast enough in order to make PC_to_RDR_XfrBlock run
 *    fast to meet the timing requirement of transmission protocols
 *    defined in ISO7816-3.  It is hardly for slow devices to achieve this
 *    purpose.
 * 2. proprietary PPS algorithm:
 *    Either keep the ATR returned by the cards or implement atr_build
 *    callbacks for protocols.  Firmware may warm reset cards on exchange
 *    failures.
 *    Small devices don't have enough ram to keep ATRs or enough code rom
 *    to implement atr_build and warm reset codes.
 *    But if someone choose to implement such PPS algorithm, the code
 *    should look like:
 *    Calculates:
 *      d = (f * D / F)
 *    Where:
 *      F = F_table[Fi]
 *      D = D_table[Di]
 *      f <= fmax_table[Fi]
 *    Ensures:
 *      d < dmax_ifd
 *      f < fmax_ifd
 *      Fi < Fi_icc, Di < Di_icc
 *    Where:
 *      Fi_icc and Di_icc is reported by ICC through ATR's TA1;
 *      fmax_ifd and dmax_ifd is reported by IFD through CCID descriptor's
 *      dwMaximumClock and dwMaxDataRate.
 */
#include "ifd_priv.h"
#include <target/lcd.h>
#include <target/state.h>
#include <target/delay.h>
#include <target/jiffies.h>
#include <target/timer.h>

/*========================================================================
 * slot command sequences
 *=======================================================================*/
static uint8_t ifd_seq_get(void);
static void ifd_seq_set(uint8_t seq); 
static void ifd_seq_reset(void);

static void ifd_seq_onoff(boolean on);
static void ifd_seq_complete(scs_err_t err);
static scs_err_t ifd_seq_start(uint8_t seq);

static void ifd_handle_slot_seq(void);
static void ifd_handle_xchg_seq(void);

static scs_err_t scs_auto_seq(boolean cmpl);

static void ifd_handle_auto_seq(void);
static void ifd_handle_power_on(void);
static void ifd_handle_power_off(void);
static void ifd_handle_set_param(void);
static void ifd_handle_interchange_xchg(void);
static void ifd_handle_interchange_slot(void);

/*========================================================================
 * interface device automatisms
 *=======================================================================*/
static scs_err_t ifd_cls_select(void);
static void ifd_deactivate(void);
static void ifd_activate(void);
static void ifd_reset(void);
#if 0
static void ifd_warm_reset(void);
static void ifd_suspend_clock(void);
static void ifd_resume_clock(void);
#endif

static void ifd_spec_mode(void);
static void ifd_nego_1st(void);
static void ifd_nego_pps(void);
static void ifd_info_xchg(void);
static void ifd_xchg_sync(void);

static void ifd_null_expire(void);
/*=======================================================================
 * slot interfaces
 *======================================================================*/
static ifd_event_t ifd_event_save(void);
static void ifd_event_restore(ifd_event_t event);
static void ifd_slot_reset(void);

static void ifd_handle_icc_out(void);
static void ifd_handle_icc_in(void);

/*========================================================================
 * xchg interfaces
 *=======================================================================*/
static void ifd_xchg_reset(void);
static void ifd_xchg_idle(void);
static void ifd_xchg_busy(void);

/*========================================================================
 * ATR exchange
 *=======================================================================*/
static void ifd_atr_cmpl(void);
static scs_err_t ifd_atr_parse(void);

/*========================================================================
 * PPS exchange
 *=======================================================================*/
static void ifd_pps_cmpl(void);

/*========================================================================
 * scs module
 *======================================================================*/

text_word_t ifd_fi_supps[NR_IFD_FIS] = {
	  372,  372,  558,  744, 1116, 1488, 1860,    0,
	    0,  512,  768, 1024, 1536, 2048,
};

text_byte_t ifd_di_supps[NR_IFD_DIS] = {
	    0,     1,     2,     4,     8,    16,    32,     0,
	   12,    20,
};

text_word_t ifd_fmax_supps[NR_IFD_FIS] = {
	 4000,  5000,  6000,  8000, 12000, 16000, 20000,     0,
	    0,  5000,  7500, 10000, 15000, 20000,
};

__near__ scs_intr_cb ifd_notifier = NULL;
__near__ scs_cmpl_cb ifd_complete = NULL;
__near__ sid_t ifd_sid = INVALID_SID;

uint8_t ifd_nr_protos = 0;
ifd_proto_t *ifd_protos[NR_IFD_PROTOS];

#ifdef CONFIG_IFD_AUTO_ATR_BUILD
#define IFD_MAX_SAVE_LEN	16
#else
#define IFD_MAX_SAVE_LEN	33
#endif

#if NR_IFD_SLOTS > 1
__far__ uint8_t ifd_xchg_buffer[NR_IFD_SLOTS][IFD_BUF_SIZE];
#ifdef CONFIG_IFD_AUTO_PPS_PROP
__near__ uint8_t ifd_save_buf[NR_IFD_SLOTS][IFD_MAX_SAVE_LEN];
__near__ uint8_t ifd_save_len[NR_IFD_SLOTS];
#endif
union ifd_tmp ifd_tmps[NR_IFD_SLOTS];
struct ifd_xfr ifd_xfrs[NR_IFD_SLOTS];
scs_tvoid_cb ifd_ne_expire[NR_IFD_SLOTS];
ifd_sid_t ifd_slid = INVALID_IFD_SID;
__near__ struct ifd_xchg ifd_xchgs[NR_IFD_SLOTS];
__near__ struct ifd_slot ifd_slots[NR_IFD_SLOTS];
__near__ uint8_t ifd_xchg_flags[NR_IFD_SLOTS];

__near__ ifd_event_t ifd_slot_flags[NR_IFD_SLOTS];
__near__ struct ifd_user ifd_users[NR_IFD_SLOTS];

#define ifd_slot_event ifd_slot_flags[ifd_slid]

#define ifd_xchg_ctrl ifd_xchgs[ifd_slid]
#define ifd_xchg_buff ifd_xchg_buffer[ifd_slid]
#define ifd_xchg_event ifd_xchg_flags[ifd_slid]

#define ifd_atr_buf ifd_save_buf[ifd_slid]
#define ifd_atr_len ifd_save_len[ifd_slid]

#define ifd_xfr_ctrl ifd_xfrs[ifd_slid]
#define ifd_xfr_ne_expire ifd_ne_expire[ifd_slid]

#define ifd_user_ctrl ifd_users[ifd_slid]
#else
__far__ uint8_t ifd_xchg_buff[IFD_BUF_SIZE];
#ifdef CONFIG_IFD_AUTO_PPS_PROP
__near__ uint8_t ifd_atr_buf[IFD_MAX_SAVE_LEN];
__near__ uint8_t ifd_atr_len;
#endif
union ifd_tmp ifd_tmps;
struct ifd_xfr ifd_xfr_ctrl;
scs_tvoid_cb ifd_xfr_ne_expire;
ifd_event_t ifd_slot_event;
__near__ struct ifd_xchg ifd_xchg_ctrl;
__near__ struct ifd_slot ifd_slot_ctrl;
uint8_t ifd_xchg_event;

/* TODO: should combine together? */
__near__ struct ifd_user ifd_user_ctrl;
#endif

#define __ifd_xchg_write(index, byte)	(ifd_xchg_buff[index] = byte)
#define __ifd_xchg_read(index)		(ifd_xchg_buff[index])

#ifdef CONFIG_IFD_SCD
static void ifd_scd_seq_complete(scs_err_t err);
static void ifd_scd_set_state(uint8_t state);
#else
#define ifd_scd_seq_complete(err)
#define ifd_scd_set_state(state)
#endif

/*=======================================================================
 * error handlings
 *======================================================================*/
void ifd_xchg_set_error(scs_err_t errno)
{
	ifd_xchg_ctrl.error = errno;
	scs_debug(SCS_DEBUG_ERROR, errno);
}

scs_err_t ifd_xchg_get_error(void)
{
	return ifd_xchg_ctrl.error;
}

uint16_t ifd_get_F(uint8_t fi)
{
	return ifd_fi_supps[fi];
}

uint8_t ifd_get_D(uint8_t di)
{
	return ifd_di_supps[di];
}

/*=======================================================================
 * slot state machine
 *======================================================================*/
void ifd_slot_raise(ifd_event_t event)
{
	scs_debug_sl_event(event);
	ifd_slot_event |= event;
	state_wakeup(ifd_sid);
}

static ifd_event_t ifd_event_save(void)
{
	ifd_event_t events;
	events = ifd_slot_event;
	ifd_slot_event = 0;
	return events;
}

static void ifd_event_restore(ifd_event_t event)
{
	ifd_slot_event |= event;
}

static void ifd_slot_set_state(uint8_t status)
{
	scs_debug_sl_state(status);
	ifd_slot_ctrl.status = status;
	ifd_handle_slot_seq();
	if (ifd_notifier) ifd_notifier();
	ifd_scd_set_state(status);
}

uint8_t ifd_slot_get_state(void)
{
	return ifd_slot_ctrl.status;
}

static void __ifd_slot_reset(void)
{
	ifd_hw_slot_reset();
	ifd_set_freq(IFD_HW_FREQ_DEF);
}

static void __ifd_class_init(uint8_t clazz)
{
	ifd_slot_ctrl.nego_clazz = clazz;
	ifd_slot_ctrl.clazz = IFD_CLASS_NONE;
	ifd_slot_ctrl.conv = IFD_CONV_DIRECT;
}

static void ifd_slot_reset(void)
{
	/* call completion and reset error, xchg and sequence */
	ifd_xchg_set_error(SCS_ERR_SUCCESS);
	ifd_xchg_set_state(IFD_XCHG_STATE_IDLE);
	ifd_xchg_reset();
	ifd_seq_complete(SCS_ERR_NOTPRESENT);
	(void)ifd_event_save();

	__ifd_slot_reset();
	__ifd_class_init(IFD_CLASS_NONE);
	ifd_slot_set_state(IFD_SLOT_STATE_NOTPRESENT);
}

void ifd_detect_icc_out(void)
{
	if (ifd_slot_get_state() != IFD_SLOT_STATE_NOTPRESENT)
		ifd_slot_raise(IFD_SLOT_EVENT_ICC_OUT);
}

void ifd_detect_icc_in(void)
{
	if (ifd_slot_get_state() == IFD_SLOT_STATE_NOTPRESENT)
		ifd_slot_raise(IFD_SLOT_EVENT_ICC_IN);
}

static void ifd_handle_icc_out(void)
{
	ifd_hw_deactivate();
	ifd_slot_reset();
}

static void ifd_handle_icc_in(void)
{
	(void)scs_auto_seq(false);
}

void ifd_conf_param(void)
{
	ifd_proto_t *cproto;

	ifd_config_fd();
	ifd_config_t();
	cproto = ifd_get_protocol();
	BUG_ON(!cproto->conf_param);
	cproto->conf_param();
}

static void ifd_handle_slot_state(void)
{
	ifd_event_t flags = ifd_event_save();
	uint8_t state = ifd_slot_get_state();

	if (flags & IFD_SLOT_EVENT_XFR_SYNC) {
		ifd_xchg_sync();
		unraise_bits(flags, IFD_SLOT_EVENT_XFR_SYNC);
	}
	if (flags & IFD_SLOT_EVENT_WARM_RST) {
		if (state >= IFD_SLOT_STATE_ATR_READY)
			ifd_slot_set_state(IFD_SLOT_STATE_ACTIVATED);
		unraise_bits(flags, IFD_SLOT_EVENT_WARM_RST);
	}
	/* critical error, require cold reset */
	if (flags & IFD_SLOT_EVENT_HW_ERR) {
		BUG_ON(ifd_xchg_get_error() == SCS_ERR_SUCCESS);
		if (state >= IFD_SLOT_STATE_SELECTED) {
			ifd_slot_set_state(IFD_SLOT_STATE_HWERROR);
			ifd_deactivate();
		}
		unraise_bits(flags, IFD_SLOT_EVENT_HW_ERR);
	}
	if (flags & IFD_SLOT_EVENT_PWR_OFF) {
		if (state >= IFD_SLOT_STATE_PRESENT) {
			__ifd_slot_reset();
			ifd_slot_set_state(IFD_SLOT_STATE_PRESENT);
		}
		unraise_bits(flags, IFD_SLOT_EVENT_PWR_OFF);
	}
	if (flags & IFD_SLOT_EVENT_ICC_OUT) {
		ifd_handle_icc_out();
		ifd_slot_set_state(IFD_SLOT_STATE_NOTPRESENT);
		unraise_bits(flags, IFD_SLOT_EVENT_ICC_OUT);
	}

	if (state == IFD_SLOT_STATE_NOTPRESENT) {
		if (flags & IFD_SLOT_EVENT_ICC_IN) {
			ifd_handle_icc_in();
			ifd_slot_set_state(IFD_SLOT_STATE_PRESENT);
			unraise_bits(flags, IFD_SLOT_EVENT_ICC_IN);
		}
	} else if (state == IFD_SLOT_STATE_PRESENT) {
		if (flags & IFD_SLOT_EVENT_CLS_CHG) {
			ifd_slot_set_state(IFD_SLOT_STATE_SELECTED);
			unraise_bits(flags, IFD_SLOT_EVENT_CLS_CHG);
		}
	} else if (state == IFD_SLOT_STATE_SELECTED) {
		if (flags & IFD_SLOT_EVENT_PWR_OK) {
			ifd_slot_set_state(IFD_SLOT_STATE_ACTIVATED);
			unraise_bits(flags, IFD_SLOT_EVENT_PWR_OK);
		}
	} else if (state == IFD_SLOT_STATE_ACTIVATED) {
		if (flags & IFD_SLOT_EVENT_ATR_OK) {
			/* our auto sequence can handle ATR_OK */
			ifd_slot_set_state(IFD_SLOT_STATE_ATR_READY);
			unraise_bits(flags, IFD_SLOT_EVENT_ATR_OK);
		}
	} else if (state == IFD_SLOT_STATE_ATR_READY) {
		if (flags & IFD_SLOT_EVENT_ATR_OK) {
			/* for sequence completion */
			ifd_slot_set_state(IFD_SLOT_STATE_ATR_READY);
			unraise_bits(flags, IFD_SLOT_EVENT_ATR_OK);
		}
		if (flags & IFD_SLOT_EVENT_PPS_OK) {
			ifd_conf_param();
			ifd_slot_set_state(IFD_SLOT_STATE_PPS_READY);
			unraise_bits(flags, IFD_SLOT_EVENT_PPS_OK);
		}
	} else if (state == IFD_SLOT_STATE_PPS_READY) {
		if (flags & IFD_SLOT_EVENT_PPS_OK) {
			/* for sequence completion */
			ifd_slot_set_state(IFD_SLOT_STATE_PPS_READY);
			unraise_bits(flags, IFD_SLOT_EVENT_PPS_OK);
		}
	}
	ifd_event_restore(flags);
}

/*========================================================================
 * automatisms
 *=======================================================================*/
static void ifd_deactivate(void)
{
	ifd_xchg_set_error(SCS_ERR_SUCCESS);
	ifd_hw_deactivate();
	ifd_slot_raise(IFD_SLOT_EVENT_PWR_OFF);
}

static scs_err_t ifd_cls_select(void)
{
	ifd_xchg_set_error(SCS_ERR_SUCCESS);
	switch (ifd_slot_ctrl.nego_clazz) {
	case IFD_CLASS_A:
	case IFD_CLASS_B:
	case IFD_CLASS_C:
		ifd_slot_ctrl.clazz = ifd_slot_ctrl.nego_clazz; 
		ifd_slot_raise(IFD_SLOT_EVENT_CLS_CHG);
		return SCS_ERR_SUCCESS;
	default:
		return IFD_ERR_BAD_CLASS;
	}
}

/* XXX: CLK contact based delay
 * The cold reset should maintain RST at state L for at least 400 clock
 * cycles.
 * So there are needs to implement such function.
 */
void ifd_delay_clock(uint8_t cycle)
{
	uint32_t loops;

	loops = mul16u(loops_per_ms, cycle);
	__delay(1+(uint16_t)div16u(loops, ifd_slot_ctrl.freq));
}

boolean ifd_detect_icc_power(void)
{
	ifd_delay_clock(200);
	if (!ifd_get_contact(ICC_CONTACT_IO)) {
		return false;
	}
	ifd_delay_clock(200);

#if 0
	ifd_hw_enable_clk();
#endif
	return true;
}

static void ifd_activate(void)
{
	ifd_xchg_set_error(SCS_ERR_SUCCESS);
	ifd_config_cls();
	ifd_config_def();
	if (ifd_hw_activate() && ifd_detect_icc_power()) {
		ifd_slot_raise(IFD_SLOT_EVENT_PWR_OK);
	} else {
		ifd_xchg_set_error(SCS_ERR_BAD_POWER);
		ifd_slot_raise(IFD_SLOT_EVENT_HW_ERR);
	}
}

#if 0
static void ifd_warm_reset(void)
{
	ifd_xchg_set_error(SCS_ERR_SUCCESS);
	ifd_config_def();
	ifd_hw_warm_reset();
	if (ifd_detect_icc_power()) {
		ifd_slot_raise(IFD_SLOT_EVENT_WARM_RST);
	} else {
		ifd_xchg_set_error(SCS_ERR_BAD_POWER);
		ifd_slot_raise(IFD_SLOT_EVENT_HW_ERR);
	}
}
#endif

static uint8_t ifd_atr_bytes(uint8_t y)
{
	return hweight8(TD_Y(y));
}

static void ifd_atr_xfr_block(scs_size_t ne)
{
	ifd_xfr_block(0, 0, ifd_atr_xchg.atr_rx, ne, 0);
	ifd_atr_xchg.atr_rx += ne;
}

static void __ifd_atr_expire(void)
{
	if (ifd_atr_xchg.atr_y) {
		ifd_atr_xchg.atr_state = IFD_ATR_TD;
		ifd_atr_xfr_block(ifd_atr_xchg.atr_y);
	} else if (ifd_atr_xchg.atr_k) {
		ifd_atr_xchg.atr_state = IFD_ATR_TK;
		ifd_atr_xfr_block(ifd_atr_xchg.atr_k);
		ifd_atr_xchg.atr_k = 0;
	} else if (ifd_atr_xchg.atr_tck) {
		ifd_atr_xchg.atr_state = IFD_ATR_TCK;
		ifd_atr_xfr_block(1);
		ifd_atr_xchg.atr_tck = false;
	} else {
		ifd_xchg_end(ifd_atr_xchg.atr_rx);
		ifd_xchg_stop(IFD_XCHG_EVENT_XFR_CMPL);
	}
}

static void ifd_atr_expire(void)
{
	uint8_t td;

	if (ifd_atr_xchg.atr_state == IFD_ATR_TS) {
		if (ifd_xfr_read(0) != ifd_get_convention()) {
			ifd_xchg_stop(IFD_XCHG_EVENT_CONV_ERR);
			return;
		}
		td = ifd_xfr_read(1);
		ifd_atr_xchg.atr_k = T0_K(td);
		ifd_atr_xchg.atr_tck = false;
		ifd_atr_xchg.atr_td_last = td;
		ifd_atr_xchg.atr_y = ifd_atr_bytes(td);
	} else if (ifd_atr_xchg.atr_state == IFD_ATR_TD) {
		td = ifd_atr_xchg.atr_td_last & TD_EXISTS ?
		     ifd_xfr_read(ifd_atr_xchg.atr_y-1) : 0;
		ifd_atr_xchg.atr_tck |= (TD_T(td) != IFD_PROTO_T0);
		ifd_atr_xchg.atr_td_last = td;
		ifd_atr_xchg.atr_y = ifd_atr_bytes(td);
	}
	__ifd_atr_expire();
}

static void ifd_reset(void)
{
	ifd_xchg_reset();
	ifd_xfr_ne_expire = ifd_atr_expire;
	ifd_atr_xchg.atr_state = IFD_ATR_TS;
	ifd_atr_xchg.atr_rx = 0;

	ifd_xchg_busy();
	/* receive TS */
	ifd_xchg_begin(0, 0, 0, 2, 0);
	ifd_atr_xchg.atr_rx += 2;
	ifd_xchg_run();
}

static uint8_t ifd_pps_bytes(uint8_t pps0)
{
	uint8_t size = 0;
	if (pps0 & IFD_PPS_EXISTS(0)) size++;
	if (pps0 & IFD_PPS_EXISTS(1)) size++;
	if (pps0 & IFD_PPS_EXISTS(2)) size++;
	return size;
}

static void ifd_pps_expire(void)
{
	if (!ifd_pps_xchg.pps_cmpl) {
		uint8_t size, pps0;

		pps0 = ifd_xfr_read(1);
		size = ifd_pps_bytes(pps0)+1;
		ifd_pps_xchg.ppsr_size = size+2;
		ifd_xfr_block(0, 0, 2, size, ifd_pps_xchg.pps_size);
		ifd_pps_xchg.pps_cmpl = true;
	} else {
		ifd_xchg_end(ifd_pps_xchg.ppsr_size);
		ifd_xchg_stop(IFD_XCHG_EVENT_XFR_CMPL);
	}
}

static void ifd_info_xchg_fail(scs_err_t err)
{
	ifd_xchg_set_error(err);
	ifd_slot_raise(IFD_SLOT_EVENT_HW_ERR);
}

static void ifd_nego_proto(void)
{
	ifd_proto_t *cproto;

	cproto = __ifd_get_protocol(ifd_slot_ctrl.nego_proto);
	BUG_ON(!cproto || !cproto->nego_pps);
	cproto->nego_pps();
}

static void ifd_spec_mode(void)
{
	uint8_t pid;

	ifd_xchg_set_error(SCS_ERR_SUCCESS);
	pid = ifd_proto_pid(ifd_slot_ctrl.spec_proto);
	if (pid >= ifd_nr_protos) {
		ifd_info_xchg_fail(IFD_ERR_BAD_PROTO);
		return;
	}
	ifd_slot_ctrl.nego_proto = ifd_slot_ctrl.spec_proto;
	ifd_slot_ctrl.nego_fd = ifd_slot_ctrl.spec_fd;
	ifd_nego_proto();
	ifd_slot_raise(IFD_SLOT_EVENT_PPS_OK);
}

static void ifd_nego_1st(void)
{
	ifd_xchg_set_error(SCS_ERR_SUCCESS);
	ifd_slot_ctrl.nego_proto = ifd_slot_ctrl.first_proto;
	ifd_nego_proto();
	ifd_slot_raise(IFD_SLOT_EVENT_PPS_OK);
}

#ifdef CONFIG_IFD_AUTO_INFO_XCHG
static scs_err_t ifd_ppsr_parse(void)
{
	uint8_t pps0, byte, nppsr, i;
	uint8_t pck = IFD_PPS_PPSS;
	uint8_t fd = ifd_slot_ctrl.fd;

	/* ISO/IEC 7816-3 9.3 Successful PPS exchange
	 */
	if (ifd_xchg_read(0) != IFD_PPS_PPSS)
		return IFD_ERR_BAD_PPSS;
	pps0 = ifd_xchg_read(1);
	pck ^= pps0;
	if (IFD_PPS_PROTO(pps0) != IFD_PPS_PROTO(ifd_pps_xchg.pps_req[0]))
		return IFD_ERR_BAD_PPSR;
	for (nppsr = 2, i = 0; i < 3; i++) {
		if (pps0 & IFD_PPS_EXISTS(nppsr-2)) {
			byte = ifd_xchg_read(nppsr);
			if (byte != ifd_pps_xchg.pps_req[nppsr-1])
				return IFD_ERR_BAD_PPSR;
			pck ^= byte;
			if (nppsr == 2) fd = byte;
			nppsr++;
		}
	}
	pck ^= ifd_xchg_read(nppsr);
	if (pck)
		return IFD_ERR_BAD_PCK;
	ifd_slot_ctrl.fd = fd;
	return SCS_ERR_SUCCESS;
}

static void ifd_pps_cmpl(void)
{
	scs_err_t err;

	err = ifd_xchg_get_error();
	if (err != SCS_ERR_SUCCESS) return;

	err = ifd_ppsr_parse();
	if (err == SCS_ERR_SUCCESS) {
		ifd_slot_raise(IFD_SLOT_EVENT_PPS_OK);
	} else {
		ifd_xchg_set_error(err);
	}
}

static void ifd_info_xchg(void)
{
	if (ifd_slot_ctrl.info_xchg == IFD_INFO_XCHG_SPECIFIC)
		goto xchg;

	if (hweight16(ifd_slot_ctrl.ind_protos) > 1 ||
	    ifd_slot_ctrl.ind_fd != IFD_FD_IMPLICIT)
		ifd_slot_ctrl.info_xchg = IFD_INFO_XCHG_NEGO_PPS;
	else
		ifd_slot_ctrl.info_xchg = IFD_INFO_XCHG_NEGO_1ST;
xchg:
	switch (ifd_slot_ctrl.info_xchg) {
	case IFD_INFO_XCHG_SPECIFIC:
		ifd_spec_mode();
		break;
	case IFD_INFO_XCHG_NEGO_1ST:
		ifd_nego_1st();
		break;
	case IFD_INFO_XCHG_NEGO_PPS:
		ifd_nego_pps();
		break;
	default:
		BUG();
		break;
	}
}

static void ifd_pps_save(void)
{
	uint8_t i, pps0;

	/* keep PPS_request */
	ifd_pps_xchg.pps_req[0] = pps0 = ifd_xchg_read(1);
	i = 2;
	if (pps0 & IFD_PPS_EXISTS(0))
		ifd_pps_xchg.pps_req[1] = ifd_xchg_read(i++);
	if (pps0 & IFD_PPS_EXISTS(1))
		ifd_pps_xchg.pps_req[2] = ifd_xchg_read(i++);
	if (pps0 & IFD_PPS_EXISTS(2))
		ifd_pps_xchg.pps_req[3] = ifd_xchg_read(i++);
}

static void __ifd_pps_build(void)
{
	uint8_t pps0 = ifd_slot_ctrl.nego_proto, pps1 = ifd_slot_ctrl.fd;
	uint8_t pck = IFD_PPS_PPSS, i;

	if (ifd_slot_ctrl.nego_fd != ifd_slot_ctrl.fd) {
		pps1 = ifd_slot_ctrl.nego_fd;
		pck ^= pps1;
		pps0 |= IFD_PPS_EXISTS(0);
	}
	pck ^= pps0;

	/* build PPS xchg */
	ifd_xchg_write(0, IFD_PPS_PPSS);
	ifd_xchg_write(1, pps0);
	i = 2;
	if (pps0 & IFD_PPS_EXISTS(0)) {
		ifd_xchg_write(i, pps1);
		i++;
	}
	ifd_xchg_write(i, pck);
	ifd_pps_save();
}

#ifdef CONFIG_IFD_AUTO_PPS_PROP
#define IFD_PPS_ITER_T		TD_T(ifd_atr_xchg.atr_td_last)

static uint8_t ifd_pps_iter_t(uint8_t iter)
{
	uint8_t nbytes;

	if (iter == 0)
		ifd_atr_xchg.atr_td_last = ifd_xfr_read(iter+1);
	nbytes = ifd_atr_bytes(ifd_atr_xchg.atr_td_last);
	if (nbytes) {
		iter += nbytes;
		ifd_atr_xchg.atr_td_last = ifd_xfr_read(iter+1);
	}
	return iter;
}

static void ifd_pps_build(void)
{
	uint32_t d;
	uint8_t curr, next;

	/* negotiate FD */
	ifd_slot_ctrl.nego_fd = ifd_slot_ctrl.ind_fd;
	if (ifd_fmax_supps[HIHALF(ifd_slot_ctrl.ind_fd)] >
	    IFD_HW_FREQ_MAX) {
		ifd_slot_ctrl.nego_fd = IFD_FD_IMPLICIT;
	} else {
		d = mul16u(ifd_fmax_supps[HIHALF(ifd_slot_ctrl.ind_fd)],
			   ifd_di_supps[LOHALF(ifd_slot_ctrl.ind_fd)]);
		d = div32u(d, ifd_fi_supps[HIHALF(ifd_slot_ctrl.ind_fd)]);
		if (d > IFD_HW_DATA_MAX) {
			ifd_slot_ctrl.nego_fd = IFD_FD_IMPLICIT;
		}
	}

	/* negotiate T */
	ifd_slot_ctrl.nego_proto = IFD_PROTO_DEF;
	curr = 0;
	do {
		next = ifd_pps_iter_t(curr);
		if (curr == next)
			break;
		if (ifd_proto_pid(IFD_PPS_ITER_T) < ifd_nr_protos) {
			ifd_slot_ctrl.nego_proto = IFD_PPS_ITER_T;
			break;
		}
		curr = next;
	} while (1);

	/* negotiate T specific parameters */
	ifd_nego_proto();
	__ifd_pps_build();
}
#else
#define ifd_pps_build()		__ifd_pps_build()
#endif
#else
static void ifd_pps_cmpl(void)
{
	//ifd_xchg_set_error(SCS_ERR_SUCCESS);
}

static void ifd_info_xchg(void)
{
	/* INFO_XCHG has been done through ifd_xchg_block */
	ifd_slot_raise(IFD_SLOT_EVENT_PPS_OK);
}

static void ifd_pps_build(void)
{
	BUG_ON(ifd_xchg_read(0) != IFD_PPS_PPSS);
	ifd_xchg_reset();
}
#endif

static void ifd_nego_pps(void)
{
	uint8_t pps0;

	ifd_pps_build();

	pps0 = ifd_xchg_read(1);
	ifd_pps_xchg.pps_cmpl = false;
	ifd_pps_xchg.pps_size = ifd_pps_bytes(pps0)+3;
	ifd_xfr_ne_expire = ifd_pps_expire;

	ifd_xchg_busy();
	ifd_xchg_begin(0, ifd_pps_xchg.pps_size,
		       0, 2, ifd_pps_xchg.pps_size);
	ifd_xchg_run();
}

/*=======================================================================
 * xchg internals
 *======================================================================*/
static void ifd_xfr_reset_nx(void)
{
	ifd_xfr_ctrl.tx = 0;
	ifd_xfr_ctrl.rx = 0;
}

void __ifd_xfr_reset(void)
{
	ifd_xfr_ctrl.nc = 0;
	ifd_xfr_ctrl.ne = 0;
	ifd_xfr_reset_nx();
}

/* TODO: make this less stack consumption */
void __ifd_xfr_block(scs_off_t tx, scs_size_t nc,
		     scs_off_t rx, scs_size_t ne)
{
	ifd_xfr_ctrl.rx_start = rx;
	ifd_xfr_ctrl.tx_start = tx;
	ifd_xfr_ctrl.nc = nc;
	ifd_xfr_ctrl.ne = ne;
	ifd_xfr_reset_nx();
	ifd_delay_gt(ifd_slot_ctrl.bgt, true);
}

void ifd_xchg_begin(scs_off_t tx, scs_size_t nc,
		    scs_off_t rx, scs_size_t ne,
		    scs_size_t nr_valid)
{
	ifd_slot_ctrl.tx = tx;
	ifd_slot_ctrl.rx = rx;
	ifd_slot_ctrl.nc = nc;
	ifd_slot_ctrl.ne = ne;
#ifdef CONFIG_IFD_PANIC_BUF
	ifd_slot_ctrl.nr = nr_valid;
#endif
}

void ifd_xchg_start(void)
{
	if (ifd_slot_ctrl.nc == 0) {
		ifd_hw_reset();
	}

	ifd_hw_start_wtc();
	ifd_xfr_block(ifd_slot_ctrl.tx,
		      ifd_slot_ctrl.nc,
		      ifd_slot_ctrl.rx,
		      ifd_slot_ctrl.ne,
		      ifd_slot_ctrl.nr_valid);
}

#ifdef CONFIG_IFD_PANIC_BUF
void ifd_xfr_block(scs_off_t tx, scs_size_t nc,
		   scs_off_t rx, scs_size_t ne,
		   scs_size_t nr_valid)
{
	BUG_ON(nr_valid > tx+nc && rx+ne > tx+nc && nc);
	__ifd_xfr_block(tx, nc, rx, ne);
}
#endif

#define IFD_TX_DATA	\
	(ifd_xchg_buff[ifd_xfr_ctrl.tx_start+ifd_xfr_ctrl.tx])
#define IFD_RX_DATA	\
	(ifd_xchg_buff[ifd_xfr_ctrl.rx_start+ifd_xfr_ctrl.rx])

static void ifd_xchg_sync(void)
{
	ifd_xchg_start();
	while (!ifd_xchg_raised_any()) {
		while (!ifd_xchg_raised_any() &&
		       ifd_xfr_ctrl.tx < ifd_xfr_ctrl.nc) {
			ifd_hw_write_byte(IFD_TX_DATA);
			ifd_xfr_ctrl.tx++;
			ifd_delay_gt(ifd_slot_ctrl.cgt, false);
		}
		ifd_delay_wt(ifd_slot_ctrl.bwt, true);
		while (!ifd_xchg_raised_any() &&
		       ifd_xfr_ctrl.rx < ifd_xfr_ctrl.ne) {
			IFD_RX_DATA = ifd_hw_read_byte();
			ifd_xfr_ctrl.rx++;
			if (ifd_xfr_ctrl.rx == ifd_xfr_ctrl.ne) {
				ifd_xfr_ne_expire();
				break;
			}
			ifd_delay_wt(ifd_slot_ctrl.cwt, false);
		}
	}
}

void ifd_xchg_run(void)
{
	/*scs_debug_xg_event(IFD_XCHG_EVENT_XFR_MARK);*/
	ifd_slot_raise(IFD_SLOT_EVENT_XFR_SYNC);
}

void ifd_xchg_stop(uint8_t event)
{
	if (!ifd_xchg_raised_any()) {
		/*scs_debug_xg_event(IFD_XCHG_EVENT_XFR_MARK);*/
		ifd_xchg_raise(event);
	}
	ifd_hw_stop_wtc();
	__ifd_xfr_reset();
}

uint8_t ifd_xfr_read(scs_off_t idx)
{
	return ifd_xchg_buff[ifd_xfr_ctrl.rx_start + idx];
}

uint8_t ifd_xfr_read_last(void)
{
	return ifd_xfr_read(ifd_xfr_ctrl.ne-1);
}

static void ifd_null_expire(void)
{
	ifd_xchg_stop(IFD_XCHG_EVENT_XFR_CMPL);
}

/*=======================================================================
 * xchg state machine
 *======================================================================*/
void ifd_xchg_raise(uint8_t event)
{
	scs_debug_xg_event(event);
	ifd_xchg_event |= event;
	state_wakeup(ifd_sid);
}

boolean ifd_xchg_raised_any(void)
{
	return ifd_xchg_event;
}

uint8_t ifd_xchg_save(void)
{
	uint8_t events;
	events = ifd_xchg_event;
	ifd_xchg_event = 0;
	return events;
}

static void ifd_xfr_reset(void)
{
	/* reset ifd_xfr */
	__ifd_xfr_reset();
	ifd_xchg_ctrl.nr = 0;
}

static void ifd_xchg_reset(void)
{
	ifd_xfr_ne_expire = ifd_null_expire;
	(void)ifd_xchg_save();
	ifd_xfr_reset();
}

uint8_t ifd_xchg_get_state(void)
{
	return ifd_xchg_ctrl.state;
}

void ifd_xchg_set_state(uint8_t state)
{
	scs_debug_xg_state(state);
	ifd_xchg_ctrl.state = state;
}

scs_err_t ifd_slot_exist(void)
{
	uint8_t seq;
	seq = ifd_seq_get();
	if (seq == IFD_SEQ_AUTO_SEQ)
		return SCS_ERR_BUSY_AUTO;
	if (seq != IFD_SEQ_IDLE)
		return SCS_ERR_BUSY_SLOT;
	if (ifd_slot_get_state() == IFD_SLOT_STATE_NOTPRESENT)
		return SCS_ERR_NOTPRESENT;
	return SCS_ERR_SUCCESS;
}

static void ifd_xchg_busy(void)
{
	/* XXX: Deactivate on Exchange Errors
	 *
	 * We choose to deactivate on exchange error rather than warm
	 * reset, this function could be safe as no ifd_reset/ifd_nego_pps
	 * will be called.
	 */
	ifd_xchg_set_error(SCS_ERR_SUCCESS);
	/* reset parameters for retransmission */
	ifd_xfr_reset();
	(void)ifd_xchg_save();
	ifd_xchg_set_state(IFD_XCHG_STATE_BUSY);
}

static void ifd_proto_cmpl(void)
{
	ifd_proto_t *cproto;
	cproto = ifd_get_protocol();
	BUG_ON(!cproto->xchg_cmpl);
	cproto->xchg_cmpl();
}

static void scs_proto_aval(void)
{
	ifd_proto_t *cproto;
	cproto = ifd_get_protocol();
	BUG_ON(!cproto->xchg_aval);
	cproto->xchg_aval();
}

static void ifd_xchg_idle(void)
{
	if (ifd_xchg_get_state() != IFD_XCHG_STATE_ERROR) {
		uint8_t state = ifd_slot_get_state();
		scs_err_t err;

		if (state == IFD_SLOT_STATE_ACTIVATED) {
			ifd_atr_cmpl();
		} else if (state == IFD_SLOT_STATE_ATR_READY) {
			ifd_pps_cmpl();
		} else if (state == IFD_SLOT_STATE_PPS_READY) {
			ifd_proto_cmpl();
		}

		err = ifd_xchg_get_error();
		if (err == SCS_ERR_PROGRESS) {
			ifd_xchg_set_error(SCS_ERR_SUCCESS);
			ifd_xchg_busy();
			scs_proto_aval();
			return;
		}
		if (err != SCS_ERR_SUCCESS) {
			if (ifd_seq_get() == IFD_SEQ_INTERCHANGE) {
				if (err != SCS_ERR_TIMEOUT &&
				    err != SCS_ERR_PARITY_ERR &&
				    err != SCS_ERR_OVERRUN) {
					/* critical errors handled by slot
					 * state machine
					 */
					/* convention or power failure */
					ifd_slot_raise(IFD_SLOT_EVENT_HW_ERR);
					ifd_xchg_set_state(IFD_XCHG_STATE_ERROR);
					return;
				}
			} else {
				ifd_slot_raise(IFD_SLOT_EVENT_HW_ERR);
			}
		}
	}
	ifd_handle_xchg_seq();
}

static void ifd_handle_xchg_state(void)
{
	uint8_t flags = ifd_xchg_save();

	if (ifd_xchg_get_state() == IFD_XCHG_STATE_BUSY) {
		/* failure: slot -> deactivate */
		if (flags & IFD_XCHG_EVENT_POWER_ERR) {
			ifd_xchg_set_error(SCS_ERR_HW_ERROR);
			ifd_xchg_idle();
			return;
		}
		if (flags & IFD_XCHG_EVENT_CONV_ERR) {
			ifd_xchg_set_error(IFD_ERR_BAD_TS);
			ifd_xchg_idle();
			return;
		}
		/* failure: xchg -> warm reset */
		if (flags & IFD_XCHG_EVENT_PARITY_ERR) {
			ifd_xchg_set_error(SCS_ERR_PARITY_ERR);
			ifd_xchg_idle();
			return;
		}
		if (flags & IFD_XCHG_EVENT_XFR_OVER) {
			ifd_xchg_set_error(SCS_ERR_OVERRUN);
			ifd_xchg_idle();
			return;
		}
		if (flags & IFD_XCHG_EVENT_WT_EXPIRE) {
			ifd_xchg_set_error(SCS_ERR_TIMEOUT);
			ifd_xchg_idle();
			return;
		}

		if (flags & IFD_XCHG_EVENT_ICC_OUT) {
			ifd_xchg_set_error(SCS_ERR_NOTPRESENT);
			ifd_xchg_idle();
			return;
		}
		/* success */
		if (flags & IFD_XCHG_EVENT_XFR_CMPL) {
			ifd_xchg_idle();
			return;
		}
	}
}

ifd_proto_t *__ifd_get_protocol(uint8_t proto)
{
	uint8_t pid;
	pid = ifd_proto_pid(proto);
	if (pid < ifd_nr_protos)
		return ifd_protos[pid];
	return NULL;
}

ifd_proto_t *ifd_get_protocol(void)
{
	return __ifd_get_protocol(ifd_slot_ctrl.proto);
}

/*=======================================================================
 * xchg externals
 *======================================================================*/
scs_err_t ifd_xchg_block(scs_size_t nc, scs_size_t ne)
{
	scs_err_t err;
	uint8_t state = ifd_slot_get_state();
	ifd_proto_t *cproto;

	err = ifd_slot_active();
	if (err != SCS_ERR_SUCCESS) return err;

	if (state == IFD_SLOT_STATE_ATR_READY) {
#ifdef CONFIG_IFD_AUTO_INFO_XCHG
		return SCS_ERR_BUSY_AUTO;
#else
		if (ifd_xchg_read(0) == IFD_PPS_PPSS) {
			ifd_nego_pps();
			goto seq_start;
		} else {
			ifd_slot_set_state(IFD_SLOT_STATE_PPS_READY);
		}
#endif
	}

	BUG_ON(state != IFD_SLOT_STATE_PPS_READY);

	ifd_xchg_reset();

	cproto = ifd_get_protocol();
	BUG_ON(!cproto->ne_expire);
	ifd_xfr_ne_expire = cproto->ne_expire;

	BUG_ON(!cproto->xchg_init);
	err = cproto->xchg_init(nc, ne);
	if (err != SCS_ERR_PROGRESS) return err;

	ifd_xchg_busy();
	scs_proto_aval();

#ifndef CONFIG_IFD_AUTO_INFO_XCHG
seq_start:
#endif
	return ifd_seq_start(IFD_SEQ_INTERCHANGE);
}

uint8_t ifd_xchg_read(scs_off_t index)
{
	return __ifd_xchg_read(index);
}

void ifd_xchg_move(scs_off_t to, scs_off_t from, scs_size_t cnt)
{
	scs_off_t i;
	if (to < from) {
		for (i = 0; i < cnt; i++) {
			ifd_xchg_buff[to+i] = ifd_xchg_buff[from+i];
		}
	} else {
		for (i = cnt; i > 0; i--) {
			ifd_xchg_buff[to+i-1] = ifd_xchg_buff[from+i-1];
		}
	}
}

void ifd_xchg_write(scs_off_t index, uint8_t byte)
{
	BUG_ON(index >= IFD_BUF_SIZE);
	__ifd_xchg_write(index, byte);
}

uint8_t ifd_read_byte(scs_off_t index)
{
	BUG_ON(index >= IFD_BUF_SIZE);
	return ifd_xchg_read(index);
}

scs_err_t ifd_write_byte_mask(scs_off_t index,
			      uint8_t byte,
			      uint8_t mask)
{
	scs_err_t err;

	err = ifd_slot_active();
	if (err != SCS_ERR_SUCCESS) return err;

	if (index >= IFD_BUF_SIZE) return SCS_ERR_OVERRUN;
	ifd_xchg_buff[index] &= ~mask;
	ifd_xchg_buff[index] |= byte & mask;
	return SCS_ERR_SUCCESS;
}

scs_err_t ifd_write_byte(scs_off_t index, uint8_t byte)
{
	scs_err_t err;

	err = ifd_slot_active();
	if (err != SCS_ERR_SUCCESS) return err;
	if (index >= IFD_BUF_SIZE)  return SCS_ERR_OVERRUN;
	ifd_xchg_write(index, byte);
	return SCS_ERR_SUCCESS;
}

scs_size_t ifd_xchg_avail(void)
{
	return ifd_xchg_ctrl.nr;
}

void ifd_xchg_end(scs_size_t nr)
{
	ifd_xchg_ctrl.nr = nr;
}

void ifd_xchg_abort(ifd_sid_t sid)
{
#if 0
	ifd_sid_t ssid = ifd_sid_save(sid);
	/* XXX: Sync Abort from USB
	 *
	 * For ifd_xchg_sync, ABORT could never be reached. When
	 * ifd_xchg_sync is running, no ABORT messages could be received
	 * by USB device controller as there isn't chances for USB state
	 * machine to handle incoming messages.
	 * Same reason as the ccid_kbd_abort.
	 *
	 * ifd_xchg_stop(IFD_XCHG_EVENT_XFR_ABORT);
	 */
	ifd_sid_restore(ssid);
#endif
}

/*=======================================================================
 * auto sequence
 *======================================================================*/
#ifdef CONFIG_IFD_AUTO_CLASS_SELECT
#if defined(CONFIG_IFD_CLASS_SELECT_DESCEND)	/* A->B->C */
static void ifd_cls_select_next(void)
{
	if (ifd_slot_ctrl.conv == IFD_CONV_DIRECT) {
		ifd_slot_ctrl.conv = IFD_CONV_INVERSE;
	} else {
		ifd_slot_ctrl.nego_clazz += 1;
		ifd_slot_ctrl.conv = IFD_CONV_DIRECT;
	}
}
#elif defined(CONFIG_IFD_CLASS_SELECT_ASCEND)	/* C->B->A */
static void ifd_cls_select_next(void)
{
	if (ifd_slot_ctrl.conv == IFD_CONV_DIRECT) {
		ifd_slot_ctrl.conv = IFD_CONV_INVERSE;
	} else {
		ifd_slot_ctrl.nego_clazz -= 1;
		ifd_slot_ctrl.conv = IFD_CONV_DIRECT;
	}
}
#endif

static boolean ifd_cls_select_auto(void)
{
	if (ifd_slot_ctrl.nego_clazz == IFD_CLASS_AUTO) {
		__ifd_class_init(IFD_CLASS_INIT);
	} else {
		ifd_cls_select_next();
	}
	return (SCS_ERR_SUCCESS == ifd_cls_select());
}
#else
static boolean ifd_cls_select_auto(void)
{
	return false;
}
#endif

#ifdef CONFIG_IFD_AUTO_PPS_PROP
#define ifd_atr_save_init()	(ifd_atr_len = 0)

static uint8_t ifd_pid_proto(uint8_t pid)
{
	if (pid >= ifd_nr_protos)
		return ifd_nr_protos;
	return ifd_protos[pid]->id;
}

boolean ifd_only_support_t0(void)
{
	if (ifd_nr_protos != 1)
		return false;
	if (ifd_pid_proto(0) == IFD_PROTO_T0)
		return true;
	return false;
}

static void ifd_atr_write(uint8_t byte)
{
	ifd_xfr_block(0, 0, ifd_atr_parser.len, 1, 0);
	ifd_atr_parser.len += 1;
	IFD_RX_DATA = byte;
	ifd_xfr_ctrl.rx++;
}

static void ifd_atr_xfr_end(void)
{
	ifd_xchg_end(ifd_atr_parser.len);
	ifd_xchg_raise(IFD_XCHG_EVENT_XFR_CMPL);
	__ifd_xfr_reset();
}

static void ifd_pps_prop_build(void)
{
	uint8_t i;

	for (i = 0; i < ifd_atr_len; i++)
		ifd_atr_write(ifd_atr_buf[i]);
}

#ifdef CONFIG_IFD_AUTO_ATR_BUILD
#define ifd_atr_save_hist(byte)	(ifd_atr_buf[ifd_atr_len++] = byte)
#define ifd_atr_save(byte)

/*
 * e.g. TA1 (1 is the round number):
 * HIHALF(tck_exists) is used for keep write-time's round number.
 * LOHALF(tck_exists) is used for keep current round number.
 */
#define IFD_BATR_RND	ifd_atr_parser.tck_exists 

boolean ifd_atr_same_round(void)
{
	uint8_t hi = HIHALF(IFD_BATR_RND);
	uint8_t lo = LOHALF(IFD_BATR_RND);

	/* have not been set */
	if (hi == 0) {
		IFD_BATR_RND = (IFD_BATR_RND << 4) | IFD_BATR_RND;
		return true;
	}
	return hi == lo;
}

void ifd_atr_reset_tmp(void)
{
	ifd_atr_parser.td_last = 0;
	ifd_atr_parser.ta = 0;
	ifd_atr_parser.tb = 0;
	ifd_atr_parser.tc = 0;
	ifd_atr_parser.td = 0;

	IFD_BATR_RND = LOHALF(IFD_BATR_RND);
}

/* This function will write one byte (td_last) at least.
 * So you must know td is present for sure. */
void __ifd_atr_write_all(void)
{
	ifd_atr_write(ifd_atr_parser.td_last | ifd_atr_parser.td);
	if (ifd_atr_parser.td_last & TA_EXISTS)
		ifd_atr_write(ifd_atr_parser.ta);
	if (ifd_atr_parser.td_last & TB_EXISTS)
		ifd_atr_write(ifd_atr_parser.tb);
	if (ifd_atr_parser.td_last & TC_EXISTS)
		ifd_atr_write(ifd_atr_parser.tc);

	ifd_atr_reset_tmp();
}

void ifd_atr_write_with_td(void)
{
	ifd_atr_parser.td_last |= TD_EXISTS;
	__ifd_atr_write_all();
}

void ifd_atr_set_ta(uint8_t byte)
{
	while (1) {
		if ((ifd_atr_parser.td_last & TA_EXISTS) == 0 &&
				(ifd_atr_same_round())) {
			ifd_atr_parser.ta = byte;
			ifd_atr_parser.td_last |= TA_EXISTS;
			return;
		}
		/* Reach here D-bit must be present.
		 * We need submit next round first. */
		ifd_atr_write_with_td();
	}
}

void ifd_atr_set_tb(uint8_t byte)
{
	while (1) {
		if ((ifd_atr_parser.td_last & TB_EXISTS) == 0 &&
				(ifd_atr_same_round())) {
			ifd_atr_parser.tb = byte;
			ifd_atr_parser.td_last |= TB_EXISTS;
			return;
		}
		/* Reach here D-bit must be present.
		 * We need submit next round first. */
		ifd_atr_write_with_td();
	}
}

void ifd_atr_set_tc(uint8_t byte)
{
	while (1) {
		if ((ifd_atr_parser.td_last & TC_EXISTS) == 0 &&
				(ifd_atr_same_round())) {
			ifd_atr_parser.tc = byte;
			ifd_atr_parser.td_last |= TC_EXISTS;
			return;
		}
		/* Reach here D-bit must be present.
		 * We need submit next round first. */
		ifd_atr_write_with_td();
	}
}

void ifd_atr_set_td(uint8_t byte)
{
	while (1) {
		if (ifd_atr_same_round()) {
			ifd_atr_parser.td = byte;
			return;
		}
		/* Reach here D-bit must be present.
		 * We need submit next round first. */
		ifd_atr_write_with_td();
	}
}

/* if no byte exist with this seq, return false */
boolean ifd_proto_byte(uint8_t proto, uint8_t seq)
{
	return false;
}

static void ifd_atr_build(void)
{
	uint8_t seq, proto, pid = 0;
	uint8_t tmp;

	ifd_xchg_reset();
	ifd_xchg_busy();

	ifd_atr_parser.len = 0;

	/* TS */
	ifd_atr_write(ifd_get_convention());

	ifd_atr_reset_tmp();

	/* for T0 */
	IFD_BATR_RND = 0;
	ifd_atr_set_td(ifd_atr_len);

	if (ifd_slot_ctrl.fd == IFD_FD_IMPLICIT &&
	    ifd_get_proto() == IFD_PROTO_DEF &&
	    ifd_slot_ctrl.n == IFD_N_IMPLICIT &&
	    ifd_only_support_t0()) {
		/* No interface bytes exist. */
		__ifd_atr_write_all();
		goto add_hist;
	}

	/* TA1 / TB1 / TC1 / TD1 */
	IFD_BATR_RND = 1;

	if (ifd_slot_ctrl.fd != IFD_FD_IMPLICIT)
		ifd_atr_set_ta(ifd_slot_ctrl.fd);
	if (ifd_slot_ctrl.n != IFD_N_IMPLICIT)
		ifd_atr_set_tc(ifd_slot_ctrl.n);

	/* need set TD1's T if T != PROTO_DEF */
	proto = ifd_pid_proto(pid++);
	if (proto != ifd_nr_protos && proto != IFD_PROTO_DEF)
		ifd_atr_set_td(proto);

	/* TA2 / TB2 / TC2 / TD2 */
	IFD_BATR_RND = 2;
	/* TA2 specific mode */
	tmp = 0x80;
	proto = ifd_pid_proto(pid++);
	if (proto != ifd_nr_protos)
		tmp |= proto;
	ifd_atr_set_ta(tmp);

	/* TC2 use T=0 seqence 0 */
	if (proto == IFD_PROTO_T0)
		ifd_proto_byte(proto, 0);

	/* TAi / TBi / TCi / TDi i > 2*/
	seq = 1;
	/* start from the last valid proto */
	while (proto != ifd_nr_protos) {
		IFD_BATR_RND++;
		if (!ifd_proto_byte(proto, seq)) {
			proto = ifd_pid_proto(pid++);
			seq = 1;
			continue;
		}
		seq++;
	}

	/* write all remainder bytes */
	__ifd_atr_write_all();
add_hist:
	ifd_pps_prop_build();
	ifd_atr_xfr_end();
}
#else
#define ifd_atr_save(byte)	(ifd_atr_buf[ifd_atr_len++] = byte)
#define ifd_atr_save_hist(byte)

void ifd_atr_build(void)
{
	ifd_xchg_reset();
	ifd_xchg_busy();
	ifd_atr_parser.len = 0;
	ifd_atr_write(ifd_get_convention());
	ifd_pps_prop_build();
	ifd_atr_xfr_end();
}
#endif
#else
#define ifd_atr_save_init()
#define ifd_atr_save(byte)
#define ifd_atr_save_hist(byte)
#endif

#ifdef CONFIG_IFD_AUTO_ACTIVATE
#define ifd_activate_auto()		ifd_activate()
#else
#define ifd_activate_auto()		ifd_seq_complete(SCS_ERR_HW_ERROR)
#endif

#ifdef CONFIG_IFD_AUTO_RESET
#define ifd_reset_auto()		ifd_reset()
#define ifd_power_on_auto()		scs_auto_seq(true)
#else
#define ifd_reset_auto()		ifd_seq_complete(SCS_ERR_HW_ERROR)
#define ifd_power_on_auto()		IFD_ERR_BAD_CLASS
#endif

#ifdef CONFIG_IFD_AUTO_PPS_PROP
#define ifd_info_xchg_auto()		ifd_info_xchg()
#define ifd_atr_build_auto()		ifd_atr_build()
#else
#define ifd_info_xchg_auto()		ifd_seq_complete(SCS_ERR_SUCCESS)
#define ifd_atr_build_auto()
#endif

#if 0
#ifdef CONFIG_IFD_CLOCK_CONTROL
static void ifd_suspend_clock(void)
{
	if (ifd_slot_ctrl.clk_stop == IFD_CLOCK_STOP_NONE) {
		ifd_xchg_set_error(SCS_ERR_UNSUPPORT);
	} else {
		ifd_hw_suspend_clock(ifd_slot_ctrl.clk_stop);
		ifd_xchg_set_error(SCS_ERR_SUCCESS);
	}
}

static void ifd_resume_clock(void)
{
	ifd_hw_resume_clock();
}
#else
static void ifd_suspend_clock(void) {}
static void ifd_resume_clock(void) {}
#endif
#endif

/*=======================================================================
 * sequences
 *======================================================================*/
static void ifd_seq_onoff(boolean on)
{
	if (on)
		ifd_slot_ctrl.cmpl = ifd_complete;
	else
		ifd_slot_ctrl.cmpl = NULL;
}

static scs_err_t ifd_seq_start(uint8_t seq)
{
	ifd_seq_set(seq);
	ifd_seq_onoff(true);
	return SCS_ERR_PROGRESS;
}

static void ifd_seq_complete(scs_err_t err)
{
	if (err == SCS_ERR_SUCCESS || ifd_xchg_get_error() == SCS_ERR_SUCCESS)
		ifd_xchg_set_error(err);
	if (ifd_slot_ctrl.cmpl)
		ifd_slot_ctrl.cmpl();
	ifd_seq_set(IFD_SEQ_IDLE);
	ifd_seq_onoff(false);
	ifd_scd_seq_complete(err);
}

void ifd_seq_reset(void)
{
	ifd_seq_set(IFD_SEQ_IDLE);
	ifd_seq_onoff(false);
}

static void ifd_seq_set(uint8_t seq)
{
	scs_debug(SCS_DEBUG_SEQ, seq);
	ifd_user_ctrl.seq = seq;
}

static uint8_t ifd_seq_get(void)
{
	return ifd_user_ctrl.seq;
}

scs_err_t ifd_slot_active(void)
{
	scs_err_t err;
	uint8_t state;

	err = ifd_slot_exist();
	if (err != SCS_ERR_SUCCESS) return err;

	state = ifd_slot_get_state();
	if (state != IFD_SLOT_STATE_ATR_READY &&
	    state != IFD_SLOT_STATE_PPS_READY)
		return SCS_ERR_BUSY_AUTO;
	return SCS_ERR_SUCCESS;
}

#if defined(CONFIG_IFD_AUTO_SEQUENCE) && !defined(CONFIG_IFD_SCD)
static scs_err_t scs_auto_seq(boolean cmpl)
{
	scs_err_t err;

	err = ifd_slot_exist();
	if (err != SCS_ERR_SUCCESS) return err;
	
	__ifd_class_init(IFD_CLASS_AUTO);
	ifd_deactivate();
	ifd_seq_set(IFD_SEQ_AUTO_SEQ);
	if (cmpl)
		ifd_seq_onoff(true);
	return SCS_ERR_PROGRESS;
}
#else
static scs_err_t scs_auto_seq(boolean cmpl)
{
	ifd_deactivate();
	return SCS_ERR_SUCCESS;
}
#endif

scs_err_t ifd_power_on(uint8_t clazz)
{
	scs_err_t err;

	if (clazz == IFD_CLASS_AUTO &&
	    ifd_seq_get() == IFD_SEQ_AUTO_SEQ) {
		ifd_seq_onoff(true);
		return SCS_ERR_PROGRESS;
	}

	err = ifd_slot_exist();
	if (err != SCS_ERR_SUCCESS) return err;

	/* turn on completion if AUTO is configured */
	if (clazz == IFD_CLASS_AUTO)
		return ifd_power_on_auto();

	__ifd_class_init(clazz);
	ifd_deactivate();
	return ifd_seq_start(IFD_SEQ_POWER_ON);
}

scs_err_t ifd_power_off(void)
{
	scs_err_t err = ifd_slot_exist();
	if (err != SCS_ERR_SUCCESS) return err;
	ifd_deactivate();
	return ifd_seq_start(IFD_SEQ_POWER_OFF);
}

scs_err_t ifd_set_param(void)
{
	scs_err_t err = ifd_slot_active();
	if (err != SCS_ERR_SUCCESS) return err;
	ifd_info_xchg();
	return ifd_seq_start(IFD_SEQ_SET_PARAM);
}

scs_err_t ifd_reset_param(void)
{
	scs_err_t err;
	
	err = ifd_slot_active();
	if (err != SCS_ERR_SUCCESS) return err;
	ifd_config_def();
	return SCS_ERR_SUCCESS;
}

#ifdef CONFIG_IFD_AUTO_SEQUENCE
static void ifd_handle_auto_seq(void)
{
	uint8_t state = ifd_slot_get_state();

	if (state == IFD_SLOT_STATE_PRESENT) {
		if (!ifd_cls_select_auto()) {
			ifd_seq_complete(SCS_ERR_HW_ERROR);
		}
	} else if (state == IFD_SLOT_STATE_SELECTED) {
		ifd_activate_auto();
	} else if (state == IFD_SLOT_STATE_ACTIVATED) {
		ifd_reset_auto();
	} else if (state == IFD_SLOT_STATE_ATR_READY) {
		ifd_info_xchg_auto();
	} else if (state == IFD_SLOT_STATE_PPS_READY) {
		ifd_atr_build_auto();
		ifd_seq_complete(SCS_ERR_SUCCESS);
	} else if (state == IFD_SLOT_STATE_HWERROR) {
		/* deactivating */
	} else {
		ifd_seq_complete(SCS_ERR_HW_ERROR);
	}
}
#else
static void ifd_handle_auto_seq(void) {}
#endif

boolean ifd_power_on_next(void)
{
	if (ifd_slot_ctrl.conv == IFD_CONV_DIRECT) {
		if (ifd_slot_ctrl.clazz == ifd_slot_ctrl.nego_clazz) {
			ifd_slot_ctrl.conv = IFD_CONV_INVERSE;
		}
		return (ifd_cls_select() == SCS_ERR_SUCCESS);
	} else {
		return false;
	}
}

static void ifd_handle_power_on(void)
{
	uint8_t state = ifd_slot_get_state();

	if (state == IFD_SLOT_STATE_PRESENT) {
		if (!ifd_power_on_next()) {
			ifd_seq_complete(SCS_ERR_HW_ERROR);
		}
	} else if (state == IFD_SLOT_STATE_SELECTED) {
		ifd_activate();
	} else if (state == IFD_SLOT_STATE_ACTIVATED) {
		ifd_reset();
	} else if (state == IFD_SLOT_STATE_ATR_READY) {
		ifd_info_xchg_auto();
	} else if (state == IFD_SLOT_STATE_PPS_READY) {
		ifd_atr_build_auto();
		ifd_seq_complete(SCS_ERR_SUCCESS);
	} else if (state == IFD_SLOT_STATE_HWERROR) {
		/* deactivating */
	} else if (state == IFD_SLOT_STATE_NOTPRESENT) {
		ifd_seq_complete(SCS_ERR_HW_ERROR);
	}
}

static void ifd_handle_power_off(void)
{
	uint8_t state = ifd_slot_get_state();

	if (state == IFD_SLOT_STATE_PRESENT ||
	    state == IFD_SLOT_STATE_NOTPRESENT) {
		ifd_seq_complete(SCS_ERR_SUCCESS);
	}
}

static void ifd_handle_set_param(void)
{
	uint8_t state = ifd_slot_get_state();

	if (state == IFD_SLOT_STATE_PPS_READY) {
		ifd_seq_complete(SCS_ERR_SUCCESS);
	} else if (state == IFD_SLOT_STATE_PRESENT ||
		   state == IFD_SLOT_STATE_NOTPRESENT) {
		ifd_seq_complete(SCS_ERR_HW_ERROR);
	}
}

#if 0
static void ifd_handle_reset_param(void)
{
	uint8_t state = ifd_slot_get_state();

	if (state == IFD_SLOT_STATE_ACTIVATED) {
		ifd_reset();
	} else if (state == IFD_SLOT_STATE_ATR_READY) {
		ifd_info_xchg_auto();
	} else if (state == IFD_SLOT_STATE_PPS_READY) {
		ifd_atr_build_auto();
		ifd_seq_complete(SCS_ERR_SUCCESS);
	} else if (state == IFD_SLOT_STATE_PRESENT ||
		   state == IFD_SLOT_STATE_NOTPRESENT) {
		ifd_seq_complete(SCS_ERR_HW_ERROR);
	}
}
#endif

static void ifd_handle_interchange_slot(void)
{
	uint8_t state;

	if (ifd_xchg_get_state() != IFD_XCHG_STATE_ERROR)
		return;

	state = ifd_slot_get_state();
	if (state == IFD_SLOT_STATE_PRESENT ||
	    state == IFD_SLOT_STATE_NOTPRESENT) {
		ifd_xchg_idle();
	}
}

static void ifd_handle_interchange_xchg(void)
{
	BUG_ON(ifd_xchg_get_state() != IFD_XCHG_STATE_IDLE);
	ifd_seq_complete(ifd_xchg_get_error());
}

static void ifd_handle_slot_seq(void)
{
	uint8_t seq = ifd_seq_get();
	if (seq == IFD_SEQ_AUTO_SEQ)
		ifd_handle_auto_seq();
	else if (seq == IFD_SEQ_POWER_ON)
		ifd_handle_power_on();
	else if (seq == IFD_SEQ_POWER_OFF)
		ifd_handle_power_off();
	else if (seq == IFD_SEQ_SET_PARAM)
		ifd_handle_set_param();
	else if (seq == IFD_SEQ_INTERCHANGE)
		ifd_handle_interchange_slot();
}

static void ifd_handle_xchg_seq(void)
{
	uint8_t seq = ifd_seq_get();

	ifd_xchg_set_state(IFD_XCHG_STATE_IDLE);
	if (seq == IFD_SEQ_INTERCHANGE)
		ifd_handle_interchange_xchg();
}

/*=======================================================================
 * ATR parsing
 *======================================================================*/
static uint8_t ifd_atr_read(void)
{
	uint8_t byte = ifd_xchg_read(ifd_atr_parser.len);

	ifd_atr_save(byte);
	scs_debug_dump(byte);
	ifd_atr_parser.len++;
	ifd_atr_parser.csum ^= byte;
	return byte;
}

static uint8_t ifd_atr_read_ibyte(void)
{
	uint8_t y;

	ifd_atr_parser.td_last = ifd_atr_parser.td;
	y = TD_Y(ifd_atr_parser.td_last);
	if (y) {
		ifd_atr_parser.ta = y & TA_EXISTS ? ifd_atr_read() : 0;
		ifd_atr_parser.tb = y & TB_EXISTS ? ifd_atr_read() : 0;
		ifd_atr_parser.tc = y & TC_EXISTS ? ifd_atr_read() : 0;
		ifd_atr_parser.td = y & TD_EXISTS ? ifd_atr_read() : 0;
	}
	return y;
}

static void scs_atr_start(void)
{
	ifd_atr_parser.len = 0;
	ifd_atr_parser.tck_exists = false;
	ifd_atr_parser.csum = 0;

	/* convention is handled on the fly */
	ifd_slot_ctrl.info_xchg = IFD_INFO_XCHG_NEGO_1ST;
	ifd_slot_ctrl.flags = 0;
	/* if no TA1 */
	ifd_slot_ctrl.ind_fd = IFD_FD_IMPLICIT;
	/* if no TD1 */
	ifd_slot_ctrl.first_proto = IFD_PROTO_T0;

	/* if no TA2 */
	ifd_slot_ctrl.spec_proto = IFD_PROTO_INVALID;
	ifd_slot_ctrl.spec_fd = IFD_FD_IMPLICIT;
	ifd_slot_ctrl.ind_protos = 0;
}

static scs_err_t ifd_atr_parse_ts(void)
{
	uint8_t byte = ifd_xchg_read(0);

	scs_debug_dump(byte);
	ifd_atr_parser.len++;
	if (byte == IFD_CONV_DIRECT ||
	    byte == IFD_CONV_INVERSE) {
		return SCS_ERR_SUCCESS;
	} else {
		return IFD_ERR_BAD_TS;
	}
}

uint8_t ifd_atr_get_ta(void)
{
	return ifd_atr_parser.ta;
}

uint8_t ifd_atr_get_tb(void)
{
	return ifd_atr_parser.tb;
}

uint8_t ifd_atr_get_tc(void)
{
	return ifd_atr_parser.tc;
}

static void ifd_atr_parse_gbyte1(uint8_t y)
{
	if (y & TA_EXISTS)
		ifd_slot_ctrl.ind_fd = ifd_atr_parser.ta;
	if (y & TC_EXISTS)
		ifd_apply_n(ifd_atr_parser.tc);
	/* TD1 means first offered protocol */
	if (y & TD_EXISTS)
		ifd_slot_ctrl.first_proto = TD_T(ifd_atr_parser.td);
}

static void ifd_atr_parse_gbyte2(uint8_t y)
{
	if (y & TA_EXISTS) {
		ifd_slot_ctrl.info_xchg = IFD_INFO_XCHG_SPECIFIC;
		if (!(ifd_atr_parser.ta & 0x10))
			ifd_slot_ctrl.spec_fd = ifd_slot_ctrl.ind_fd;
		if (!(ifd_atr_parser.ta & 0x80))
			ifd_slot_ctrl.flags |= IFD_FLAG_SPEC_CAP_CHANGE;
		ifd_slot_ctrl.spec_proto = TD_T(ifd_atr_parser.ta);
	}
}

static void ifd_atr_parse_gbyte15(uint8_t y)
{
	ifd_slot_ctrl.flags |= IFD_FLAG_GT_USE_IND_FD;
	if (y & TA_EXISTS) {
		ifd_slot_ctrl.clk_stop = ifd_atr_parser.ta & IFD_CLOCK_STOP_MASK;
		ifd_slot_ctrl.ind_cls = ifd_atr_parser.ta & IFD_CLASS_MASK;
	}
	/* ignore TB after T=15 */
}

static void ifd_atr_parse_t(void)
{
	uint8_t y = TD_Y(ifd_atr_parser.td_last);
	uint8_t t = TD_T(ifd_atr_parser.td_last);
	ifd_proto_t *cproto;

	if  (t == IFD_GLOBAL_T15) {
		ifd_atr_parse_gbyte15(y);
	} else {
		uint8_t pid;

		if (!ifd_atr_parser.tck_exists)
			ifd_atr_parser.tck_exists = (t != IFD_PROTO_T0);
		/* unsupported protocols will be ignored */
		pid = ifd_proto_pid(t);
		if (pid < ifd_nr_protos) {
			ifd_slot_ctrl.ind_protos |= (1<<t);
			cproto = ifd_protos[pid];
			if (cproto->atr_parse)
				cproto->atr_parse(y);
		}
	}
}

static scs_err_t ifd_atr_parse(void)
{
	scs_err_t err;
	uint8_t temp, y, k;

	scs_atr_start();
	ifd_atr_save_init();

	err = ifd_atr_parse_ts();
	if (err != SCS_ERR_SUCCESS) return err;

	/* T0 */
	ifd_atr_parser.td = ifd_atr_read();
	k = T0_K(ifd_atr_parser.td);

	y = ifd_atr_read_ibyte();
	/* parse Y0, TA1, TB1, TC1 */
	ifd_atr_parse_gbyte1(y);
	if (!y) goto ibyte_done;

	y = ifd_atr_read_ibyte();
	/* parse Y1, TA2, TB2, TC2 */
	ifd_atr_parse_gbyte2(y);

	while (y) {
		ifd_atr_parse_t();
		/* parse Yi-1, TAi, TBi, TCi */
		y = ifd_atr_read_ibyte();
	}

ibyte_done:
	/* skip historical byte */
	while (k > 0) {
		temp = ifd_atr_read();
		ifd_atr_save_hist(temp);
		k--;
	}
	/* validate check byte */
	if (ifd_atr_parser.tck_exists) {
		temp = ifd_atr_read();
		ifd_atr_save_hist(temp);
		if (ifd_atr_parser.csum != 0)
			return IFD_ERR_BAD_TCK;
	}
	if (ifd_xchg_avail() != ifd_atr_parser.len)
		return SCS_ERR_OVERRUN;
	return SCS_ERR_SUCCESS;
}

static void ifd_atr_cmpl(void)
{
	scs_err_t err;

	err = ifd_xchg_get_error();
	if (err != SCS_ERR_SUCCESS) return;
	err = ifd_atr_parse();
	if (err == SCS_ERR_SUCCESS) {
		ifd_slot_raise(IFD_SLOT_EVENT_ATR_OK);
	} else {
		ifd_xchg_set_error(err);
	}
}

/*=======================================================================
 * ATR building
 *======================================================================*/
uint8_t ifd_get_proto(void)
{
	return ifd_slot_ctrl.proto;
}

uint32_t ifd_get_data(void)
{
	uint8_t fi = HIHALF(ifd_slot_ctrl.fd);
	uint8_t di = LOHALF(ifd_slot_ctrl.fd);
	return ifd_cf_get_data(ifd_get_freq(), di, fi);
}

#ifndef CONFIG_IFD_AUTO_PPS_PROP
scs_err_t ifd_set_freq_data(uint32_t freq, uint32_t rate)
{
	uint8_t i;
	uint32_t d;
	uint8_t fi = HIHALF(ifd_slot_ctrl.fd);
	uint8_t di = LOHALF(ifd_slot_ctrl.fd);
	boolean valid_f = false;
		
	if (ifd_cf_nr_freq() <= 1)
		return SCS_ERR_UNSUPPORT;
	
	d = ifd_cf_get_data(freq, di, fi);
	if (d > IFD_HW_DATA_MAX /* || d != rate */)
		return IFD_ERR_BAD_FD;

	/* ifd_cf_get_data contains sanity check of fi,
	 * thus, f sanity must follows d sanity check codes.
	 */
	for (i = 0; i < ifd_cf_nr_freq(); i++) {
		if (freq == ifd_cf_get_freq(i))
			valid_f = true;
	}
	if (!valid_f || freq > ifd_fmax_supps[fi])
		return IFD_ERR_BAD_FD;

	ifd_set_freq(freq);
	return SCS_ERR_SUCCESS;
}
#endif

#ifdef CONFIG_IFD_MECHA_CONTROL
scs_err_t ifd_lock_card(void)
{
	return SCS_ERR_UNSUPPORT;
}

scs_err_t ifd_unlock_card(void)
{
	return SCS_ERR_UNSUPPORT;
}
#endif

#ifdef CONFIG_IFD_CLOCK_CONTROL
scs_err_t ifd_restart_clock(void)
{
	return SCS_ERR_UNSUPPORT;
}

scs_err_t ifd_stop_clock(void)
{
	return SCS_ERR_UNSUPPORT;
}

uint8_t ifd_clock_status(void)
{
	return IFD_CLOCK_RUNNING;
}
#endif

uint8_t ifd_cf_nr_freq(void)
{
	return ifd_hw_cf_nr_freq();
}

uint32_t ifd_cf_get_freq(uint8_t index)
{
	return ifd_hw_cf_get_freq(index);
}

uint32_t ifd_get_freq(void)
{
	return ifd_slot_ctrl.freq;
}

void ifd_set_freq(uint32_t freq)
{
	BUG_ON(freq > IFD_HW_FREQ_MAX);
	ifd_slot_ctrl.freq = freq;
	ifd_hw_set_freq(freq);
}

uint8_t ifd_cf_nr_data(void)
{
	uint8_t c, d, f;
	uint8_t nr_rates = 0;

	for (c = 0; c < ifd_cf_nr_freq(); c++) {
		for (d = 0; d < NR_IFD_DIS; d++) {
			for (f = 0; f < NR_IFD_FIS; f++) {
				if (ifd_cf_get_data(ifd_cf_get_freq(c), d, f))
					nr_rates++;
			}
		}
	}
	return nr_rates;
}

uint32_t ifd_cf_get_data(uint32_t freq, uint8_t di, uint8_t fi)
{
	uint32_t rate;
	if (di >= NR_IFD_DIS || fi >= NR_IFD_FIS)
		return 0;
	if (ifd_get_D(di) == 0 || ifd_get_F(fi) == 0)
		return 0;
	rate = div32u((mul16u(freq,
			      mul16u(1000, ifd_get_D(di)))),
		      ifd_get_F(fi));
	if (rate > IFD_HW_DATA_MAX)
		return 0;
	return rate;
}

boolean ifd_config_clk(uint8_t clk_stop)
{
	ifd_slot_ctrl.clk_stop = clk_stop;
	return false;
}

void ifd_config_cls(void)
{
	scs_debug(SCS_DEBUG_CLASS, ifd_slot_ctrl.clazz);
	scs_debug(SCS_DEBUG_CONV, ifd_slot_ctrl.conv);
	ifd_hw_config_cls(ifd_slot_ctrl.clazz);
	ifd_hw_config_conv(ifd_slot_ctrl.conv);
}

scs_err_t ifd_valid_fd(uint8_t fd)
{
	if (HIHALF(fd) > NR_IFD_FIS || LOHALF(fd) > NR_IFD_DIS)
		return IFD_ERR_BAD_FD;
	if (!ifd_get_F(HIHALF(fd)) ||
	    !ifd_get_D(LOHALF(fd))) {
		return IFD_ERR_BAD_FD;
	}
	return SCS_ERR_SUCCESS;
}

void ifd_config_t(void)
{
	ifd_slot_ctrl.proto = ifd_slot_ctrl.nego_proto;
}

void ifd_config_fd(void)
{
	uint16_t F;
	uint8_t D;
	
	ifd_slot_ctrl.fd = ifd_slot_ctrl.nego_fd;

	BUG_ON(HIHALF(ifd_slot_ctrl.fd) > NR_IFD_FIS ||
	       LOHALF(ifd_slot_ctrl.fd) > NR_IFD_DIS);
	BUG_ON(!ifd_get_F(HIHALF(ifd_slot_ctrl.fd)) ||
	       !ifd_get_D(LOHALF(ifd_slot_ctrl.fd)));

	F = ifd_get_F(HIHALF(ifd_slot_ctrl.fd));
	D = ifd_get_D(LOHALF(ifd_slot_ctrl.fd));
	scs_debug(SCS_DEBUG_CONF_ETU, HIBYTE(F));
	scs_debug(SCS_DEBUG_CONF_ETU, LOBYTE(F));
	scs_debug(SCS_DEBUG_CONF_ETU, D);
	ifd_hw_config_etu(F, D);
	return; 
}

void ifd_apply_n(uint8_t n)
{
	ifd_slot_ctrl.n = n;
}

void ifd_config_def(void)
{
	uint8_t pid;
	ifd_proto_t *cproto;

	/* set default ind value scs_default_ind */
	ifd_slot_ctrl.ind_fd = IFD_FD_IMPLICIT;
	ifd_slot_ctrl.ind_protos = 0;
	ifd_apply_n(IFD_N_IMPLICIT);
	ifd_apply_cgt(12);
	ifd_apply_bgt(IFD_N_IMPLICIT+12);
	ifd_apply_cwt(IFD_WT_PPS);
	ifd_apply_bwt(IFD_WT_PPS+12);

	/* set default nego value scs_default_nego */
	ifd_slot_ctrl.nego_proto = IFD_PROTO_DEF; 
	ifd_slot_ctrl.nego_fd = IFD_FD_IMPLICIT;

	for (pid = 0; pid < ifd_nr_protos; pid++) {
		cproto = ifd_protos[pid];
		if (cproto->reset_param)
			cproto->reset_param();
	}
	ifd_conf_param();
}

void ifd_apply_cgt(uint16_t cgt)
{
	scs_debug(SCS_DEBUG_CONF_GT, HIBYTE(cgt));
	scs_debug(SCS_DEBUG_CONF_GT, LOBYTE(cgt));
	ifd_slot_ctrl.cgt = cgt;
}

void ifd_apply_bgt(uint16_t bgt)
{
	scs_debug(SCS_DEBUG_CONF_GT, HIBYTE(bgt));
	scs_debug(SCS_DEBUG_CONF_GT, LOBYTE(bgt));
	ifd_slot_ctrl.bgt = bgt;
}

void ifd_apply_cwt(uint32_t cwt)
{
	scs_debug(SCS_DEBUG_CONF_WT, HIBYTE(HIWORD(cwt)));
	scs_debug(SCS_DEBUG_CONF_WT, LOBYTE(HIWORD(cwt)));
	scs_debug(SCS_DEBUG_CONF_WT, HIBYTE(LOWORD(cwt)));
	scs_debug(SCS_DEBUG_CONF_WT, LOBYTE(LOWORD(cwt)));
	ifd_slot_ctrl.cwt = cwt;
}

void ifd_apply_bwt(uint32_t bwt)
{
	scs_debug(SCS_DEBUG_CONF_WT, HIBYTE(HIWORD(bwt)));
	scs_debug(SCS_DEBUG_CONF_WT, LOBYTE(HIWORD(bwt)));
	scs_debug(SCS_DEBUG_CONF_WT, HIBYTE(LOWORD(bwt)));
	scs_debug(SCS_DEBUG_CONF_WT, LOBYTE(LOWORD(bwt)));
	ifd_slot_ctrl.bwt = bwt;
}

/* GT = 12 etu + ((Fi/Di) * (N/f)) = 12 + N * (Fi/(Di*f)),
 * we take (Fi/(Di*f)) as 1 currently
 */
void ifd_delay_gt(uint16_t gt, boolean blk)
{
	ifd_hw_delay_gt(gt, blk);
}

void ifd_delay_wt(uint32_t wt, boolean blk)
{
	ifd_hw_delay_wt(wt, blk);
}

uint8_t ifd_get_convention(void)
{
	return ifd_slot_ctrl.conv;
}

uint8_t ifd_get_conv(void)
{
	return ifd_get_convention() == IFD_CONV_DIRECT ? 0 : 2;
}

void ifd_config_crep(uint8_t on)
{
	ifd_hw_config_crep(on);
}

uint8_t ifd_proto_pid(uint8_t proto)
{
	uint8_t i;
	ifd_proto_t *cproto;

	for (i = 0; i < ifd_nr_protos; i++) {
		cproto = ifd_protos[i];
		if (proto == cproto->id)
			return i;
	}
	return NR_IFD_PROTOS;
}

uint8_t ifd_register_protocol(ifd_proto_t *proto)
{
	uint8_t pid;

	pid = ifd_nr_protos;
	if (pid < NR_IFD_PROTOS) {
		ifd_protos[pid] = proto;
		ifd_nr_protos++;
		return pid;
	}
	BUG();
	return pid;
}

void ifd_register_handlers(scs_intr_cb notifier, scs_cmpl_cb completion)
{
	ifd_notifier = notifier;
	ifd_complete = completion;
}

#if NR_IFD_SLOTS > 1
void ifd_sid_restore(ifd_sid_t sid)
{
	ifd_slid = sid;
	ifd_hw_slot_select(sid);
}

ifd_sid_t ifd_sid_save(ifd_sid_t sid)
{
	ifd_sid_t o_sid = ifd_slid;
	ifd_sid_restore(sid);
	return o_sid;
}
#endif

#ifdef CONFIG_IFD_PRES_POLL
#define IFD_PRES_POLL_TIMEOUT	20

__near__ tid_t ifd_tid = INVALID_TID;

static void ifd_pres_init(void)
{
	ifd_tid = timer_register(ifd_sid, TIMER_DELAYABLE);
	timer_schedule_shot(ifd_tid, IFD_PRES_POLL_TIMEOUT);
}

static void ifd_pres_handler(void)
{
	if (timer_timeout_raised(ifd_tid, TIMER_DELAYABLE)) {
		if (ifd_hw_icc_present()) {
			ifd_detect_icc_in();
		} else {
			ifd_detect_icc_out();
		}
	}

	timer_schedule_shot(ifd_tid, IFD_PRES_POLL_TIMEOUT);
}
#else
#define ifd_pres_init()
#define ifd_pres_handler()
#endif

#ifdef CONFIG_IFD_SCD
#include <target/scd.h>

__near__ scd_t ifd_scds[NR_IFD_SLOTS];

scd_t ifd_scd_dev(ifd_sid_t sid)
{
	BUG_ON(sid >= NR_IFD_SLOTS);
	return ifd_scds[sid];
}

ifd_sid_t ifd_scd_slot(scd_t scd)
{
	ifd_sid_t sid;

	for (sid = 0; sid < NR_IFD_SLOTS; sid++) {
		if (scd == ifd_scds[sid])
			return sid;
	}
	BUG();
	return INVALID_IFD_SID;
}

scs_err_t ifd_scd_error(scs_err_t err)
{
	switch (err) {
	case SCS_ERR_SUCCESS:
	case SCS_ERR_PROGRESS:
	case SCS_ERR_OVERRUN:
	case SCS_ERR_TIMEOUT:
	case SCS_ERR_NOTPRESENT:
		return err;
	default:
		return SCS_ERR_HW_ERROR;
	}
}

uint8_t ifd_scd_state(uint8_t state)
{
	switch (state) {
	case IFD_SLOT_STATE_NOTPRESENT:
		return SCD_DEV_STATE_NOTPRESENT;
	case IFD_SLOT_STATE_PRESENT:
	case IFD_SLOT_STATE_SELECTED:
	case IFD_SLOT_STATE_ACTIVATED:
		return SCD_DEV_STATE_PRESENT;
	case IFD_SLOT_STATE_ATR_READY:
	case IFD_SLOT_STATE_PPS_READY:
		return SCD_DEV_STATE_ACTIVE;
	case IFD_SLOT_STATE_HWERROR:
	default:
		return SCD_DEV_STATE_HWERROR;
	}
}

static scs_err_t ifd_scd_power_on(boolean cmpl)
{
	scs_err_t err;

	err = ifd_slot_exist();
	if (err != SCS_ERR_SUCCESS) return err;

	__ifd_class_init(IFD_CLASS_AUTO);
	ifd_deactivate();
	ifd_seq_set(IFD_SEQ_AUTO_SEQ);
	if (cmpl)
		ifd_seq_onoff(true);
	return SCS_ERR_PROGRESS;
}

static void ifd_scd_select(void)
{
	ifd_sid_select(ifd_scd_slot(scd_id));
}

static scs_err_t ifd_scd_activate(void)
{
	scs_err_t err;
	err = ifd_scd_power_on(true);
	return ifd_scd_error(err);
}

static scs_err_t ifd_scd_deactivate(void)
{
	scs_err_t err;
	err = ifd_power_off();
	return ifd_scd_error(err);
}

static scs_err_t ifd_scd_xchg_block(scs_size_t nc, scs_size_t ne)
{
	scs_err_t err;
	err = ifd_xchg_block(nc, ne);
	return ifd_scd_error(err);
}

static scs_size_t ifd_scd_xchg_avail(void)
{
	return ifd_xchg_avail();
}

static scs_err_t ifd_scd_write_byte(scs_off_t index, uint8_t byte)
{
	scs_err_t err;

	err = ifd_write_byte(index, byte);
	return ifd_scd_error(err);
}

static uint8_t ifd_scd_read_byte(scs_off_t index)
{
	return ifd_read_byte(index);
}

scd_driver_t ifd_scd = {
	ifd_scd_select,
	ifd_scd_activate,
	ifd_scd_deactivate,
	ifd_scd_xchg_block,
	ifd_scd_xchg_avail,
	ifd_scd_write_byte,
	ifd_scd_read_byte,
};

static void ifd_scd_seq_complete(scs_err_t err)
{
	scd_dev_select(ifd_scd_dev(ifd_slid));
	scd_seq_complete(ifd_scd_error(err));
}

static void ifd_scd_set_state(uint8_t state)
{
	scd_dev_select(ifd_scd_dev(ifd_slid));
	scd_dev_set_state(ifd_scd_state(state));
}

static void ifd_scd_init()
{
	ifd_sid_t sid;

	for (sid = 0; sid < NR_IFD_SLOTS; sid++) {
		ifd_scds[sid] = scd_register_device(&ifd_scd);
	}
}
#else
#define ifd_scd_init()
#endif

static void ifd_state_handler(void)
{
	uint8_t sid, ssid;
	for (sid = 0; sid < NR_IFD_SLOTS; sid++) {
		ssid = ifd_sid_save(sid);
		/*scs_debug(SCS_DEBUG_SLOT, sid);*/
		ifd_handle_slot_state();
		ifd_handle_xchg_state();
		ifd_sid_restore(ssid);
	}
}

static void ifd_handler(uint8_t event)
{
	switch (event) {
	case STATE_EVENT_SHOT:
		ifd_pres_handler();
		break;
	case STATE_EVENT_WAKE:
		ifd_state_handler();
		break;
	default:
		BUG();
		break;
	}
}

void ifd_init(void)
{
	ifd_sid_t sid, ssid;

	DEVICE_FUNC(DEVICE_FUNC_IFD);
	ifd_sid = state_register(ifd_handler);

	/* only one IFD is allowed */
	ifd_hw_ctrl_init();
	ifd_t0_init();
	ifd_t1_init();
	ifd_pres_init();
	ifd_scd_init();

	for (sid = 0; sid < NR_IFD_SLOTS; sid++) {
		ssid = ifd_sid_save(sid);
		ifd_seq_reset();
		ifd_slot_reset();
		ifd_sid_restore(ssid);
	}
}
