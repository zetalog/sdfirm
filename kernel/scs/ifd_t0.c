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
 * @(#)ifd_t0.c: ISO/IEC 7816-3 T=0 protocol functions
 * $Id: ifd_t0.c,v 1.62 2011-10-26 10:09:44 zhenglv Exp $
 */

#include "ifd_priv.h"

#if NR_IFD_SLOTS > 1
__near__ struct ifd_conf_t0 ifd_t0_confs[NR_IFD_SLOTS];
#define ifd_t0_conf ifd_t0_confs[ifd_slid]
#else
__near__ struct ifd_conf_t0 ifd_t0_conf;
#endif
static scs_err_t __ifd_t0_xchg_init(scs_size_t nc, scs_size_t ne);

static void ifd_t0_apply_wi(uint8_t wi)
{
	ifd_t0_conf.wi = wi;
}

uint8_t ifd_get_t0_param(uint8_t what)
{
	switch (what) {
	case IFD_T0_PARAM_FD:
		return ifd_slot_ctrl.fd;
	case IFD_T0_PARAM_TCCKS:
		return ifd_get_conv();
	case IFD_T0_PARAM_GT:
		return ifd_slot_ctrl.n;
	case IFD_T0_PARAM_WI:
		return ifd_t0_conf.wi;
	case IFD_T0_PARAM_CLOCKSTOP:
		return ifd_slot_ctrl.clk_stop;
	}
	return 0;
}

#ifdef CONFIG_IFD_T0_APDU
#define IFD_T0_APDU_CHANGE_RESP		0x01
#define IFD_T0_APDU_CHANGE_ENV		0x02
scs_err_t ifd_set_t0_apdu(uint8_t mask, uint8_t cla_resp, uint8_t cla_env)
{
	/* XXX: No T0APDU Support Currently
	 *
	 * According to our design, when APDU's Nc, Ne exceed transfer
	 * limit, a OVERRUN will be reported
	 * When 61XY indicates that Nr has exceeded the transfer limit,
	 * we will relay 61XY to host, which requires host side
	 * retransmission handling.
	 * So we can conclude that:
	 * 1. We will never use EVELOPE
	 * 2. When we use GET RESPONSE, CLA should always reflect CLA in
	 *    the command APDU.
	 */
	if (mask & IFD_T0_APDU_CHANGE_RESP)
		ifd_t0_conf.cla_resp = cla_resp;
	if (mask & IFD_T0_APDU_CHANGE_ENV)
		ifd_t0_conf.cla_env = cla_env;
	return SCS_ERR_SUCCESS;
}
#else
scs_err_t ifd_set_t0_apdu(uint8_t mask, uint8_t cla_resp, uint8_t cla_env)
{
	return SCS_ERR_UNSUPPORT;
}
#endif

#ifndef CONFIG_IFD_AUTO_PPS_PROP
scs_err_t ifd_set_t0_param(uint8_t fd, uint8_t gi,
			   uint8_t wi, boolean clk_stop)
{
	scs_err_t err;

	err = ifd_valid_fd(fd);
	if (err != SCS_ERR_SUCCESS) return err;

	/* configure negotiation parameters */
	ifd_slot_ctrl.nego_fd = fd;
	ifd_slot_ctrl.nego_proto = SCS_PROTO_T0;

	/* not negotiatables */
	ifd_apply_n(gi);
	ifd_t0_apply_wi(wi);

	return ifd_set_param();
}
#endif

static void ifd_t0_reset_param(void)
{
	ifd_t0_apply_wi(IFD_T0_WI_IMPLICIT);
}

#define IFD_T0_WWT_FACTOR	(uint16_t)960

static void ifd_t0_nego_pps(void)
{
	//ifd_t0_apply_wi(ifd_t0_conf.ind_wi);
}

static void ifd_t0_conf_param(void)
{
	uint32_t wwt;

	if (ifd_slot_ctrl.n == IFD_N_MINIMUM) {
		ifd_apply_cgt(12);
		ifd_apply_bgt(12);
	} else {
		ifd_apply_cgt(ifd_slot_ctrl.n+12);
		ifd_apply_bgt(ifd_slot_ctrl.n+12);
	}

	wwt = mul16u(ifd_t0_conf.wi *
		     ifd_get_D(LOHALF(ifd_slot_ctrl.fd)),
			       (uint16_t)mul16u(IFD_WT_RATIO,
						IFD_T0_WWT_FACTOR));
#ifdef CONFIG_IFD_T0_APDU
	/* TODO: up layer(CCID) should set this value (SCD_MUTE_APDU_CLASS)
	 * by ifd_set_t0_apdu */
	ifd_t0_conf.cla_resp = 0x00;
	ifd_t0_conf.cla_env = 0x00;
#endif
	ifd_config_crep(true);
	ifd_apply_cwt(wwt);
	ifd_apply_bwt(wwt);
}

static void ifd_t0_atr_parse(uint8_t y)
{
	uint8_t tci = ifd_atr_get_tc();

	if (y & TC_EXISTS)
		ifd_t0_apply_wi(tci ? tci : IFD_T0_WI_IMPLICIT);
	else
		ifd_t0_apply_wi(IFD_T0_WI_IMPLICIT);
}

void ifd_t0_xfr_block(scs_size_t nc, scs_size_t ne)
{
	ifd_xfr_block(ifd_t0_xchg.tx, nc,
		      ifd_t0_xchg.rx, ne,
		      ifd_t0_xchg.nc);
	ifd_t0_xchg.tx += nc;
	ifd_t0_xchg.rx += ne;
}

#ifdef CONFIG_IFD_XCHG_APDU
#ifdef CONFIG_IFD_T0_APDU
#define IFD_INS_ENVELOPE	0xC2
#define IFD_INS_GET_RESPONSE	0xC0
void ifd_t0_envelop(scs_size_t nc)
{
	ifd_xchg_write(0, ifd_t0_conf.cla_env);
	ifd_xchg_write(1, IFD_INS_ENVELOPE);
}
#else
#define ifd_t0_envelop(nc)
#endif

scs_err_t ifd_t0_xfr_parse(scs_size_t nc)
{
	scs_off_t offset;

	ifd_t0_xchg.lc = 0;
	ifd_t0_xchg.le = 0;
	if (nc < 4)
		return SCS_ERR_OVERRUN;

#ifdef CONFIG_IFD_T0_6C
	ifd_t0_xchg.cla = ifd_xchg_read(0);
#endif
	ifd_t0_xchg.ins = ifd_xchg_read(1);
	if (nc == 4) {
		ifd_xchg_write(4, 0);
	} else if (nc == 5) {
		ifd_t0_xchg.le = (scs_size_t)ifd_xchg_read(4);
		if (!ifd_t0_xchg.le)
			ifd_t0_xchg.le = 256;
	} else {
		ifd_t0_xchg.lc = (scs_size_t)ifd_xchg_read(4);
		offset = 6 + ifd_t0_xchg.lc;
		if (nc > offset) {
			return SCS_ERR_OVERRUN;
		} else if (nc == offset) {
			ifd_t0_xchg.le = (scs_size_t)ifd_xchg_read(nc-1);
			if (!ifd_t0_xchg.le)
				ifd_t0_xchg.le = 256;
		} else if (ifd_t0_xchg.lc > nc - 5) {
			/* lc > data field length */
			ifd_t0_envelop(nc);
		}
	}
	/* initialize T0 transfer attributes */
	ifd_t0_xchg.nc = ifd_t0_xchg.lc + 5;
	ifd_t0_xchg.ne = ifd_t0_xchg.le + 2;
	return SCS_ERR_SUCCESS;
}
#else
scs_err_t ifd_t0_xfr_parse(scs_size_t nc)
{
	if (nc < 5)
		return SCS_ERR_OVERRUN;
	ifd_t0_xchg.ins = ifd_xchg_read(1);
	if (nc == 5) {
		ifd_t0_xchg.lc = 0;
		ifd_t0_xchg.le = (scs_size_t)ifd_xchg_read(4);
		if (!ifd_t0_xchg.le)
			ifd_t0_xchg.le = 256;
	} else {
		ifd_t0_xchg.lc = (scs_size_t)ifd_xchg_read(4);
		if (nc != ifd_t0_xchg.lc + 5)
			return SCS_ERR_OVERRUN;
		ifd_t0_xchg.le = 0;
	}
	/* initialize T0 transfer attributes */
	ifd_t0_xchg.nc = ifd_t0_xchg.lc + 5;
	ifd_t0_xchg.ne = ifd_t0_xchg.le + 2;
	return SCS_ERR_SUCCESS;
}
#endif

#if defined(CONFIG_IFD_T0_6C) || defined(CONFIG_IFD_T0_APDU)
#define ifd_t0_xfr_set_hook()	(ifd_t0_xchg.6x_hooked = true)
#define ifd_t0_xfr_unset_hook()	(ifd_t0_xchg.6x_hooked = false)
#else
#define ifd_t0_xfr_set_hook()
#define ifd_t0_xfr_unset_hook()
#endif

#ifdef CONFIG_IFD_T0_6C
#define ifd_t0_6c_process() ifd_t0_xfr_set_hook()
static void ifd_t0_6c_expire(void)
{
	/* rebuild command header with new Ne */
	uint8_t nr = ifd_xchg_read(1);
	ifd_xchg_write(0, ifd_t0_xchg.cla);
	ifd_xchg_write(1, ifd_t0_xchg.ins);
	ifd_xchg_write(4, nr);

	__ifd_t0_xchg_init(5, nr);
}
#else
#define ifd_t0_6c_process()
#define ifd_t0_6c_expire()
#endif

#ifdef CONFIG_IFD_T0_APDU
static void ifd_t0_61_process(void)
{
	/* ISO7816-4 says:
	 * the absence of Le field is the standard way 
	 * for receiving no response data field.
	 */
	if (ifd_t0_xchg.le)
		ifd_t0_xfr_set_hook();
}

static void ifd_t0_61_expire(void)
{
	uint8_t nr = ifd_xchg_read(1);

	ifd_xchg_write(0, ifd_t0_conf.cla_resp);
	ifd_xchg_write(1, IFD_INS_GET_RESPONSE);
	ifd_xchg_write(2, 0x00);
	ifd_xchg_write(3, 0x00);
	ifd_xchg_write(4, nr);

	__ifd_t0_xchg_init(5, nr);
}
#else
#define ifd_t0_61_process()
#define ifd_t0_61_expire()
#endif

static scs_err_t __ifd_t0_xchg_init(scs_size_t nc, scs_size_t ne)
{
	scs_err_t err;
	
	err = ifd_t0_xfr_parse(nc);
	if (err != SCS_ERR_SUCCESS) return err;
	ifd_t0_xchg.tx = ifd_t0_xchg.rx = 0;

	return SCS_ERR_PROGRESS;
}

scs_err_t ifd_t0_xchg_init(scs_size_t nc, scs_size_t ne)
{
	scs_err_t err = __ifd_t0_xchg_init(nc, ne);
	
	if (err != SCS_ERR_PROGRESS)
		return err;
	ifd_t0_xfr_unset_hook();

	return SCS_ERR_PROGRESS;
}

#define SW1_6X(byte)	(((byte) & 0xF0) == 0x60)
#define SW1_9X(byte)	(((byte) & 0xF0) == 0x90)

static void __ifd_t0_ne_expire_proc(void)
{
	uint8_t sw1;
	boolean sending;

	sw1 = ifd_xfr_read_last();
	if (SW1_6X(sw1) || SW1_9X(sw1)) {
		if (sw1 == 0x60) {
			ifd_t0_xchg.rx--;
			ifd_t0_xfr_block(0, 1);
			return;
		}
		ifd_t0_xfr_block(0, 1);
		if (sw1 == 0x61)
			ifd_t0_61_process();
		if (sw1 == 0x6C)
			ifd_t0_6c_process();
		ifd_t0_xchg.cmpl = true;
		return;
	}
	ifd_t0_xchg.rx--;
	sending = ifd_t0_xchg.tx < ifd_t0_xchg.nc;
	if (sw1 ^ ifd_t0_xchg.ins) {
		if (sending)
			ifd_t0_xfr_block(1, 1);
		else
			ifd_t0_xfr_block(0, 2);
	} else {
		if (sending)
			ifd_t0_xfr_block(ifd_t0_xchg.nc-ifd_t0_xchg.tx, 1);
		else
			ifd_t0_xfr_block(0, (ifd_t0_xchg.ne-1)-ifd_t0_xchg.rx);
	}
}

static void __ifd_t0_ne_expire_cmpl(void)
{
	ifd_xchg_end(ifd_t0_xchg.rx);
	ifd_xchg_stop(IFD_XCHG_EVENT_XFR_CMPL);
}

#if defined(CONFIG_IFD_T0_6C) || defined(CONFIG_IFD_T0_APDU)
static void __ifd_t0_6x_expire(void)
{
	if (!ifd_t0_xchg.6x_hooked)
		return;

	if (ifd_xchg_read(0) == 0x61)
		ifd_t0_61_expire();
	if (ifd_xchg_read(0) == 0x6C)
		ifd_t0_6c_expire();
	return;
}
#else
#define __ifd_t0_6x_expire()
#endif

static void ifd_t0_ne_expire(void)
{
	if (!ifd_t0_xchg.cmpl) {
		__ifd_t0_ne_expire_proc();
	} else {
		__ifd_t0_6x_expire();
		__ifd_t0_ne_expire_cmpl();
	}
}

void ifd_t0_xchg_aval(void)
{
	ifd_xchg_begin(ifd_t0_xchg.tx, 5,
		       ifd_t0_xchg.rx, 1, ifd_t0_xchg.nc);
	ifd_t0_xchg.tx += 5;
	ifd_t0_xchg.rx += 1;
	ifd_t0_xchg.cmpl = false;
	ifd_xchg_run();
}

void ifd_t0_xchg_cmpl(void)
{
#if defined(CONFIG_IFD_T0_6C) || defined(CONFIG_IFD_T0_APDU)
	if (ifd_t0_xchg.6x_hooked) {
		ifd_t0_xfr_unset_hook();
		/* prepare to send the new header. */
		ifd_xchg_set_error(SCS_ERR_PROGRESS);
	}
#endif
	/* T=0 does not handle transfers in task context */
}

ifd_proto_t ifd_t0 = {
	SCS_PROTO_T0,
	0,
	ifd_t0_atr_parse,
	ifd_t0_xchg_init,
	ifd_t0_xchg_aval,
	ifd_t0_xchg_cmpl,
	ifd_t0_ne_expire,
	ifd_t0_reset_param,
	ifd_t0_conf_param,
	ifd_t0_nego_pps,
};

void ifd_t0_init(void)
{
	ifd_register_protocol(&ifd_t0);
}
