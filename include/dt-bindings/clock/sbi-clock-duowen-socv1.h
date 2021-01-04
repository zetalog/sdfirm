/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2021 CAS SmartCore Co., Ltd.
 *    Author: 2021 Lv Zheng <zhenglv@smarco.cn>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_DUOWEN_SOCv1_H
#define __DT_BINDINGS_CLOCK_SBI_DUOWEN_SOCv1_H

#include <dt-bindings/clock/sbi-clock.h>

#ifdef CONFIG_DUOWEN_ASIC
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

/* CLK_PLL */
#define NR_PLL_CLKS		DUOWEN_MAX_PLLS
#define soc_pll			clkid(CLK_PLL, SOC_PLL)
#define ddr_bus_pll		clkid(CLK_PLL, DDR_BUS_PLL)
#define ddr_pll			clkid(CLK_PLL, DDR_PLL)
#define pcie_pll		clkid(CLK_PLL, PCIE_PLL)
#define cohfab_pll		clkid(CLK_PLL, COHFAB_PLL)
#define cl0_pll			clkid(CLK_PLL, CL0_PLL)
#define cl1_pll			clkid(CLK_PLL, CL1_PLL)
#define cl2_pll			clkid(CLK_PLL, CL2_PLL)
#define cl3_pll			clkid(CLK_PLL, CL3_PLL)

/* CLK_SELECT: CLK_SEL_CFG */
#define SOC_CLK_SEL		0
#define DDR_BUS_CLK_SEL		1
#define DDR_CLK_SEL		2
#define DDR_CLK_DIV4_SEL	3
#define SD_RX_TX_CLK_SEL	4
#define COHFAB_CLK_SEL		5
#define CL0_CLK_SEL		6
#define CL1_CLK_SEL		7
#define CL2_CLK_SEL		8
#define CL3_CLK_SEL		9
#define NR_SELECT_CLKS		(CL3_CLK_SEL + 1)
#define soc_clk_sel		clkid(CLK_SELECT, SOC_CLK_SEL)
#define ddr_bus_clk_sel		clkid(CLK_SELECT, DDR_BUS_CLK_SEL)
#define ddr_clk_sel		clkid(CLK_SELECT, DDR_CLK_SEL)
#define ddr_clk_div4_sel	clkid(CLK_SELECT, DDR_CLK_DIV4_SEL)
#define sd_rx_tx_clk_sel	clkid(CLK_SELECT, SD_RX_TX_CLK_SEL)
#define cohfab_clk_sel		clkid(CLK_SELECT, COHFAB_CLK_SEL)
#define cl0_clk_sel		clkid(CLK_SELECT, CL0_CLK_SEL)
#define cl1_clk_sel		clkid(CLK_SELECT, CL1_CLK_SEL)
#define cl2_clk_sel		clkid(CLK_SELECT, CL2_CLK_SEL)
#define cl3_clk_sel		clkid(CLK_SELECT, CL3_CLK_SEL)
#endif

#ifdef CONFIG_DUOWEN_ZEBU
#define CLK_INPUT		0
#define CLK_SELECT		1
#define CLK_OUTPUT		2
#define CLK_DIV			3

/* CLK_INPUT */
#define CLK_PLL			CLK_INPUT
#define XO_CLK			(DUOWEN_MAX_PLLS + 0)
#define TIC_CLK			(DUOWEN_MAX_PLLS + 1)
#define JTAG_CLK		(DUOWEN_MAX_PLLS + 2)
#define NR_INPUT_CLKS		(DUOWEN_MAX_PLLS + 1)
#define xo_clk			clkid(CLK_INPUT, XO_CLK)
#define tic_clk			clkid(CLK_INPUT, TIC_CLK)
#define jtag_clk		clkid(CLK_INPUT, JTAG_CLK)
#define soc_pll			clkid(CLK_PLL, SOC_PLL)
#define ddr_bus_pll		clkid(CLK_PLL, DDR_BUS_PLL)
#define ddr_pll			clkid(CLK_PLL, DDR_PLL)
#define pcie_pll		clkid(CLK_PLL, PCIE_PLL)
#define cohfab_pll		clkid(CLK_PLL, COHFAB_PLL)
#define cl0_pll			clkid(CLK_PLL, CL0_PLL)
#define cl1_pll			clkid(CLK_PLL, CL1_PLL)
#define cl2_pll			clkid(CLK_PLL, CL2_PLL)
#define cl3_pll			clkid(CLK_PLL, CL3_PLL)

