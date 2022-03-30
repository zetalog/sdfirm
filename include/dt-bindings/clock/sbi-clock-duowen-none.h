/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2021 CAS SmartCore Co., Ltd.
 *    Author: 2021 Lv Zheng <zhenglv@smart-core.cn>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_DUOWEN_NONE_H
#define __DT_BINDINGS_CLOCK_SBI_DUOWEN_NONE_H

#ifdef CONFIG_DUOWEN_PLL_NONE
#define XO_CLK_FREQ		25000000
#define SOC_PLL_FREQ		XO_CLK_FREQ
#define DDR_BUS_PLL_FREQ	XO_CLK_FREQ
#define DDR_PLL_FREQ		XO_CLK_FREQ
#define CFAB_PLL_FREQ		XO_CLK_FREQ
#define CL_PLL_FREQ		XO_CLK_FREQ
#define ETH_PLL_FREQ		XO_CLK_FREQ
#define SFAB_PLL_FREQ		XO_CLK_FREQ
#define SGMII_PLL_FREQ		XO_CLK_FREQ

#define SOC_CLK_FREQ		SOC_PLL_FREQ
#define IMC_CLK_FREQ		SOC_CLK_FREQ /* Not div2 */
#define APC_CLK_FREQ		CL_PLL_FREQ
#define SFAB_CLK_FREQ		SFAB_PLL_FREQ
#define CFAB_CLK_FREQ		CFAB_PLL_FREQ
#define APB_CLK_FREQ		SFAB_CLK_FREQ
#define AHB_CLK_FREQ		SFAB_CLK_FREQ

#define CLK_INPUT		0
#define CLK_OUTPUT		1
#define CLK_DIV			2

