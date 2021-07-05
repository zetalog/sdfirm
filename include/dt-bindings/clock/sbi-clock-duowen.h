/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2021 CAS SmartCore Co., Ltd.
 *    Author: 2021 Lv Zheng <zhenglv@smart-core.cn>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_DUOWEN_H
#define __DT_BINDINGS_CLOCK_SBI_DUOWEN_H

#include <dt-bindings/clock/sbi-clock.h>

#define clkid2(clkid)		((clkid) | ((CLK_DUAL) << 8))

#ifdef CONFIG_DUOWEN_PLL_REAL
#define XO_CLK_FREQ		25000000 /* 25MHz */
#define SOC_PLL_FREQ		1000000000 /* 1GHz */
#define DDR_BUS_PLL_FREQ	800000000
#define DDR_PLL_FREQ		800000000
#define PCIE_PLL_FREQ		100000000 /* 100MHz */
#define CFAB_PLL_FREQ		1400000000 /* 1.4GHz */
#define CL_PLL_FREQ		2000000000 /* 2GHz */
#if defined(CONFIG_DUOWEN_ETH_312500KHZ)
#define ETH_PLL_FREQ		312500000 /* 312.5MHz */
#elif defined(CONFIG_DUOWEN_ETH_156250KHZ)
#define ETH_PLL_FREQ		156250000 /* 156.25MHz */
#elif defined(CONFIG_DUOWEN_ETH_78125KHZ)
#define ETH_PLL_FREQ		78125000 /* 78.125MHz */
#else
#define ETH_PLL_FREQ		78125000 /* 78.125MHz */
#endif
#define SGMII_PLL_FREQ		125000000 /* 125MHz */
#endif
#ifdef CONFIG_DUOWEN_PLL_FAKE
#ifdef CONFIG_DUOWEN_XO_25M
#define XO_CLK_FREQ		25000000
#endif
#ifdef CONFIG_DUOWEN_XO_125M
#define XO_CLK_FREQ		125000000
#endif
#define SOC_PLL_FREQ		1000000000
#define DDR_BUS_PLL_FREQ	800000000
#define DDR_PLL_FREQ		800000000
#define PCIE_PLL_FREQ		100000000
#define CFAB_PLL_FREQ		1400000000
#define CL_PLL_FREQ		2000000000
#define ETH_PLL_FREQ		156250000
#define SGMII_PLL_FREQ		125000000
#endif

#define APC_CLK_FREQ		CL_PLL_FREQ
#define AHB_CLK_FREQ		SFAB_CLK_FREQ
#define CFAB_CLK_FREQ		CFAB_PLL_FREQ

#define SFAB_PLL_FREQ		100000000
#define APB_CLK_FREQ		SFAB_CLK_FREQ
#define SOC_CLK_FREQ		SOC_PLL_FREQ
#define IMC_CLK_FREQ		(SOC_CLK_FREQ/2) /* 500MHz IMC/RAM/ROM */
#define SFAB_CLK_FREQ		SFAB_PLL_FREQ /* 100MHz SFAB peripherals */

#ifdef CONFIG_DUOWEN_UART_ACCEL
#define UART_CURRENT_SPEED	(APB_CLK_FREQ/16)
#else /* CONFIG_DUOWEN_UART_ACCEL */
#ifdef CONFIG_CONSOLE_4000000
#define UART_CURRENT_SPEED	4000000
#endif
#ifdef CONFIG_CONSOLE_3500000
#define UART_CURRENT_SPEED	3500000
#endif
#ifdef CONFIG_CONSOLE_3000000
#define UART_CURRENT_SPEED	3000000
#endif
#ifdef CONFIG_CONSOLE_2500000
#define UART_CURRENT_SPEED	2500000
#endif
#ifdef CONFIG_CONSOLE_2000000
#define UART_CURRENT_SPEED	2000000
#endif
#ifdef CONFIG_CONSOLE_1500000
#define UART_CURRENT_SPEED	1500000
#endif
#ifdef CONFIG_CONSOLE_1000000
#define UART_CURRENT_SPEED	1000000
#endif
#ifdef CONFIG_CONSOLE_500000
#define UART_CURRENT_SPEED	500000
#endif
#ifdef CONFIG_CONSOLE_1152000
#define UART_CURRENT_SPEED	1152000
#endif
#ifdef CONFIG_CONSOLE_921600
#define UART_CURRENT_SPEED	921600
#endif
#ifdef CONFIG_CONSOLE_576000
#define UART_CURRENT_SPEED	576000
#endif
#ifdef CONFIG_CONSOLE_460800
#define UART_CURRENT_SPEED	460800
#endif
#ifdef CONFIG_CONSOLE_230400
#define UART_CURRENT_SPEED	230400
#endif
#ifdef CONFIG_CONSOLE_115200
#define UART_CURRENT_SPEED	115200
#endif
#ifdef CONFIG_CONSOLE_76800
#define UART_CURRENT_SPEED	76800
#endif
#ifdef CONFIG_CONSOLE_57600
#define UART_CURRENT_SPEED	57600
#endif
#ifdef CONFIG_CONSOLE_38400
#define UART_CURRENT_SPEED	38400
#endif
#ifdef CONFIG_CONSOLE_19200
#define UART_CURRENT_SPEED	19200
#endif
#ifdef CONFIG_CONSOLE_9600
#define UART_CURRENT_SPEED	9600
#endif
#ifdef CONFIG_CONSOLE_4800
#define UART_CURRENT_SPEED	4800
#endif
#ifdef CONFIG_CONSOLE_2400
#define UART_CURRENT_SPEED	2400
#endif
#endif /* CONFIG_DUOWEN_UART_ACCEL */

#define I2C_MASTER_SPEED 400000

/* PLL IDs is kept AS IS to make base addresses simpler, see
 * COHFAB_CLK_PLL() and COHFAB_CLK_SEL_PLL() for details.
 */
