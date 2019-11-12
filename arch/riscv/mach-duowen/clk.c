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
 * @(#)clk.c: DUOWEN clock controller implementation
 * $Id: clk.c,v 1.1 2019-11-06 10:39:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/delay.h>

#ifdef CONFIG_DUOWEN_CLK_TEST
void clk_hw_ctrl_init(void)
{
	int i;

	crcntl_pll_enable(DDR_PLL, PLL_DDR_FREQ);
	crcntl_pll_enable(SOC_PLL, PLL_SOC_FREQ);
	__raw_writel(0, CRCNTL_CLK_SEL_CFG);
	for (i = 0; i < 5; i++)
		__raw_writel(0xFFFFFFFF, CRCNTL_CLK_EN_CFG(i));
}
#else
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
#define CLK_SELECTED	_BV(11)
#define CLK_ENABLED	_BV(12)
};

struct output_clk output_clks[] = {
	[CRCNTL_DMA_MST] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DMA_AHB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_PCIE_M] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_PCIE_S] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_PCIE_PHY_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
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
		.freq = 0,
		.clk_sels = {
			ddr_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRC0_CORE] = {
		.freq = 0,
		.clk_sels = {
			ddr_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRC0_SBR] = {
		.freq = 0,
		.clk_sels = {
			ddr_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP0_DFI] = {
		.freq = 0,
		.clk_sels = {
			ddr_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP0] = {
		/* XXX: CRCNTL_DDRP0_BYPASS_P */
		.flags = CLK_R,
	},
	[CRCNTL_DDRC0_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP0_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP0_DFICTL] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(DDR_DFI_CLK_SEL),
	},
	[CRCNTL_DDRP0_SCAN] = {
		.flags = CLK_R,
	},
	[CRCNTL_DDRC1_SLAVE] = {
		.freq = 0,
		.clk_sels = {
			ddr_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRC1_CORE] = {
		.freq = 0,
		.clk_sels = {
			ddr_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRC1_SBR] = {
		.freq = 0,
		.clk_sels = {
			ddr_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP1_DFI] = {
		.freq = 0,
		.clk_sels = {
			ddr_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP1] = {
		/* XXX: CRCNTL_DDRP0_BYPASS_P */
		.flags = CLK_R,
	},
	[CRCNTL_DDRC1_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP1_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_DDRP1_DFICTL] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(DDR_DFI_CLK_SEL),
	},
	[CRCNTL_DDRP1_SCAN] = {
		.flags = CLK_R,
	},
	[CRCNTL_COHFAB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SYSFAB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SYSFAB_CLK_SEL),
	},
	[CRCNTL_CFGFAB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(CFGFAB_CLK_SEL),
	},
	[CRCNTL_SYSFAB_DBG] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SYSFAB_TIC] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_COHFAB_CFG] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_COHFAB_PLL_CFG] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CLUSTER0_AHB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CLUSTER1_AHB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CLUSTER2_AHB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CLUSTER3_AHB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CRCNTL_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_WDT0_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_WDT1_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_UART0_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_UART1_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_UART2_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_UART3_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST0_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST1_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST2_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST3_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST4_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST5_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST6_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_MST7_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_SLV0_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_SLV1_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_SLV2_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_I2C_SLV3_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SPI_MST0_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SPI_MST1_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SPI_MST2_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SPI_MST3_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SPI_SLV_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_GPIO_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TLMM_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_PLIC] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_BROM] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIC] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_CORESIGHT] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_IMC] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR | CLK_SEL(IMC_CLK_SEL),
	},
	[CRCNTL_CRCNTL] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_WDT0] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_WDT1] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_UART0] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(UART0_CLK_SEL),
	},
	[CRCNTL_UART1] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(UART1_CLK_SEL),
	},
	[CRCNTL_UART2] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(UART2_CLK_SEL),
	},
	[CRCNTL_UART3] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(UART3_CLK_SEL),
	},
	[CRCNTL_I2C_MST0] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST0_CLK_SEL),
	},
	[CRCNTL_I2C_MST1] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST1_CLK_SEL),
	},
	[CRCNTL_I2C_MST2] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST2_CLK_SEL),
	},
	[CRCNTL_I2C_MST3] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST3_CLK_SEL),
	},
	[CRCNTL_I2C_MST4] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST4_CLK_SEL),
	},
	[CRCNTL_I2C_MST5] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST5_CLK_SEL),
	},
	[CRCNTL_I2C_MST6] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST6_CLK_SEL),
	},
	[CRCNTL_I2C_MST7] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_MST7_CLK_SEL),
	},
	[CRCNTL_I2C_SLV0] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_SLV0_CLK_SEL),
	},
	[CRCNTL_I2C_SLV1] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_SLV1_CLK_SEL),
	},
	[CRCNTL_I2C_SLV2] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_SLV2_CLK_SEL),
	},
	[CRCNTL_I2C_SLV3] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(I2C_SLV3_CLK_SEL),
	},
	[CRCNTL_SPI_MST0] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SPI_MST0_CLK_SEL),
	},
	[CRCNTL_SPI_MST1] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SPI_MST1_CLK_SEL),
	},
	[CRCNTL_SPI_MST2] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SPI_MST2_CLK_SEL),
	},
	[CRCNTL_SPI_MST3] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SPI_MST3_CLK_SEL),
	},
	[CRCNTL_SPI_SLV] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SPI_SLV_CLK_SEL),
	},
	[CRCNTL_SD_HCLK] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_SD_BCLK] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SD_BCLK_CLK_SEL),
	},
	[CRCNTL_SD_CCLK_TX] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SD_CCLK_TX_CLK_SEL),
	},
	[CRCNTL_SD_CCLK_RX] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
			xo_clk,
		},
		.flags = CLK_CR | CLK_SEL(SD_CCLK_RX_CLK_SEL),
	},
	[CRCNTL_SD_TM_RX] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER0] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER1] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER2] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER3] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER4] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER5] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER6] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER7] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER8] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER9] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER10] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER11] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER12] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER13] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER14] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER15] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER16] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER17] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER18] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER19] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER20] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER21] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER22] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER23] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER24] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER_7_0_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER_15_8_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER_23_16_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
	[CRCNTL_TIMER_24_APB] = {
		.freq = 0,
		.clk_sels = {
			soc_pll,
		},
		.flags = CLK_CR,
	},
};