/* CLK_INPUT */
#define CLK_PLL			CLK_INPUT
#define CLK_SELECT		CLK_INPUT
#define SGMII_PLL		(__DUOWEN_MAX_PLLS + 1)
#define XO_CLK			(__DUOWEN_MAX_PLLS + 2)
#define TIC_CLK			(__DUOWEN_MAX_PLLS + 3)
#define JTAG_CLK		(__DUOWEN_MAX_PLLS + 4)
#define SOC_CLK_DIV2_SEL	(__DUOWEN_MAX_PLLS + 5) /* SOC_HIGH_SPEED_CLK_SEL */
#define SYSFAB_CLK_SEL		(__DUOWEN_MAX_PLLS + 6) /* SOC_LOW_SPEED_CLK_SEL */
#define DDR_BUS_CLK_SEL		(__DUOWEN_MAX_PLLS + 7)
#define DDR_CLK_SEL		(__DUOWEN_MAX_PLLS + 8)
#define SOC_CLK_SEL		(__DUOWEN_MAX_PLLS + 9) /* PCIE_AXI_CLK_SEL */
#define COHFAB_CLK_SEL		(__DUOWEN_MAX_PLLS + 10)
#define CL0_CLK_SEL		(__DUOWEN_MAX_PLLS + 11)
#define CL1_CLK_SEL		(__DUOWEN_MAX_PLLS + 12)
#define CL2_CLK_SEL		(__DUOWEN_MAX_PLLS + 13)
#define CL3_CLK_SEL		(__DUOWEN_MAX_PLLS + 14)
#define NR_INPUT_CLKS		(CL3_CLK_SEL + 1)
#define soc_pll			clkid(CLK_PLL, SOC_PLL)
#define eth_pll			clkid(CLK_PLL, ETH_PLL)
#define sgmii_pll		clkid(CLK_PLL, SGMII_PLL)
#define xo_clk			clkid(CLK_INPUT, XO_CLK)
#define tic_clk			clkid(CLK_INPUT, TIC_CLK)
#define jtag_clk		clkid(CLK_INPUT, JTAG_CLK)
#define soc_clk_div2_sel	clkid(CLK_SELECT, SOC_CLK_DIV2_SEL)
#define sysfab_clk_sel		clkid(CLK_SELECT, SYSFAB_CLK_SEL)
#define ddr_bus_clk_sel		clkid(CLK_SELECT, DDR_BUS_CLK_SEL)
#define ddr_clk_sel		clkid(CLK_SELECT, DDR_CLK_SEL)
#define soc_clk_sel		clkid(CLK_SELECT, SOC_CLK_SEL)
#define cohfab_clk_sel		clkid(CLK_SELECT, COHFAB_CLK_SEL)
#define cl0_clk_sel		clkid(CLK_SELECT, CL0_CLK_SEL)
#define cl1_clk_sel		clkid(CLK_SELECT, CL1_CLK_SEL)
#define cl2_clk_sel		clkid(CLK_SELECT, CL2_CLK_SEL)
#define cl3_clk_sel		clkid(CLK_SELECT, CL3_CLK_SEL)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define CLK_INPUT2		(CLK_INPUT | CLK_DUAL)
#define CLK_SELECT2		(CLK_SELECT | CLK_DUAL)
#define CLK_PLL2		(CLK_PLL | CLK_DUAL)
#define soc_pll2		clkid(CLK_PLL2, SOC_PLL)
#define eth_pll2		clkid(CLK_PLL2, ETH_PLL)
#define sgmii_pll2		clkid(CLK_PLL2, SGMII_PLL)
#define xo_clk2			clkid(CLK_INPUT2, XO_CLK)
#define tic_clk2		clkid(CLK_INPUT2, TIC_CLK)
#define jtag_clk2		clkid(CLK_INPUT2, JTAG_CLK)
#define soc_clk_div2_sel2	clkid(CLK_SELECT2, SOC_CLK_DIV2_SEL)
#define sysfab_clk_sel2		clkid(CLK_SELECT2, SYSFAB_CLK_SEL)
#define ddr_bus_clk_sel2	clkid(CLK_SELECT2, DDR_BUS_CLK_SEL)
#define ddr_clk_sel2		clkid(CLK_SELECT2, DDR_CLK_SEL)
#define soc_clk_sel2		clkid(CLK_SELECT2, SOC_CLK_SEL)
#define cohfab_clk_sel2		clkid(CLK_SELECT2, COHFAB_CLK_SEL)
#define cl0_clk_sel2		clkid(CLK_SELECT2, CL0_CLK_SEL)
#define cl1_clk_sel2		clkid(CLK_SELECT2, CL1_CLK_SEL)
#define cl2_clk_sel2		clkid(CLK_SELECT2, CL2_CLK_SEL)
#define cl3_clk_sel2		clkid(CLK_SELECT2, CL3_CLK_SEL)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* In ZeBu environment, the select clocks need to be programmed to output
 * PLL P/R clkouts. Thus the PLL clocks are controlled by the select
 * clocks.
 */
#define soc_clk_div2_src	soc_clk_div2_sel
#define soc_clk_src		soc_clk_sel
#define sysfab_clk_src		sysfab_clk_sel
#define ddr_bus_clk_src		ddr_bus_clk_sel
#define ddr_clk_src		ddr_clk_sel
#define cohfab_clk_src		cohfab_clk_sel
#define cl0_clk_src		cl0_clk_sel
#define cl1_clk_src		cl1_clk_sel
#define cl2_clk_src		cl2_clk_sel
#define cl3_clk_src		cl3_clk_sel
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define soc_clk_div2_src2	soc_clk_div2_sel2
#define soc_clk_src2		soc_clk_sel2
#define sysfab_clk_src2		sysfab_clk_sel2
#define ddr_bus_clk_src2	ddr_bus_clk_sel2
#define ddr_clk_src2		ddr_clk_sel2
#define cohfab_clk_src2		cohfab_clk_sel2
#define cl0_clk_src2		cl0_clk_sel2
#define cl1_clk_src2		cl1_clk_sel2
#define cl2_clk_src2		cl2_clk_sel2
#define cl3_clk_src2		cl3_clk_sel2
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#endif /* CONFIG_DUOWEN_PLL_NONE */

#endif /* __DT_BINDINGS_CLOCK_SBI_DUOWEN_NONE_H */