#define SOC_PLL			0
#define DDR_BUS_PLL		1
#define DDR_PLL			2
#define PCIE_PLL		3
#define COHFAB_PLL		4
#define CL0_PLL			5
#define CL1_PLL			6
#define CL2_PLL			7
#define CL3_PLL			8
#define ETH_PLL			9
#define __DUOWEN_MAX_PLLS	10
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define DUOWEN_MAX_PLLS		(2 * __DUOWEN_MAX_PLLS)
#define socpll(pll, soc)	((soc) ? __DUOWEN_MAX_PLLS + (pll): (pll))
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define DUOWEN_MAX_PLLS		10
#define socpll(pll, soc)	(pll)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

#define CLK_DUAL		8 /* DUAL socket clock bit */
#ifdef CONFIG_DUOWEN_PLL_REAL
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
#define PCIE_VCO		PCIE_PLL
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
#define PCIE_VCO2		(__DUOWEN_MAX_PLLS + PCIE_PLL)
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
#define pcie_vco		clkid(CLK_VCO, PCIE_VCO)
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
#define pcie_vco2		clkid(CLK_VCO2, PCIE_VCO)
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
#define pcie_pll		clkid(CLK_PLL, PCIE_PLL)
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
#define PCIE_PLL2		(DUOWEN_MAX_PLLS + PCIE_PLL)
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
#define pcie_pll2		clkid(CLK_PLL2, PCIE_PLL)
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
#define pcie_ref_clk_sel	clkid(CLK_SELECT, PCIE_REF_CLK_SEL)
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
#define pcie_ref_clk_sel2	clkid(CLK_SELECT2, PCIE_REF_CLK_SEL)
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
#define pcie_ref_clk_src	pcie_pll
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
#define pcie_ref_clk_src2	pcie_pll2
#define cohfab_clk_src2		cohfab_pll2
#define cl0_clk_src2		cl0_pll2
#define cl1_clk_src2		cl1_pll2
#define cl2_clk_src2		cl2_pll2
#define cl3_clk_src2		cl3_pll2
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#endif /* CONFIG_DUOWEN_PLL_REAL */

#ifdef CONFIG_DUOWEN_PLL_FAKE
#define CLK_INPUT		0
#define CLK_SELECT		1
#define CLK_OUTPUT		2
#define CLK_DIV			3

/* CLK_INPUT */
#define CLK_PLL			CLK_INPUT
#define SYSFAB_PLL		(__DUOWEN_MAX_PLLS + 0)
#define SGMII_PLL		(__DUOWEN_MAX_PLLS + 1)
#define XO_CLK			(__DUOWEN_MAX_PLLS + 2)
#define TIC_CLK			(__DUOWEN_MAX_PLLS + 3)
#define JTAG_CLK		(__DUOWEN_MAX_PLLS + 4)
#define NR_INPUT_CLKS		(JTAG_CLK + 1)
#define soc_pll			clkid(CLK_PLL, SOC_PLL)
#define ddr_bus_pll		clkid(CLK_PLL, DDR_BUS_PLL)
#define ddr_pll			clkid(CLK_PLL, DDR_PLL)
#define pcie_pll		clkid(CLK_PLL, PCIE_PLL)
#define cohfab_pll		clkid(CLK_PLL, COHFAB_PLL)
#define cl0_pll			clkid(CLK_PLL, CL0_PLL)
#define cl1_pll			clkid(CLK_PLL, CL1_PLL)
#define cl2_pll			clkid(CLK_PLL, CL2_PLL)
#define cl3_pll			clkid(CLK_PLL, CL3_PLL)
#define eth_pll			clkid(CLK_PLL, ETH_PLL)
#define sysfab_pll		clkid(CLK_PLL, SYSFAB_PLL)
#define sgmii_pll		clkid(CLK_PLL, SGMII_PLL)
#define xo_clk			clkid(CLK_INPUT, XO_CLK)
#define tic_clk			clkid(CLK_INPUT, TIC_CLK)
#define jtag_clk		clkid(CLK_INPUT, JTAG_CLK)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define CLK_INPUT2		(CLK_INPUT | CLK_DUAL)
#define CLK_PLL2		(CLK_PLL | CLK_DUAL)
#define soc_pll2		clkid(CLK_PLL2, SOC_PLL)
#define ddr_bus_pll2		clkid(CLK_PLL2, DDR_BUS_PLL)
#define ddr_pll2		clkid(CLK_PLL2, DDR_PLL)
#define pcie_pll2		clkid(CLK_PLL2, PCIE_PLL)
#define cohfab_pll2		clkid(CLK_PLL2, COHFAB_PLL)
#define cl0_pll2		clkid(CLK_PLL2, CL0_PLL)
#define cl1_pll2		clkid(CLK_PLL2, CL1_PLL)
#define cl2_pll2		clkid(CLK_PLL2, CL2_PLL)
#define cl3_pll2		clkid(CLK_PLL2, CL3_PLL)
#define eth_pll2		clkid(CLK_PLL2, ETH_PLL)
#define sysfab_pll2		clkid(CLK_PLL2, SYSFAB_PLL)
#define sgmii_pll2		clkid(CLK_PLL2, SGMII_PLL)
#define xo_clk2			clkid(CLK_INPUT2, XO_CLK)
#define tic_clk2		clkid(CLK_INPUT2, TIC_CLK)
#define jtag_clk2		clkid(CLK_INPUT2, JTAG_CLK)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* CLK_SELECT: CLK_SEL_CFG */
#define SOC_CLK_DIV2_SEL	0
#define SYSFAB_CLK_SEL		1
#define DDR_BUS_CLK_SEL		2
#define DDR_CLK_SEL		3
#define PCIE_REF_CLK_SEL	4
#define SOC_CLK_SEL		5 /* PCIE_AXI_CLK_SEL */
#define COHFAB_CLK_SEL		7
#define CL0_CLK_SEL		8
#define CL1_CLK_SEL		9
#define CL2_CLK_SEL		10
#define CL3_CLK_SEL		11
#define NR_SELECT_CLKS		(CL3_CLK_SEL + 1)
#define soc_clk_div2_sel	clkid(CLK_SELECT, SOC_CLK_DIV2_SEL)
#define sysfab_clk_sel		clkid(CLK_SELECT, SYSFAB_CLK_SEL)
#define ddr_bus_clk_sel		clkid(CLK_SELECT, DDR_BUS_CLK_SEL)
#define ddr_clk_sel		clkid(CLK_SELECT, DDR_CLK_SEL)
#define pcie_ref_clk_sel	clkid(CLK_SELECT, PCIE_REF_CLK_SEL)
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
#define ddr_bus_clk_sel2	clkid(CLK_SELECT2, DDR_BUS_CLK_SEL)
#define ddr_clk_sel2		clkid(CLK_SELECT2, DDR_CLK_SEL)
#define pcie_ref_clk_sel2	clkid(CLK_SELECT2, PCIE_REF_CLK_SEL)
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
#define pcie_ref_clk_src	pcie_ref_clk_sel
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
#define pcie_ref_clk_src2	pcie_ref_clk_sel2
#define cohfab_clk_src2		cohfab_clk_sel2
#define cl0_clk_src2		cl0_clk_sel2
#define cl1_clk_src2		cl1_clk_sel2
#define cl2_clk_src2		cl2_clk_sel2
#define cl3_clk_src2		cl3_clk_sel2
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#endif /* CONFIG_DUOWEN_PLL_FAKE */

