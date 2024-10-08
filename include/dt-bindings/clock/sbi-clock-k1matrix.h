/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2023 SpacemiT Co., Ltd.
 *    Author: 2022 Lv Zheng <lv.zheng@spacemit.com>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_K1MATRIX_H
#define __DT_BINDINGS_CLOCK_SBI_K1MATRIX_H

#ifdef CONFIG_K1MATRIX_ASIC
#define OSC_CLK_FREQ		25000000
#define COM_PLL_FREQ		1200000000
#define MESH_PLL_FREQ		1500000000
#define PERI_PLL_FREQ		1000000000
#define DDR0_PLL_FREQ		1200000000
#define DDR1_PLL_FREQ		1000000000
#define CPU0_PLL_FREQ		2500000000
#define CPU1_PLL_FREQ		2000000000
#endif

#ifdef CONFIG_K1MATRIX_S2C
#define S2CCLK_1_FREQ		20000000
#define S2CCLK_2_FREQ		20000000
#define S2CCLK_3_FREQ		20000000
#define S2CCLK_4_FREQ		20000000
#define S2CCLK_5_FREQ		20000000
#define OSC_CLK_FREQ		20000000
#define COM_PLL_FREQ		S2CCLK_1_FREQ
#define MESH_PLL_FREQ		S2CCLK_3_FREQ
#define PERI_PLL_FREQ		S2CCLK_2_FREQ
#define DDR0_PLL_FREQ		S2CCLK_4_FREQ
#define DDR1_PLL_FREQ		S2CCLK_4_FREQ
#define CPU0_PLL_FREQ		S2CCLK_3_FREQ
#define CPU1_PLL_FREQ		S2CCLK_3_FREQ
#define COM_FOUT1PH0_FREQ	S2CCLK_1_FREQ
#define PERI_FOUT1PH0_FREQ	S2CCLK_2_FREQ
#define mac_clk1		phy2gmac_rx_clk
#endif

#ifdef CONFIG_K1MATRIX_PZ1
#define OSC_CLK_FREQ            50000000
#define COM_PLL_FREQ            125000000
#define MESH_PLL_FREQ           166666667
#define PERI_PLL_FREQ           781250000
#define DDR0_PLL_FREQ           125000000
#define DDR1_PLL_FREQ           781250000
#define CPU0_PLL_FREQ           2500000000
#define CPU1_PLL_FREQ           1250000000
#define COM_FOUT1PH0_FREQ	45000000
#define PERI_FOUT1PH0_FREQ	500000000

#define PERI_SUB_MCLK_FREQ	500000000
#define PERI_SUB_SCLK_FREQ	300000000
#define PHY2GMAC_RX_CLK_FREQ	125000000

#define mac_clk1		phy2gmac_rx_clk

#define GMAC_125M		PERI_SUB_MCLK_FREQ/4
#define GMAC_50M		PERI_SUB_MCLK_FREQ/10
#define GMAC_25M		GMAC_50M/2
#define GMAC_2_5M		GMAC_25M/10
#endif
/*
#ifdef CONFIG_K1MATRIX_64C
#define OSC_CLK_FREQ            25000000
#define COM_PLL_FREQ            1200000000
#define MESH_PLL_FREQ           1500000000
#define PERI_PLL_FREQ           1000000000
#define DDR0_PLL_FREQ           1200000000
#define DDR1_PLL_FREQ           1000000000
#define CPU0_PLL_FREQ           2500000000
#define CPU1_PLL_FREQ           2000000000
#define COM_FOUT1PH0_FREQ	600000000
#define PERI_FOUT1PH0_FREQ	500000000
#endif
*/
#define CLK_INPUT		0
#define CLK_PLL			1
#define CLK_SEL			2
#endif

#include <dt-bindings/clock/sbi-clock.h>

#define CLK_INPUT		0
#define CLK_PLL			1
#define CLK_SEL			2
#define CLK_DYN			3
#define CLK_DIV			4
#define CLK_RST			5
#define CLK_MESH		6

/* INPUT */
#define OSC_CLK			0
#define PERI_SUB_MCLK		1
#define PERI_SUB_SCLK		2
#define PHY2GMAC_RX_CLK		3
#define NR_INPUT_CLKS		4

