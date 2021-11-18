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
 * @(#)pll.h: DPU-LP PLL controller definitions
 * $Id: pll.h,v 1.1 2021-11-15 14:31:00 zhenglv Exp $
 */

#ifndef __PLL_DPULP_H_INCLUDE__
#define __PLL_DPULP_H_INCLUDE__

#define CRU_PLL_REG(pll, offset)	\
	CRU_REG(0x100 + ((pll) << 6) + (offset))
#define ETH_PLL_REG(pll, offset)	\
	(ETH0_BASE + ((pll) << 22) + (offset))
#define DPU_PLL_REG(pll, offset)	\
	((pll > NR_CRU_PLLS) ?		\
	 CRU_PLL_REG((pll), offset) :	\
	 ETH_PLL_REG((pll) - NR_CRU_PLLS, offset))

#define NR_CRU_PLLS			9
#define NR_ETH_PLLS			2
#define NR_PLLS				(NR_CRU_PLLS + NR_ETH_PLLS)

/* DW PLL5GHz TSMC12FFC registers */
#define DW_PLL_F_REFCLK(pll)		XO_CLK_FREQ
#define DW_PLL_CFG0(pll)		DPU_PLL_REG(pll, 0x00)
#define DW_PLL_CFG1(pll)		DPU_PLL_REG(pll, 0x04)
#define DW_PLL_CFG2(pll)		DPU_PLL_REG(pll, 0x08)
#define DW_PLL_STATUS(pll)		DPU_PLL_REG(pll, 0x0C)
/* PLL register access */
#define PLL_REG_ACCESS(pll)		DPU_PLL_REG(pll, 0x10)
#define PLL_REG_TIMING(pll)		DPU_PLL_REG(pll, 0x14)
#define PLL_HW_CFG(pll)			DPU_PLL_REG(pll, 0x18)
#define PLL_REFCLK_FREQ(pll)		DPU_PLL_REG(pll, 0x1C)
#define PLL_CLK_P_FREQ(pll)		DPU_PLL_REG(pll, 0x20)
#define PLL_CLK_R_FREQ(pll)		DPU_PLL_REG(pll, 0x24)
#define PLL_CNT(pll)			DPU_PLL_REG(pll, 0x28)

#define dpulp_pll_enable(pll, freq)			\
	dw_pll5ghz_tsmc12ffc_pwron(pll, freq)
#define dpulp_pll_enable2(pll, fvco, fpclk, frclk)	\
	dw_pll5ghz_tsmc12ffc_pwron2(pll, fvco, fpclk, frclk)
#define dpulp_pll_disable(pll)				\
	dw_pll5ghz_tsmc12ffc_pwrdn(pll)

#define dpulp_div_enable(pll, fvco, freq, r)		\
	dw_pll5ghz_tsmc12ffc_enable(pll, fvco, freq, r)
#define dpulp_div_disable(pll, r)			\
	dw_pll5ghz_tsmc12ffc_disable(pll, r)

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

#define dpulp_pll_reg_set_timing(pll, setup, hold, read, write)	\
	__raw_writel(PLL_TSETUP(setup) | PLL_THOLD(hold) |	\
		     PLL_TREAD(read) | PLL_TWRITE(write),	\
		     PLL_REG_TIMING(pll));

#define __dw_pll_read(pll, reg)					\
	dpulp_pll_reg_read(pll, reg)
#define __dw_pll_write(pll, reg, val)				\
	dpulp_pll_reg_write(pll, reg, val)
#ifdef CONFIG_DW_PLL5GHZ_TSMC12FFC_ACCEL
#define __dw_pll_wait_cmpclk(pll, cycles)	wmb()
#else
/* 40 loops of cmpclk cycles should ensure at least 1us */
#define __dw_pll_wait_cmpclk(pll, cycles)	udelay(((cycles) + 39) / 40)
#endif
#include <driver/dw_pll5ghz_tsmc12ffc.h>

#ifndef __ASSEMBLY__
void dpulp_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val);
uint8_t dpulp_pll_reg_read(uint8_t pll, uint8_t reg);
#endif

#endif /* __PLL_DPULP_H_INCLUDE__ */