/* CLK_SELECT: CLK_SEL_CFG */
#define SOC_CLK_SEL		0
#define DDR_BUS_CLK_SEL		1
#define DDR_CLK_SEL		2
#define DDR_CLK_DIV4_SEL	3
#define SD_RX_TX_CLK_SEL	4
#define COHFAB_CLK_SEL		5
#define CL0_CLK_SEL		6
#define CL1_CLK_SEL		7
#define CL2_CLK_SEL		8
#define CL3_CLK_SEL		9
#define NR_SELECT_CLKS		(CL3_CLK_SEL + 1)
#define soc_clk_sel		clkid(CLK_SELECT, SOC_CLK_SEL)
#define ddr_bus_clk_sel		clkid(CLK_SELECT, DDR_BUS_CLK_SEL)
#define ddr_clk_sel		clkid(CLK_SELECT, DDR_CLK_SEL)
#define ddr_clk_div4_sel	clkid(CLK_SELECT, DDR_CLK_DIV4_SEL)
#define sd_rx_tx_clk_sel	clkid(CLK_SELECT, SD_RX_TX_CLK_SEL)
#define cohfab_clk_sel		clkid(CLK_SELECT, COHFAB_CLK_SEL)
#define cl0_clk_sel		clkid(CLK_SELECT, CL0_CLK_SEL)
#define cl1_clk_sel		clkid(CLK_SELECT, CL1_CLK_SEL)
#define cl2_clk_sel		clkid(CLK_SELECT, CL2_CLK_SEL)
#define cl3_clk_sel		clkid(CLK_SELECT, CL3_CLK_SEL)
#endif

#ifdef CONFIG_DUOWEN_FPGA
#define CLK_INPUT		0
#define CLK_OUTPUT		1
#define CLK_DIV			2

/* CLK_INPUT */
#define CLK_PLL			CLK_INPUT
#define CLK_SELECT		CLK_INPUT
#define XO_CLK			(DUOWEN_MAX_PLLS + 0)
#define TIC_CLK			(DUOWEN_MAX_PLLS + 1)
#define JTAG_CLK		(DUOWEN_MAX_PLLS + 2)
#define NR_INPUT_CLKS		(DUOWEN_MAX_PLLS + 3)
#define xo_clk			clkid(CLK_INPUT, XO_CLK)
#define tic_clk			clkid(CLK_INPUT, TIC_CLK)
#define jtag_clk		clkid(CLK_INPUT, JTAG_CLK)
#define soc_pll			clkid(CLK_PLL, SOC_PLL)
#define ddr_bus_pll		clkid(CLK_PLL, DDR_BUS_PLL)
#define ddr_pll			clkid(CLK_PLL, DDR_PLL)
#define pcie_pll		clkid(CLK_PLL, PCIE_PLL)
#define cohfab_pll		clkid(CLK_PLL, COHFAB_PLL)
#define cl0_pll			clkid(CLK_PLL, CL0_PLL)
#define cl1_pll			clkid(CLK_PLL, CL1_PLL)
#define cl2_pll			clkid(CLK_PLL, CL2_PLL)
#define cl3_pll			clkid(CLK_PLL, CL3_PLL)
#define soc_clk_sel		soc_pll
#define ddr_bus_clk_sel		ddr_bus_pll
#define ddr_clk_sel		ddr_pll
#define ddr_clk_div4_sel	ddr_clk_sel_div4
#define sd_rx_tx_clk_sel	soc_pll_div10
#define cohfab_clk_sel		cohfab_pll
#define cl0_clk_sel		cl0_pll
#define cl1_clk_sel		cl1_pll
#define cl2_clk_sel		cl2_pll
#define cl3_clk_sel		cl3_pll
#endif

