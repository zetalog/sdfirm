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
 * @(#)ifd_priv.h: ISO/IEC 7816-3 interface device internal interfaces
 * $Id: ifd_priv.h,v 1.31 2011-11-11 09:01:39 zhenglv Exp $
 */

#ifndef __IFD_PRIV_H_INCLUDE__
#define __IFD_PRIV_H_INCLUDE__

#include <target/ifd.h>

#define IFD_PPS_PPSS		0xFF
#define IFD_PPS_EXISTS(n)	(1<<(4+n))
#define IFD_PPS_PROTO(pps0)	(pps0 & 0x0F)

/* for software configuration */
void ifd_config_fd(void);
void ifd_config_t(void);
void ifd_apply_n(uint8_t n);
scs_err_t ifd_valid_fd(uint8_t fd);

/* for hardware configuration */
void ifd_config_cls(void);
boolean ifd_config_clk(uint8_t clk_stop);
void ifd_config_conv(uint8_t conv);
void ifd_config_crep(uint8_t on);
void ifd_delay_gt(uint16_t gt, boolean blk);
void ifd_delay_wt(uint32_t wt, boolean blk);

void ifd_apply_cgt(uint16_t cgt);
void ifd_apply_cwt(uint32_t cwt);
void ifd_apply_bgt(uint16_t bgt);
void ifd_apply_bwt(uint32_t bwt);

void ifd_config_def(void);
void ifd_conf_param(void);

uint8_t ifd_get_conv(void);

scs_err_t ifd_slot_exist(void);
scs_err_t ifd_slot_active(void);
void ifd_interpret_ne_null(void);

union ifd_tmp {
	struct ifd_atr_parser ap;
	struct ifd_xchg_atr ax;
	struct ifd_xchg_pps px;
	struct ifd_xchg_t0 t0;
	struct ifd_xchg_t1 t1;
};

#if NR_IFD_SLOTS > 1
extern union ifd_tmp ifd_tmps[NR_IFD_SLOTS];
extern __near__ struct ifd_slot ifd_slots[NR_IFD_SLOTS];
#define ifd_atr_parser ifd_tmps[ifd_slid].ap
#define ifd_atr_xchg ifd_tmps[ifd_slid].ax
#define ifd_pps_xchg ifd_tmps[ifd_slid].px
#define ifd_t0_xchg ifd_tmps[ifd_slid].t0
#define ifd_t1_xchg ifd_tmps[ifd_slid].t1
#define ifd_slot_ctrl ifd_slots[ifd_slid]
#else
extern union ifd_tmp ifd_tmps;
extern __near__ struct ifd_slot ifd_slot_ctrl;
#define ifd_atr_parser ifd_tmps.ap
#define ifd_atr_xchg ifd_tmps.ax
#define ifd_pps_xchg ifd_tmps.px
#define ifd_t0_xchg ifd_tmps.t0
#define ifd_t1_xchg ifd_tmps.t1
#endif

#endif /* __IFD_PRIV_H_INCLUDE__ */
