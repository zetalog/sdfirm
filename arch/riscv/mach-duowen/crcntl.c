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
 * @(#)crcntl.c: DUOWEN clock/reset controller (CRCNTL) implementation
 * $Id: crcntl.c,v 1.1 2019-11-06 10:39:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/panic.h>
#include <target/cmdline.h>

struct output_clk {
	clk_t clk_dep;
	clk_t clk_src;
	uint32_t flags;
#define CLK_CLK_SEL_F	_BV(6)
#define CLK_CLK_EN_F	_BV(7)
#define CLK_SW_RST_F	_BV(8)
#define CLK_CR		(CLK_CLK_EN_F | CLK_SW_RST_F)
#define CLK_C		CLK_CLK_EN_F
#define CLK_R		CLK_SW_RST_F
};

struct output_clk output_clks[] = {
	/* 4.2 Cluster Clocks */
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
	/* 4.3 Coherence Fabric Clocks */
	[COHFAB_HCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_clk,
		.flags = CLK_CR,
	},
	/* 4.4 System Fabric Clocks */
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
	/* 4.5 DMA Clocks */
	[DMA_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_clk,
		.flags = CLK_CR,
	},
	/* 4.6 DDR Clocks */
	[DDR_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = ddr_clk_div4_sel,
		.flags = CLK_CR,
	},
	[DDR_BYPASS_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = ddr_clk_sel,
		.flags = CLK_CR,
	},
	/* 4.7 PCIE Clocks */
	[PCIE_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = soc_clk,
		.flags = CLK_CR,
	},
	/* 4.8 Timer Clocks */
	[TIMER0_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_R,
	},
	[TIMER1_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_R,
	},
	[TIMER2_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_R,
	},
	[TIMER3_PCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = invalid_clk,
		.flags = CLK_R,
	},
	[TIMER0_1_CLK] = {
		.clk_dep = timer0_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER0_2_CLK] = {
		.clk_dep = timer0_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER0_3_CLK] = {
		.clk_dep = timer0_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER0_4_CLK] = {
		.clk_dep = timer0_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER0_5_CLK] = {
		.clk_dep = timer0_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER0_6_CLK] = {
		.clk_dep = timer0_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER0_7_CLK] = {
		.clk_dep = timer0_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER0_8_CLK] = {
		.clk_dep = timer0_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER1_1_CLK] = {
		.clk_dep = timer1_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER1_2_CLK] = {
		.clk_dep = timer1_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER1_3_CLK] = {
		.clk_dep = timer1_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER1_4_CLK] = {
		.clk_dep = timer1_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER1_5_CLK] = {
		.clk_dep = timer1_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER1_6_CLK] = {
		.clk_dep = timer1_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER1_7_CLK] = {
		.clk_dep = timer1_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER1_8_CLK] = {
		.clk_dep = timer1_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER2_1_CLK] = {
		.clk_dep = timer2_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER2_2_CLK] = {
		.clk_dep = timer2_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER2_3_CLK] = {
		.clk_dep = timer2_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER2_4_CLK] = {
		.clk_dep = timer2_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER2_5_CLK] = {
		.clk_dep = timer2_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER2_6_CLK] = {
		.clk_dep = timer2_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER2_7_CLK] = {
		.clk_dep = timer2_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER2_8_CLK] = {
		.clk_dep = timer2_pclk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_C,
	},
	[TIMER3_CLK] = {
		.clk_dep = __timer3_pclk,
		.clk_src = xo_clk,
		.flags = CLK_CR,
	},
	/* 4.9 SD Clocks */
	[SD_HCLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	/* 4.10 UART Clocks */
	[UART0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[UART1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[UART2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[UART3_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	/* 4.11 SPI Clocks */
	[SPI0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[SPI1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[SPI2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[SPI3_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[SPI4_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	/* 4.12 GPIO Clocks */
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
	/* 4.13 I2C Clocks */
	[I2C0_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C1_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C2_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C3_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C4_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C5_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C6_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C7_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C8_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C9_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C10_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	[I2C11_CLK] = {
		.clk_dep = invalid_clk,
		.clk_src = sysfab_half_clk,
		.flags = CLK_CR,
	},
	/* 4.14 Thermal Sensor Clocks */
	[TSENSOR0_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = xo_clk_div4,
		.flags = CLK_CR,
	},
	[TSENSOR1_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = xo_clk_div4,
		.flags = CLK_CR,
	},
	[TSENSOR2_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = xo_clk_div4,
		.flags = CLK_CR,
	},
	[TSENSOR3_CLK] = {
		.clk_dep = sysfab_half_clk,
		.clk_src = xo_clk_div4,
		.flags = CLK_CR,
	},
	/* Additional resets */
	[CRCNTL_RST_PCIE_BUTTON] = {
		.flags = CLK_R,
	},
	[CRCNTL_RST_PCIE_POWER_UP] = {
		.flags = CLK_R,
	},
	[CRCNTL_RST_PCIE_TEST] = {
		.flags = CLK_R,
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
	[DMA_CLK] = "dma_clk",
	/* 4.6 DDR Clocks */
	[DDR_CLK] = "ddr_clk",
	[DDR_BYPASS_PCLK] = "ddrp0_bypass_pclk",
	/* 4.7 PCIE Clocks */
	[PCIE_CLK] = "pcie_clk",
	/* 4.8 Timer Clocks */
	[TIMER0_PCLK] = "timer0_pclk",
	[TIMER1_PCLK] = "timer1_pclk",
	[TIMER2_PCLK] = "timer2_pclk",
	[TIMER3_PCLK] = "timer3_pclk",
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
	[SD_HCLK] = "sd_hclk",
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
	/* 4.12 GPIO Clocks */
	[GPIO0_PCLK] = "gpio0_pclk",
	[GPIO1_PCLK] = "gpio1_pclk",
	[GPIO2_PCLK] = "gpio2_pclk",
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
	[TSENSOR0_CLK] = "tsensor0_clk",
	[TSENSOR1_CLK] = "tsensor1_clk",
	[TSENSOR2_CLK] = "tsensor2_clk",
	[TSENSOR3_CLK] = "tsensor3_clk",
	/* Additional Resets */
	[CRCNTL_RST_PCIE_BUTTON] = "pcie_button_rst",
	[CRCNTL_RST_PCIE_POWER_UP] = "pcie_power_up_rst",
	[CRCNTL_RST_PCIE_TEST] = "pcie_test_rst",
	[TIMER0_PCLK] = "timer0_pclk",
	[TIMER1_PCLK] = "timer1_pclk",
	[TIMER2_PCLK] = "timer2_pclk",
	[TIMER3_PCLK] = "timer3_pclk",
#if 0
	[SYSFAB_DBG_CLK] = "sysfab_dbg_clk",
	[SYSFAB_TIC_CLK] = "sysfab_tic_clk",
	/* [TIC_CLK] = "tic_clk", */
	[CORESIGHT_CLK] = "coresight_clk",
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

	if (output_clks[clk].clk_dep != invalid_clk)
		clk_enable(output_clks[clk].clk_dep);
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

	if (output_clks[clk].clk_dep != invalid_clk)
		clk_disable(output_clks[clk].clk_dep);
	if (output_clks[clk].clk_src != invalid_clk)
		clk_disable(output_clks[clk].clk_src);
	if (output_clks[clk].flags & CLK_SW_RST_F)
		crcntl_clk_assert(clk);
	if (output_clks[clk].flags & CLK_CLK_EN_F)
		crcntl_clk_disable(clk);
}

static uint32_t get_output_clk_freq(clk_clk_t clk)
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

struct pll_clk {
	uint32_t freq;
	bool enabled;
};

struct pll_clk pll_clks[NR_PLL_CLKS] = {
	[SOC_PLL] = {
		.freq = SOC_PLL_FREQ,
		.enabled = false,
	},
	[DDR_PLL] = {
		.freq = DDR_PLL_FREQ,
		.enabled = false,
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *pll_clk_names[NR_PLL_CLKS] = {
	[SOC_PLL] = "soc_pll",
	[DDR_PLL] = "ddr_pll",
};

const char *get_pll_name(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return NULL;
	return pll_clk_names[clk];
}
#else
#define get_pll_name		NULL
#endif

static void __enable_pll(clk_clk_t clk)
{
	if (!pll_clks[clk].enabled) {
		crcntl_pll_enable(clk, pll_clks[clk].freq);
		pll_clks[clk].enabled = true;
	}
}

static void __disable_pll(clk_clk_t clk)
{
	if (pll_clks[clk].enabled) {
		pll_clks[clk].enabled = false;
		crcntl_pll_disable(clk);
	}
}

static int enable_pll(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return -EINVAL;
	__enable_pll(clk);
	return 0;
}

static void disable_pll(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return;
	__disable_pll(clk);
}

static uint32_t get_pll_freq(clk_clk_t clk)
{
	if (clk >= NR_PLL_CLKS)
		return INVALID_FREQ;
	return pll_clks[clk].freq;
}

static int set_pll_freq(clk_clk_t clk, uint32_t freq)
{
	if (clk >= NR_PLL_CLKS)
		return -EINVAL;

	if (pll_clks[clk].freq != freq) {
		__disable_pll(clk);
		pll_clks[clk].freq = freq;
	}
	__enable_pll(clk);
	return 0;
}

struct clk_driver clk_pll = {
	.max_clocks = NR_PLL_CLKS,
	.enable = enable_pll,
	.disable = disable_pll,
	.get_freq = get_pll_freq,
	.set_freq = set_pll_freq,
	.select = NULL,
	.get_name = get_pll_name,
};

struct select_clk {
	clk_t clk_sels[2];
	bool flags;
};

struct select_clk select_clks[] = {
	[SYSFAB_CLK_SEL] = {
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
	},
	[DDR_CLK_SEL] = {
		.clk_sels = {
			ddr_pll,
			xo_clk,
		},
	},
	[DDR_CLK_DIV4_SEL] = {
		.clk_sels = {
			ddr_pll,
			ddr_pll_div4,
		},
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *sel_clk_names[NR_DIV_CLKS] = {
	[SYSFAB_CLK_SEL] = "soc_clk(sysfab_clk_sel)",
	[DDR_CLK_SEL] = "ddr_clk(ddr_clk_sel)",
	[DDR_CLK_DIV4_SEL] = "ddr_clk_div4(ddr_clk_div4_sel)",
};

static const char *get_clk_sel_name(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return NULL;
	return sel_clk_names[clk];
}
#else
#define get_clk_sel_name	NULL
#endif

static int enable_clk_sel(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return -EINVAL;
	if (!(select_clks[clk].flags & CLK_CLK_SEL_F)) {
		clk_enable(select_clks[clk].clk_sels[0]);
		if (!crcntl_clk_selected(clk))
			__raw_clearl(_BV(clk), CRCNTL_CLK_SEL_CFG);
		if (select_clks[clk].flags & CLK_CLK_EN_F)
			clk_disable(select_clks[clk].clk_sels[1]);
		else
			select_clks[clk].flags |= CLK_CLK_EN_F;
		select_clks[clk].flags |= CLK_CLK_SEL_F;
	}
	return 0;
}

static void disable_clk_sel(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return;
	if (select_clks[clk].flags & CLK_CLK_SEL_F) {
		clk_enable(select_clks[clk].clk_sels[1]);
		if (!crcntl_clk_selected(clk))
			__raw_setl(_BV(clk), CRCNTL_CLK_SEL_CFG);
		if (select_clks[clk].flags & CLK_CLK_EN_F)
			clk_disable(select_clks[clk].clk_sels[0]);
		else
			select_clks[clk].flags |= CLK_CLK_EN_F;
		select_clks[clk].flags &= ~CLK_CLK_SEL_F;
	}
}

static uint32_t get_clk_sel_freq(clk_clk_t clk)
{
	if (clk >= NR_SELECT_CLKS)
		return INVALID_FREQ;
	if (crcntl_clk_selected(clk))
		return clk_get_frequency(select_clks[clk].clk_sels[0]);
	else
		return clk_get_frequency(select_clks[clk].clk_sels[1]);
}

struct clk_driver clk_select = {
	.max_clocks = NR_SELECT_CLKS,
	.enable = enable_clk_sel,
	.disable = disable_clk_sel,
	.get_freq = get_clk_sel_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_clk_sel_name,
};

struct div_clk {
	clk_t derived;
	uint8_t div;
};

struct div_clk div_clks[NR_DIV_CLKS] = {
	[SYSFAB_CLK] = {
		.derived = soc_clk,
		.div = 4,
	},
	[SYSFAB_HALF_CLK] = {
		.derived = sysfab_clk,
		.div = 8,
	},
	[SOC_PLL_DIV10] = {
		.derived = soc_pll,
		.div = 10,
	},
	[SD_TM_CLK] = {
		.derived = soc_pll_div10,
		.div = 100,
	},
	[DDR_PLL_DIV4] = {
		.derived = ddr_pll,
		.div = 4,
	},
	[XO_CLK_DIV4] = {
		.derived = xo_clk,
		.div = 4,
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *div_clk_names[NR_DIV_CLKS] = {
	[SYSFAB_CLK] = "sysfab_clk",
	[SYSFAB_HALF_CLK] = "sysfab_half_clk",
	[SOC_PLL_DIV10] = "soc_pll_div10",
	[SD_TM_CLK] = "sd_tm_clk",
	[DDR_PLL_DIV4] = "ddr_pll_div4",
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
	return clk_enable(div_clks[clk].derived);
}

static void disable_pll_div(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return;
	clk_disable(div_clks[clk].derived);
}

static uint32_t get_pll_div_freq(clk_clk_t clk)
{
	if (clk >= NR_DIV_CLKS)
		return INVALID_FREQ;
	return clk_get_frequency(div_clks[clk].derived) /
	       div_clks[clk].div;
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

uint32_t input_clks[NR_INPUT_CLKS] = {
	[XO_CLK] = XO_CLK_FREQ,
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *input_clk_names[NR_INPUT_CLKS] = {
	[XO_CLK] = "xo_clk",
};

static const char *get_input_clk_name(clk_clk_t clk)
{
	if (clk >= NR_INPUT_CLKS)
		return NULL;
	return input_clk_names[clk];
}
#else
#define get_input_clk_name	NULL
#endif

static uint32_t get_input_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_INPUT_CLKS)
		return INVALID_FREQ;
	return input_clks[clk];
}

struct clk_driver clk_input = {
	.max_clocks = NR_INPUT_CLKS,
	.enable = NULL,
	.disable = NULL,
	.get_freq = get_input_clk_freq,
	.set_freq = NULL,
	.select = NULL,
	.get_name = get_input_clk_name,
};

/*===========================================================================
 * Clock tree APIs
 *===========================================================================*/
static bool clk_hw_init = false;

void crcntl_init_zsbl(void)
{
	if (!clk_hw_init) {
		clk_register_driver(CLK_INPUT, &clk_input);
		clk_register_driver(CLK_PLL, &clk_pll);
		clk_register_driver(CLK_SELECT, &clk_select);
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

/*===========================================================================
 * CRCNTL APIs
 *===========================================================================*/
bool crcntl_clk_selected(clk_clk_t clk)
{
	return !(__raw_readl(CRCNTL_CLK_SEL_CFG) & _BV(clk));
}

bool crcntl_clk_enabled(clk_clk_t clk)
{
	uint8_t n, bit;

	BUG_ON(!(output_clks[clk].flags & CLK_CLK_EN_F));
	bit = clk & REG_5BIT_MASK;
	n = clk >> 5;
	return !!(__raw_readl(CRCNTL_CLK_EN_CFG(n)) & _BV(bit));
}

bool crcntl_clk_asserted(clk_clk_t clk)
{
	uint8_t n, bit;

	BUG_ON(!(output_clks[clk].flags & CLK_SW_RST_F));
	bit = clk & REG_5BIT_MASK;
	n = clk >> 5;
	return !!(__raw_readl(CRCNTL_SW_RST_CFG(n)) & _BV(bit));
}

void crcntl_clk_enable(clk_clk_t clk)
{
	uint8_t n, bit;

	if (!(output_clks[clk].flags & CLK_CLK_EN_F))
		return;
	if (!crcntl_clk_enabled(clk)) {
		bit = clk & REG_5BIT_MASK;
		n = clk >> 5;
		__raw_setl(_BV(bit), CRCNTL_CLK_EN_CFG(n));
	}
}

void crcntl_clk_disable(clk_clk_t clk)
{
	uint8_t n, bit;

	if (!(output_clks[clk].flags & CLK_CLK_EN_F))
		return;
	if (crcntl_clk_enabled(clk)) {
		bit = clk & REG_5BIT_MASK;
		n = clk >> 5;
		__raw_clearl(_BV(bit), CRCNTL_CLK_EN_CFG(n));
	}
}

void crcntl_clk_assert(clk_clk_t clk)
{
	uint8_t n, bit;

	if (!(output_clks[clk].flags & CLK_SW_RST_F))
		return;
	if (!crcntl_clk_asserted(clk)) {
		bit = clk & REG_5BIT_MASK;
		n = clk >> 5;
		__raw_setl(_BV(bit), CRCNTL_SW_RST_CFG(n));
	}
}

void crcntl_clk_deassert(clk_clk_t clk)
{
	uint8_t n, bit;

	if (!(output_clks[clk].flags & CLK_SW_RST_F))
		return;
	if (crcntl_clk_asserted(clk)) {
		bit = clk & REG_5BIT_MASK;
		n = clk >> 5;
		__raw_clearl(_BV(bit), CRCNTL_SW_RST_CFG(n));
	}
}

void crcntl_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val)
{
	__raw_writel(PLL_REG_WRITE | PLL_REG_SEL(reg) | PLL_REG_WDATA(val),
		     CRCNTL_PLL_REG_ACCESS(pll));
	while (!(__raw_readl(CRCNTL_PLL_REG_ACCESS(pll)) & PLL_REG_IDLE));
}

uint8_t crcntl_pll_reg_read(uint8_t pll, uint8_t reg)
{
	uint32_t val;

	__raw_writel(PLL_REG_READ | PLL_REG_SEL(reg),
		     CRCNTL_PLL_REG_ACCESS(pll));
	do {
		val = __raw_readl(CRCNTL_PLL_REG_ACCESS(pll));
	} while (val & PLL_REG_IDLE);
	return PLL_REG_RDATA(val);
}

static int do_crcntl_dump(int argc, char *argv[])
{
	int i;

	printf("type id  %20s %20s\n", "name", "source");
	for (i = 0; i < NR_PLL_CLKS; i++) {
		if (pll_clk_names[i]) {
			printf("pll  %3d %20s %20s\n",
			       i, pll_clk_names[i], "xo_clk");
		}
	}
	for (i = 0; i < NR_DIV_CLKS; i++) {
		if (div_clk_names[i]) {
			printf("div  %3d %20s %20s\n",
			       i, div_clk_names[i],
			       clk_get_mnemonic(div_clks[i].derived));
		}
	}
	for (i = 0; i < NR_SELECT_CLKS; i++) {
		if (sel_clk_names[i]) {
			printf("clk  %3d %20s %20s\n",
			       i, sel_clk_names[i],
			       clk_get_mnemonic(select_clks[i].clk_sels[0]));
			if (select_clks[i].clk_sels[1] != invalid_clk)
				printf("%4s %3s %20s %20s\n", "", "", "",
				       clk_get_mnemonic(
					       select_clks[i].clk_sels[1]));
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
