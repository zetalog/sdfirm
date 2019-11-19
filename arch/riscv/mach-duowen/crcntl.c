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
#include <target/delay.h>
#include <target/cmdline.h>
#include <stdio.h>
#include <string.h>

struct output_clk {
	uint32_t freq;
	clk_t clk_sels[2];
	uint32_t flags;
#define CLK_SEL_OFFSET	0
#define CLK_SEL_MASK	REG_5BIT_MASK
#define CLK_CLK_SEL_F	_BV(6)
#define CLK_SEL(value)	(_SET_FV(CLK_SEL, value) | CLK_CLK_SEL_F)
#define clk_sel(value)	_GET_FV(CLK_SEL, value)
#define CLK_CLK_EN_F	_BV(7)
#define CLK_SW_RST_F	_BV(8)
#define CLK_CR		(CLK_CLK_EN_F | CLK_SW_RST_F)
#define CLK_C		CLK_CLK_EN_F
#define CLK_R		CLK_SW_RST_F
#define CLK_BOOT	_BV(11)
#define CLK_ENABLED	_BV(12)
#define PCLK_ENABLED	_BV(13)
};

#ifdef CONFIG_CNCNTL_APB_SOC_PLL_DIV8
#define apb_clk		soc_pll_div8
#else
#define apb_clk		sysfab_clk
#endif