void periph_select_clock(clk_t clkid)
{
	clk_clk_t clk;
	uint8_t bit;

	if (clk_cat(clkid) != CLK_OUTPUT)
		return;
	clk = clk_clk(clkid);
	if (output_clks[clk].flags & CLK_CLK_SEL_F) {
		bit = clk_sel(output_clks[clk].flags);
		if (!(output_clks[clk].flags & CLK_SELECTED)) {
			clk_enable(output_clks[clk].clk_sels[1]);
			__raw_setl(_BV(bit), CRCNTL_CLK_SEL_CFG);
			clk_disable(output_clks[clk].clk_sels[0]);
			raise_bits(output_clks[clk].flags, CLK_SELECTED);
		}
	}
}

void periph_deselect_clock(clk_t clkid)
{
	clk_clk_t clk;
	uint8_t bit;

	if (clk_cat(clkid) != CLK_OUTPUT)
		return;
	clk = clk_clk(clkid);
	if (output_clks[clk].flags & CLK_CLK_SEL_F) {
		bit = clk_sel(output_clks[clk].flags);
		if (output_clks[clk].flags & CLK_SELECTED) {
			clk_enable(output_clks[clk].clk_sels[0]);
			__raw_clearl(_BV(bit), CRCNTL_CLK_SEL_CFG);
			clk_disable(output_clks[clk].clk_sels[1]);
			unraise_bits(output_clks[clk].flags, CLK_SELECTED);
		}
	}
}

static int enable_output_clk(clk_clk_t clk)
{
	uint8_t n, bit;

	if (clk >= NR_OUTPUT_CLKS)
		return -EINVAL;

	if (output_clks[clk].flags & CLK_CLK_SEL_F) {
		bit = clk_sel(output_clks[clk].flags);
		if (output_clks[clk].flags & CLK_SELECTED) {
			clk_enable(output_clks[clk].clk_sels[1]);
			__raw_setl(_BV(bit), CRCNTL_CLK_SEL_CFG);
		} else {
			clk_enable(output_clks[clk].clk_sels[0]);
			__raw_clearl(_BV(bit), CRCNTL_CLK_SEL_CFG);
		}
	}
	if (output_clks[clk].flags & CLK_CLK_EN_F &&
	    !(output_clks[clk].flags & CLK_ENABLED)) {
		bit = clk & REG_5BIT_MASK;
		n = clk >> 5;
		__raw_setl(_BV(bit), CRCNTL_CLK_EN_CFG(n));
	}
	return 0;
}

static void disable_output_clk(clk_clk_t clk)
{
	uint8_t n, bit;

	if (clk >= NR_OUTPUT_CLKS)
		return;

	if (output_clks[clk].flags & CLK_CLK_EN_F &&
	    output_clks[clk].flags & CLK_ENABLED) {
		bit = clk & REG_5BIT_MASK;
		n = clk >> 5;
		__raw_clearl(_BV(bit), CRCNTL_CLK_EN_CFG(n));
	}
}

static uint32_t get_output_clk_freq(clk_clk_t clk)
{
	if (clk >= NR_OUTPUT_CLKS)
		return INVALID_FREQ;
	return INVALID_FREQ;
}

static int set_output_clk_freq(clk_clk_t clk, uint32_t freq)
{
	if (clk >= NR_OUTPUT_CLKS)
		return -EINVAL;
	return 0;
}

struct clk_driver clk_output = {
	.max_clocks = NR_OUTPUT_CLKS,
	.enable = enable_output_clk,
	.disable = disable_output_clk,
	.get_freq = get_output_clk_freq,
	.set_freq = set_output_clk_freq,
};

struct pll_clk {
	uint32_t freq;
	bool enabled;
};

struct pll_clk pll_clks[NR_PLL_CLKS] = {
	[SOC_PLL] = {
		.freq = PLL_SOC_FREQ,
		.enabled = false,
	},
	[DDR_PLL] = {
		.freq = PLL_DDR_FREQ,
		.enabled = false,
	},
};

static void __enable_pll(clk_clk_t clk)
{
	if (!pll_clks[clk].enabled) {
		crcntl_pll_enable(clk, pll_clks[clk].freq);
		pll_clks[clk].enabled = true;
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
	crcntl_pll_disable(clk);
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
		pll_clks[clk].freq = freq;
		pll_clks[clk].enabled = false;
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
};

uint32_t input_clks[NR_INPUT_CLKS] = {
	[XO_CLK] = XO_CLK_FREQ,
};

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
};

static bool clk_hw_init = false;

void board_init_clock(void)
{
	if (!clk_hw_init) {
		clk_register_driver(CLK_INPUT, &clk_input);
		clk_register_driver(CLK_PLL, &clk_pll);
		clk_register_driver(CLK_OUTPUT, &clk_output);
	}
	clk_hw_init = true;
}

void clk_hw_ctrl_init(void)
{
	board_init_clock();
}
#endif