/* Alias for select clocks */
#define sysfab_100m_clk		sysfab_clk_src
#define sysfab_500m_clk		soc_clk_div2_src
#define soc_clk			sysfab_500m_clk
#define sysfab_clk		sysfab_100m_clk
#define pcie_axi_clk		soc_clk_src
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define sysfab_100m_clk2	sysfab_clk_src2
#define sysfab_500m_clk2	soc_clk_div2_src2
#define soc_clk2		sysfab_500m_clk2
#define sysfab_clk2		sysfab_100m_clk2
#define pcie_axi_clk2		soc_clk_src2
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* CLK_OUTPUT: CLK_EN_CFG0 */
#define DMA_CLK			0
#define PCIE_POR		1 /* SW_RST only */
#define PCIE_CLK		2
#define DDR_POR			4 /* SW_RST only */
#define DDR_BYPASS_PCLK		5 /* CLK_EN only */
#define DDR_PCLK		6
#define DDR_ACLK		7
#define DDR_CLK			8 /* CLK_EN only */
#define DDR_RST			9 /* SW_RST only */
#define SYSFAB_DBG_CLK		10
#define SYSFAB_TIC_CLK		11
#define CLUSTER0_HCLK		12
#define CLUSTER1_HCLK		13
#define CLUSTER2_HCLK		14
#define CLUSTER3_HCLK		15
#define COHFAB_HCLK		16
#define LCSR_CLK		18
#define SCSR_CLK		19
#define TLMM_CLK		20
#define PLIC_CLK		21
#define TMR0_CLK		25
#define TMR1_CLK		26
#define TMR2_CLK		27
#define TMR3_CLK		28
#define WDT0_CLK		29
#define WDT1_CLK		30

/* CLK_OUTPUT: CLK_EN_CFG1 */
#define GPIO0_CLK		32
#define GPIO1_CLK		33
#define GPIO2_CLK		34
#define UART0_CLK		35
#define UART1_CLK		36
#define UART2_CLK		37
#define UART3_CLK		38
#define I2C0_CLK		39
#define I2C1_CLK		40
#define I2C2_CLK		41
#define I2C3_CLK		42
#define I2C4_CLK		43
#define I2C5_CLK		44
#define I2C6_CLK		45
#define I2C7_CLK		46
#define I2C8_CLK		47
#define I2C9_CLK		48
#define I2C10_CLK		49
#define I2C11_CLK		50
#define SPI0_CLK		51
#define SPI1_CLK		52
#define SPI2_CLK		53
#define SPI3_CLK		54
#define SPI4_CLK		55
#define SPI_FLASH_CLK		56
#define SD_CLK			57
#define ETH_CLK			58
#define TSENSOR_CLK		63

/* CLK_OUTPUT: CLK_EN_CFG2 */

/* CLK_OUTPUT: CLK_EN_CFG3 */
#define IMC_CLK			96

/* Additional output clocks */
#define COHFAB_CFG_CLK		97
#define PCIE_PCLK		98
#define ETH_ALT_REF_CLK		99
#define SGMII_REF_CLK		100

/* CLK_OUTPUT: CLUSTER/COHFAB CLK_CFG */
#define COHFAB_CLK		104
#define CLUSTER0_CLK		105
#define CLUSTER1_CLK		106
#define CLUSTER2_CLK		107
#define CLUSTER3_CLK		108

/* CLK_OUTPUT: CLUSTER internal CLK_CG/RST_CTRL */
#define CLUSTER0_APC0_CPU0_CLK	112
#define CLUSTER0_APC0_CPU1_CLK	113
#define CLUSTER0_APC1_CPU0_CLK	114
#define CLUSTER0_APC1_CPU1_CLK	115
#define CLUSTER0_APC0_L2_CLK	116
#define CLUSTER0_APC1_L2_CLK	117
#define CLUSTER1_APC0_CPU0_CLK	118
#define CLUSTER1_APC0_CPU1_CLK	119
#define CLUSTER1_APC1_CPU0_CLK	120
#define CLUSTER1_APC1_CPU1_CLK	121
#define CLUSTER1_APC0_L2_CLK	122
#define CLUSTER1_APC1_L2_CLK	123
#define CLUSTER2_APC0_CPU0_CLK	124
#define CLUSTER2_APC0_CPU1_CLK	125
#define CLUSTER2_APC1_CPU0_CLK	126
#define CLUSTER2_APC1_CPU1_CLK	127
#define CLUSTER2_APC0_L2_CLK	128
#define CLUSTER2_APC1_L2_CLK	129
#define CLUSTER3_APC0_CPU0_CLK	130
#define CLUSTER3_APC0_CPU1_CLK	131
#define CLUSTER3_APC1_CPU0_CLK	132
#define CLUSTER3_APC1_CPU1_CLK	133
#define CLUSTER3_APC0_L2_CLK	134
#define CLUSTER3_APC1_L2_CLK	135
#define NR_OUTPUT_CLKS		(CLUSTER3_APC1_L2_CLK + 1)

