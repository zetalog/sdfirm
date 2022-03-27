/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2021 CAS SmartCore Co., Ltd.
 *    Author: 2021 Lv Zheng <zhenglv@smart-core.cn>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_DUOWEN_REAL_H
#define __DT_BINDINGS_CLOCK_SBI_DUOWEN_REAL_H

#ifdef CONFIG_DUOWEN_PLL_REAL
#define XO_CLK_FREQ		25000000 /* 25MHz */
#define SOC_PLL_FREQ		800000000 /* 800MHz */
#define DDR_BUS_PLL_FREQ	800000000
#define DDR_PLL_FREQ		800000000
#define CFAB_PLL_FREQ		1000000000 /* 1GHz */
#define CL_PLL_FREQ		1500000000 /* 1.5GHz */
#if defined(CONFIG_DUOWEN_ETH_312500KHZ)
#define ETH_PLL_FREQ		312500000 /* 312.5MHz */
#elif defined(CONFIG_DUOWEN_ETH_156250KHZ)
#define ETH_PLL_FREQ		156250000 /* 156.25MHz */
#elif defined(CONFIG_DUOWEN_ETH_78125KHZ)
#define ETH_PLL_FREQ		78125000 /* 78.125MHz */
#else
#define ETH_PLL_FREQ		78125000 /* 78.125MHz */
#endif
#define SFAB_PLL_FREQ		100000000 /* 100MHz */
#define SGMII_PLL_FREQ		125000000 /* 125MHz */

#define SOC_CLK_FREQ		SOC_PLL_FREQ
#define IMC_CLK_FREQ		(SOC_CLK_FREQ/2)
#define APC_CLK_FREQ		CL_PLL_FREQ
#define SFAB_CLK_FREQ		SFAB_PLL_FREQ
#define CFAB_CLK_FREQ		CFAB_PLL_FREQ
#define APB_CLK_FREQ		SFAB_CLK_FREQ
#define AHB_CLK_FREQ		SFAB_CLK_FREQ

#define CLK_INPUT		0
#define CLK_VCO			1
#define CLK_PLL			2
#define CLK_SELECT		3
#define CLK_OUTPUT		4
#define CLK_DIV			5

/* CLK_INPUT */
#define XO_CLK			0
#define TIC_CLK			1
#define JTAG_CLK		2
#define NR_INPUT_CLKS		(JTAG_CLK + 1)
#define xo_clk			clkid(CLK_INPUT, XO_CLK)
#define tic_clk			clkid(CLK_INPUT, TIC_CLK)
#define jtag_clk		clkid(CLK_INPUT, JTAG_CLK)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define CLK_INPUT2		(CLK_INPUT | CLK_DUAL)
#define xo_clk2			clkid(CLK_INPUT2, XO_CLK)
#define tic_clk2		clkid(CLK_INPUT2, TIC_CLK)
#define jtag_clk2		clkid(CLK_INPUT2, JTAG_CLK)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* CLK_VCO */
#define SOC_VCO			SOC_PLL
#define DDR_BUS_VCO		DDR_BUS_PLL
#define DDR_VCO			DDR_PLL
#define COHFAB_VCO		COHFAB_PLL
#define CL0_VCO			CL0_PLL
#define CL1_VCO			CL1_PLL
#define CL2_VCO			CL2_PLL
#define CL3_VCO			CL3_PLL
#define ETH_VCO			ETH_PLL
#ifdef CONFIG_DUOWEN_SBI_DUAL
/* Only used as vco_clks indexes */
#define SOC_VCO2		(__DUOWEN_MAX_PLLS + SOC_PLL)
#define DDR_BUS_VCO2		(__DUOWEN_MAX_PLLS + DDR_BUS_PLL)
#define DDR_VCO2		(__DUOWEN_MAX_PLLS + DDR_PLL)
#define COHFAB_VCO2		(__DUOWEN_MAX_PLLS + COHFAB_PLL)
#define CL0_VCO2		(__DUOWEN_MAX_PLLS + CL0_PLL)
#define CL1_VCO2		(__DUOWEN_MAX_PLLS + CL1_PLL)
#define CL2_VCO2		(__DUOWEN_MAX_PLLS + CL2_PLL)
#define CL3_VCO2		(__DUOWEN_MAX_PLLS + CL3_PLL)
#define ETH_VCO2		(__DUOWEN_MAX_PLLS + ETH_PLL)
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#define __NR_VCO_CLKS		__DUOWEN_MAX_PLLS
#define NR_VCO_CLKS		DUOWEN_MAX_PLLS
#define soc_vco			clkid(CLK_VCO, SOC_VCO)
#define ddr_bus_vco		clkid(CLK_VCO, DDR_BUS_VCO)
#define ddr_vco			clkid(CLK_VCO, DDR_VCO)
#define cohfab_vco		clkid(CLK_VCO, COHFAB_VCO)
#define cl0_vco			clkid(CLK_VCO, CL0_VCO)
#define cl1_vco			clkid(CLK_VCO, CL1_VCO)
#define cl2_vco			clkid(CLK_VCO, CL2_VCO)
#define cl3_vco			clkid(CLK_VCO, CL3_VCO)
#define eth_vco			clkid(CLK_VCO, ETH_VCO)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define CLK_VCO2		(CLK_VCO | CLK_DUAL)
#define soc_vco2		clkid(CLK_VCO2, SOC_VCO)
#define ddr_bus_vco2		clkid(CLK_VCO2, DDR_BUS_VCO)
#define ddr_vco2		clkid(CLK_VCO2, DDR_VCO)
#define cohfab_vco2		clkid(CLK_VCO2, COHFAB_VCO)
#define cl0_vco2		clkid(CLK_VCO2, CL0_VCO)
#define cl1_vco2		clkid(CLK_VCO2, CL1_VCO)
#define cl2_vco2		clkid(CLK_VCO2, CL2_VCO)
#define cl3_vco2		clkid(CLK_VCO2, CL3_VCO)
#define eth_vco2		clkid(CLK_VCO2, ETH_VCO)
#define clk2vco(clk, soc)	((soc) ? __NR_VCO_CLKS + (clk) : (clk))
#else
#define clk2vco(clk, soc)	(clk)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* CLK_PLL */
/* P PLL_OUT clocks use same name as PLLs, R PLL_OUT clocks are defined
 * here.
 */