/* PLL */
#define COM_PLL			0
#define MESH_PLL		1
#define PERI_PLL		2
#define DDR0_PLL		3
#define DDR1_PLL		4
#define CPU0_PLL		5
#define CPU1_PLL		6
#define COM_FOUT1PH0		7
#define PERI_FOUT1PH0		8
#define NR_PLL_CLKS		9

/* MESH PLL */
#define MESH_CLK		0
#define NR_MESH_CLKS		1

/* DYNAMIC_PLL */
#define DDR_CLK			0
#define CPU_CLK			1
#define NR_DYN_CLKS		2

#ifdef CONFIG_K1MATRIX_32C
/* CLKSEL */
#define CPU_NIC_CLKSEL		0
#define PCIE_TOP_CFG_CLKSEL	1
#define PCIE_TOP_AUX_CLKSEL	2
#define PCIE_TOP_XCLKSEL	3
#define PCIE_BOT_CFG_CLKSEL	4
#define PCIE_BOT_AUX_CLKSEL	5
#define PCIE_BOT_XCLKSEL	6
#define PERI_SUB_CLKSEL		7
#define PERI_GMAC_TXCLK_SEL	8
#define RMU_CLKSEL		9
#define NR_SEL_CLKS		10
#endif

#ifdef CONFIG_K1MATRIX_64C
/*top*/
#define LTOP_CFG_CLKSEL		0
#define LTOP_PCIE_CFG_CLKSEL	1
#define LTOP_PCIE_AUX_CLKSEL	2
#define LTOP_PCIE_TFR_CLKSEL	3
#define LTOP_PHY_FW_CLKSEL	4
#define RTOP_CFG_CLKSEL		5
#define RTOP_PCIE_CFG_CLKSEL	6
#define RTOP_PCIE_AUX_CLKSEL	7
#define RTOP_PCIE_TFR_CLKSEL	8
#define RTOP_PHY_FW_CLKSEL	9
#define BOT_CFG_CLKSEL		10
#define BOT_PCIE_CFG_CLKSEL	11
#define BOT_PCIE_AUX_CLKSEL	12
#define BOT_PCIE_TFR_CLKSEL	13
#define BOT_PHY_FW_CLKSEL	14
#define CPU_NIC_CLKSEL		15
#define TRMU_IOPMP_CFG_CLKSEL	16
#define DDR_CFG_CLKSEL		17
#define TPERI_SUB_MCLKSEL	18
#define TPERI_SUB_SCLKSEL	19
#define HNI_PERI_CFG_SCLKSEL	20
#define RMU_CLK_SEL		21
#define RMU_EFUSE_S_CLK_SEL	22
#define RMU_EFUSE_NS_CLK_SEL	23
#define RMU_LPC_IO_CLK_SEL	24
#define RMU_PVTC_CLK_SEL	25
/*rmu*/
#define NR_SEL_CLKS		26
#endif

#ifdef CONFIG_K1MATRIX_32C
/* CLKDIV */
#define CPU_NIC_CLKDIV		0
#define CPU_HAP_CLKDIV		1
#define PCIE_TOP_CFG_CLKDIV	2
#define PCIE_TOP_AUX_CLKDIV	3
#define PCIE_BOT_CFG_CLKDIV	4
#define PCIE_BOT_AUX_CLKDIV	5
#define RMU_QSPI_CLKEN		6
#define RMU_LPC_CLKEN		7
#define RMU_ESPI_CLKEN		8
#define RMU_UART0_CLKEN		9
#define RMU_UART1_CLKEN		10
#define RMU_MAILBOX_S_CLKEN	11
#define RMU_MAILBOX_NS_CLKEN	12
#define PERI_SMBUS0_CLKEN	13
#define PERI_SMBUS1_CLKEN	14
#define PERI_I2C0_CLKEN		15
#define PERI_I2C1_CLKEN		16
#define PERI_I2C2_CLKEN		17
#define PERI_I2C3_CLKEN		18
#define PERI_I2C4_CLKEN		19
#define PERI_I2C5_CLKEN		20
#define PERI_UART0_CLKEN	21
#define PERI_UART1_CLKEN	22
#define PERI_UART2_CLKEN	23
#define PERI_UART3_CLKEN	24
#define PERI_GPIO0_DB_CLKDIV	25
#define PERI_GPIO1_DB_CLKDIV	26
#define PERI_GPIO2_DB_CLKDIV	27
#define PERI_GPIO3_DB_CLKDIV	28
#define PERI_GPIO4_DB_CLKDIV	29
#define PERI_DMAC_CLKEN		30
#define PERI_GMAC_AXI_CLKDIV	31
#define RMU_AXI_CLK_DIV		32
#define RMU_AHB_CLK_DIV		33
#define RMU_APB_CLK_DIV		34
#define PERI_GMAC_AHB_CLKDIV	35
#define SYS_APB_CFG_CLKEN	37
#define C0_CLK_CFG_EN		38
#define RMU_LPC_CLK_DIV		39
#define NR_DIV_CLKS		40
#endif