/* CLK_OUTPUT: CLK_EN_CFG0 */
#define DMA_CLK			0
#define PCIE_POR		1 /* SW_RST only */
#define PCIE_CLK		2
#define DDR_POR			4 /* SW_RST only */
#define DDR_BYPASS_PCLK		5 /* CLK_EN only */
#define DDR_PCLK		6
#define DDR_ACLK		7
#define DDR_CLK			8
#define SYSFAB_DBG_CLK		9
#define SYSFAB_TIC_CLK		10
#define CLUSTER0_HCLK		12
#define CLUSTER1_HCLK		13
#define CLUSTER2_HCLK		14
#define CLUSTER3_HCLK		15
#define COHFAB_HCLK		16
#define COHFAB_PCLK		17
#define SCSR_CLK		19
#define TLMM_CLK		20
#define PLIC_CLK		21
/* #define TIC_CLK			23  */
#define CORESIGHT_CLK		24
#define TIMER0_RST		25 /* SW_RST only */
#define TIMER1_RST		26 /* SW_RST only */
#define TIMER2_RST		27 /* SW_RST only */
#define TIMER3_RST		28 /* SW_RST only */
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
#define TSENSOR_CLK		63

/* CLK_OUTPUT: CLK_EN_CFG2 */
#define TIMER0_1_CLK		64 /* CLK_EN only */
#define TIMER0_2_CLK		65 /* CLK_EN only */
#define TIMER0_3_CLK		66 /* CLK_EN only */
#define TIMER0_4_CLK		67 /* CLK_EN only */
#define TIMER0_5_CLK		68 /* CLK_EN only */
#define TIMER0_6_CLK		69 /* CLK_EN only */
#define TIMER0_7_CLK		70 /* CLK_EN only */
#define TIMER0_8_CLK		71 /* CLK_EN only */
#define TIMER1_1_CLK		72 /* CLK_EN only */
#define TIMER1_2_CLK		73 /* CLK_EN only */
#define TIMER1_3_CLK		74 /* CLK_EN only */
#define TIMER1_4_CLK		75 /* CLK_EN only */
#define TIMER1_5_CLK		76 /* CLK_EN only */
#define TIMER1_6_CLK		77 /* CLK_EN only */
#define TIMER1_7_CLK		78 /* CLK_EN only */
#define TIMER1_8_CLK		79 /* CLK_EN only */
#define TIMER2_1_CLK		80 /* CLK_EN only */
#define TIMER2_2_CLK		81 /* CLK_EN only */
#define TIMER2_3_CLK		82 /* CLK_EN only */
#define TIMER2_4_CLK		83 /* CLK_EN only */
#define TIMER2_5_CLK		84 /* CLK_EN only */
#define TIMER2_6_CLK		85 /* CLK_EN only */
#define TIMER2_7_CLK		86 /* CLK_EN only */
#define TIMER2_8_CLK		87 /* CLK_EN only */
#define TIMER3_CLK		88 /* CLK_EN only */

/* CLK_OUTPUT: CLK_EN_CFG3 */
#define IMC_CLK			96