#define SYSFAB_PLL		(__DUOWEN_MAX_PLLS + SOC_PLL)
#define SGMII_PLL		(__DUOWEN_MAX_PLLS + ETH_PLL)
#define __NR_PLL_CLKS		(__DUOWEN_MAX_PLLS * 2)
#define NR_PLL_CLKS		(DUOWEN_MAX_PLLS * 2)
#define soc_pll			clkid(CLK_PLL, SOC_PLL)
#define ddr_bus_pll		clkid(CLK_PLL, DDR_BUS_PLL)
#define ddr_pll			clkid(CLK_PLL, DDR_PLL)
#define cohfab_pll		clkid(CLK_PLL, COHFAB_PLL)
#define cl0_pll			clkid(CLK_PLL, CL0_PLL)
#define cl1_pll			clkid(CLK_PLL, CL1_PLL)
#define cl2_pll			clkid(CLK_PLL, CL2_PLL)
#define cl3_pll			clkid(CLK_PLL, CL3_PLL)
#define eth_pll			clkid(CLK_PLL, ETH_PLL)
#define sysfab_pll		clkid(CLK_PLL, SYSFAB_PLL)
#define sgmii_pll		clkid(CLK_PLL, SGMII_PLL)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define CLK_PLL2		(CLK_PLL | CLK_DUAL)
/* Only used as pll_clks indexes */
#define SOC_PLL2		(DUOWEN_MAX_PLLS + SOC_PLL)
#define DDR_BUS_PLL2		(DUOWEN_MAX_PLLS + DDR_BUS_PLL)
#define DDR_PLL2		(DUOWEN_MAX_PLLS + DDR_PLL)
#define COHFAB_PLL2		(DUOWEN_MAX_PLLS + COHFAB_PLL)
#define CL0_PLL2		(DUOWEN_MAX_PLLS + CL0_PLL)
#define CL1_PLL2		(DUOWEN_MAX_PLLS + CL1_PLL)
#define CL2_PLL2		(DUOWEN_MAX_PLLS + CL2_PLL)
#define CL3_PLL2		(DUOWEN_MAX_PLLS + CL3_PLL)
#define ETH_PLL2		(DUOWEN_MAX_PLLS + ETH_PLL)
#define SYSFAB_PLL2		(DUOWEN_MAX_PLLS + SYSFAB_PLL)
#define SGMII_PLL2		(DUOWEN_MAX_PLLS + SGMII_PLL)
#define soc_pll2		clkid(CLK_PLL2, SOC_PLL)
#define ddr_bus_pll2		clkid(CLK_PLL2, DDR_BUS_PLL)
#define ddr_pll2		clkid(CLK_PLL2, DDR_PLL)
#define cohfab_pll2		clkid(CLK_PLL2, COHFAB_PLL)
#define cl0_pll2		clkid(CLK_PLL2, CL0_PLL)
#define cl1_pll2		clkid(CLK_PLL2, CL1_PLL)
#define cl2_pll2		clkid(CLK_PLL2, CL2_PLL)
#define cl3_pll2		clkid(CLK_PLL2, CL3_PLL)
#define eth_pll2		clkid(CLK_PLL2, ETH_PLL)
#define sysfab_pll2		clkid(CLK_PLL2, SYSFAB_PLL)
#define sgmii_pll2		clkid(CLK_PLL2, SGMII_PLL)
#define clk2pll(clk, soc)	((soc) ? __NR_PLL_CLKS + (clk) : (clk))
#else
#define clk2pll(clk, soc)	(clk)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* CLK_SELECT: CLK_SEL_CFG */
#define SOC_CLK_DIV2_SEL	0 /* SOC_PLL P clock div2 */
#define SYSFAB_CLK_SEL		1 /* SOC_PLL R clock */
#define DDR_BUS_CLK_SEL		2
#define DDR_CLK_SEL		3
#define PCIE_REF_CLK_SEL	4
#define SOC_CLK_SEL		5 /* SOC_PLL P clock */
#define COHFAB_CLK_SEL		7
#define CL0_CLK_SEL		8
#define CL1_CLK_SEL		9
#define CL2_CLK_SEL		10
#define CL3_CLK_SEL		11
#define NR_SELECT_CLKS		(CL3_CLK_SEL + 1)
#define soc_clk_div2_sel	clkid(CLK_SELECT, SOC_CLK_DIV2_SEL)
#define sysfab_clk_sel		clkid(CLK_SELECT, SYSFAB_CLK_SEL)
#define soc_clk_sel		clkid(CLK_SELECT, SOC_CLK_SEL)
#define ddr_bus_clk_sel		clkid(CLK_SELECT, DDR_BUS_CLK_SEL)
#define ddr_clk_sel		clkid(CLK_SELECT, DDR_CLK_SEL)
#define soc_clk_sel		clkid(CLK_SELECT, SOC_CLK_SEL)
#define cohfab_clk_sel		clkid(CLK_SELECT, COHFAB_CLK_SEL)
#define cl0_clk_sel		clkid(CLK_SELECT, CL0_CLK_SEL)
#define cl1_clk_sel		clkid(CLK_SELECT, CL1_CLK_SEL)
#define cl2_clk_sel		clkid(CLK_SELECT, CL2_CLK_SEL)
#define cl3_clk_sel		clkid(CLK_SELECT, CL3_CLK_SEL)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define CLK_SELECT2		(CLK_SELECT | CLK_DUAL)
#define soc_clk_div2_sel2	clkid(CLK_SELECT2, SOC_CLK_DIV2_SEL)
#define sysfab_clk_sel2		clkid(CLK_SELECT2, SYSFAB_CLK_SEL)
#define soc_clk_sel2		clkid(CLK_SELECT2, SOC_CLK_SEL)
#define ddr_bus_clk_sel2	clkid(CLK_SELECT2, DDR_BUS_CLK_SEL)
#define ddr_clk_sel2		clkid(CLK_SELECT2, DDR_CLK_SEL)
#define soc_clk_sel2		clkid(CLK_SELECT2, SOC_CLK_SEL)
#define cohfab_clk_sel2		clkid(CLK_SELECT2, COHFAB_CLK_SEL)
#define cl0_clk_sel2		clkid(CLK_SELECT2, CL0_CLK_SEL)
#define cl1_clk_sel2		clkid(CLK_SELECT2, CL1_CLK_SEL)
#define cl2_clk_sel2		clkid(CLK_SELECT2, CL2_CLK_SEL)
#define cl3_clk_sel2		clkid(CLK_SELECT2, CL3_CLK_SEL)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* In ASIC environment, dynamic PLL changes are prevented by automatically
 * switching to xo_clk before changing P/R clkouts. Thus the select clocks
 * are controlled by the PLL clocks.
 */
#define soc_clk_div2_src	soc_clk_div2
#define soc_clk_src		soc_pll
#define sysfab_clk_src		sysfab_pll
#define ddr_bus_clk_src		ddr_bus_pll
#define ddr_clk_src		ddr_pll
#define cohfab_clk_src		cohfab_pll
#define cl0_clk_src		cl0_pll
#define cl1_clk_src		cl1_pll
#define cl2_clk_src		cl2_pll
#define cl3_clk_src		cl3_pll
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define soc_clk_div2_src2	soc_clk_div22
#define soc_clk_src2		soc_pll2
#define sysfab_clk_src2		sysfab_pll2
#define ddr_bus_clk_src2	ddr_bus_pll2
#define ddr_clk_src2		ddr_pll2
#define cohfab_clk_src2		cohfab_pll2
#define cl0_clk_src2		cl0_pll2
#define cl1_clk_src2		cl1_pll2
#define cl2_clk_src2		cl2_pll2
#define cl3_clk_src2		cl3_pll2
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#endif /* CONFIG_DUOWEN_PLL_REAL */

#endif /* __DT_BINDINGS_CLOCK_SBI_DUOWEN_REAL_H */