#ifdef CONFIG_K1MATRIX_64C
/*top*/
#define LTOP_CFG_CLKDIV		0
#define LTOP_PCIE_CFG_CLKDIV	1
#define LTOP_PCIE_AUX_CLKDIV	2
#define RTOP_CFG_CLKDIV		3
#define RTOP_PCIE_CFG_CLKDIV	4
#define RTOP_PCIE_AUX_CLKDIV	5
#define BOT_CFG_CLKDIV		6
#define BOT_PCIE_CFG_CLKDIV	7
#define BOT_PCIE_AUX_CLKDIV	8
#define CPU_NIC_CLKDIV		9
#define CPU_HAP_CLKDIV		10
#define TRMU_IOPMP_CFG_CLKDIV	11
#define DDR_CFG_CLKDIV		12
#define TPERI_SUB_SCLK_DIV	13
#define HNI_PERI_CFG_CLKDIV	14
/*rmu*/
#define RMU_AXI_CLK_DIV		15
#define RMU_LPC_LCLK_DIV	16
#define RMU_AHB_CLK_DIV		17
#define RMU_APB_CLK_DIV		18
/*pcie*/
#define C0_CLK_EN		19
#define RMU_UART0_CLKEN		20
#define NR_DIV_CLKS		21
#endif/*CONFIG_K1MATRIX_64C*/
#ifdef CONFIG_K1MATRIX_32C
/*CLKRSTN*/
#define CPU_SUB_RSTN		0
#define PCIE_TOP_RSTN		1
#define PCIE_BOT_RSTN		2
#define PERI_SUB_RSTN		3
#define MESH_SUB_RSTN		4
#define DDR_SUB_RSTN		5
#define RAS_SRST_N		6
#define CPU_SUB_SRST_N		7
#define RMU_SRAM_SW_RSTN	8
#define PCIE0_PERST_N		9
#define PCIE1_PERST_N		10
#define PCIE2_PERST_N		11
#define PCIE3_PERST_N		12
#define PCIE4_PERST_N		13
#define PCIE5_PERST_N		14
#define PCIE6_PERST_N		15
#define PCIE7_PERST_N		16
#define PCIE8_PERST_N		17
#define PCIE9_PERST_N		18
#define CLUSTER0_SRST_N		19
#define C0_CFG_SRST_N		20
#define C1_CFG_SRST_N		21
#define NR_RSTN_CLKS		22
#endif/*CONFIG_K1MATRIX_32C*/

#ifdef CONFIG_K1MATRIX_64C
#define MESH_SUB_RSTN		0
#define TPERI_SUB_RSTN		1
#define CLUSTER0_RST_N		2
#define C0_RST_N		3
#define CPU_SUB_RSTN		4
#define CPU_SUB_SRST_N		5
#define RMU_UART0_SW_RSTN	6
#define NR_RSTN_CLKS		7
#endif/*CONFIG_K1MATRIX_64C*/

#define SYS_CLK_FREQ		20000000
#define CFG_CLK_FREQ		10000000
#define DDR_CLK_FREQ		100000000

#define CPU_CLK_FREQ		SYS_CLK_FREQ
#ifdef CONFIG_K1MATRIX_S2C
#define APB_CLK_FREQ		(COM_FOUT1PH0_FREQ/2)
#else
#define APB_CLK_FREQ		(COM_FOUT1PH0_FREQ/2/4)

#endif
#define PIC_CLK_FREQ		CFG_CLK_FREQ

/* MESH2SYS CPU -> SYS_SUB_AXI
 * SYS2MESH DMA -> MESH AXI
 * DDR ACLK
 */
#define AXI_CLK_FREQ		SYS_CLK_FREQ

/* __DT_BINDINGS_CLOCK_SBI_K1MATRIX_H */
