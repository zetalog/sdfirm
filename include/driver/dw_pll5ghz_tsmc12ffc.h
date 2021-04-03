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

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC
/* To use this driver, followings need to be defined:
 * 1. DW_PLL5GHZ_REFCLK_FREQ;
 * 2. PLL_CFG0/PLL_CFG1/PLL_CFG2/PLL_STATUS register addresses;
 * 3. __dw_pll_read/__dw_pll_write to access PLL registers.
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

#define PLL_STATE_MASK		(PLL_STANDBY | PLL_PWRON | PLL_GEAR_SHIFT)
#define PLL_RESET_MASK		(PLL_RESET | PLL_TEST_RESET)

/* PLL_CFG2 */
#define PLL_LOCK_TIME_OFFSET	0
#define PLL_LOCK_TIME_MASK	REG_16BIT_MASK
#define PLL_LOCK_TIME(value)	_SET_FV(PLL_LOCK_TIME, value)

/* PLL_STATUS */
#define PLL_LOCKED		_BV(0)
#define PLL_STANDBYEFF		_BV(1)
/* XXX: DPU CFG and STATUS bits:
 *
 * DPU PLL2 actually contains 2 PLLs, each PLL is used for 1 DDR
 * subsystem. Since DDR configuration is synchronized, there is only 1 CFG
 * register set for both PLLs, but their STATUS register bits should be
 * shadowed to allow software to query individual status of the 2 PLLs:
 *  BIT 0-1: used for PLL2 of DDR0
 *  BIT 2-3: used for PLL2 of DDR1
 * Thus we should always use masked STATUS register value to be safe for
 * such cases.
 */
#define PLL_OPMODE_OFFSET	0
#define PLL_OPMODE_MASK		REG_2BIT_MASK
#define PLL_OPMODE(value)	_GET_FV(PLL_OPMODE, (value))

/* PLL ranges */
#define PLL_RANGE1	(PLL_VCO_MODE | PLL_LOWFREQ)
#define PLL_RANGE2	(PLL_VCO_MODE)
#define PLL_RANGE3	(PLL_LOWFREQ)
#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_RANGE2
#define PLL_RANGE23	PLL_RANGE2
#else
#define PLL_RANGE23	PLL_RANGE3
#endif

/* PLL control registers */
#define PLL_LOOPCNT1		0
#define PLL_LOOPCNT2		1
#define PLL_CTM1		2
#define PLL_CTM2		3
#define PLL_ANAREG05		5
#define PLL_ANAREG06		6
#define PLL_ANAREG07		7
#define PLL_ANAREG10		10
#define PLL_ANAREG11		11
#define PLL_LOCK_CONTROL	16
#define PLL_LOCK_WINDOW		20
#define PLL_UNLOCK_WINDOW	21
#define PLL_LOCK_COUNTS		22
#define PLL_UNLOCK_COUNTS	23
#define PLL_SCFRAC_CNT		24

/* LOOPCNT1 */
#define PLL_INT_CNTRL_OFFSET		3
#define PLL_INT_CNTRL_MASK		REG_5BIT_MASK
#define PLL_INT_CNTRL(value)		_SET_FV(PLL_INT_CNTRL, value)
/* LOOPCNT2 */
#define PLL_PROP_CNTRL_OFFSET		4
#define PLL_PROP_CNTRL_MASK		REG_4BIT_MASK
#define PLL_PROP_CNTRL(value)		_SET_FV(PLL_PROP_CNTRL, value)
/* CTM1 */
#define PLL_CP_CNTRL_PROP		_BV(5)
/* CTM2 */
#define PLL_CP_INT_HALF			_BV(0)
/* ANAREG5 */
#define PLL_TEST_BYPASS			_BV(0)
/* ANAREG6 */
#define PLL_BYPASS_SYNC_R		_BV(3)
#define PLL_BYPASS_SYNC_P		_BV(4)
/* ANAREG7 */
#define PLL_PRSTDUR_OFFSET		5
#define PLL_PRSTDUR_MASK		REG_3BIT_MASK
#define PLL_PRSTDUR(value)		_SET_FV(PLL_PRSTDUR, value)
/* ANAREG10 */
#define PLL_GS_INT_CNTRL_OFFSET		3
#define PLL_GS_INT_CNTRL_MASK		REG_3BIT_MASK
#define PLL_GS_INT_CNTRL(value)		_SET_FV(PLL_GS_INT_CNTRL, value)
/* ANAREG11 */
#define PLL_GS_PROP_CNTRL_OFFSET	4
#define PLL_GS_PROP_CNTRL_MASK		REG_4BIT_MASK
#define PLL_GS_PROP_CNTRL(value)	_SET_FV(PLL_GS_PROP_CNTRL, value)
/* LOCK_CONTROL */
#define PLL_LOCKCTR_OFFSET		0
#define PLL_LOCKCTR_MASK		REG_3BIT_MASK
#define PLL_LOCKCTR(value)		_SET_FV(PLL_LOCKCTR, value)
#define PLL_OVR_LOCK			_BV(7)
/* SCFRAC_CNT */
#define PLL_FRAC_EN			_BV(0)