struct output_clk output_clks[] = {
	[CRCNTL_DMA_MST] = {
		.freq = SOC_PLL_FREQ,
		.clk_sels = {
			axi_clk,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DMA_AHB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_PCIE_M] = {
		.freq = SOC_PLL_FREQ,
		.clk_sels = {
			soc_pll,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_PCIE_S] = {
		.freq = SOC_PLL_FREQ,
		.clk_sels = {
			soc_pll,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_PCIE_PHY_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_PCIE_BUTTON] = {
		.flags = CLK_R,
	},
	[CRCNTL_PCIE_POWER_UP] = {
		.flags = CLK_R,
	},
	[CRCNTL_PCIE_TEST] = {
		.flags = CLK_R,
	},
	[CRCNTL_DDRC0_SLAVE] = {
		.freq = DDR_PLL_FREQ,
		.clk_sels = {
			ddr_pll,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRC0_CORE] = {
		.freq = DDR_PLL_FREQ,
		.clk_sels = {
			ddr_high_clk,
			ddr_low_clk,
		},
		.flags = CLK_CR | CLK_SEL(DDR_DFI_CLK_SEL),
	},
	[CRCNTL_DDRC0_SBR] = {
		.freq = DDR_PLL_FREQ,
		.clk_sels = {
			ddr_high_clk,
			ddr_low_clk,
		},
		.flags = CLK_CR | CLK_SEL(DDR_DFI_CLK_SEL),
	},
	[CRCNTL_DDRP0_DFI] = {
		.freq = DDR_PLL_FREQ,
		.clk_sels = {
			ddr_high_clk,
			ddr_low_clk,
		},
		.flags = CLK_CR | CLK_SEL(DDR_DFI_CLK_SEL),
	},
	[CRCNTL_DDRP0] = {
		.freq = DDR_PLL_FREQ, /* ddrp0_bypass_pclk */
		.clk_sels = {
			ddr_pll,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRC0_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP0_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP0_DFICTL] = {
		.freq = DDR_PLL_FREQ,
		.clk_sels = {
			ddr_high_clk,
			ddr_low_clk,
		},
		.flags = CLK_CR | CLK_SEL(DDR_DFI_CLK_SEL),
	},
	[CRCNTL_DDRP0_SCAN] = {
		.flags = CLK_R,
	},
	[CRCNTL_DDRC1_SLAVE] = {
		.freq = DDR_PLL_FREQ,
		.clk_sels = {
			ddr_pll,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRC1_CORE] = {
		.freq = DDR_PLL_FREQ,
		.clk_sels = {
			ddr_high_clk,
			ddr_low_clk,
		},
		.flags = CLK_CR | CLK_SEL(DDR_DFI_CLK_SEL),
	},
	[CRCNTL_DDRC1_SBR] = {
		.freq = DDR_PLL_FREQ,
		.clk_sels = {
			ddr_high_clk,
			ddr_low_clk,
		},
		.flags = CLK_CR | CLK_SEL(DDR_DFI_CLK_SEL),
	},
	[CRCNTL_DDRP1_DFI] = {
		.freq = DDR_PLL_FREQ,
		.clk_sels = {
			ddr_high_clk,
			ddr_low_clk,
		},
		.flags = CLK_CR | CLK_SEL(DDR_DFI_CLK_SEL),
	},
	[CRCNTL_DDRP1] = {
		.freq = DDR_PLL_FREQ, /* ddrp0_bypass_pclk */
		.clk_sels = {
			ddr_pll,
			invalid_clk,
		},
		.flags = CLK_R,
	},
	[CRCNTL_DDRC1_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP1_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP1_DFICTL] = {
		.freq = DDR_PLL_FREQ,
		.clk_sels = {
			ddr_high_clk,
			ddr_low_clk,
		},
		.flags = CLK_CR | CLK_SEL(DDR_DFI_CLK_SEL),
	},
	[CRCNTL_DDRP1_SCAN] = {
		.flags = CLK_R,
	},
	[CRCNTL_COHFAB] = {
		.freq = FAB_PLL_FREQ,
		.clk_sels = {
			fab_pll,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SYSFAB] = {
		.freq = SOC_PLL_DIV4_FREQ,
		.clk_sels = {
			soc_pll_div4,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SYSFAB_CLK_SEL),
	},
	[CRCNTL_CFGFAB] = {
		.freq = SOC_PLL_DIV4_FREQ,
		.clk_sels = {
			sysfab_clk_250,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(CFGFAB_CLK_SEL),
	},
	[CRCNTL_SYSFAB_DBG] = {
		.freq = SOC_PLL_DIV4_FREQ,
		.clk_sels = {
			sysfab_clk_250,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SYSFAB_TIC] = {
		.freq = SOC_PLL_DIV4_FREQ,
		.clk_sels = {
			sysfab_clk_250,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_COHFAB_CFG] = {
		.freq = SOC_PLL_DIV4_FREQ,
		.clk_sels = {
			sysfab_clk_250,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_COHFAB_PLL_CFG] = {
		.freq = SOC_PLL_DIV4_FREQ,
		.clk_sels = {
			sysfab_clk_250,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CLUSTER0_AHB] = {
		.freq = C0_PLL_FREQ,
		.clk_sels = {
			c0_pll,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CLUSTER1_AHB] = {
		.freq = C1_PLL_FREQ,
		.clk_sels = {
			c1_pll,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CLUSTER2_AHB] = {
		.freq = C2_PLL_FREQ,
		.clk_sels = {
			c2_pll,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CLUSTER3_AHB] = {
		.freq = C3_PLL_FREQ,
		.clk_sels = {
			c3_pll,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CRCNTL_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_WDT0_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_WDT1_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_UART0_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_UART1_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_UART2_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_UART3_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST0_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST1_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST2_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST3_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST4_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST5_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST6_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST7_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_SLV0_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_SLV1_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_SLV2_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_SLV3_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SPI_MST0_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SPI_MST1_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SPI_MST2_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SPI_MST3_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SPI_SLV_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_GPIO_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TLMM_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_PLIC] = {
		.freq = SOC_PLL_DIV4_FREQ,
		.clk_sels = {
			sysfab_clk_250,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_BROM] = {
		.freq = SOC_PLL_DIV4_FREQ,
		.clk_sels = {
			sysfab_clk_250,
			xo_clk,
		},
		.flags = CLK_CR | CLK_BOOT,
	},
#if 0
	[CRCNTL_TIC] = {
		.freq = 0,
		.clk_sels = {
			tic_xclk,
		},
		.flags = CLK_CR,
	},
#endif
	[CRCNTL_CORESIGHT] = {
		.freq = SOC_PLL_DIV4_FREQ,
		.clk_sels = {
			sysfab_clk_250,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_IMC] = {
		.freq = SOC_PLL_DIV4_FREQ,
		.clk_sels = {
			sysfab_clk_250,
			xo_clk,
		},
#ifdef CONFIG_CRCNTL_IMC_BOOT
		.flags = CLK_CR | CLK_SEL(IMC_CLK_SEL) | CLK_BOOT,
#else
		.flags = CLK_CR | CLK_SEL(IMC_CLK_SEL),
#endif
	},
	[CRCNTL_CRCNTL] = {
		.freq = XO_CLK_FREQ,
		.clk_sels = {
			xo_clk,
			xo_clk,
		},
		.flags = CLK_CR | CLK_BOOT,
	},
	[CRCNTL_WDT0] = {
		.freq = XO_CLK_FREQ,
		.clk_sels = {
			xo_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_WDT1] = {
		.freq = XO_CLK_FREQ,
		.clk_sels = {
			xo_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_UART0] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(UART0_CLK_SEL),
	},
	[CRCNTL_UART1] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(UART1_CLK_SEL),
	},
	[CRCNTL_UART2] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(UART2_CLK_SEL),
	},
	[CRCNTL_UART3] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(UART3_CLK_SEL),
	},
	[CRCNTL_I2C_MST0] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST0_CLK_SEL),
	},
	[CRCNTL_I2C_MST1] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST1_CLK_SEL),
	},
	[CRCNTL_I2C_MST2] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST2_CLK_SEL),
	},
	[CRCNTL_I2C_MST3] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST3_CLK_SEL),
	},
	[CRCNTL_I2C_MST4] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST4_CLK_SEL),
	},
	[CRCNTL_I2C_MST5] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST5_CLK_SEL),
	},
	[CRCNTL_I2C_MST6] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST6_CLK_SEL),
	},
	[CRCNTL_I2C_MST7] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST7_CLK_SEL),
	},
	[CRCNTL_I2C_SLV0] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_SLV0_CLK_SEL),
	},
	[CRCNTL_I2C_SLV1] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_SLV1_CLK_SEL),
	},
	[CRCNTL_I2C_SLV2] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_SLV2_CLK_SEL),
	},
	[CRCNTL_I2C_SLV3] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_SLV3_CLK_SEL),
	},
	[CRCNTL_SPI_MST0] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SPI_MST0_CLK_SEL),
	},
	[CRCNTL_SPI_MST1] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SPI_MST1_CLK_SEL),
	},
	[CRCNTL_SPI_MST2] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SPI_MST2_CLK_SEL),
	},
	[CRCNTL_SPI_MST3] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SPI_MST3_CLK_SEL),
	},
	[CRCNTL_SPI_SLV] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SPI_SLV_CLK_SEL),
	},
	[CRCNTL_SD_HCLK] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SD_BCLK] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SD_BCLK_CLK_SEL),
	},
	[CRCNTL_SD_CCLK_TX] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SD_CCLK_TX_CLK_SEL),
	},
	[CRCNTL_SD_CCLK_RX] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SD_CCLK_RX_CLK_SEL),
	},
	[CRCNTL_SD_TM_CLK] = {
		.freq = SD_TM_CLK_FREQ,
		.clk_sels = {
			xo_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER0] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER1] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER2] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER3] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER4] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER5] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER6] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER7] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER8] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER9] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER10] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER11] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER12] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER13] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER14] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER15] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER16] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER17] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER18] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER19] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER20] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER21] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER22] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER23] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			sysfab_clk_125,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER24] = {
		.freq = XO_CLK_FREQ,
		.clk_sels = {
			xo_clk,
			xo_clk,
		},
		.flags = CLK_CR | CLK_BOOT,
	},
	[CRCNTL_TIMER_7_0_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER_15_8_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER_23_16_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			invalid_clk,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER_24_APB] = {
		.freq = SOC_PLL_DIV8_FREQ,
		.clk_sels = {
			apb_clk,
			xo_clk,
		},
		.flags = CLK_CR,
	},
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *output_clk_names[] = {
	[CRCNTL_DMA_MST] = "dma_mst_clk",
	[CRCNTL_DMA_AHB] = "dma_ahb_clk",
	[CRCNTL_PCIE_M] = "pcie_m_clk",
	[CRCNTL_PCIE_S] = "pcie_s_clk",
	[CRCNTL_PCIE_PHY_APB] = "pcie_phy_apb_clk",
	[CRCNTL_PCIE_BUTTON] = "pcie_button_rst",
	[CRCNTL_PCIE_POWER_UP] = "pcie_power_up_rst",
	[CRCNTL_PCIE_TEST] = "pcie_test_rst",
	[CRCNTL_DDRC0_SLAVE] = "ddrc0_slave_clk",
	[CRCNTL_DDRC0_CORE] = "ddrc0_core_clk",
	[CRCNTL_DDRC0_SBR] = "ddrc0_sbr_clk",
	[CRCNTL_DDRP0_DFI] = "ddrp0_dfi_clk",
	[CRCNTL_DDRP0] = "ddrp0_bypass_pclk",
	[CRCNTL_DDRC0_APB] = "ddrc0_apb_clk",
	[CRCNTL_DDRP0_APB] = "ddrp0_apb_clk",
	[CRCNTL_DDRP0_DFICTL] = "ddrp0_dfictl_clk",
	[CRCNTL_DDRP0_SCAN] = "ddrp0_scan_rst",
	[CRCNTL_DDRC1_SLAVE] = "ddrc1_slave_clk",
	[CRCNTL_DDRC1_CORE] = "ddrc1_core_clk",
	[CRCNTL_DDRC1_SBR] = "ddrc1_sbr_clk",
	[CRCNTL_DDRP1_DFI] = "ddrp1_dfi_clk",
	[CRCNTL_DDRP1] = "ddrp1_bypass_pclk",
	[CRCNTL_DDRC1_APB] = "ddrc1_apb_clk",
	[CRCNTL_DDRP1_APB] = "ddrp1_apb_clk",
	[CRCNTL_DDRP1_DFICTL] = "ddrp1_dfictl_clk",
	[CRCNTL_DDRP1_SCAN] = "ddrp1_scan_rst",
	[CRCNTL_COHFAB] = "cohfab_clk",
	[CRCNTL_SYSFAB] = "sysfab_clk",
	[CRCNTL_CFGFAB] = "cfgfab_clk",
	[CRCNTL_SYSFAB_DBG] = "sysfab_dbg_clk",
	[CRCNTL_SYSFAB_TIC] = "sysfab_tic_clk",
	[CRCNTL_COHFAB_CFG] = "cohfab_cfg_clk",
	[CRCNTL_COHFAB_PLL_CFG] = "cohfab_pll_cfg_clk",
	[CRCNTL_CLUSTER0_AHB] = "cluster0_ahb_clk",
	[CRCNTL_CLUSTER1_AHB] = "cluster1_ahb_clk",
	[CRCNTL_CLUSTER2_AHB] = "cluster2_ahb_clk",
	[CRCNTL_CLUSTER3_AHB] = "cluster3_ahb_clk",
	[CRCNTL_CRCNTL_APB] = "crcntl_apb_clk",
	[CRCNTL_WDT0_APB] = "wdt0_apb_clk",
	[CRCNTL_WDT1_APB] = "wdt1_apb_clk",
	[CRCNTL_UART0_APB] = "uart0_apb_clk",
	[CRCNTL_UART1_APB] = "uart1_apb_clk",
	[CRCNTL_UART2_APB] = "uart2_apb_clk",
	[CRCNTL_UART3_APB] = "uart3_apb_clk",
	[CRCNTL_I2C_MST0_APB] = "i2c_mst0_apb_clk",
	[CRCNTL_I2C_MST1_APB] = "i2c_mst1_apb_clk",
	[CRCNTL_I2C_MST2_APB] = "i2c_mst2_apb_clk",
	[CRCNTL_I2C_MST3_APB] = "i2c_mst3_apb_clk",
	[CRCNTL_I2C_MST4_APB] = "i2c_mst4_apb_clk",
	[CRCNTL_I2C_MST5_APB] = "i2c_mst5_apb_clk",
	[CRCNTL_I2C_MST6_APB] = "i2c_mst6_apb_clk",
	[CRCNTL_I2C_MST7_APB] = "i2c_mst7_apb_clk",
	[CRCNTL_I2C_SLV0_APB] = "i2c_slv0_apb_clk",
	[CRCNTL_I2C_SLV1_APB] = "i2c_slv1_apb_clk",
	[CRCNTL_I2C_SLV2_APB] = "i2c_slv2_apb_clk",
	[CRCNTL_I2C_SLV3_APB] = "i2c_slv3_apb_clk",
	[CRCNTL_SPI_MST0_APB] = "spi_mst0_apb_clk",
	[CRCNTL_SPI_MST1_APB] = "spi_mst1_apb_clk",
	[CRCNTL_SPI_MST2_APB] = "spi_mst2_apb_clk",
	[CRCNTL_SPI_MST3_APB] = "spi_mst3_apb_clk",
	[CRCNTL_SPI_SLV_APB] = "spi_slv_apb_clk",
	[CRCNTL_GPIO_APB] = "gpio_apb_clk",
	[CRCNTL_TLMM_APB] = "tlmm_apb_clk",
	[CRCNTL_PLIC] = "plic_clk",
	[CRCNTL_BROM] = "brom_clk",
	/* [CRCNTL_TIC] = "tic_clk", */
	[CRCNTL_CORESIGHT] = "coresight_clk",
	[CRCNTL_IMC] = "imc_clk",
	[CRCNTL_CRCNTL] = "crcntl_clk",
	[CRCNTL_WDT0] = "wdt0_clk",
	[CRCNTL_WDT1] = "wdt1_clk",
	[CRCNTL_UART0] = "uart0_clk",
	[CRCNTL_UART1] = "uart1_clk",
	[CRCNTL_UART2] = "uart2_clk",
	[CRCNTL_UART3] = "uart3_clk",
	[CRCNTL_I2C_MST0] = "i2c_mst0_clk",
	[CRCNTL_I2C_MST1] = "i2c_mst1_clk",
	[CRCNTL_I2C_MST2] = "i2c_mst2_clk",
	[CRCNTL_I2C_MST3] = "i2c_mst3_clk",
	[CRCNTL_I2C_MST4] = "i2c_mst4_clk",
	[CRCNTL_I2C_MST5] = "i2c_mst5_clk",
	[CRCNTL_I2C_MST6] = "i2c_mst6_clk",
	[CRCNTL_I2C_MST7] = "i2c_mst7_clk",
	[CRCNTL_I2C_SLV0] = "i2c_slv0_clk",
	[CRCNTL_I2C_SLV1] = "i2c_slv1_clk",
	[CRCNTL_I2C_SLV2] = "i2c_slv2_clk",
	[CRCNTL_I2C_SLV3] = "i2c_slv3_clk",
	[CRCNTL_SPI_MST0] = "spi_mst0_clk",
	[CRCNTL_SPI_MST1] = "spi_mst1_clk",
	[CRCNTL_SPI_MST2] = "spi_mst2_clk",
	[CRCNTL_SPI_MST3] = "spi_mst3_clk",
	[CRCNTL_SPI_SLV] = "spi_slv_clk",
	[CRCNTL_SD_HCLK] = "sd_hclk_clk",
	[CRCNTL_SD_BCLK] = "sd_bclk_clk",
	[CRCNTL_SD_CCLK_TX] = "sd_cclk_tx_clk",
	[CRCNTL_SD_CCLK_RX] = "sd_cclk_rx_clk",
	[CRCNTL_SD_TM_CLK] = "sd_tm_rx_clk",
	[CRCNTL_TIMER0] = "timer0_clk",
	[CRCNTL_TIMER1] = "timer1_clk",
	[CRCNTL_TIMER2] = "timer2_clk",
	[CRCNTL_TIMER3] = "timer3_clk",
	[CRCNTL_TIMER4] = "timer4_clk",
	[CRCNTL_TIMER5] = "timer5_clk",
	[CRCNTL_TIMER6] = "timer6_clk",
	[CRCNTL_TIMER7] = "timer7_clk",
	[CRCNTL_TIMER8] = "timer8_clk",
	[CRCNTL_TIMER9] = "timer9_clk",
	[CRCNTL_TIMER10] = "timer10_clk",
	[CRCNTL_TIMER11] = "timer11_clk",
	[CRCNTL_TIMER12] = "timer12_clk",
	[CRCNTL_TIMER13] = "timer13_clk",
	[CRCNTL_TIMER14] = "timer14_clk",
	[CRCNTL_TIMER15] = "timer15_clk",
	[CRCNTL_TIMER16] = "timer16_clk",
	[CRCNTL_TIMER17] = "timer17_clk",
	[CRCNTL_TIMER18] = "timer18_clk",
	[CRCNTL_TIMER19] = "timer19_clk",
	[CRCNTL_TIMER20] = "timer20_clk",
	[CRCNTL_TIMER21] = "timer21_clk",
	[CRCNTL_TIMER22] = "timer22_clk",
	[CRCNTL_TIMER23] = "timer23_clk",
	[CRCNTL_TIMER24] = "timer24_clk",
	[CRCNTL_TIMER_7_0_APB] = "timer_7_0_apb_clk",
	[CRCNTL_TIMER_15_8_APB] = "timer_15_8_apb_clk",
	[CRCNTL_TIMER_23_16_APB] = "timer_23_16_apb_clk",
	[CRCNTL_TIMER_24_APB] = "timer_24_apb_clk",
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
	clk_t clkid = clkid(CLK_OUTPUT, clk);

	if (clk >= NR_OUTPUT_CLKS)
		return -EINVAL;

	if (output_clks[clk].flags & CLK_BOOT)
		clk_hw_select_boot(clkid);
	else
		clk_hw_select_run(clkid);
	if (output_clks[clk].flags & CLK_CLK_EN_F) {
		crcntl_clk_enable(clk);
		crcntl_clk_deassert(clk);
		raise_bits(output_clks[clk].flags, CLK_ENABLED);
	}
	return 0;
}

