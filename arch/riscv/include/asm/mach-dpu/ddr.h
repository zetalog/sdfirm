/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)ddr.h: DPU specific DDR definitions
 * $Id: ddr.h,v 1.1 2020-03-26 08:55:00 zhenglv Exp $
 */

#ifndef __DDR_DPU_H_INCLUDE__
#define __DDR_DPU_H_INCLUDE__

#include <target/arch.h>
#include <target/clk.h>

#ifdef CONFIG_DPU_DDR
#ifndef ARCH_HAVE_DDR
#define ARCH_HAVE_DDR		1
#else
#error "Multiple DDR controller defined"
#endif
#endif

#define DW_UMCTL2_BASE(n)	(DDR0_CTRL_BASE + ((n) << 15))

#ifdef CONFIG_DPU_DDR_INTLV
#define DDR_HW_MAX_CHANNELS	2
#define DDR_HW_MAX_MODULES	2
#else /* CONFIG_DPU_DDR_INTLV */
#ifdef CONFIG_DPU_DDR_BANK0
#ifdef CONFIG_DPU_DDR_BANK1
#define DDR_HW_MAX_CHANNELS	2
#define DDR_HW_MAX_MODULES	2
#else /* CONFIG_DPU_DDR_BANK1 */
#define DDR_HW_MAX_CHANNELS	1
#define DDR_HW_MAX_MODULES	1
#endif /* CONFIG_DPU_DDR_BANK1 */
#else /* CONFIG_DPU_DDR_BANK0 */
#define DDR_HW_MAX_CHANNELS	1
#define DDR_HW_MAX_MODULES	1
#endif /* CONFIG_DPU_DDR_BANK0 */
#endif /* CONFIG_DPU_DDR_INTLV */

#include <driver/dw_umctl2.h>

#define NR_DDR_SPEEDS		(DDR4_3200 + 1)
#define ddr_por			ddr0_pwr_ok_in
#define ddr_prst		ddr0_prst_n
#define ddr_arst		ddr0_arst_n
#define ddr_rst			ddr0_rst_n

#ifdef CONFIG_DPU_DDR
clk_freq_t ddr_get_fvco(int speed);
clk_freq_t ddr_get_fpclk(int speed);
clk_freq_t ddr_get_frclk(int speed);

/* DDR frequency plans */
#define ddr_hw_ctrl_init()	dw_umctl2_init()
#define ddr_hw_ctrl_start()	dw_umctl2_start()

#define ddr_hw_mr_write(n, c, ranks, v, r)	\
	dw_umctl2_mr_write(n, c, ranks, v, r)
#define ddr_hw_mr_read(n, c, ranks, r)		\
	dw_umctl2_mr_read(n, c, ranks, r)
#define ddr4_hw_config_refresh(n, mode, tREFI, tRFCmin)	\
	dw_umctl2_ddr4_config_refresh(n, mode, tREFI, tRFCmin)
#define ddr_hw_init_MR(n, v, r)		dw_umctl2_init_mr(n, v, r)
#define ddr4_hw_init_RESET_n(n, tPW_RESET)		\
	dw_umctl2_ddr4_init_rstn(n, tPW_RESET)
#define ddr4_hw_init_CKE(n, tCKEactive, tCKEstab)	\
	dw_umctl2_ddr4_init_pre_cke(n, tCKEactive, tCKEstab)
#define ddr4_hw_init_tXPR(n, tXS)			\
	dw_umctl2_ddr4_init_post_cke(n, tXS)
#define ddr4_hw_init_ZQCL(n, tZQinit)			\
	dw_umctl2_ddr4_init_zqcl(n, tZQinit)
#define ddr_hw_mr_write(n, c, ranks, v, r)		\
	dw_umctl2_mr_write(n, c, ranks, v, r)
#define ddr_hw_mr_read(n, c, ranks, r)			\
	dw_umctl2_mr_read(n, c, ranks, r)

void ddr_hw_config_speed(uint8_t speed);
void ddr_hw_enable_speed(uint8_t speed);
void ddr_hw_wait_dfi(uint32_t cycles);
void ddr_hw_chan_select(ddr_cid_t cid);
void ddr_hw_slot_select(ddr_sid_t sid);
void ddr_hw_slot_reset(void);
void ddr_hw_chan_reset(void);
#else
#define ddr_get_fvco(speed)	PLL2_VCO_FREQ
#define ddr_get_fpclk(speed)	PLL2_P_FREQ
#define ddr_get_frclk(speed)	PLL2_R_FREQ
#endif

#endif /* __DDR_DPU_H_INCLUDE__ */