/* PLL bypass modes */
#define PLL_BYPASS_NONE			0
#define PLL_BYPASS_CORE			1
#define PLL_BYPASS_ALL			2

/* configure bypass mode */
void dw_pll5ghz_tsmc12ffc_bypass(uint8_t pll, uint8_t mode);
/* power on sequences */
void dw_pll5ghz_tsmc12ffc_pwron(uint8_t pll, uint64_t fvco);
void dw_pll5ghz_tsmc12ffc_pwron2(uint8_t pll, uint64_t fvco,
				 uint64_t fpclk, uint64_t frclk);
void dw_pll5ghz_tsmc12ffc_pwrup(uint8_t pll);
/* power down sequence */
void dw_pll5ghz_tsmc12ffc_pwrdn(uint8_t pll);
/* P/R output enable */
void dw_pll5ghz_tsmc12ffc_enable(uint8_t pll, uint64_t fvco,
				 uint64_t freq, bool r);
/* P/R output disable */
void dw_pll5ghz_tsmc12ffc_disable(uint8_t pll, bool r);
/* enter standby sequence */
void dw_pll5ghz_tsmc12ffc_standby(uint8_t pll);
/* leave standby sequence */
void dw_pll5ghz_tsmc12ffc_relock(uint8_t pll);

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_BYPASS_SYNC
void dw_pll5ghz_tsmc12ffc_bypass_sync(uint8_t pll, bool r, bool bypass);
#else
#define dw_pll5ghz_tsmc12ffc_bypass_sync(pll, r, bypass)	\
	do { } while (0)
#endif
#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_BYPASS_TEST
void dw_pll5ghz_tsmc12ffc_bypass_test(uint8_t pll, bool bypass);
#else
#define dw_pll5ghz_tsmc12ffc_bypass_test(pll, bypass)		\
	do { } while (0)
#endif
#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_PRSTDUR
void dw_pll5ghz_tsmc12ffc_prstdur(uint8_t pll);
#else
#define dw_pll5ghz_tsmc12ffc_prstdur(pll)			\
	do { } while (0)
#endif

#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_REG_ACCESS
void dw_pll_write(uint8_t pll, uint8_t reg, uint8_t val);
uint8_t dw_pll_read(uint8_t pll, uint8_t reg);
#else
#define dw_pll_write(pll, reg, val)	do { } while (0)
#define dw_pll_read(pll, reg)		0
#endif
#else
static __inline void dw_pll5ghz_tsmc12ffc_bypass(uint8_t pll, uint8_t mode)
{
}
static __inline void dw_pll5ghz_tsmc12ffc_pwron(uint8_t pll, uint64_t fvco)
{
}
static __inline void dw_pll5ghz_tsmc12ffc_pwron2(uint8_t pll, uint64_t fvco,
						 uint64_t fpclk,
						 uint64_t frclk)
{
}
static __inline void dw_pll5ghz_tsmc12ffc_pwrup(uint8_t pll)
{
}
static __inline void dw_pll5ghz_tsmc12ffc_pwrdn(uint8_t pll)
{
}
static __inline void dw_pll5ghz_tsmc12ffc_enable(uint8_t pll, uint64_t fvco,
						 uint64_t freq, bool r)
{
}
static __inline void dw_pll5ghz_tsmc12ffc_disable(uint8_t pll, bool r)
{
}
static __inline void dw_pll5ghz_tsmc12ffc_standby(uint8_t pll)
{
}
static __inline void dw_pll5ghz_tsmc12ffc_relock(uint8_t pll)
{
}
#endif

#endif /* __DW_PLL5GHZ_TSMC12FFC_H_INCLUDE__ */