static void disable_output_clk(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return;

	if (output_clks[clk].flags & CLK_CLK_EN_F) {
		crcntl_clk_assert(clk);
		crcntl_clk_disable(clk);
		unraise_bits(output_clks[clk].flags, CLK_ENABLED);
	}
}

static uint32_t get_output_clk_freq(clk_clk_t clk)
{
	uint32_t freq;

	if (clk >= NR_OUTPUT_CLKS)
		return INVALID_FREQ;
	if (output_clks[clk].flags & CLK_CLK_SEL_F) {
		if (crcntl_clk_selected(clk))
			freq = clk_get_frequency(
					output_clks[clk].clk_sels[1]);
		else
			freq = clk_get_frequency(
					output_clks[clk].clk_sels[0]);
	} else {
		freq = clk_get_frequency(output_clks[clk].clk_sels[0]);
		if (freq == INVALID_FREQ)
			freq = clk_get_frequency(
					output_clks[clk].clk_sels[1]);
	}
	return freq;
}

static void select_output_clk_src(clk_clk_t clk, clk_t src)
{
	clk_t clkid = clkid(CLK_OUTPUT, clk);

	if (clk >= NR_OUTPUT_CLKS)
		return;
	if (src == output_clks[clk].clk_sels[0])
		clk_hw_select_run(clkid);
	if (src == output_clks[clk].clk_sels[1])
		clk_hw_select_boot(clkid);
}

