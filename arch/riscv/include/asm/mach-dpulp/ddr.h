/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)ddr.h: DPU-LP specific DDR definitions
 * $Id: ddr.h,v 1.1 2021-11-01 15:25:00 zhenglv Exp $
 */

#ifndef __DDR_DPULP_H_INCLUDE__
#define __DDR_DPULP_H_INCLUDE__

#include <target/arch.h>
#include <target/clk.h>

#ifdef CONFIG_DPULP_DDR
#ifndef ARCH_HAVE_DDR
#define ARCH_HAVE_DDR		1
#else
#error "Multiple DDR controller defined"
#endif
#endif

#include <driver/dw_umctl2.h>

#define NR_DDR_SPEEDS		(DDR4_3200 + 1)
#define ddr_por			xo_clk
#define ddr_prst		xo_clk
#define ddr_arst		xo_clk
#define ddr_rst			xo_clk

#ifdef CONFIG_DPULP_DDR
clk_freq_t ddr_get_fvco(int speed);
clk_freq_t ddr_get_fpclk(int speed);
clk_freq_t ddr_get_frclk(int speed);

/* DDR frequency plans */
#define ddr_hw_ctrl_init()	dw_umctl2_init()
#define ddr_hw_ctrl_start()	dw_umctl2_start()
void ddr_hw_config_speed(uint8_t speed);
void ddr_hw_enable_speed(uint8_t speed);
void ddr_hw_wait_dfi(uint32_t cycles);
#else
#define ddr_get_fvco(speed)	PLL2_VCO_FREQ
#define ddr_get_fpclk(speed)	PLL2_P_FREQ
#define ddr_get_frclk(speed)	PLL2_R_FREQ
#endif

#endif /* __DDR_DPULP_H_INCLUDE__ */
