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
 * @(#)pll.h: DPU PLL clock/reset controller definitions
 * $Id: pll.h,v 1.1 2020-03-08 14:54:00 zhenglv Exp $
 */

#ifndef __PLL_DPU_H_INCLUDE__
#define __PLL_DPU_H_INCLUDE__

#include <target/arch.h>

#ifdef CONFIG_MMU
#define __PLL_REG_BASE		dpu_pll_reg_base
extern caddr_t dpu_pll_reg_base;
#else
#define __PLL_REG_BASE		PLL_REG_BASE
#endif
#define PLL_REG_REG(offset)		(__PLL_REG_BASE + (offset))
#define PLL_REG(pll, offset)		\
	PLL_REG_REG(((pll) << 4) + (offset))

#define NR_PLLS				6

/* DW PLL5GHz TSMC12FFC registers */
#define DW_PLL_F_REFCLK(pll)		XIN_FREQ
#define DW_PLL_CFG0(pll)		PLL_REG(pll, 0x00)
#define DW_PLL_CFG1(pll)		PLL_REG(pll, 0x04)
#define DW_PLL_STATUS(pll)		PLL_REG(pll, 0x08)

#define dpu_pll_enable(pll, freq)		\
	dw_pll5ghz_tsmc12ffc_pwron(pll, freq)
#define dpu_pll_disable(pll)			\
	dw_pll5ghz_tsmc12ffc_pwrdn(pll)

#define dpu_div_enable(pll, fvco, freq, r)	\
	dw_pll5ghz_tsmc12ffc_enable(pll, fvco, freq, r)
#define dpu_div_disable(pll, r)			\
	dw_pll5ghz_tsmc12ffc_disable(pll, r)

/* PLL GMUX registers */
#define PLL_GMUX_CFG(pll)		PLL_REG(pll, 0x0C)

/* PLL_GMUX_CFG */
#define PLL_GMUX_EN_R			_BV(3)
#define PLL_GMUX_SEL_R			_BV(2)
#define PLL_GMUX_EN_P			_BV(1)
#define PLL_GMUX_SEL_P			_BV(0)
#define DPU_GMUX_SEL(r)			((r) ? PLL_GMUX_SEL_R : PLL_GMUX_SEL_P)
#define DPU_GMUX_EN(r)			((r) ? PLL_GMUX_EN_R : PLL_GMUX_EN_P)

#define dpu_gmux_selected(pll, r)	\
	(!(__raw_readl(PLL_GMUX_CFG(pll)) & DPU_GMUX_SEL(r)))
#define dpu_gmux_select(pll, r)		\
	__raw_clearl(DPU_GMUX_SEL(r), PLL_GMUX_CFG(pll))
#define dpu_gmux_deselect(pll, r)	\
	__raw_setl(DPU_GMUX_SEL(r), PLL_GMUX_CFG(pll))
#define dpu_gmux_enabled(pll, r)	\
	(!!(__raw_readl(PLL_GMUX_CFG(pll)) & DPU_GMUX_EN(r)))
#define dpu_gmux_enable(pll, r)		\
	__raw_setl(DPU_GMUX_EN(r), PLL_GMUX_CFG(pll))
#define dpu_gmux_disable(pll, r)	\
	__raw_clearl(DPU_GMUX_EN(r), PLL_GMUX_CFG(pll))

/* Soft reset registers */
#ifdef CONFIG_DPU_GEN2
#define SRST_BASE			PLL_REG(NR_PLLS, 0x14)
#else /* CONFIG_DPU_GEN2 */
#define SRST_BASE			PLL_REG(NR_PLLS, 0)
#endif /* CONFIG_DPU_GEN2 */
#define SRST_REG(offset)		(SRST_BASE + (offset))
#define SRST_1BIT_REG(off, n)		REG_1BIT_ADDR(SRST_BASE+(off), n)

#define PLL_GLOBAL_RST			SRST_REG(0x00)
#define PLL_SOFT_RST(n)			SRST_1BIT_REG(0x00, n)

/* PLL_SOFT_RST */
#define PLL_SRST(n)			_BV(REG_1BIT_OFFSET(n))

/* global reset bits */
#define SRST_SYS			_BV(30)
#define WDT_RST_DIS			_BV(31)

/* SOFT_RST IDs */
#define SRST_GPDPU			0
#define SRST_PCIE0			1
#define SRST_PCIE1			2
#define SRST_SPI			3
#define SRST_I2C0			4
#define SRST_I2C1			5
#define SRST_I2C2			6
#define SRST_UART			7
#define SRST_PLIC			8
#define SRST_GPIO			9
#define SRST_RAM			10
#define SRST_ROM			11
#define SRST_TMR			12
#define SRST_WDT			13
#define SRST_TCSR			14
#define SRST_VPU			15
#define SRST_IMC			16
#define SRST_NOC			17
#define SRST_FLASH			18
#define SRST_DDR0_0			19
#define SRST_DDR0_1			20
#define SRST_DDR1_0			21
#define SRST_DDR1_1			22
#define SRST_DDR0_POR			23
#define SRST_DDR1_POR			24
#define SRST_PCIE0_POR			25
#define SRST_PCIE1_POR			26
#ifdef CONFIG_DPU_GEN2
#define SRST_TSENSOR_XO			27
#define SRST_TSENSOR_METS		28
#endif /* CONFIG_DPU_GEN2 */
#define POR_ARST			32

#ifdef CONFIG_DPU_GEN2
#define NR_PLL_RSTS			(SRST_TSENSOR_METS + 1)
#else /* CONFIG_DPU_GEN2 */
#define NR_PLL_RSTS			(SRST_PCIE1_POR + 1)
#endif /* CONFIG_DPU_GEN2 */

/* PLL register access */
#define PLL_REG_ACCESS(pll)		SRST_REG(0x08 + ((pll) << 2))
#define PLL_REG_TIMING(pll)		SRST_REG(0x0C + ((pll) << 2))

/* PLL_REG_ACCESS */
/* PLL2 specific register bit. DDR0 and DDR1 share same settings, but only
 * when PLL_REG_READ=1, data source can be selected if DDR0 or DDR1
 * register data should be returned.
 */
#define PLL_REG_PLL2_SEL	_BV(26)
#define PLL_REG_INVALID		_BV(25)
#define PLL_REG_IDLE		_BV(24)
#define PLL_REG_RDATA_OFFSET	16
#define PLL_REG_RDATA_MASK	REG_8BIT_MASK
#define PLL_REG_RDATA(value)	_GET_FV(PLL_REG_RDATA, value)
#define PLL_REG_WDATA_OFFSET	8
#define PLL_REG_WDATA_MASK	REG_8BIT_MASK
#define PLL_REG_WDATA(value)	_SET_FV(PLL_REG_WDATA, value)
#define PLL_REG_SEL_OFFSET	2
#define PLL_REG_SEL_MASK	REG_6BIT_MASK
#define PLL_REG_SEL(value)	_SET_FV(PLL_REG_SEL, value)
#define PLL_REG_WRITE		_BV(1)
#define PLL_REG_READ		_BV(0)

/* PLL_REG_TIMING */
#define PLL_TWRITE_OFFSET	12
#define PLL_TWRITE_MASK		REG_4BIT_MASK
#define PLL_TWRITE(value)	_SET_FV(PLL_TWRITE, value)
#define PLL_TREAD_OFFSET	8
#define PLL_TREAD_MASK		REG_4BIT_MASK
#define PLL_TREAD(value)	_SET_FV(PLL_TREAD, value)
#define PLL_THOLD_OFFSET	4
#define PLL_THOLD_MASK		REG_4BIT_MASK
#define PLL_THOLD(value)	_SET_FV(PLL_THOLD, value)
#define PLL_TSETUP_OFFSET	0
#define PLL_TSETUP_MASK		REG_4BIT_MASK
#define PLL_TSETUP(value)	_SET_FV(PLL_TSETUP, value)

#define dpu_pll_reg_set_timing(pll, setup, hold, read, write)	\
	__raw_writel(PLL_TSETUP(setup) | PLL_THOLD(hold) |	\
		     PLL_TREAD(read) | PLL_TWRITE(write),	\
		     PLL_REG_TIMING(pll));
#define dpu_pll_reg_select_ddr0()				\
	__raw_clearl(PLL_REG_PLL2_SEL, PLL_REG_ACCESS(2))
#define dpu_pll_reg_select_ddr1()				\
	__raw_setl(PLL_REG_PLL2_SEL, PLL_REG_ACCESS(2))

#define __dw_pll_read(pll, reg)			dpu_pll_reg_read(pll, reg)
#define __dw_pll_write(pll, reg, val)		dpu_pll_reg_write(pll, reg, val)
#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_ACCEL
#define __dw_pll_wait_cmpclk(pll, cycles)	wmb()
#else
/* 40 loops of cmpclk cycles should ensure at least 1us */
#define __dw_pll_wait_cmpclk(pll, cycles)	udelay(((cycles) + 39) / 40)
#endif
#include <driver/dw_pll5ghz_tsmc12ffc.h>

#ifndef __ASSEMBLY__
void dpu_pll_soft_reset(uint8_t comp);
void dpu_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val);
uint8_t dpu_pll_reg_read(uint8_t pll, uint8_t reg);
#endif
#define dpu_pll_sys_reset()			\
	__raw_clear(SRST_SYS, PLL_GLOBAL_RST)
#define dpu_pll_wdt_reset()			\
	__raw_clear(WDT_RST_DIS, PLL_GLOBAL_RST)

/* Frequency plans */
#ifndef __ASSEMBLY__
/* Internal APIs */
clk_freq_t freqplan_get_fvco(int pll, int plan);
clk_freq_t freqplan_get_fpclk(int pll, int plan);
clk_freq_t freqplan_get_fpclk_nodef(int pll, int plan);
clk_freq_t freqplan_get_frclk(int pll, int plan);
/* External APIs */
clk_freq_t freqplan_get_frequency(clk_t clk, int plan);
#endif /* __ASSEMBLY__ */

#endif /* __PLL_DPU_H_INCLUDE__ */