/* CLK_OUTPUT: CLK_EN_CFG0 */
/* Integrated overall clocks */
#define dma_clk			clkid(CLK_OUTPUT, DMA_CLK)
#define ddr_por			clkid(CLK_OUTPUT, DDR_POR)
#define ddr_bypass_pclk		clkid(CLK_OUTPUT, DDR_BYPASS_PCLK)
#define ddr_pclk		clkid(CLK_OUTPUT, DDR_PCLK)
#define ddr_aclk		clkid(CLK_OUTPUT, DDR_ACLK)
#define ddr_clk			clkid(CLK_OUTPUT, DDR_CLK)
#define ddr_rst			clkid(CLK_OUTPUT, DDR_RST)
#define pcie_por		clkid(CLK_OUTPUT, PCIE_POR)
#define pcie_clk		clkid(CLK_OUTPUT, PCIE_CLK)
#define sysfab_dbg_clk		clkid(CLK_OUTPUT, SYSFAB_DBG_CLK)
#define sysfab_tic_clk		clkid(CLK_OUTPUT, SYSFAB_TIC_CLK)
#define cluster0_hclk		clkid(CLK_OUTPUT, CLUSTER0_HCLK)
#define cluster1_hclk		clkid(CLK_OUTPUT, CLUSTER1_HCLK)
#define cluster2_hclk		clkid(CLK_OUTPUT, CLUSTER2_HCLK)
#define cluster3_hclk		clkid(CLK_OUTPUT, CLUSTER3_HCLK)
#define cohfab_hclk		clkid(CLK_OUTPUT, COHFAB_HCLK)
#define lcsr_clk		clkid(CLK_OUTPUT, LCSR_CLK)
#define scsr_clk		clkid(CLK_OUTPUT, SCSR_CLK)
#define tlmm_clk		clkid(CLK_OUTPUT, TLMM_CLK)
#define plic_clk		clkid(CLK_OUTPUT, PLIC_CLK)
#define tic_rst			clkid(CLK_OUTPUT, TIC_RST)
#define dbg_rst			clkid(CLK_OUTPUT, DBG_RST)
#define tmr0_clk		clkid(CLK_OUTPUT, TMR0_CLK)
#define tmr1_clk		clkid(CLK_OUTPUT, TMR1_CLK)
#define tmr2_clk		clkid(CLK_OUTPUT, TMR2_CLK)
#define tmr3_clk		clkid(CLK_OUTPUT, TMR3_CLK)
#define wdt0_clk		clkid(CLK_OUTPUT, WDT0_CLK)
#define wdt1_clk		clkid(CLK_OUTPUT, WDT1_CLK)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define CLK_OUTPUT2		(CLK_OUTPUT | CLK_DUAL)
#define dma_clk2		clkid(CLK_OUTPUT2, DMA_CLK)
#define ddr_por2		clkid(CLK_OUTPUT2, DDR_POR)
#define ddr_bypass_pclk2	clkid(CLK_OUTPUT2, DDR_BYPASS_PCLK)
#define ddr_pclk2		clkid(CLK_OUTPUT2, DDR_PCLK)
#define ddr_aclk2		clkid(CLK_OUTPUT2, DDR_ACLK)
#define ddr_clk2		clkid(CLK_OUTPUT2, DDR_CLK)
#define ddr_rst2		clkid(CLK_OUTPUT2, DDR_RST)
#define pcie_por2		clkid(CLK_OUTPUT2, PCIE_POR)
#define pcie_clk2		clkid(CLK_OUTPUT2, PCIE_CLK)
#define sysfab_dbg_clk2		clkid(CLK_OUTPUT2, SYSFAB_DBG_CLK)
#define sysfab_tic_clk2		clkid(CLK_OUTPUT2, SYSFAB_TIC_CLK)
#define cluster0_hclk2		clkid(CLK_OUTPUT2, CLUSTER0_HCLK)
#define cluster1_hclk2		clkid(CLK_OUTPUT2, CLUSTER1_HCLK)
#define cluster2_hclk2		clkid(CLK_OUTPUT2, CLUSTER2_HCLK)
#define cluster3_hclk2		clkid(CLK_OUTPUT2, CLUSTER3_HCLK)
#define cohfab_hclk2		clkid(CLK_OUTPUT2, COHFAB_HCLK)
#define lcsr_clk2		clkid(CLK_OUTPUT2, LCSR_CLK)
#define scsr_clk2		clkid(CLK_OUTPUT2, SCSR_CLK)
#define tlmm_clk2		clkid(CLK_OUTPUT2, TLMM_CLK)
#define plic_clk2		clkid(CLK_OUTPUT2, PLIC_CLK)
#define tic_rst2		clkid(CLK_OUTPUT2, TIC_RST)
#define dbg_rst2		clkid(CLK_OUTPUT2, DBG_RST)
#define tmr0_clk2		clkid(CLK_OUTPUT2, TMR0_CLK)
#define tmr1_clk2		clkid(CLK_OUTPUT2, TMR1_CLK)
#define tmr2_clk2		clkid(CLK_OUTPUT2, TMR2_CLK)
#define tmr3_clk2		clkid(CLK_OUTPUT2, TMR3_CLK)
#define wdt0_clk2		clkid(CLK_OUTPUT2, WDT0_CLK)
#define wdt1_clk2		clkid(CLK_OUTPUT2, WDT1_CLK)
#endif /* CONFIG_DUOWEN_SBI_DUAL */
/* Additional clocks */
#define cohfab_cfg_clk		clkid(CLK_OUTPUT, COHFAB_CFG_CLK)
#define pcie_pclk		clkid(CLK_OUTPUT, PCIE_PCLK)
#define eth_alt_ref_clk		clkid(CLK_OUTPUT, ETH_ALT_REF_CLK)
#define sgmii_ref_clk		clkid(CLK_OUTPUT, SGMII_REF_CLK)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define cohfab_cfg_clk2		clkid(CLK_OUTPUT2, COHFAB_CFG_CLK)
#define pcie_pclk2		clkid(CLK_OUTPUT2, PCIE_PCLK)
#define eth_alt_ref_clk2	clkid(CLK_OUTPUT2, ETH_ALT_REF_CLK)
#define sgmii_ref_clk2		clkid(CLK_OUTPUT2, SGMII_REF_CLK)
#endif /* CONFIG_DUOWEN_SBI_DUAL */
/* Alias for AO domain clocks */
#define ram_clk			soc_clk
#define rom_clk			soc_clk
/* clocke/reset alias required by DDR/PCIe drivers */
#define ddr_prst		ddr_pclk
#define ddr_arst		ddr_aclk
#define pcie_aux_clk		xo_clk
#define pcie_aclk		pcie_axi_clk
#define pcie_alt_ref_clk	pcie_clk
#define pcie_rst		pcie_alt_ref_clk
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define ram_clk2		soc_clk2
#define rom_clk2		soc_clk2
/* clocke/reset alias required by DDR/PCIe drivers */
#define ddr_prst2		ddr_pclk2
#define ddr_arst2		ddr_aclk2
#define pcie_aux_clk2		xo_clk2
#define pcie_aclk2		pcie_axi_clk2
#define pcie_alt_ref_clk2	pcie_clk2
#define pcie_rst2		pcie_alt_ref_clk2
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* CLK_OUTPUT: CLK_EN_CFG1 */
/* Integrated overall clocks */
#define gpio0_clk		clkid(CLK_OUTPUT, GPIO0_CLK)
#define gpio1_clk		clkid(CLK_OUTPUT, GPIO1_CLK)
#define gpio2_clk		clkid(CLK_OUTPUT, GPIO2_CLK)
#define uart0_clk		clkid(CLK_OUTPUT, UART0_CLK)
#define uart1_clk		clkid(CLK_OUTPUT, UART1_CLK)
#define uart2_clk		clkid(CLK_OUTPUT, UART2_CLK)
#define uart3_clk		clkid(CLK_OUTPUT, UART3_CLK)
#define i2c0_clk		clkid(CLK_OUTPUT, I2C0_CLK)
#define i2c1_clk		clkid(CLK_OUTPUT, I2C1_CLK)
#define i2c2_clk		clkid(CLK_OUTPUT, I2C2_CLK)
#define i2c3_clk		clkid(CLK_OUTPUT, I2C3_CLK)
#define i2c4_clk		clkid(CLK_OUTPUT, I2C4_CLK)
#define i2c5_clk		clkid(CLK_OUTPUT, I2C5_CLK)
#define i2c6_clk		clkid(CLK_OUTPUT, I2C6_CLK)
#define i2c7_clk		clkid(CLK_OUTPUT, I2C7_CLK)
#define i2c8_clk		clkid(CLK_OUTPUT, I2C8_CLK)
#define i2c9_clk		clkid(CLK_OUTPUT, I2C9_CLK)
#define i2c10_clk		clkid(CLK_OUTPUT, I2C10_CLK)
#define i2c11_clk		clkid(CLK_OUTPUT, I2C11_CLK)
#define spi_flash_clk		clkid(CLK_OUTPUT, SPI_FLASH_CLK)
#define spi0_clk		clkid(CLK_OUTPUT, SPI0_CLK)
#define spi1_clk		clkid(CLK_OUTPUT, SPI1_CLK)
#define spi2_clk		clkid(CLK_OUTPUT, SPI2_CLK)
#define spi3_clk		clkid(CLK_OUTPUT, SPI3_CLK)
#define spi4_clk		clkid(CLK_OUTPUT, SPI4_CLK)
#define sd_clk			clkid(CLK_OUTPUT, SD_CLK)
#define eth_clk			clkid(CLK_OUTPUT, ETH_CLK)
#define tsensor_clk		clkid(CLK_OUTPUT, TSENSOR_CLK)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define gpio0_clk2		clkid(CLK_OUTPUT2, GPIO0_CLK)
#define gpio1_clk2		clkid(CLK_OUTPUT2, GPIO1_CLK)
#define gpio2_clk2		clkid(CLK_OUTPUT2, GPIO2_CLK)
#define uart0_clk2		clkid(CLK_OUTPUT2, UART0_CLK)
#define uart1_clk2		clkid(CLK_OUTPUT2, UART1_CLK)
#define uart2_clk2		clkid(CLK_OUTPUT2, UART2_CLK)
#define uart3_clk2		clkid(CLK_OUTPUT2, UART3_CLK)
#define i2c0_clk2		clkid(CLK_OUTPUT2, I2C0_CLK)
#define i2c1_clk2		clkid(CLK_OUTPUT2, I2C1_CLK)
#define i2c2_clk2		clkid(CLK_OUTPUT2, I2C2_CLK)
#define i2c3_clk2		clkid(CLK_OUTPUT2, I2C3_CLK)
#define i2c4_clk2		clkid(CLK_OUTPUT2, I2C4_CLK)
#define i2c5_clk2		clkid(CLK_OUTPUT2, I2C5_CLK)
#define i2c6_clk2		clkid(CLK_OUTPUT2, I2C6_CLK)
#define i2c7_clk2		clkid(CLK_OUTPUT2, I2C7_CLK)
#define i2c8_clk2		clkid(CLK_OUTPUT2, I2C8_CLK)
#define i2c9_clk2		clkid(CLK_OUTPUT2, I2C9_CLK)
#define i2c10_clk2		clkid(CLK_OUTPUT2, I2C10_CLK)
#define i2c11_clk2		clkid(CLK_OUTPUT2, I2C11_CLK)
#define spi_flash_clk2		clkid(CLK_OUTPUT2, SPI_FLASH_CLK)
#define spi0_clk2		clkid(CLK_OUTPUT2, SPI0_CLK)
#define spi1_clk2		clkid(CLK_OUTPUT2, SPI1_CLK)
#define spi2_clk2		clkid(CLK_OUTPUT2, SPI2_CLK)
#define spi3_clk2		clkid(CLK_OUTPUT2, SPI3_CLK)
#define spi4_clk2		clkid(CLK_OUTPUT2, SPI4_CLK)
#define sd_clk2			clkid(CLK_OUTPUT2, SD_CLK)
#define eth_clk2		clkid(CLK_OUTPUT2, ETH_CLK)
#define tsensor_clk2		clkid(CLK_OUTPUT2, TSENSOR_CLK)
#endif /* CONFIG_DUOWEN_SBI_DUAL */
/* TODO: dependency clocks:
 * sd_tm_clk: depends sd_clk, sources soc_pll_div10/100
 */

