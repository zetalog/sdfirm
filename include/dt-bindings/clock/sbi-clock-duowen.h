/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  Copyright (C) 2021 CAS SmartCore Co., Ltd.
 *    Author: 2021 Lv Zheng <zhenglv@smarco.cn>
 */

#ifndef __DT_BINDINGS_CLOCK_SBI_DUOWEN_H
#define __DT_BINDINGS_CLOCK_SBI_DUOWEN_H

#include <dt-bindings/clock/sbi-clock.h>

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
#define DUOWEN_MAX_PLLS		10

#ifdef CONFIG_DUOWEN_SOCv1
#include <dt-bindings/clock/sbi-clock-duowen-socv1.h>
#endif
#ifdef CONFIG_DUOWEN_SOCv2
#include <dt-bindings/clock/sbi-clock-duowen-socv2.h>
#endif

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

#endif /* __DT_BINDINGS_CLOCK_SBI_DUOWEN_H */
