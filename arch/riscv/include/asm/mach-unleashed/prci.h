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
 * @(#)prci.h: FU540 (unleashed) power reset clocking interrupt definitions
 * $Id: prci.h,v 1.1 2019-10-16 14:09:00 zhenglv Exp $
 */

#ifndef __PRCI_UNLEASHED_H_INCLUDE__
#define __PRCI_UNLEASHED_H_INCLUDE__

#ifdef CONFIG_MMU
#define __PRCI_BASE		sifive_prci_reg_base
extern caddr_t sifive_prci_reg_base;
#else
#define __PRCI_BASE		PRCI_BASE
#endif
#define PRCI_REG(offset)	(__PRCI_BASE + (offset))

#define PRCI_HFXOSCCFG		PRCI_REG(0x00)
#define PRCI_COREPLL		0
#define PRCI_DDRPLL		1
#define PRCI_GEMGXLPLL		3
#define PRCI_PLLCFG(pll)	PRCI_REG(0x04 + (pll) * 0x08)
#define PRCI_PLLOUT(pll)	PRCI_REG(0x08 + (pll) * 0x08)
#define PRCI_CORECLKSEL		PRCI_REG(0x24)
#define PRCI_DEVICESRESET	PRCI_REG(0x28)
#define PRCI_CLKMUXSTATUS	PRCI_REG(0x2C)
#define PRCI_PROCMONCFG		PRCI_REG(0xF0)

/* HFXOSCCFG */
#define XOSC_RDY		_BV(29)
#define XOSCCFG_EN		_BV(30)

/* COREPLLCFG/DDRPLLCFG/GEMGXLPLLCFG */
#define PLL_DIVR_OFFSET	0
#define PLL_DIVR_MASK		REG_6BIT_MASK
#define PLL_DIVR(value)		_SET_FV(PLL_DIVR, value)
#define PLL_DIVR_DEFAULT	0x1
#define PLL_DIVF_OFFSET		6
#define PLL_DIVF_MASK		REG_9BIT_MASK
#define PLL_DIVF(value)		_SET_FV(PLL_DIVF, value)
#define PLL_DIVF_DEFAULT	0x1F
#define PLL_DIVQ_OFFSET		15
#define PLL_DIVQ_MASK		REG_3BIT_MASK
#define PLL_DIVQ(value)		_SET_FV(PLL_DIVQ, value)
#define PLL_DIVQ_DEFAULT	0x3
#define PLL_RANGE_OFFSET	18
#define PLL_RANGE_MASK		REG_3BIT_MASK
#define PLL_RANGE(value)	_SET_FV(PLL_RANGE, value)
#define PLL_RANGE_DEFAULT	0
#define PLL_RANGE_33MHZ		0x4
#define PLL_BYPASS		_BV(24)
#define PLL_FSE			_BV(25) /* Internal feedback */
#define PLL_LOCK		_BV(31)

/* COREPLLOUT/DDRPLLOUT/GEMGXLPLLOUT */
#if 1
/* In FU540-SDK firmware sample */
#define PLL_DIV_OFFSET		0
#define PLL_DIV_MASK		REG_7BIT_MASK
#define PLL_DIV(value)		_SET_FV(PLL_DIV, value)
#define PLL_DIV_DEFAULT		0x0
#define PLL_DIV_BY_1		_BV(8)
#define PLL_CKE			_BV(31)
#else
/* In FU540-C000 spec */
#define PLL_CKE			_BV(24)
#endif

/* CORECLKSEL */
#define CORECLKSEL		_BV(0)

/* DEVICESRESET */
#define DDR_CTRL_RST_N		_BV(0)
#define DDR_AXI_RST_N		_BV(1)
#define DDR_AHB_RST_N		_BV(2)
#define DDR_PHY_RST_N		_BV(3)
#define GEMGXL_RST_N		_BV(5)

/* CLKMUSTATUS */
#define CLKMUX_CORECLKPLLSEL	_BV(0)
#define CLKMUX_TLCLKSEL		_BV(1)
#define CLKMUX_RTCXSEL		_BV(2)
#define CLKMUX_DDRCTRLCLKSEL	_BV(3)
#define CLKMUX_DDRPHYCLKSEL	_BV(4)
#define CLKMUX_GEMGXLCLKSEL	_BV(6)

#define __xosc_input_enable()		\
	__raw_setl(XOSCCFG_EN, PRCI_HFXOSCCFG)
#define __xosc_input_ready()		\
	(__raw_readl(PRCI_HFXOSCCFG) & XOSC_RDY)
#define xosc_enable()				\
	do {					\
		__xosc_input_enable();		\
		while (!__xosc_input_ready());	\
	} while (0)
#define pll_config_locked(pll)	\
	(__raw_readl(PRCI_PLLCFG(pll)) & PLL_LOCK)
/* PLL_DIV/PLL_DIV_BY_1=0 to use default value */
#define pll_output_enable(pll)	\
	__raw_writel(PLL_CKE, PRCI_PLLOUT(pll))
#define pll_output_disable(pll)	\
	__raw_writel(0, PRCI_PLLOUT(pll))
void pll_config_freq(uint8_t pll, uint32_t output_freq);

#define core_select_corepll()	__raw_clearl(CORECLKSEL, PRCI_CORECLKSEL)
#define core_select_hfclk()	__raw_setl(CORECLKSEL, PRCI_CORECLKSEL)
#define core_selected_corepll()	\
	(!(__raw_readl(PRCI_CORECLKSEL) & CORECLKSEL))

#define ddrc_reset()		__raw_setl(DDR_CTRL_RST_N, PRCI_DEVICESRESET)
#define ddrc_axi_reset()	__raw_setl(DDR_AXI_RST_N, PRCI_DEVICESRESET)
#define ddrc_ahb_reset()	__raw_setl(DDR_AHB_RST_N, PRCI_DEVICESRESET)
#define ddr_phy_reset()		__raw_setl(DDR_PHY_RST_N, PRCI_DEVICESRESET)
#define gbe_reset()		__raw_setl(GEMGXL_RST_N, PRCI_DEVICESRESET)

#define clk_selected_coreclkpll()	\
	(!!(__raw_readl(PRCI_CLKMUXSTATUS) & CLKMUX_CORECLKPLLSEL))
#define clk_selected_tlclk()		\
	(!!(__raw_readl(PRCI_CLKMUXSTATUS) & CLKMUX_TLCLKSEL))
#define clk_selected_rtcx()		\
	(!!(__raw_readl(PRCI_CLKMUXSTATUS) & CLKMUX_RTCXSEL))
#define clk_selected_ddrctrlclk()	\
	(!!(__raw_readl(PRCI_CLKMUXSTATUS) & CLKMUX_DDRCTRLCLKSEL))
#define clk_selected_ddrphyclk()	\
	(!!(__raw_readl(PRCI_CLKMUXSTATUS) & CLKMUX_DDRPHYCLKSEL))
#define clk_selected_gemgxlclk()	\
	(!!(__raw_readl(PRCI_CLKMUXSTATUS) & CLKMUX_GEMGXLCLKSEL))

#endif /* __PRCI_UNLEASHED_H_INCLUDE__ */