/* Additional output clocks */
#define COHFAB_CFG_CLK		97
#define DMA_HCLK		98
#define PCIE_PCLK		99
#define SD_BCLK			100
#define SD_CCLK			101
#define DDR_RST			102

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
#define pcie_por		clkid(CLK_OUTPUT, PCIE_POR)
#define pcie_clk		clkid(CLK_OUTPUT, PCIE_CLK)
#define sysfab_dbg_clk		clkid(CLK_OUTPUT, SYSFAB_DBG_CLK)
#define sysfab_tic_clk		clkid(CLK_OUTPUT, SYSFAB_TIC_CLK)
/* Internal bus clocks */
#define cluster0_hclk		clkid(CLK_OUTPUT, CLUSTER0_HCLK)
#define cluster1_hclk		clkid(CLK_OUTPUT, CLUSTER1_HCLK)
#define cluster2_hclk		clkid(CLK_OUTPUT, CLUSTER2_HCLK)
#define cluster3_hclk		clkid(CLK_OUTPUT, CLUSTER3_HCLK)
#define cohfab_hclk		clkid(CLK_OUTPUT, COHFAB_HCLK)
#define scsr_clk		clkid(CLK_OUTPUT, SCSR_CLK)
#define tlmm_clk		clkid(CLK_OUTPUT, TLMM_CLK)
#define plic_clk		clkid(CLK_OUTPUT, PLIC_CLK)
/* #define tic_clk		clkid(CLK_OUTPUT, TIC_CLK) */
#define coresight_clk		clkid(CLK_OUTPUT, CORESIGHT_CLK)
#define timer0_rst		clkid(CLK_OUTPUT, TIMER0_RST)
#define timer1_rst		clkid(CLK_OUTPUT, TIMER1_RST)
#define timer2_rst		clkid(CLK_OUTPUT, TIMER2_RST)
#define timer3_rst		clkid(CLK_OUTPUT, TIMER3_RST)
#define wdt0_clk		clkid(CLK_OUTPUT, WDT0_CLK)
#define wdt1_clk		clkid(CLK_OUTPUT, WDT1_CLK)
/* Additional clocks */
#define cohfab_cfg_clk		clkid(CLK_OUTPUT, COHFAB_CFG_CLK)
#define dma_hclk		clkid(CLK_OUTPUT, DMA_HCLK)
#define pcie_pclk		clkid(CLK_OUTPUT, PCIE_PCLK)
#define sd_bclk			clkid(CLK_OUTPUT, SD_BCLK)
#define sd_cclk			clkid(CLK_OUTPUT, SD_CCLK)
#define ddr_rst			clkid(CLK_OUTPUT, DDR_RST)
/* Alias for AO domain clocks */
#define ram_aclk		sysfab_clk
#define brom_hclk		sysfab_clk
#define tmr3_clk		timer3_clk
#define sd_hclk			sysfab_half_clk
/* clocke/reset alias required by DDR/PCIe drivers */
#define ddr_prst		ddr_pclk
#define ddr_arst		ddr_aclk
#define pcie_aux_clk		xo_clk
#define pcie_aclk		sysfab_clk
#define pcie_alt_ref_clk	pcie_clk
#define pcie_rst		pcie_alt_ref_clk

/* CLK_OUTPUT: CLK_EN_CFG1 */
/* Internal bus clocks */
#define gpio0_clk		clkid(CLK_OUTPUT, GPIO0_CLK)
#define gpio1_clk		clkid(CLK_OUTPUT, GPIO1_CLK)
#define gpio2_clk		clkid(CLK_OUTPUT, GPIO2_CLK)
/* Integrated overall clocks */
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
#define tsensor_clk		clkid(CLK_OUTPUT, TSENSOR_CLK)
/* TODO: dependency clocks:
 * sd_tm_clk: depends sd_clk, sources soc_pll_div10/100
 */