/* CLK_OUTPUT: CLK_EN_CFG2 */

/* CLK_OUTPUT: CLK_EN_CFG3 */
/* Integrated overall clocks */
#define imc_clk			clkid(CLK_OUTPUT, IMC_CLK)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define imc_clk2		clkid(CLK_OUTPUT2, IMC_CLK)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* CLK_OUTPUT: COHFAB/CLUSTER CLK_CFG */
#define cohfab_clk		clkid(CLK_OUTPUT, COHFAB_CLK)
#define cluster0_clk		clkid(CLK_OUTPUT, CLUSTER0_CLK)
#define cluster1_clk		clkid(CLK_OUTPUT, CLUSTER1_CLK)
#define cluster2_clk		clkid(CLK_OUTPUT, CLUSTER2_CLK)
#define cluster3_clk		clkid(CLK_OUTPUT, CLUSTER3_CLK)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define cohfab_clk2		clkid(CLK_OUTPUT2, COHFAB_CLK)
#define cluster0_clk2		clkid(CLK_OUTPUT2, CLUSTER0_CLK)
#define cluster1_clk2		clkid(CLK_OUTPUT2, CLUSTER1_CLK)
#define cluster2_clk2		clkid(CLK_OUTPUT2, CLUSTER2_CLK)
#define cluster3_clk2		clkid(CLK_OUTPUT2, CLUSTER3_CLK)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* CLK_OUT: CLUSTER internal CLK_CG/RST_CTRL */
#define cluster0_apc0_cpu0_clk	clkid(CLK_OUTPUT, CLUSTER0_APC0_CPU0_CLK)
#define cluster0_apc0_cpu1_clk	clkid(CLK_OUTPUT, CLUSTER0_APC0_CPU1_CLK)
#define cluster0_apc1_cpu0_clk	clkid(CLK_OUTPUT, CLUSTER0_APC1_CPU0_CLK)
#define cluster0_apc1_cpu1_clk	clkid(CLK_OUTPUT, CLUSTER0_APC1_CPU1_CLK)
#define cluster0_apc0_l2_clk	clkid(CLK_OUTPUT, CLUSTER0_APC0_L2_CLK)
#define cluster0_apc1_l2_clk	clkid(CLK_OUTPUT, CLUSTER0_APC1_L2_CLK)
#define cluster1_apc0_cpu0_clk	clkid(CLK_OUTPUT, CLUSTER1_APC0_CPU0_CLK)
#define cluster1_apc0_cpu1_clk	clkid(CLK_OUTPUT, CLUSTER1_APC0_CPU1_CLK)
#define cluster1_apc1_cpu0_clk	clkid(CLK_OUTPUT, CLUSTER1_APC1_CPU0_CLK)
#define cluster1_apc1_cpu1_clk	clkid(CLK_OUTPUT, CLUSTER1_APC1_CPU1_CLK)
#define cluster1_apc0_l2_clk	clkid(CLK_OUTPUT, CLUSTER1_APC0_L2_CLK)
#define cluster1_apc1_l2_clk	clkid(CLK_OUTPUT, CLUSTER1_APC1_L2_CLK)
#define cluster2_apc0_cpu0_clk	clkid(CLK_OUTPUT, CLUSTER2_APC0_CPU0_CLK)
#define cluster2_apc0_cpu1_clk	clkid(CLK_OUTPUT, CLUSTER2_APC0_CPU1_CLK)
#define cluster2_apc1_cpu0_clk	clkid(CLK_OUTPUT, CLUSTER2_APC1_CPU0_CLK)
#define cluster2_apc1_cpu1_clk	clkid(CLK_OUTPUT, CLUSTER2_APC1_CPU1_CLK)
#define cluster2_apc0_l2_clk	clkid(CLK_OUTPUT, CLUSTER2_APC0_L2_CLK)
#define cluster2_apc1_l2_clk	clkid(CLK_OUTPUT, CLUSTER2_APC1_L2_CLK)
#define cluster3_apc0_cpu0_clk	clkid(CLK_OUTPUT, CLUSTER3_APC0_CPU0_CLK)
#define cluster3_apc0_cpu1_clk	clkid(CLK_OUTPUT, CLUSTER3_APC0_CPU1_CLK)
#define cluster3_apc1_cpu0_clk	clkid(CLK_OUTPUT, CLUSTER3_APC1_CPU0_CLK)
#define cluster3_apc1_cpu1_clk	clkid(CLK_OUTPUT, CLUSTER3_APC1_CPU1_CLK)
#define cluster3_apc0_l2_clk	clkid(CLK_OUTPUT, CLUSTER3_APC0_L2_CLK)
#define cluster3_apc1_l2_clk	clkid(CLK_OUTPUT, CLUSTER3_APC1_L2_CLK)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define cluster0_apc0_cpu0_clk2	clkid(CLK_OUTPUT2, CLUSTER0_APC0_CPU0_CLK)
#define cluster0_apc0_cpu1_clk2	clkid(CLK_OUTPUT2, CLUSTER0_APC0_CPU1_CLK)
#define cluster0_apc1_cpu0_clk2	clkid(CLK_OUTPUT2, CLUSTER0_APC1_CPU0_CLK)
#define cluster0_apc1_cpu1_clk2	clkid(CLK_OUTPUT2, CLUSTER0_APC1_CPU1_CLK)
#define cluster0_apc0_l2_clk2	clkid(CLK_OUTPUT2, CLUSTER0_APC0_L2_CLK)
#define cluster0_apc1_l2_clk2	clkid(CLK_OUTPUT2, CLUSTER0_APC1_L2_CLK)
#define cluster1_apc0_cpu0_clk2	clkid(CLK_OUTPUT2, CLUSTER1_APC0_CPU0_CLK)
#define cluster1_apc0_cpu1_clk2	clkid(CLK_OUTPUT2, CLUSTER1_APC0_CPU1_CLK)
#define cluster1_apc1_cpu0_clk2	clkid(CLK_OUTPUT2, CLUSTER1_APC1_CPU0_CLK)
#define cluster1_apc1_cpu1_clk2	clkid(CLK_OUTPUT2, CLUSTER1_APC1_CPU1_CLK)
#define cluster1_apc0_l2_clk2	clkid(CLK_OUTPUT2, CLUSTER1_APC0_L2_CLK)
#define cluster1_apc1_l2_clk2	clkid(CLK_OUTPUT2, CLUSTER1_APC1_L2_CLK)
#define cluster2_apc0_cpu0_clk2	clkid(CLK_OUTPUT2, CLUSTER2_APC0_CPU0_CLK)
#define cluster2_apc0_cpu1_clk2	clkid(CLK_OUTPUT2, CLUSTER2_APC0_CPU1_CLK)
#define cluster2_apc1_cpu0_clk2	clkid(CLK_OUTPUT2, CLUSTER2_APC1_CPU0_CLK)
#define cluster2_apc1_cpu1_clk2	clkid(CLK_OUTPUT2, CLUSTER2_APC1_CPU1_CLK)
#define cluster2_apc0_l2_clk2	clkid(CLK_OUTPUT2, CLUSTER2_APC0_L2_CLK)
#define cluster2_apc1_l2_clk2	clkid(CLK_OUTPUT2, CLUSTER2_APC1_L2_CLK)
#define cluster3_apc0_cpu0_clk2	clkid(CLK_OUTPUT2, CLUSTER3_APC0_CPU0_CLK)
#define cluster3_apc0_cpu1_clk2	clkid(CLK_OUTPUT2, CLUSTER3_APC0_CPU1_CLK)
#define cluster3_apc1_cpu0_clk2	clkid(CLK_OUTPUT2, CLUSTER3_APC1_CPU0_CLK)
#define cluster3_apc1_cpu1_clk2	clkid(CLK_OUTPUT2, CLUSTER3_APC1_CPU1_CLK)
#define cluster3_apc0_l2_clk2	clkid(CLK_OUTPUT2, CLUSTER3_APC0_L2_CLK)
#define cluster3_apc1_l2_clk2	clkid(CLK_OUTPUT2, CLUSTER3_APC1_L2_CLK)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* CLK_DIV */
#define SOC_CLK_DIV2		0
#define DDR_CLK_SEL_DIV4	1
#define XO_CLK_DIV4		2
#define ETH_CLK_DIV2		3
#define NR_DIV_CLKS		(ETH_CLK_DIV2 + 1)
#define soc_clk_div2		clkid(CLK_DIV, SOC_CLK_DIV2)
#define ddr_clk_sel_div4	clkid(CLK_DIV, DDR_CLK_SEL_DIV4)
#define xo_clk_div4		clkid(CLK_DIV, XO_CLK_DIV4)
#define eth_clk_div2		clkid(CLK_DIV, ETH_CLK_DIV2)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define CLK_DIV2		(CLK_DIV | CLK_DUAL)
#define soc_clk_div22		clkid(CLK_DIV2, SOC_CLK_DIV2)
#define ddr_clk_sel_div42	clkid(CLK_DIV2, DDR_CLK_SEL_DIV4)
#define xo_clk_div42		clkid(CLK_DIV2, XO_CLK_DIV4)
#define eth_clk_div22		clkid(CLK_DIV2, ETH_CLK_DIV2)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

