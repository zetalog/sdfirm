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
 * @(#)iso7816_t0.h: ISO/IEC 7816-3 T=0 protocol interfaces
 * $Id: ifd_t0.h,v 1.14 2010-11-05 03:58:54 zhenglv Exp $
 */

#ifndef __ISO7816_T0_H_INCLUDE__
#define __ISO7816_T0_H_INCLUDE__

struct ifd_conf_t0 {
#define IFD_T0_GI_IMPLICIT	22
#define IFD_T0_WI_IMPLICIT	10
	uint8_t wi;

#ifdef CONFIG_IFD_T0_APDU
	uint8_t cla_resp;
	uint8_t cla_env;
#endif
};

struct ifd_xchg_t0 {
	boolean cmpl;
#ifdef CONFIG_IFD_T0_6C
	uint8_t cla;
#endif
#if defined(CONFIG_IFD_T0_6C) || defined(CONFIG_IFD_T0_APDU)
	boolean 6x_hooked;
#endif

	uint8_t ins;
	scs_size_t lc;
	scs_size_t le;

	scs_size_t nc;
	scs_size_t ne;
	scs_off_t tx;
	scs_off_t rx;
};

#define IFD_T0_PARAM_FD		0x00
#define IFD_T0_PARAM_TCCKS	0x01
#define IFD_T0_PARAM_GT		0x02
#define IFD_T0_PARAM_WI		0x03
#define IFD_T0_PARAM_CLOCKSTOP	0x04

#define IFD_T0_APDU_GET_RESPONSE	0x01
#define IFD_T0_APDU_ENVELOPE		0x02

#ifdef CONFIG_IFD_T0
scs_err_t ifd_set_t0_apdu(uint8_t mask,
			  uint8_t cla_resp, uint8_t cla_env);
scs_err_t ifd_set_t0_param(uint8_t fd, uint8_t gt,
			   uint8_t wi, boolean clk_stop);
uint8_t ifd_get_t0_param(uint8_t what);
void ifd_t0_init(void);
#else
#define ifd_set_t0_param(fd, gt, wt, clock_stop)	\
	SCS_ERR_UNSUPPORT
#define ifd_set_t0_apdu(maks, cla_resp, cla_env)	\
	SCS_ERR_UNSUPPORT
#define ifd_get_t0_param(what)				0
#define ifd_t0_init()
#endif

#endif /* __ISO7816_T0_H_INCLUDE__ */
