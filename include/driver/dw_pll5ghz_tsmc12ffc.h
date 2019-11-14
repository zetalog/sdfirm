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
 * @(#)dw_pll5ghz_tsmc12ffc.h: DWC PLL5GHz TSMC12FFC definitions
 * $Id: dw_pll5ghz_tsmc12ffc.h,v 1.1 2019-11-14 09:07:00 zhenglv Exp $
 */

#ifndef __DW_PLL5GHZ_TSMC12FFC_H_INCLUDE__
#define __DW_PLL5GHZ_TSMC12FFC_H_INCLUDE__

#include <target/arch.h>
#include <target/generic.h>

/* To use this driver, followings need to be defined:
 * 1. DW_PLL5GHZ_REFCLK_FREQ;
 * 2. PLL_CFG0/PLL_CFG1/PLL/CFG2/PLL_STATUS register addresses.
 */

/* PLL_CFG0 */
#define PLL_MFRAC_OFFSET	0
#define PLL_MFRAC_MASK		REG_16BIT_MASK
#define PLL_MFRAC(value)	_SET_FV(PLL_MFRAC, value)
#define PLL_MINT_OFFSET		16
#define PLL_MINT_MASK		REG_10BIT_MASK
#define PLL_MINT(value)		_SET_FV(PLL_MINT, value)
#define PLL_PREDIV_OFFSET	26
#define PLL_PREDIV_MASK		REG_5BIT_MASK
#define PLL_PREDIV(value)	_SET_FV(PLL_PREDIV, value)

/* PLL_CFG1 */
#define PLL_DIVVCOR_OFFSET	0
#define PLL_DIVVCOR_MASK	REG_4BIT_MASK
#define PLL_DIVVCOR(value)	_SET_FV(PLL_DIVVCOR, value)
#define PLL_DIVVCOP_OFFSET	4
#define PLL_DIVVCOP_MASK	REG_4BIT_MASK
#define PLL_DIVVCOP(value)	_SET_FV(PLL_DIVVCOP, value)
#define PLL_R_OFFSET		8
#define PLL_R_MASK		REG_6BIT_MASK
#define PLL_R(value)		_SET_FV(PLL_R, value)
#define PLL_P_OFFSET		14
#define PLL_P_MASK		REG_6BIT_MASK
#define PLL_P(value)		_SET_FV(PLL_P, value)
#define PLL_VCO_MODE		_BV(20)
#define PLL_STANDBY		_BV(21)
#define PLL_BYPASS		_BV(22)
#define PLL_ENR			_BV(23)
#define PLL_ENP			_BV(24)
#define PLL_LOWFREQ		_BV(25)
#define PLL_GEAR_SHIFT		_BV(26)
#define PLL_RESET		_BV(27)
#define PLL_PWRON		_BV(28)
#define PLL_TEST_RESET		_BV(29)
#if 0 /* obtained from RTL */
#define PLL_TEST_BYPASS		_BV(30)
#endif

/* PLL_CFG2 */
#define PLL_LOCK_TIME_OFFSET	0
#define PLL_LOCK_TIME_MASK	REG_16BIT_MASK
#define PLL_LOCK_TIME(value)	_SET_FV(PLL_LOCK_TIME, value)

/* PLL_STATUS */
#define PLL_LOCKED		_BV(0)
#define PLL_STANDBYEFF		_BV(1)
#define PLL_CNT_LOCKED		_BV(2)

/* PLL ranges */
#define PLL_RANGE1	(PLL_VCO_MODE | PLL_LOWFREQ)
#define PLL_RANGE2	(PLL_VCO_MODE)
#define PLL_RANGE3	(PLL_LOWFREQ)
#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_RANGE2
#define PLL_RANGE23	PLL_RANGE2
#else
#define PLL_RANGE23	PLL_RANGE3
#endif
#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_GEAR
#define PLL_STARTUP	(PLL_PWRON | PLL_GEAR_SHIFT)
#else
#define PLL_STARTUP	PLL_PWRON
#endif

void dw_pll5ghz_tsmc12ffc_pwron(uint8_t pll, uint64_t fvco);
void dw_pll5ghz_tsmc12ffc_pwrdn(uint8_t pll);
void dw_pll5ghz_tsmc12ffc_standby(uint8_t pll);
void dw_pll5ghz_tsmc12ffc_relock(uint8_t pll);

#endif /* __DW_PLL5GHZ_TSMC12FFC_H_INCLUDE__ */
