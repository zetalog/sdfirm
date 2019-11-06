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
 * @(#)clk.h: DUOWEN clock controller definitions
 * $Id: clk.h,v 1.1 2019-11-06 10:36:00 zhenglv Exp $
 */

#ifndef __CLK_DUOWEN_H_INCLUDE__
#define __CLK_DUOWEN_H_INCLUDE__

#include <target/arch.h>
#include <target/generic.h>

/* XXX: This implementation is based on undocumented PLL RTL
 *      sequence, and may subject to change.
 */

#define CRCNTL_BASE			UL(0x60000000)
#define CRCNTL_REG(offset)		(CRCNTL_BASE + (offset))

#define CRCNTL_PWRUP			CRCNTL_REG(0x0C)
#define CRCNTL_CLKGRP0			CRCNTL_REG(0x80)
#define CRCNTL_CLKGRP1			CRCNTL_REG(0x84)
#define CRCNTL_CLKGRP2			CRCNTL_REG(0x88)
#define CRCNTL_CLKGRP3			CRCNTL_REG(0x8C)
#define CRCNTL_CLKGRP4			CRCNTL_REG(0x90)
#define CRCNTL_PLLSEL			CRCNTL_REG(0x94)

#define CRCNTL_PLL_REG(pll, offset)	CRCNTL_REG(((pll)<<4)+0x10+(offset))
#define CRCNTL_PLLDIV(pll)		CRCNTL_PLL_REG(pll, 0x00)
#define CRCNTL_PLLCFG1(pll)		CRCNTL_PLL_REG(pll, 0x04)
#define CRCNTL_PLLSTS(pll)		CRCNTL_PLL_REG(pll, 0x0C)

#define CRCNTL_DDRPLL		0
#define CRCNTL_SOCPLL		2

/* PWRUP */
#define PWRUP_SFAB		_BV(0)

/* PLL */
#define PLL_MFRAC_OFFSET	0
#define PLL_MFRAC_MASK		REG_16BIT_MASK
#define PLL_MFRAC(value)	_SET_FV(PLL_MFRAC, value)
#define PLL_MINT_OFFSET		16
#define PLL_MINT_MASK		REG_10BIT_MASK
#define PLL_MINT(value)		_SET_FV(PLL_MINT, value)
#define PLL_PREDIV_OFFSET	26
#define PLL_PREDIV_MASK		REG_5BIT_MASK
#define PLL_PREDIV(value)	_SET_FV(PLL_PREDIV, value)

/* PLLCFG1 */
#define PLL_DIVVCOR_OFFSET	0
#define PLL_DIVVCOR_MASK	REG_4BIT_MASK
#define PLL_DIVVCOR(value)	_SET_FV(PLL_DIVVCOR, value)
#define PLL_DIVVCOP_OFFSET	4
#define PLL_DIVVCOP_MASK	REG_4BIT_MASK
#define PLL_DIVVCOP(value)	_SET_FV(PLL_DIVVCOP, value)
#define PLL_K_OFFSET		8
#define PLL_K_MASK		REG_6BIT_MASK
#define PLL_K(value)		_SET_FV(PLL_K, value)
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
#define PLL_TEST_BYPASS		_BV(30)

/* PLLSTS */
#define PLL_LOCK		_BV(0)

/* TODO: Wait imc_rst_n, wait PS_HOLD */
#define sfab_power_up()		__raw_writel(PWRUP_SFAB, CRCNTL_PWRUP)
#define ddrpll_enable()		pll_enable(CRCNTL_DDRPLL, 16)
#define socpll_enable()		pll_enable(CRCNTL_SOCPLL, 64)
#define pll_select()		__raw_writel(0x0, CRCNTL_PLLSEL)
#define enable_all_clocks()					\
	do {							\
		__raw_writel(0xFFFFFFFF, CRCNTL_CLKGRP0);	\
		__raw_writel(0xFFFFFFFF, CRCNTL_CLKGRP1);	\
		__raw_writel(0xFFFFFFFF, CRCNTL_CLKGRP2);	\
		__raw_writel(0xFFFFFFFF, CRCNTL_CLKGRP3);	\
		__raw_writel(0xFFFFFFFF, CRCNTL_CLKGRP4);	\
	} while (0)

void pll_enable(uint8_t pll, uint8_t mint);
void clk_hw_ctrl_init(void);

#endif /* __CLK_DUOWEN_H_INCLUDE__ */
