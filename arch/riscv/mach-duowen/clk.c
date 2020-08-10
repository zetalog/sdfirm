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
	 * +--------------------------------------------------------------+
	 * |                                                              v
	 * +--------+ -> +---------+ -> +--------------+    +-------------+ -> cluster_clk
	 * | xo_clk |    | soc_pll |    | soc_pll_div4 |    | soc_clk_sel |
	 * +--------+    +---------+    +--------------+ -> +-------------+
	 */
#if 0
	[CLUSTER0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cl0_clk,
		.flags = CLK_CR,
	},
	[CLUSTER1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cl1_clk,
		.flags = CLK_CR,
	},
	[CLUSTER2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cl2_clk,
		.flags = CLK_CR,
	},
	[CLUSTER3_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cl3_clk,
		.flags = CLK_CR,
	},
#endif
	[CLUSTER0_HCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[CLUSTER1_HCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[CLUSTER2_HCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[CLUSTER3_HCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.3 Coherence Fabric Clocks
	 * +--------------------------------------------------------------+
	 * |                                                              v
	 * +--------+ -> +---------+ -> +--------------+ -> +-------------+ -> cohfab_clk
	 * | xo_clk |    | soc_pll |    | soc_pll_div4 |    | soc_clk_sel |
	 * +--------+    +---------+    +--------------+    +-------------+
	 * |                       |    +--------------+ -> +-------------+ -> cohfab_cfg_hclk
	 * |                       |    | soc_pll_div2 |    | soc_clk_sel |
	 * |                       +--> +--------------+    +-------------+
	 * |                                                              ^
	 * +--------------------------------------------------------------+
	 */
#if 0
	[COHFAB_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = cfab_clk,
		.flags = CLK_CR,
	},
#endif
	[COHFAB_HCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[COHFAB_CFG_CLK] = {
		.clk_dep = sysfab_clk,
		.clk_src = soc_pll_div2,
		.flags = 0,
	},
	/* 4.4 System Fabric Clocks
	 * +--------------------------------------------------------------+
	 * |                                                              v
	 * +--------+ -> +---------+ -> +--------------+ -> +-------------+ -> imc_clk
	 * | xo_clk |    | soc_pll |    | soc_pll_div4 |    | soc_clk_sel |    ram_aclk/rom_hclk
	 * +--------+    +---------+    +--------------+    +-------------+    plic_hclk
	 *                                                  v
	 *                                                  +--------------+ -> tlmm_pclk
	 *                                                  | soc_clk_div2 |    scsr_pclk
	 *                                                  +--------------+    wdt0/1_pclk
	 */
	[PLIC_HCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	[TLMM_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[SCSR_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[WDT0_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[WDT1_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	/* 4.5 DMA Clocks
	 * +--------------------------------------------------------------+
	 * |                                                              v
	 * +--------+ -> +---------+ -> +--------------+ -> +-------------+ -> dma_clk
	 * | xo_clk |    | soc_pll |    | soc_pll_div2 |    | soc_clk_sel |
	 * +--------+    +---------+    +--------------+    +-------------+
	 * |                       |    +--------------+ -> +-------------+ -> dma_hclk
	 * |                       |    | soc_pll_div8 |    | soc_clk_sel |
	 * |                       +--> +--------------+    +-------------+
	 * |                                                              ^
	 * +--------------------------------------------------------------+
	 */
	[DMA_CLK] = {
		.clk_dep = dma_hclk,
		.clk_src = soc_pll_div2,
		.flags = CLK_CR,
	},
	[DMA_HCLK] = {
		.clk_dep = sysfab_clk,
		.clk_src = soc_pll_div8,
		.flags = 0,
	},
	/* 4.6 DDR Clocks
	 * +--------------------------------------------------+
	 * |                                                  v
	 * +--------+ -> +-------------+ -> +-----------------+ -> ddr_aclk
	 * | xo_clk |    | ddr_bus_pll |    | ddr_bus_clk_sel |
	 * +--------+    +-------------+    +-----------------+
	 * |
	 * +--------------------------------------------------------------+
	 * |                                                              v
	 * |             +---------+ -> +--------------+ -> +-------------+ -> ddr_pclk
	 * |             | soc_pll |    | soc_pll_div8 |    | soc_clk_sel |
	 * +-----------> +---------+    +--------------+    +-------------+
	 * |
	 * +------------------------------------------+
	 * |                                          v
	 * |             +---------+ -> +-------------+ ---------------------------+ -> ddr_bypass_pclk
	 * |             | ddr_pll |    | ddr_clk_sel |                            |
	 * +-----------> +---------+    +-------------+                            |
	 *                              v                                          v
	 *                              +------------------+ -> +------------------+ -> ddr_clk
	 *                              | ddr_clk_sel_div4 |    | ddr_clk_div4_sel |
	 *                              +------------------+    +------------------+
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
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[DDR_BYPASS_PCLK] = {
		.clk_dep = ddr_clk_div4_sel,
		.clk_src = ddr_clk_sel,
		.flags = CLK_C | CLK_REVERSE_DEP_F,
	},
	[DDR_CLK] = {
		.clk_dep = ddr_pclk,
		.clk_src = ddr_clk_div4_sel,
		.flags = CLK_CR,
	},
	/* 4.7 PCIE Clocks
	 * +-> pcie_aux_clk              +-----+ -> pcie_alt_ref_clk_n
	 * |                             | inv |
	 * +-----------> +----------+ -> +-----+ -> pcie_alt_ref_clk_p
	 * |             | pcie_pll |
	 * |             +----------+
	 * +---------------------------------------------------------------+
	 * |                                                               v
	 * +--------+ -> +---------+ -> +---------------+ -> +-------------+ -> pcie_pclk
	 * | xo_clk |    | soc_pll |    | soc_pll_div12 |    | soc_clk_sel |
	 * +--------+    +---------+    +---------------+    +-------------+
	 * |                       |                         +-------------+ -> pcie_aclk
	 * |                       |                         | soc_clk_sel |
	 * |                       +-----------------------> +-------------+
	 * |                                                               ^
	 * +---------------------------------------------------------------+
	 */
	[PCIE_POR] = {
		.clk_dep = sysfab_clk,
		.clk_src = invalid_clk,
		.flags = CLK_R,
	},
	[PCIE_CLK] = {
		.clk_dep = pcie_pclk,
		.clk_src = pcie_pll,
		.flags = CLK_CR,
	},
	[PCIE_PCLK] = {
		.clk_dep = pcie_por,
		.clk_src = soc_pll_div12,
		.flags = 0,
	},
	/* 4.8 Timer Clocks
	 * +--------------------------------------------------------------+
	 * |                                                              v
	 * +--------+ -> +---------+ -> +--------------+ -> +-------------+ -> +--------------+ -> timer_pclk
	 * | xo_clk |    | soc_pll |    | soc_pll_div4 |    | soc_clk_sel |    | soc_clk_div2 |
	 * +--------+    +---------+    +--------------+    +-------------+    +--------------+
	 * |                       |    +--------------+ -> +-------------+ ---------------------> timer_clk
	 * |                       |    | soc_pll_div8 |    | soc_clk_sel |
	 * |                       +--> +--------------+    +-------------+
	 * |                                                              ^
	 * +--------------------------------------------------------------+
	 */
	[TIMER0_RST] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_R,
	},
	[TIMER1_RST] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_R,
	},
	[TIMER2_RST] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_R,
	},
	[TIMER3_RST] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.flags = CLK_R,
	},
	[TIMER0_1_CLK] = {
		.clk_dep = timer0_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER0_2_CLK] = {
		.clk_dep = timer0_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER0_3_CLK] = {
		.clk_dep = timer0_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER0_4_CLK] = {
		.clk_dep = timer0_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER0_5_CLK] = {
		.clk_dep = timer0_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER0_6_CLK] = {
		.clk_dep = timer0_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER0_7_CLK] = {
		.clk_dep = timer0_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER0_8_CLK] = {
		.clk_dep = timer0_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER1_1_CLK] = {
		.clk_dep = timer1_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER1_2_CLK] = {
		.clk_dep = timer1_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER1_3_CLK] = {
		.clk_dep = timer1_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER1_4_CLK] = {
		.clk_dep = timer1_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER1_5_CLK] = {
		.clk_dep = timer1_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER1_6_CLK] = {
		.clk_dep = timer1_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER1_7_CLK] = {
		.clk_dep = timer1_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER1_8_CLK] = {
		.clk_dep = timer1_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER2_1_CLK] = {
		.clk_dep = timer2_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER2_2_CLK] = {
		.clk_dep = timer2_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER2_3_CLK] = {
		.clk_dep = timer2_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER2_4_CLK] = {
		.clk_dep = timer2_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER2_5_CLK] = {
		.clk_dep = timer2_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER2_6_CLK] = {
		.clk_dep = timer2_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER2_7_CLK] = {
		.clk_dep = timer2_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER2_8_CLK] = {
		.clk_dep = timer2_rst,
		.clk_src = soc_pll_div8,
		.flags = CLK_C,
	},
	[TIMER3_CLK] = {
		.clk_dep = timer3_rst,
		.clk_src = xo_clk,
		.flags = CLK_CR,
	},
	/* 4.9 SD Clocks
	 * +-------------------------------------------+
	 * |                                           v
	 * +--------+ -> +---------+ -> +--------------+ -> +-------------+ -> +--------------+ -> sd_hclk
	 * | xo_clk |    | soc_pll |    | soc_pll_div4 |    | soc_clk_sel |    | soc_clk_div2 |
	 * +--------+    +---------+    +--------------+    +-------------+    +--------------+
	 * |             |         |    +--------------+ -> +-------------+ ---------------------> sd_bclk
	 * |             |         |    | soc_pll_div8 |    | soc_clk_sel |
	 * |             |         +--> +--------------+    +-------------+
	 * |             |                             ^
	 * +-------------|-----------------------------+---------+
	 * |             v                                       v
	 * |             +---------------+ -> +------------------+ ------------------------------> sd_cclk
	 * |             | soc_pll_div10 |    | sd_rx_tx_clk_sel |  +----------------------------> sd_clk
	 * v             +---------------+    +------------------+  |
	 * +--------------+              +--> +------------------+ -+-> +---------------------+ -> sd_tm_clk
	 * | xo_clk_div10 |                   | sd_rx_tx_clk_sel |      | sd_rx_tx_clk_div100 |
	 * +--------------+ ----------------> +------------------+      +---------------------+
	 */
	[SD_BCLK] = {
		.clk_dep = sysfab_clk,
		.clk_src = soc_pll_div8,
		.flags = 0,
	},
	[SD_CCLK] = {
		.clk_dep = sd_bclk,
		.clk_src = sd_rx_tx_clk_sel,
		.flags = 0,
	},
	[SD_CLK] = {
		.clk_dep = sd_cclk,
		.clk_src = soc_pll_div10,
		.flags = CLK_CR,
	},
	/* 4.10 UART Clocks
	 * +-------------------------------------------+
	 * |                                           v
	 * +--------+ -> +---------+ -> +--------------+ -> +-------------+ -> +--------------+ -> uart_pclk
	 * | xo_clk |    | soc_pll |    | soc_pll_div4 |    | soc_clk_sel |    | soc_clk_div2 |
	 * +--------+    +---------+    +--------------+    +-------------+    +--------------+
	 * |                       |    +--------------+ -> +-------------+ ---------------------> uart_clk
	 * |                       |    | soc_pll_div8 |    | soc_clk_sel |
	 * |                       +--> +--------------+    +-------------+
	 * |                                           ^
	 * +-------------------------------------------+
	 * SW developers needn't know uart_pclk.
	 */
	[UART0_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[UART1_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[UART2_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[UART3_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	/* 4.11 SPI Clocks
	 * +-------------------------------------------+
	 * |                                           v
	 * +--------+ -> +---------+ -> +--------------+ -> +-------------+ -> +--------------+ -> spi_pclk
	 * | xo_clk |    | soc_pll |    | soc_pll_div4 |    | soc_clk_sel |    | soc_clk_div2 |
	 * +--------+    +---------+    +--------------+    +-------------+    +--------------+
	 * |                       |    +--------------+ -> +-------------+ ---------------------> spi_clk
	 * |                       |    | soc_pll_div8 |    | soc_clk_sel |
	 * |                       +--> +--------------+    +-------------+
	 * |                                           ^
	 * +-------------------------------------------+
	 * SW developers needn't know spi_pclk.
	 */
	[SPI0_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[SPI1_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[SPI2_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[SPI3_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[SPI4_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	/* 4.12 SPI Flash Clocks
	 * +-------------------------------------------+
	 * |                                           v
	 * +--------+ -> +---------+ -> +--------------+ -> +-------------+ -> spi_flash_pclk
	 * | xo_clk |    | soc_pll |    | soc_pll_div4 |    | soc_clk_sel |
	 * +--------+    +---------+    +--------------+    +-------------+
	 */
	[SPI_FLASH_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.13 GPIO Clocks
	 * +-------------------------------------------+
	 * |                                           v
	 * +--------+ -> +---------+ -> +--------------+ -> +-------------+ -> +--------------+ -> gpio_pclk
	 * | xo_clk |    | soc_pll |    | soc_pll_div4 |    | soc_clk_sel |    | soc_clk_div2 |
	 * +--------+    +---------+    +--------------+    +-------------+    +--------------+
	 */
	[GPIO0_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[GPIO1_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[GPIO2_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	/* 4.14 I2C Clocks
	 * +-------------------------------------------+
	 * |                                           v
	 * +--------+ -> +---------+ -> +--------------+ -> +-------------+ -> +--------------+ -> i2c_pclk
	 * | xo_clk |    | soc_pll |    | soc_pll_div4 |    | soc_clk_sel |    | soc_clk_div2 |
	 * +--------+    +---------+    +--------------+    +-------------+    +--------------+
	 * |                       |    +--------------+ -> +-------------+ ---------------------> i2c_clk
	 * |                       |    | soc_pll_div8 |    | soc_clk_sel |
	 * |                       +--> +--------------+    +-------------+
	 * |                                           ^
	 * +-------------------------------------------+
	 * SW developers needn't know i2c_pclk.
	 */
	[I2C0_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C1_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C2_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C3_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C4_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C5_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C6_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C7_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C8_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C9_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C10_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	[I2C11_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_pll_div8,
		.flags = CLK_CR,
	},
	/* 4.15 Thermal Sensor Clocks */
	[TSENSOR_CLK] = {
		.clk_dep = sysfab_half_clk,
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
	[TIC_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = tic_xclk,
		.flags = CLK_CR,
	},
	[CORESIGHT_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,,
		.flags = CLK_CR,
	},
#endif
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *output_clk_names[] = {
	/* 4.2 Cluster Clocks */
	[CLUSTER0_HCLK] = "cluster0_hclk",
	[CLUSTER1_HCLK] = "cluster1_hclk",
	[CLUSTER2_HCLK] = "cluster2_hclk",
	[CLUSTER3_HCLK] = "cluster3_hclk",
	/* 4.3 Coherence Fabric Clocks */
	[COHFAB_HCLK] = "cohfab_hclk",
	/* 4.4 System Fabric Clocks */
	[PLIC_HCLK] = "plic_hclk",
	[TLMM_PCLK] = "tlmm_pclk",
	[SCSR_PCLK] = "scsr_pclk",
	[WDT0_PCLK] = "wdt0_pclk",
	[WDT1_PCLK] = "wdt1_pclk",
	/* 4.5 DMA Clocks */
	[DMA_HCLK] = "dma_hclk",
	/* 4.6 DDR Clocks */
	[DDR_CLK] = "ddr_clk",
	[DDR_BYPASS_PCLK] = "ddrp0_bypass_pclk",
	/* 4.7 PCIE Clocks */
	[PCIE_CLK] = "pcie_clk",
	/* 4.8 Timer Clocks */
	[TIMER0_RST] = "timer0_rst",
	[TIMER1_RST] = "timer1_rst",
	[TIMER2_RST] = "timer2_rst",
	[TIMER3_RST] = "timer3_rst",
	[TIMER0_1_CLK] = "timer0_1_clk",
	[TIMER0_2_CLK] = "timer0_2_clk",
	[TIMER0_3_CLK] = "timer0_3_clk",
	[TIMER0_4_CLK] = "timer0_4_clk",
	[TIMER0_5_CLK] = "timer0_5_clk",
	[TIMER0_6_CLK] = "timer0_6_clk",
	[TIMER0_7_CLK] = "timer0_7_clk",
	[TIMER0_8_CLK] = "timer0_8_clk",
	[TIMER1_1_CLK] = "timer1_1_clk",
	[TIMER1_2_CLK] = "timer1_2_clk",
	[TIMER1_3_CLK] = "timer1_3_clk",
	[TIMER1_4_CLK] = "timer1_4_clk",
	[TIMER1_5_CLK] = "timer1_5_clk",
	[TIMER1_6_CLK] = "timer1_6_clk",
	[TIMER1_7_CLK] = "timer1_7_clk",
	[TIMER1_8_CLK] = "timer1_8_clk",
	[TIMER2_1_CLK] = "timer2_1_clk",
	[TIMER2_2_CLK] = "timer2_2_clk",
	[TIMER2_3_CLK] = "timer2_3_clk",
	[TIMER2_4_CLK] = "timer2_4_clk",
	[TIMER2_5_CLK] = "timer2_5_clk",
	[TIMER2_6_CLK] = "timer2_6_clk",
	[TIMER2_7_CLK] = "timer2_7_clk",
	[TIMER2_8_CLK] = "timer2_8_clk",
	[TIMER3_CLK] = "timer3_clk",
	/* 4.9 SD Clocks */
	[SD_CLK] = "sd_clk",
	/* 4.10 UART Clocks */
	[UART0_CLK] = "uart0_clk",
	[UART1_CLK] = "uart1_clk",
	[UART2_CLK] = "uart2_clk",
	[UART3_CLK] = "uart3_clk",
	/* 4.11 SPI Clocks */
	[SPI0_CLK] = "spi0_clk",
	[SPI1_CLK] = "spi1_clk",
	[SPI2_CLK] = "spi2_clk",
	[SPI3_CLK] = "spi3_clk",
	[SPI4_CLK] = "spi4_clk",
	/* 4.12 SPI Flash Clocks */
	[SPI_FLASH_PCLK] = "spi_flash_pclk",
	/* 4.13 GPIO Clocks */
	[GPIO0_PCLK] = "gpio0_pclk",
	[GPIO1_PCLK] = "gpio1_pclk",
	[GPIO2_PCLK] = "gpio2_pclk",
	/* 4.14 I2C Clocks */
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
	/* 4.15 Thermal Sensor Clocks */
	[TSENSOR_CLK] = "tsensor_clk",
	/* Additional Resets */
	[TIMER0_RST] = "timer0_rst",
	[TIMER1_RST] = "timer1_rst",
	[TIMER2_RST] = "timer2_rst",
	[TIMER3_RST] = "timer3_rst",
#if 0
	[SYSFAB_DBG_CLK] = "sysfab_dbg_clk",
	[SYSFAB_TIC_CLK] = "sysfab_tic_clk",
	/* [TIC_CLK] = "tic_clk", */
	[CORESIGHT_CLK] = "coresight_clk",
#endif
	/* Additional clocks */
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

	if (output_clks[clk].clk_dep != invalid_clk) {
		if (output_clks[clk].flags & CLK_REVERSE_DEP_F)
			clk_disable(output_clks[clk].clk_dep);
		else
			clk_enable(output_clks[clk].clk_dep);
	}
	if (output_clks[clk].clk_src != invalid_clk)
		clk_enable(output_clks[clk].clk_src);
	if (output_clks[clk].flags & CLK_CLK_EN_F)
		crcntl_clk_enable(clk);
	if (output_clks[clk].flags & CLK_SW_RST_F)
		crcntl_clk_deassert(clk);
	return 0;
}

static void disable_output_clk(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return;

	if (output_clks[clk].clk_dep != invalid_clk) {
		if (output_clks[clk].flags & CLK_REVERSE_DEP_F)
			clk_enable(output_clks[clk].clk_dep);
		else
			clk_disable(output_clks[clk].clk_dep);
	}
	if (output_clks[clk].clk_src != invalid_clk)
		clk_disable(output_clks[clk].clk_src);
	if (output_clks[clk].flags & CLK_SW_RST_F)
		crcntl_clk_assert(clk);
	if (output_clks[clk].flags & CLK_CLK_EN_F)
		crcntl_clk_disable(clk);
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
	[SOC_PLL_DIV2] = {
		.src = soc_pll,
		.div = 2,
	},
	[SOC_PLL_DIV4] = {
		.src = soc_pll,
		.div = 4,
	},
	[SOC_PLL_DIV8] = {
		.src = soc_pll,
		.div = 8,
	},
	[SOC_PLL_DIV10] = {
		.src = soc_pll,
		.div = 10,
	},
	[SOC_PLL_DIV12] = {
		.src = soc_pll,
		.div = 12,
	},
	[SOC_CLK_SEL_DIV2] = {
		.src = soc_clk_sel,
		.div = 2,
	},
	[DDR_CLK_SEL_DIV4] = {
		.src = ddr_clk_sel,
		.div = 4,
	},
	[SD_TM_CLK] = {
		.src = sd_clk,
		.div = 100,
	},
	[XO_CLK_DIV4] = {
		.src = xo_clk,
		.div = 4,
	},
	[XO_CLK_DIV10] = {
		.src = xo_clk,
		.div = 10,
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *div_clk_names[NR_DIV_CLKS] = {
	[SOC_PLL_DIV2] = "soc_pll_div2",
	[SOC_PLL_DIV4] = "soc_pll_div4", /* sysfab_clk src */
	[SOC_PLL_DIV8] = "soc_pll_div8",
	[SOC_PLL_DIV10] = "soc_pll_div10",
	[SOC_PLL_DIV12] = "soc_pll_div12",
	[SOC_CLK_SEL_DIV2] = "soc_clk_sel_div2", /* sysfab_half_clk */
	[DDR_CLK_SEL_DIV4] = "ddr_clk_sel_div4",
	[SD_TM_CLK] = "sd_tm_clk",
	[XO_CLK_DIV4] = "xo_clk_div4",
	[XO_CLK_DIV10] = "xo_clk_div10",
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
	return clk_enable(div_clks[clk].src);
}

static void disable_pll_div(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return;
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
static bool clk_hw_init = false;

void crcntl_init(void)
{
	if (!clk_hw_init) {
		clk_pll_init();
		clk_register_driver(CLK_OUTPUT, &clk_output);
		clk_register_driver(CLK_DIV, &clk_div);

		/* Update the status of the default enabled clocks */
		clk_enable(soc_pll);
		clk_enable(sysfab_clk);
		clk_enable(sysfab_half_clk);
	}
	clk_hw_init = true;
}

void clk_hw_ctrl_init(void)
{
	board_init_clock();
}

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
