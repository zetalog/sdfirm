/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2023 SpacemiT Co., Ltd.
 *    Author: 2022 Lv Zheng <lv.zheng@spacemit.com>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_K1MATRIX_H
#define __DT_BINDINGS_CLOCK_SBI_K1MATRIX_H

#define OSC_CLK_FREQ		25000000
#define COM_PLL_FREQ		1200000000
#define MESH_PLL_FREQ		1500000000
#define PERI_PLL_FREQ		1000000000
#define DDR0_PLL_FREQ		1200000000
#define DDR1_PLL_FREQ		1000000000
#define CPU0_PLL_FREQ		2500000000
#define CPU1_PLL_FREQ		2000000000

#include <dt-bindings/clock/sbi-clock.h>

#define CLK_INPUT		0
#define CLK_PLL			1
#define CLK_SEL			2
#define CLK_DYN			3
#define CLK_DIV			4
#define CLK_OUTPUT		5
#define CLK_RESET		6

/* INPUT */
#define OSC_CLK			0
#define NR_INPUT_CLKS		1

/* PLL */
#define COM_PLL			0
#define MESH_PLL		1
#define PERI_PLL		2
#define DDR0_PLL		3
#define DDR1_PLL		4
#define CPU0_PLL		5
#define CPU1_PLL		6
#define NR_PLL_CLKS		7

/* DYNAMIC_PLL */
#define DDR_CLK			0
#define CPU_CLK			1
#define NR_DYN_CLKS		2

/* CLKSEL */
#define MESH_SUB_CLKSEL		0
#define DDR_SUB_CLKSEL		1
#define CPU_CLKSEL		2
#define NR_SEL_CLKS		3

/* CLKDIV */
#define CPU_NIC_CLKDIV		0
#define CPU_HAP_CLKDIV		1
#define PCIE_TOP_AUX_CLKDIV	2
#define PCIE_TOP_CFG_CLKDIV	3
#define PCIE_BOT_AUX_CLKDIV	4
#define PCIE_BOT_CFG_CLKDIV	5

#define SYS_CLK_FREQ		20000000
#define CFG_CLK_FREQ		10000000
#define DDR_CLK_FREQ		100000000

#define CPU_CLK_FREQ		SYS_CLK_FREQ
#define APB_CLK_FREQ		CFG_CLK_FREQ
#define PIC_CLK_FREQ		CFG_CLK_FREQ

/* MESH2SYS CPU -> SYS_SUB_AXI
 * SYS2MESH DMA -> MESH AXI
 * DDR ACLK
 */
#define AXI_CLK_FREQ		SYS_CLK_FREQ

#endif /* __DT_BINDINGS_CLOCK_SBI_K1MATRIX_H */