/* Linux clk-duowen bindings */
#define sbi_soc_clk		0
#define sbi_sysfab_clk		1
#define sbi_cohfab_clk		2
#define sbi_cluster0_clk	3
#define sbi_cluster1_clk	4
#define sbi_cluster2_clk	5
#define sbi_cluster3_clk	6
#define sbi_dma_clk		7
#define sbi_tmr0_clk		8
#define sbi_tmr1_clk		9
#define sbi_tmr2_clk		10
#define sbi_wdt0_clk		11
#define sbi_wdt1_clk		12
#define sbi_tlmm_clk		13
#define sbi_gpio0_clk		14
#define sbi_gpio1_clk		15
#define sbi_gpio2_clk		16
#define sbi_uart0_clk		17
#define sbi_uart1_clk		18
#define sbi_uart2_clk		19
#define sbi_uart3_clk		20
#define sbi_i2c0_clk		21
#define sbi_i2c1_clk		22
#define sbi_i2c2_clk		23
#define sbi_i2c3_clk		24
#define sbi_i2c4_clk		25
#define sbi_i2c5_clk		26
#define sbi_i2c6_clk		27
#define sbi_i2c7_clk		28
#define sbi_i2c8_clk		29
#define sbi_i2c9_clk		30
#define sbi_i2c10_clk		31
#define sbi_i2c11_clk		32
#define sbi_spi0_clk		33
#define sbi_spi1_clk		34
#define sbi_spi2_clk		35
#define sbi_spi3_clk		36
#define sbi_spi4_clk		37
#define sbi_sd_clk		38
#define sbi_tsensor_clk		39
#define sbi_pcie_pclk		40
#define sbi_pcie_aclk		41
#define sbi_pcie_aux_clk	42
#define sbi_pcie_alt_ref_clk	43
#define sbi_eth_alt_ref_clk	44
#define sbi_sgmii_ref_clk	45

