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
 * @(#)clk_asic.c: DUOWEN ASIC frequency plan implementation
 * $Id: clk_asic.c,v 1.1 2020-06-30 14:31:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/panic.h>
#include <target/cmdline.h>

struct output_clk {
	clk_t clk_dep;
	clk_t clk_src;
	uint8_t flags;
};

struct output_clk output_clks[] = {
	/* 4.2 Cluster Clocks
	 * +-----------------------------+
	 * |                             v
	 * +----------> +-----------+ -> +---------------+ -> cluster0-3_clk
	 * |            | cl0-3_pll |    | cl0-3_clk_sel |
	 * |            +-----------+    +---------------+
	 * +-----------------------------+
	 * |                             v
	 * +--------+ -> +----------+ -> +--------------+ --> cluster_hclk
	 * | xo_clk |    | sfab_pll |    | sfab_clk_sel |
	 * +--------+    +----------+    +--------------+
	 */
	[CLUSTER0_CLK] = {
		.clk_dep = cluster0_hclk,
		.clk_src = cl0_clk_sel,
		.flags = CLK_CR | CLK_COHFAB_CFG_F,
	},
	[CLUSTER1_CLK] = {
		.clk_dep = cluster1_hclk,
		.clk_src = cl1_clk_sel,
		.flags = CLK_CR | CLK_COHFAB_CFG_F,
	},
	[CLUSTER2_CLK] = {
		.clk_dep = cluster2_hclk,
		.clk_src = cl2_clk_sel,
		.flags = CLK_CR | CLK_COHFAB_CFG_F,
	},
	[CLUSTER3_CLK] = {
		.clk_dep = cluster3_hclk,
		.clk_src = cl3_clk_sel,
		.flags = CLK_CR | CLK_COHFAB_CFG_F,
	},
	[CLUSTER0_HCLK] = {
		.clk_dep = cohfab_cfg_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[CLUSTER1_HCLK] = {
		.clk_dep = cohfab_cfg_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[CLUSTER2_HCLK] = {
		.clk_dep = cohfab_cfg_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[CLUSTER3_HCLK] = {
		.clk_dep = cohfab_cfg_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.3 Coherence Fabric Clocks
	 * +-----------------------------+
	 * |                             v
	 * +----------> +----------+ --> +--------------+ -> cohfab_clk
	 * |            | cfab_pll |     | cfab_clk_sel |
	 * |            +----------+     +--------------+
	 * +-----------------------------+
	 * |                             v
	 * +----------> +----------+ --> +--------------+ -> cohfab_hclk
	 * |            | sfab_pll |     | sfab_clk_sel |
	 * |            +----------+     +--------------+
	 * +-----------------------------+
	 * |                         /2  v
	 * +--------+ -> +---------+ --> +-------------+ --> cohfab_cfg_clk
	 * | xo_clk |    | soc_pll |     | soc_clk_sel |
	 * +--------+    +---------+     +-------------+
	 */
	[COHFAB_CLK] = {
		.clk_dep = cohfab_hclk,
		.clk_src = cohfab_clk_sel,
		.flags = CLK_CR | CLK_COHFAB_CFG_F,
	},
	[COHFAB_HCLK] = {
		.clk_dep = cohfab_cfg_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[COHFAB_CFG_CLK] = {
		.clk_dep = sysfab_clk,
		.clk_src = pcie_clk,
		.flags = 0,
	},
	/* 4.4 System Fabric Clocks
	 * +-----------------------------+
	 * |                             v
	 * +----------> +----------+ --> +--------------+ -> tlmm_pclk
	 * |            | sfab_pll |     | sfab_clk_sel |    plic_pclk
	 * |            +----------+     +--------------+    wdt0/1_pclk
	 * +-----------------------------+                   trm0/1/2/3_pclk
	 * |                         /2  v
	 * +--------+ -> +---------+ --> +-------------+ --> imc_clk
	 * | xo_clk |    | soc_pll |     | soc_clk_sel |     ram_aclk/rom_hclk
	 * +--------+    +---------+     +-------------+     scsr_hclk
	 * |
	 * +-----------------------------------------------> tmr3_clk
	 */
	[IMC_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_clk,
		.flags = CLK_CR,
	},
	[PLIC_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[TLMM_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[SCSR_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_clk,
		.flags = CLK_CR,
	},
	[LCSR_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = soc_clk,
		.flags = CLK_CR,
	},
	[WDT0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[WDT1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[TMR0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[TMR1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[TMR2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[TMR3_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.5 DMA Clocks
	 * +-----------------------------+
	 * |                             v
	 * +----------> +----------+ --> +--------------+ -> dma_hclk
	 * |            | sfab_pll |     | sfab_clk_sel |
	 * |            +----------+     +--------------+
	 * +-----------------------------+
	 * |                         /2  v
	 * +--------+ -> +---------+ --> +-------------+ --> dma_clk
	 * | xo_clk |    | soc_pll |     | soc_clk_sel |
	 * +--------+    +---------+     +-------------+
	 */
	[DMA_CLK] = {
		.clk_dep = sysfab_clk, /* dma_hclk */
		.clk_src = soc_clk,
		.flags = CLK_CR,
	},
	/* 4.6 DDR Clocks
	 * +----------------------------+
	 * |                            v
	 * +-------> +-------------+ -> +-----------------+ ------> ddr_aclk
	 * |         | ddr_bus_pll |    | ddr_bus_clk_sel |
	 * |         +-------------+    +-----------------+
	 * +----------------------------+
	 * |                            v
	 * +----------> +----------+ -> +--------------+ ---------> ddr_pclk
	 * |            | sfab_pll |    | sfab_clk_sel |
	 * |            +----------+    +--------------+
	 * +----------------------------+
	 * |                            v
	 * +--------+ -> +---------+ -> +-------------+ ---> ddr_bypass_pclk
	 * | xo_clk |    | ddr_pll |    | ddr_clk_sel |          
	 * +--------+    +---------+    +-------------+ ---+
	 *                            /4|                  |
	 *                              v                  v
	 *                              +------------------+ ------> ddr_clk
	 *                              | ddr_clk_sel_div4 |
	 *                              +------------------+
	 */
	[DDR_POR] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.flags = CLK_R,
	},
	[DDR_ACLK] = {
		.clk_dep = invalid_clk,
		.clk_src = ddr_bus_clk_sel,
		.flags = CLK_CR,
	},
	[DDR_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[DDR_BYPASS_PCLK] = {
		.clk_dep = ddr_clk,
		.clk_src = ddr_clk_sel_div4,
		.flags = CLK_C,
	},
	[DDR_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = ddr_clk_sel,
		.flags = CLK_C,
	},
	[DDR_RST] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.flags = CLK_R,
	},
	/* 4.7 PCIE Clocks
	 *                                       +-----+ -> pcie_alt_ref_clk_n
	 *                                       | inv |
	 * +----------------------------+        +-----+
	 * |                            v              ^
	 * +----------> +----------+ -> +--------------+ -> pcie_alt_ref_clk_p
	 * |            | pcie_pll |    | pcie_clk_sel |
	 * |            +----------+    +--------------+
	 * +----------------------------+
	 * |                            v
	 * +------------+----------+ -> +--------------+ -> pcie_pclk
	 * |            | sfab_pll |    | sfab_clk_sel |
	 * |            +----------+    +--------------+
	 * +----------------------------+
	 * |                            v
	 * +--------+ -> +---------+ -> +-------------+ --> pcie_aclk
	 * | xo_clk |    | soc_pll |    | soc_clk_sel |
	 * +--------+    +---------+    +-------------+
	 * |
	 * +----------------------------------------------> pcie_aux_clk
	 */
	[PCIE_POR] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.flags = CLK_R,
	},
	[PCIE_CLK] = {
		.clk_dep = pcie_pclk,
		.clk_src = pcie_axi_clk,
		.flags = CLK_CR,
	},
	[PCIE_PCLK] = {
		.clk_dep = pcie_por,
		.clk_src = sysfab_clk,
		.flags = 0,
	},
	/* 4.8 SD Clocks
	 * +--------------------------------------------+
	 * |                                            v
	 * +--------+ -> +----------+ -> +--------------+ -> sd_clk
	 * | xo_clk |    | sfab_pll |    | sfab_clk_sel |
	 * +--------+    +----------+    +--------------+
	 */
	[SD_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.9 ETH Clocks
	 */
	[ETH_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[ETH_ALT_REF_CLK] = {
		.clk_dep = eth_clk,
		.clk_src = eth_pll,
		.flags = 0,
	},
	[SGMII_REF_CLK] = {
		.clk_dep = eth_clk,
		.clk_src = sgmii_pll,
		.flags = 0,
	},
	/* 4.9 UART Clocks
	 * +-----------------------------+
	 * |                             v
	 * +--------+ -> +----------+ -> +--------------+ -> uart_clk
	 * | xo_clk |    | sfab_pll |    | sfab_clk_sel |
	 * +--------+    +----------+    +--------------+
	 */
	[UART0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[UART1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[UART2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[UART3_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.10 SPI Clocks
	 * +-----------------------------+
	 * |                             v
	 * +--------+ -> +----------+ -> +--------------+ -> spi_clk
	 * | xo_clk |    | sfab_pll |    | sfab_clk_sel |
	 * +--------+    +----------+    +--------------+
	 */
	[SPI0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[SPI1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[SPI2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[SPI3_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[SPI4_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.11 SPI Flash Clocks
	 * +-----------------------------+
	 * |                             v
	 * +--------+ -> +----------+ -> +--------------+ -> spi_flash_clk
	 * | xo_clk |    | sfab_pll |    | sfab_clk_sel |
	 * +--------+    +----------+    +--------------+
	 */
	[SPI_FLASH_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.12 GPIO Clocks
	 * +-----------------------------+
	 * |                             v
	 * +--------+ -> +----------+ -> +--------------+ -> gpio_clk
	 * | xo_clk |    | sfab_pll |    | sfab_clk_sel |
	 * +--------+    +----------+    +--------------+
	 */
	[GPIO0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[GPIO1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[GPIO2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.13 I2C Clocks
	 * +-----------------------------+
	 * |                             v
	 * +--------+ -> +----------+ -> +--------------+ -> i2c_clk
	 * | xo_clk |    | sfab_pll |    | sfab_clk_sel |
	 * +--------+    +----------+    +--------------+
	 */
	[I2C0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C3_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C4_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C5_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C6_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C7_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C8_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C9_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C10_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[I2C11_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.14 Thermal Sensor Clocks
	 * +-----------------------------+
	 * |                             v
	 * +--------+ -> +----------+ -> +--------------+ -> tsensor_pclk
	 * | xo_clk |    | sfab_pll |    | sfab_clk_sel |
	 * +--------+    +----------+    +--------------+
	 * |
	 * +---------------------------> +-------------+ --> tsensor_clk
	 *                               | xo_clk_div4 |
	 *                               +-------------+
	 */
	[TSENSOR_CLK] = {
		.clk_dep = sysfab_clk, /* tsensor_pclk */
		.clk_src = xo_clk_div4,
		.flags = CLK_CR,
	},
#if 0
	[SYSFAB_DBG_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,,
		.flags = CLK_CR,
	},
	[SYSFAB_TIC_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,,
		.flags = CLK_CR,
	},
	[TIC_RST] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.flags = CLK_R,
	},
	[DBG_RST] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,,
		.flags = CLK_R,
	},
#endif
	/* 4.15 Thermal Sensor Clocks */
	[CLUSTER0_APC0_CPU0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster0_apc0_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER0_APC0_CPU1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster0_apc0_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER0_APC1_CPU0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster0_apc1_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER0_APC1_CPU1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster0_apc1_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER0_APC0_L2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster0_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER0_APC1_L2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster0_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER1_APC0_CPU0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster1_apc0_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER1_APC0_CPU1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster1_apc0_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER1_APC1_CPU0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster1_apc1_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER1_APC1_CPU1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster1_apc1_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER1_APC0_L2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster1_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER1_APC1_L2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster1_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER2_APC0_CPU0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster2_apc0_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER2_APC0_CPU1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster2_apc0_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER2_APC1_CPU0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster2_apc1_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER2_APC1_CPU1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster2_apc1_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER2_APC0_L2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER2_APC1_L2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER3_APC0_CPU0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster3_apc0_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER3_APC0_CPU1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster3_apc0_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER3_APC1_CPU0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster3_apc1_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER3_APC1_CPU1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster3_apc1_l2_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER3_APC0_L2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster3_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
	[CLUSTER3_APC1_L2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cluster3_clk,
		.flags = CLK_CR | CLK_CLUSTER_CFG_F,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *output_clk_names[] = {
	/* 4.2 Cluster Clocks */
	[CLUSTER0_CLK] = "cluster0_clk",
	[CLUSTER1_CLK] = "cluster1_clk",
	[CLUSTER2_CLK] = "cluster2_clk",
	[CLUSTER3_CLK] = "cluster3_clk",
	[CLUSTER0_HCLK] = "cluster0_hclk",
	[CLUSTER1_HCLK] = "cluster1_hclk",
	[CLUSTER2_HCLK] = "cluster2_hclk",
	[CLUSTER3_HCLK] = "cluster3_hclk",
	/* 4.3 Coherence Fabric Clocks */
	[COHFAB_CLK] = "cohfab_clk",
	[COHFAB_HCLK] = "cohfab_hclk",
	[COHFAB_CFG_CLK] = "cohfab_cfg_clk",
	/* 4.4 System Fabric Clocks */
	[IMC_CLK] = "imc_clk",
	[PLIC_CLK] = "plic_clk",
	[TLMM_CLK] = "tlmm_clk",
	[SCSR_CLK] = "scsr_clk",
	[LCSR_CLK] = "lcsr_clk",
	[TMR0_CLK] = "tmr0_clk",
	[TMR1_CLK] = "tmr1_clk",
	[TMR2_CLK] = "tmr2_clk",
	[TMR3_CLK] = "tmr3_clk",
	[WDT0_CLK] = "wdt0_clk",
	[WDT1_CLK] = "wdt1_clk",
	/* 4.5 DMA Clocks */
	[DMA_CLK] = "dma_clk",
	/* 4.6 DDR Clocks */
	[DDR_POR] = "ddr_por",
	[DDR_ACLK] = "ddr_aclk",
	[DDR_PCLK] = "ddr_pclk",
	[DDR_BYPASS_PCLK] = "ddr_bypass_pclk",
	[DDR_CLK] = "ddr_clk",
	[DDR_RST] = "ddr_rst",
	/* 4.7 PCIE Clocks */
	[PCIE_POR] = "pcie_por",
	[PCIE_PCLK] = "pcie_pclk",
	[PCIE_CLK] = "pcie_clk",
	/* 4.8 SD Clocks */
	[SD_CLK] = "sd_clk",
	/* 4.9 ETH Clocks */
	[ETH_CLK] = "eth_clk",
	[ETH_ALT_REF_CLK] = "eth_alt_ref_clk",
	[SGMII_REF_CLK] = "sgmii_ref_clk",
	/* 4.9 UART Clocks */
	[UART0_CLK] = "uart0_clk",
	[UART1_CLK] = "uart1_clk",
	[UART2_CLK] = "uart2_clk",
	[UART3_CLK] = "uart3_clk",
	/* 4.10 SPI Clocks */
	[SPI0_CLK] = "spi0_clk",
	[SPI1_CLK] = "spi1_clk",
	[SPI2_CLK] = "spi2_clk",
	[SPI3_CLK] = "spi3_clk",
	[SPI4_CLK] = "spi4_clk",
	/* 4.11 SPI Flash Clocks */
	[SPI_FLASH_CLK] = "spi_flash_clk",
	/* 4.12 GPIO Clocks */
	[GPIO0_CLK] = "gpio0_clk",
	[GPIO1_CLK] = "gpio1_clk",
	[GPIO2_CLK] = "gpio2_clk",
	/* 4.13 I2C Clocks */
	[I2C0_CLK] = "i2c0_clk",
	[I2C1_CLK] = "i2c1_clk",
	[I2C2_CLK] = "i2c2_clk",
	[I2C3_CLK] = "i2c3_clk",
	[I2C4_CLK] = "i2c4_clk",
	[I2C5_CLK] = "i2c5_clk",
	[I2C6_CLK] = "i2c6_clk",
	[I2C7_CLK] = "i2c7_clk",
	[I2C8_CLK] = "i2c8_clk",
	[I2C9_CLK] = "i2c9_clk",
	[I2C10_CLK] = "i2c10_clk",
	[I2C11_CLK] = "i2c11_clk",
	/* 4.14 Thermal Sensor Clocks */
	[TSENSOR_CLK] = "tsensor_clk",
	/* Cluster internal clock/reset */
	[CLUSTER0_APC0_CPU0_CLK] = "cluster0_apc0_cpu0_clk",
	[CLUSTER0_APC0_CPU1_CLK] = "cluster0_apc0_cpu1_clk",
	[CLUSTER0_APC1_CPU0_CLK] = "cluster0_apc1_cpu0_clk",
	[CLUSTER0_APC1_CPU1_CLK] = "cluster0_apc1_cpu1_clk",
	[CLUSTER0_APC0_L2_CLK] = "cluster0_apc0_l2_clk",
	[CLUSTER0_APC1_L2_CLK] = "cluster0_apc1_l2_clk",
	[CLUSTER1_APC0_CPU0_CLK] = "cluster1_apc0_cpu0_clk",
	[CLUSTER1_APC0_CPU1_CLK] = "cluster1_apc0_cpu1_clk",
	[CLUSTER1_APC1_CPU0_CLK] = "cluster1_apc1_cpu0_clk",
	[CLUSTER1_APC1_CPU1_CLK] = "cluster1_apc1_cpu1_clk",
	[CLUSTER1_APC0_L2_CLK] = "cluster1_apc0_l2_clk",
	[CLUSTER1_APC1_L2_CLK] = "cluster1_apc1_l2_clk",
	[CLUSTER2_APC0_CPU0_CLK] = "cluster2_apc0_cpu0_clk",
	[CLUSTER2_APC0_CPU1_CLK] = "cluster2_apc0_cpu1_clk",
	[CLUSTER2_APC1_CPU0_CLK] = "cluster2_apc1_cpu0_clk",
	[CLUSTER2_APC1_CPU1_CLK] = "cluster2_apc1_cpu1_clk",
	[CLUSTER2_APC0_L2_CLK] = "cluster2_apc0_l2_clk",
	[CLUSTER2_APC1_L2_CLK] = "cluster2_apc1_l2_clk",
	[CLUSTER3_APC0_CPU0_CLK] = "cluster3_apc0_cpu0_clk",
	[CLUSTER3_APC0_CPU1_CLK] = "cluster3_apc0_cpu1_clk",
	[CLUSTER3_APC1_CPU0_CLK] = "cluster3_apc1_cpu0_clk",
	[CLUSTER3_APC1_CPU1_CLK] = "cluster3_apc1_cpu1_clk",
	[CLUSTER3_APC0_L2_CLK] = "cluster3_apc0_l2_clk",
	[CLUSTER3_APC1_L2_CLK] = "cluster3_apc1_l2_clk",
#if 0
	[SYSFAB_DBG_CLK] = "sysfab_dbg_clk",
	[SYSFAB_TIC_CLK] = "sysfab_tic_clk",
	[TIC_RST] = "tic_rst",
	[DBG_CLK] = "dbg_rst",
#endif
};

static const char *get_output_clk_name(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return NULL;
	return output_clk_names[clk];
}
#else
#define get_output_clk_name		NULL
#endif

static int enable_output_clk(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return -EINVAL;

	crcntl_trace(true, get_output_clk_name(clk));
	if (output_clks[clk].clk_dep != invalid_clk)
		clk_enable(output_clks[clk].clk_dep);
	if (output_clks[clk].clk_src != invalid_clk)
		clk_enable(output_clks[clk].clk_src);
	if (output_clks[clk].flags & CLK_CLK_EN_F) {
		if (output_clks[clk].flags & CLK_COHFAB_CFG_F)
			cohfab_clk_enable(clk);
		else if (output_clks[clk].flags & CLK_CLUSTER_CFG_F)
			cluster_clk_enable(clk);
		else
			crcntl_clk_enable(clk);
	}
	if (output_clks[clk].flags & CLK_SW_RST_F) {
		if (output_clks[clk].flags & CLK_COHFAB_CFG_F)
			cohfab_clk_deassert(clk);
		else if (output_clks[clk].flags & CLK_CLUSTER_CFG_F)
			cluster_clk_deassert(clk);
		else
			crcntl_clk_deassert(clk);
	}
	return 0;
}

static void disable_output_clk(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return;

	crcntl_trace(false, get_output_clk_name(clk));
	if (output_clks[clk].clk_dep != invalid_clk)
		clk_disable(output_clks[clk].clk_dep);
	if (output_clks[clk].clk_src != invalid_clk)
		clk_disable(output_clks[clk].clk_src);
	if (output_clks[clk].flags & CLK_SW_RST_F) {
		if (output_clks[clk].flags & CLK_COHFAB_CFG_F)
			cohfab_clk_assert(clk);
		else if (output_clks[clk].flags & CLK_CLUSTER_CFG_F)
			cluster_clk_assert(clk);
		else
			crcntl_clk_assert(clk);
	}
	if (output_clks[clk].flags & CLK_CLK_EN_F) {
		if (output_clks[clk].flags & CLK_COHFAB_CFG_F)
			cohfab_clk_disable(clk);
		else if (output_clks[clk].flags & CLK_CLUSTER_CFG_F)
			cluster_clk_disable(clk);
		else
			crcntl_clk_disable(clk);
	}
}

static clk_freq_t get_output_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return INVALID_FREQ;
	return clk_get_frequency(output_clks[clk].clk_src);
}

struct clk_driver clk_output = {
	.max_clocks = NR_OUTPUT_CLKS,
	.enable = enable_output_clk,
	.disable = disable_output_clk,
	.select = NULL,
	.get_freq = get_output_clk_freq,
	.set_freq = NULL,
	.get_name = get_output_clk_name,
};

struct div_clk {
	clk_t src;
	uint8_t div;
};

struct div_clk div_clks[NR_DIV_CLKS] = {
	[SOC_CLK_DIV2] = {
		.src = soc_pll,
		.div = 2,
	},
	[DDR_CLK_SEL_DIV4] = {
		.src = ddr_clk_sel,
		.div = 4,
	},
	[XO_CLK_DIV4] = {
		.src = xo_clk,
		.div = 4,
	},
};

#ifdef CONFIG_CLK_MNEMONICS
const char *div_clk_names[NR_DIV_CLKS] = {
	[SOC_CLK_DIV2] = "soc_clk_div2",
	[DDR_CLK_SEL_DIV4] = "ddr_clk_sel_div4",
	[XO_CLK_DIV4] = "xo_clk_div4",
};

static const char *get_pll_div_name(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return NULL;
	return div_clk_names[clk];
}
#else
#define get_pll_div_name	NULL
#endif

static int enable_pll_div(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return -EINVAL;
	crcntl_trace(true, get_pll_div_name(clk));
	return clk_enable(div_clks[clk].src);
}

static void disable_pll_div(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return;
	crcntl_trace(false, get_pll_div_name(clk));
	clk_disable(div_clks[clk].src);
}

static clk_freq_t get_pll_div_freq(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return INVALID_FREQ;
	return clk_get_frequency(div_clks[clk].src) / div_clks[clk].div;
}

struct clk_driver clk_div = {
	.max_clocks = NR_DIV_CLKS,
	.enable = enable_pll_div,
	.disable = disable_pll_div,
	.get_freq = get_pll_div_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_pll_div_name,
};

/*===========================================================================
 * Clock tree APIs
 *===========================================================================*/
#define DUOWEN_CLK_REG_INIT	_BV(0)
#define DUOWEN_CLK_IMC_INIT	_BV(1)
#define DUOWEN_CLK_APC_INIT	_BV(2)

static uint8_t clk_hw_init;

#ifdef CONFIG_DUOWEN_APC_16
clk_t duowen_apc_clocks[MAX_APC_NUM] = {
	cluster0_apc0_cpu0_clk,
	cluster0_apc0_cpu1_clk,
	cluster0_apc1_cpu0_clk,
	cluster0_apc1_cpu1_clk,
	cluster1_apc0_cpu0_clk,
	cluster1_apc0_cpu1_clk,
	cluster1_apc1_cpu0_clk,
	cluster1_apc1_cpu1_clk,
	cluster2_apc0_cpu0_clk,
	cluster2_apc0_cpu1_clk,
	cluster2_apc1_cpu0_clk,
	cluster2_apc1_cpu1_clk,
	cluster3_apc0_cpu0_clk,
	cluster3_apc0_cpu1_clk,
	cluster3_apc1_cpu0_clk,
	cluster3_apc1_cpu1_clk,
};
#endif
#ifdef CONFIG_DUOWEN_APC_4
clk_t duowen_apc_clocks[MAX_APC_NUM] = {
	cluster0_apc0_cpu0_clk,
	cluster1_apc0_cpu0_clk,
	cluster2_apc0_cpu0_clk,
	cluster3_apc0_cpu0_clk,
};
#endif
#ifdef CONFIG_DUOWEN_APC_1
clk_t duowen_apc_clocks[MAX_CPU_NUM] = {
	cluster0_apc0_cpu0_clk,
};
#endif

void duowen_clk_apc_init(void)
{
	cpu_t cpu;

	if (!(clk_hw_init & DUOWEN_CLK_APC_INIT)) {
		clk_enable(cohfab_clk);
		for (cpu = 0; cpu < MAX_APC_NUM; cpu++)
			clk_enable(duowen_apc_clocks[cpu]);
		clk_hw_init |= DUOWEN_CLK_APC_INIT;
	}
}

void duowen_clk_imc_init(void)
{
	if (!(clk_hw_init & DUOWEN_CLK_IMC_INIT)) {
		clk_enable(sysfab_clk);
		/* Update the status of the default enabled clocks */
		clk_enable(imc_clk);
		clk_hw_init |= DUOWEN_CLK_IMC_INIT;
	}
}

#ifdef CONFIG_DUOWEN_IMC
#define duowen_clk_boot()		duowen_clk_imc_init()
#endif
#ifdef CONFIG_DUOWEN_APC
#define duowen_clk_boot()		duowen_clk_apc_init()
#endif

void duowen_clk_init(void)
{
	if (!(clk_hw_init & DUOWEN_CLK_REG_INIT)) {
		clk_pll_init();
		clk_register_driver(CLK_OUTPUT, &clk_output);
		clk_register_driver(CLK_DIV, &clk_div);
		clk_hw_init |= DUOWEN_CLK_REG_INIT;
	}
	duowen_clk_boot();
}

void clk_hw_ctrl_init(void)
{
	board_init_clock();
}

#ifdef CONFIG_CONSOLE_COMMAND
static int do_crcntl_dump(int argc, char *argv[])
{
	int i;

	printf("type id  %20s %20s\n", "name", "source");
	clk_pll_dump();
	for (i = 0; i < NR_DIV_CLKS; i++) {
		if (div_clk_names[i]) {
			printf("div  %3d %20s %20s\n",
			       i, div_clk_names[i],
			       clk_get_mnemonic(div_clks[i].src));
		}
	}
	for (i = 0; i < NR_OUTPUT_CLKS; i++) {
		if (output_clk_names[i] &&
		    output_clks[i].flags & CLK_CLK_EN_F) {
			printf("clk  %3d %20s %20s\n",
			       i, output_clk_names[i],
			       clk_get_mnemonic(output_clks[i].clk_src));
			if (output_clks[i].clk_dep != invalid_clk)
				printf("%4s %3s %20s %20s\n", "", "", "",
				       clk_get_mnemonic(
					       output_clks[i].clk_dep));
		}
	}
	return 0;
}
#else
static inline int do_crcntl_dump(int argc, char *argv[])
{
	return 0;
}
#endif

static int do_crcntl(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0)
		return do_crcntl_dump(argc, argv);
	return -ENODEV;
}

DEFINE_COMMAND(crcntl, do_crcntl, "Clock/reset controller (CRCNTL)",
	"crcntl dump\n"
	"    -display clock tree source multiplexing\n"
);