struct clk_driver clk_output = {
	.max_clocks = NR_OUTPUT_CLKS,
	.enable = enable_output_clk,
	.disable = disable_output_clk,
	.get_freq = get_output_clk_freq,
	.set_freq = NULL,
	.select = select_output_clk_src,
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

struct div_clk {
	clk_t derived;
	uint8_t div;
};

struct div_clk div_clks[NR_DIV_CLKS] = {
	[SOC_PLL_DIV4] = {
		.derived = soc_pll,
		.div = 4,
	},
	[SOC_PLL_DIV8] = {
		.derived = soc_pll,
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
};

#ifdef CONFIG_CONSOLE_COMMAND
const char *div_clk_names[NR_DIV_CLKS] = {
	[SOC_PLL_DIV4] = "sysfab_clk_250",
	[SOC_PLL_DIV8] = "sysfab_clk_125",
	[SOC_PLL_DIV10] = "soc_pll_div10",
	[SD_TM_CLK] = "sd_tm_clk",
	[DDR_PLL_DIV4] = "ddr_pll_div4",
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
void clk_hw_select_boot(clk_t clkid)
{
	clk_clk_t clk;
	uint8_t bit;

	if (clk_cat(clkid) != CLK_OUTPUT)
		return;
	clk = clk_clk(clkid);
	if (output_clks[clk].flags & CLK_CLK_SEL_F) {
		bit = clk_sel(output_clks[clk].flags);
		clk_enable(output_clks[clk].clk_sels[1]);
		__raw_setl(_BV(bit), CRCNTL_CLK_SEL_CFG);
		if (output_clks[clk].flags & PCLK_ENABLED)
			clk_disable(output_clks[clk].clk_sels[0]);
	} else {
		clk_enable(output_clks[clk].clk_sels[1]);
		if (output_clks[clk].flags & PCLK_ENABLED)
			clk_disable(output_clks[clk].clk_sels[0]);
	}
	output_clks[clk].flags |= PCLK_ENABLED;
}

void clk_hw_select_run(clk_t clkid)
{
	clk_clk_t clk;
	uint8_t bit;

	if (clk_cat(clkid) != CLK_OUTPUT)
		return;
	clk = clk_clk(clkid);
	if (output_clks[clk].flags & CLK_CLK_SEL_F) {
		bit = clk_sel(output_clks[clk].flags);
		clk_enable(output_clks[clk].clk_sels[0]);
		__raw_clearl(_BV(bit), CRCNTL_CLK_SEL_CFG);
		if (output_clks[clk].flags & PCLK_ENABLED)
			clk_disable(output_clks[clk].clk_sels[1]);
	} else {
		clk_enable(output_clks[clk].clk_sels[0]);
		if (output_clks[clk].flags & PCLK_ENABLED)
			clk_disable(output_clks[clk].clk_sels[1]);
	}
	output_clks[clk].flags |= PCLK_ENABLED;
}

static bool clk_hw_init = false;

void board_init_clock(void)
{
	if (!clk_hw_init) {
		clk_register_driver(CLK_INPUT, &clk_input);
		clk_register_driver(CLK_PLL, &clk_pll);
		clk_register_driver(CLK_OUTPUT, &clk_output);
		clk_register_driver(CLK_DIV, &clk_div);

		/* Update the status of the default enabled clocks */
		clk_enable(soc_pll);
		clk_enable(brom_clk);
		clk_enable(imc_clk);
		clk_enable(timer24_clk);
		clk_enable(crcntl_clk);
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
	uint8_t bit;

	BUG_ON(!(output_clks[clk].flags & CLK_CLK_SEL_F));
	bit = clk_sel(output_clks[clk].flags);
	return !!(__raw_readl(CRCNTL_CLK_SEL_CFG) & _BV(bit));
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
	for (i = 0; i < NR_OUTPUT_CLKS; i++) {
		if (output_clk_names[i] &&
		    output_clks[i].flags & CLK_CLK_EN_F) {
			printf("clk  %3d %20s %20s\n",
			       i, output_clk_names[i],
			       clk_get_mnemonic(output_clks[i].clk_sels[0]));
			if (output_clks[i].clk_sels[1] != invalid_clk)
				printf("%4s %3s %20s %20s\n", "", "", "",
				       clk_get_mnemonic(
					       output_clks[i].clk_sels[1]));
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
	"\n"
);