/* CLK_OUTPUT: CLK_EN_CFG2 */
/* Integrated overall clocks */
#define timer0_1_clk		clkid(CLK_OUTPUT, TIMER0_1_CLK)
#define timer0_2_clk		clkid(CLK_OUTPUT, TIMER0_2_CLK)
#define timer0_3_clk		clkid(CLK_OUTPUT, TIMER0_3_CLK)
#define timer0_4_clk		clkid(CLK_OUTPUT, TIMER0_4_CLK)
#define timer0_5_clk		clkid(CLK_OUTPUT, TIMER0_5_CLK)
#define timer0_6_clk		clkid(CLK_OUTPUT, TIMER0_6_CLK)
#define timer0_7_clk		clkid(CLK_OUTPUT, TIMER0_7_CLK)
#define timer0_8_clk		clkid(CLK_OUTPUT, TIMER0_8_CLK)
#define timer1_1_clk		clkid(CLK_OUTPUT, TIMER1_1_CLK)
#define timer1_2_clk		clkid(CLK_OUTPUT, TIMER1_2_CLK)
#define timer1_3_clk		clkid(CLK_OUTPUT, TIMER1_3_CLK)
#define timer1_4_clk		clkid(CLK_OUTPUT, TIMER1_4_CLK)
#define timer1_5_clk		clkid(CLK_OUTPUT, TIMER1_5_CLK)
#define timer1_6_clk		clkid(CLK_OUTPUT, TIMER1_6_CLK)
#define timer1_7_clk		clkid(CLK_OUTPUT, TIMER1_7_CLK)
#define timer1_8_clk		clkid(CLK_OUTPUT, TIMER1_8_CLK)
#define timer2_1_clk		clkid(CLK_OUTPUT, TIMER2_1_CLK)
#define timer2_2_clk		clkid(CLK_OUTPUT, TIMER2_2_CLK)
#define timer2_3_clk		clkid(CLK_OUTPUT, TIMER2_3_CLK)
#define timer2_4_clk		clkid(CLK_OUTPUT, TIMER2_4_CLK)
#define timer2_5_clk		clkid(CLK_OUTPUT, TIMER2_5_CLK)
#define timer2_6_clk		clkid(CLK_OUTPUT, TIMER2_6_CLK)
#define timer2_7_clk		clkid(CLK_OUTPUT, TIMER2_7_CLK)
#define timer2_8_clk		clkid(CLK_OUTPUT, TIMER2_8_CLK)
#define timer3_clk		clkid(CLK_OUTPUT, TIMER3_CLK)

/* CLK_OUTPUT: CLK_EN_CFG3 */
/* Integrated overall clocks */
#define imc_clk			clkid(CLK_OUTPUT, IMC_CLK)

/* CLK_OUTPUT: COHFAB/CLUSTER CLK_CFG */
#define cohfab_clk		clkid(CLK_OUTPUT, COHFAB_CLK)
#define cluster0_clk		clkid(CLK_OUTPUT, CLUSTER0_CLK)
#define cluster1_clk		clkid(CLK_OUTPUT, CLUSTER1_CLK)
#define cluster2_clk		clkid(CLK_OUTPUT, CLUSTER2_CLK)
#define cluster3_clk		clkid(CLK_OUTPUT, CLUSTER3_CLK)

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

/* CLK_DIV */
#define SOC_PLL_DIV2		0
#define SOC_PLL_DIV4		1
#define SOC_PLL_DIV8		2
#define SOC_PLL_DIV10		3
#define SOC_PLL_DIV12		4
#define SOC_CLK_SEL_DIV2	5
#define DDR_CLK_SEL_DIV4	6
#define SD_TM_CLK		7
#define XO_CLK_DIV4		8
#define XO_CLK_DIV10		9
#define NR_DIV_CLKS		(XO_CLK_DIV10 + 1)
#define soc_pll_div4		clkid(CLK_DIV, SOC_PLL_DIV4)
#define soc_pll_div8		clkid(CLK_DIV, SOC_PLL_DIV8)
#define soc_pll_div2		clkid(CLK_DIV, SOC_PLL_DIV2)
#define soc_pll_div10		clkid(CLK_DIV, SOC_PLL_DIV10)
#define soc_pll_div12		clkid(CLK_DIV, SOC_PLL_DIV12)
#define soc_clk_sel_div2	clkid(CLK_DIV, SOC_CLK_SEL_DIV2)
#define ddr_clk_sel_div4	clkid(CLK_DIV, DDR_CLK_SEL_DIV4)
#define sd_tm_clk		clkid(CLK_DIV, SD_TM_CLK)
#define xo_clk_div4		clkid(CLK_DIV, XO_CLK_DIV4)
#define xo_clk_div10		clkid(CLK_DIV, XO_CLK_DIV10)
#define sysfab_clk		soc_clk_sel
#define sysfab_half_clk		soc_clk_sel_div2

#endif /* __DT_BINDINGS_CLOCK_SBI_DUOWEN_SOCv1_H */