#ifdef CONFIG_DUOWEN_SBI_DUAL
#define sbi_dual_clk		64
#define sbi_soc_clk2		(sbi_soc_clk | sbi_dual_clk)
#define sbi_sysfab_clk2		(sbi_sysfab_clk | sbi_dual_clk)
#define sbi_cohfab_clk2		(sbi_cohfab_clk | sbi_dual_clk)
#define sbi_cluster0_clk2	(sbi_cluster0_clk | sbi_dual_clk)
#define sbi_cluster1_clk2	(sbi_cluster1_clk | sbi_dual_clk)
#define sbi_cluster2_clk2	(sbi_cluster2_clk | sbi_dual_clk)
#define sbi_cluster3_clk2	(sbi_cluster3_clk | sbi_dual_clk)
#define sbi_dma_clk2		(sbi_dma_clk | sbi_dual_clk)
#define sbi_tmr0_clk2		(sbi_tmr0_clk | sbi_dual_clk)
#define sbi_tmr1_clk2		(sbi_tmr1_clk | sbi_dual_clk)
#define sbi_tmr2_clk2		(sbi_tmr2_clk | sbi_dual_clk)
#define sbi_wdt0_clk2		(sbi_wdt0_clk | sbi_dual_clk)
#define sbi_wdt1_clk2		(sbi_wdt1_clk | sbi_dual_clk)
#define sbi_tlmm_clk2		(sbi_tlmm_clk | sbi_dual_clk)
#define sbi_gpio0_clk2		(sbi_gpio0_clk | sbi_dual_clk)
#define sbi_gpio1_clk2		(sbi_gpio1_clk | sbi_dual_clk)
#define sbi_gpio2_clk2		(sbi_gpio2_clk | sbi_dual_clk)
#define sbi_uart0_clk2		(sbi_uart0_clk | sbi_dual_clk)
#define sbi_uart1_clk2		(sbi_uart1_clk | sbi_dual_clk)
#define sbi_uart2_clk2		(sbi_uart2_clk | sbi_dual_clk)
#define sbi_uart3_clk2		(sbi_uart3_clk | sbi_dual_clk)
#define sbi_i2c0_clk2		(sbi_i2c0_clk | sbi_dual_clk)
#define sbi_i2c1_clk2		(sbi_i2c1_clk | sbi_dual_clk)
#define sbi_i2c2_clk2		(sbi_i2c2_clk | sbi_dual_clk)
#define sbi_i2c3_clk2		(sbi_i2c3_clk | sbi_dual_clk)
#define sbi_i2c4_clk2		(sbi_i2c4_clk | sbi_dual_clk)
#define sbi_i2c5_clk2		(sbi_i2c5_clk | sbi_dual_clk)
#define sbi_i2c6_clk2		(sbi_i2c6_clk | sbi_dual_clk)
#define sbi_i2c7_clk2		(sbi_i2c7_clk | sbi_dual_clk)
#define sbi_i2c8_clk2		(sbi_i2c8_clk | sbi_dual_clk)
#define sbi_i2c9_clk2		(sbi_i2c9_clk | sbi_dual_clk)
#define sbi_i2c10_clk2		(sbi_i2c10_clk | sbi_dual_clk)
#define sbi_i2c11_clk2		(sbi_i2c11_clk | sbi_dual_clk)
#define sbi_spi0_clk2		(sbi_spi0_clk | sbi_dual_clk)
#define sbi_spi1_clk2		(sbi_spi1_clk | sbi_dual_clk)
#define sbi_spi2_clk2		(sbi_spi2_clk | sbi_dual_clk)
#define sbi_spi3_clk2		(sbi_spi3_clk | sbi_dual_clk)
#define sbi_spi4_clk2		(sbi_spi4_clk | sbi_dual_clk)
#define sbi_sd_clk2		(sbi_sd_clk | sbi_dual_clk)
#define sbi_tsensor_clk2	(sbi_tsensor_clk | sbi_dual_clk)
#define sbi_pcie_pclk2		(sbi_pcie_pclk | sbi_dual_clk)
#define sbi_pcie_aclk2		(sbi_pcie_aclk | sbi_dual_clk)
#define sbi_pcie_aux_clk2	(sbi_pcie_aux_clk | sbi_dual_clk)
#define sbi_pcie_alt_ref_clk2	(sbi_pcie_alt_ref_clk | sbi_dual_clk)
#define sbi_eth_alt_ref_clk2	(sbi_eth_alt_ref_clk | sbi_dual_clk)
#define sbi_sgmii_ref_clk2	(sbi_sgmii_ref_clk | sbi_dual_clk)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

#endif /* __DT_BINDINGS_CLOCK_SBI_DUOWEN_H */
