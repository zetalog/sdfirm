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
 * @(#)clk.h: DUOWEN clock controller definitions
 * $Id: clk.h,v 1.1 2019-11-06 10:36:00 zhenglv Exp $
 */

#ifndef __CLK_DUOWEN_H_INCLUDE__
#define __CLK_DUOWEN_H_INCLUDE__

#include <target/arch.h>
#include <asm/mach/crcntl.h>

#define NR_FREQPLANS		1
#define FREQPLAN_RUN		0
#define INVALID_FREQPLAN	NR_FREQPLANS

#define invalid_clk		clkid(0xFF, 0xFF)

#define XO_CLK_FREQ		UL(25000000)
#define CL0_PLL_FREQ		UL(2000000000)
#define CL1_PLL_FREQ		UL(2000000000)
#define CL2_PLL_FREQ		UL(2000000000)
#define CL3_PLL_FREQ		UL(2000000000)
#define COHFAB_PLL_FREQ		UL(2000000000)
#define SOC_PLL_FREQ		UL(1000000000)
#define SOC_PLL_DIV4_FREQ	UL(250000000)
#define SOC_PLL_DIV8_FREQ	UL(125000000)
#define SOC_PLL_DIV10_FREQ	UL(100000000)
#define DDR_PLL_FREQ		UL(800000000)
#define DDR_PLL_DIV4_FREQ	UL(200000000)
#define DDR_CLK_MIN_FREQ	UL(600000000)
#define DDR_CLK_MAX_FREQ	UL(800000000)
#define DDR_BYPASS_PCLK_FREQ	UL(1066666666)
#define SD_TM_CLK_FREQ		UL(1000000)

#define CLK_INPUT		((clk_cat_t)0)
#define XO_CLK			((clk_clk_t)0)
#define TIC_CLK			((clk_clk_t)1)
#define JTAG_CLK		((clk_clk_t)2)
#define NR_INPUT_CLKS		(JTAG_CLK + 1)
#define xo_clk			clkid(CLK_INPUT, XO_CLK)
#define tic_clk			clkid(CLK_INPUT, TIC_CLK)
#define jrag_clk		clkid(CLK_INPUT, JTAG_CLK)

#define CLK_PLL			((clk_cat_t)1)
#define DDR_PLL			((clk_clk_t)0)
#define SOC_PLL			((clk_clk_t)1)
#define COHFAB_PLL		((clk_clk_t)2)
#define CL0_PLL			((clk_clk_t)3)
#define CL1_PLL			((clk_clk_t)4)
#define CL2_PLL			((clk_clk_t)5)
#define CL3_PLL			((clk_clk_t)6)
#define NR_PLL_CLKS		(CL3_PLL + 1)
#define ddr_pll			clkid(CLK_PLL, DDR_PLL)
#define soc_pll			clkid(CLK_PLL, SOC_PLL)
#define cohfab_pll		clkid(CLK_PLL, COHFAB_PLL)
#define cl0_pll			clkid(CLK_PLL, CL0_PLL)
#define cl1_pll			clkid(CLK_PLL, CL1_PLL)
#define cl2_pll			clkid(CLK_PLL, CL2_PLL)
#define cl3_pll			clkid(CLK_PLL, CL3_PLL)

#define CLK_OUTPUT		((clk_cat_t)2)

/* CLK_EN_CFG0 */
#define DMA_CLK			((clk_clk_t)0)
#define DDR_CLK			((clk_clk_t)1)
#define DDR_BYPASS_PCLK		((clk_clk_t)2)
#define PCIE_CLK		((clk_clk_t)4)
#define SYSFAB_DBG_CLK		((clk_clk_t)9)
#define SYSFAB_TIC_CLK		((clk_clk_t)10)
#define CLUSTER0_AHB_CLK	((clk_clk_t)12)
#define CLUSTER1_AHB_CLK	((clk_clk_t)13)
#define CLUSTER2_AHB_CLK	((clk_clk_t)14)
#define CLUSTER3_AHB_CLK	((clk_clk_t)15)
#define COHFAB_AHB_CLK		((clk_clk_t)16)
#define SCSR_CLK		((clk_clk_t)19)
#define TLMM_CLK		((clk_clk_t)20)
#define PLIC_CLK		((clk_clk_t)21)
/* #define TIC_CLK			((clk_clk_t)23)  */
#define CORESIGHT_CLK		((clk_clk_t)24)
#define TIMER3_CLK		((clk_clk_t)28)
#define WDT0_CLK		((clk_clk_t)29)
#define WDT1_CLK		((clk_clk_t)30)

/* CLK_EN_CFG1 */
#define GPIO0_CLK		((clk_clk_t)32)
#define GPIO1_CLK		((clk_clk_t)33)
#define GPIO2_CLK		((clk_clk_t)34)
#define UART0_CLK		((clk_clk_t)35)
#define UART1_CLK		((clk_clk_t)36)
#define UART2_CLK		((clk_clk_t)37)
#define UART3_CLK		((clk_clk_t)38)
#define I2C0_CLK		((clk_clk_t)39)
#define I2C1_CLK		((clk_clk_t)40)
#define I2C2_CLK		((clk_clk_t)41)
#define I2C3_CLK		((clk_clk_t)42)
#define I2C4_CLK		((clk_clk_t)43)
#define I2C5_CLK		((clk_clk_t)44)
#define I2C6_CLK		((clk_clk_t)45)
#define I2C7_CLK		((clk_clk_t)46)
#define I2C8_CLK		((clk_clk_t)47)
#define I2C9_CLK		((clk_clk_t)48)
#define I2C10_CLK		((clk_clk_t)49)
#define I2C11_CLK		((clk_clk_t)50)
#define SPI0_CLK		((clk_clk_t)51)
#define SPI1_CLK		((clk_clk_t)52)
#define SPI2_CLK		((clk_clk_t)53)
#define SPI3_CLK		((clk_clk_t)54)
#define SPI4_CLK		((clk_clk_t)55)
#define SD_CLK			((clk_clk_t)57)

/* CLK_EN_CFG2 */
#define TIMER0_1_CLK		((clk_clk_t)64)
#define TIMER0_2_CLK		((clk_clk_t)65)
#define TIMER0_3_CLK		((clk_clk_t)66)
#define TIMER0_4_CLK		((clk_clk_t)67)
#define TIMER0_5_CLK		((clk_clk_t)68)
#define TIMER0_6_CLK		((clk_clk_t)69)
#define TIMER0_7_CLK		((clk_clk_t)70)
#define TIMER0_8_CLK		((clk_clk_t)71)
#define TIMER1_1_CLK		((clk_clk_t)72)
#define TIMER1_2_CLK		((clk_clk_t)73)
#define TIMER1_3_CLK		((clk_clk_t)74)
#define TIMER1_4_CLK		((clk_clk_t)75)
#define TIMER1_5_CLK		((clk_clk_t)76)
#define TIMER1_6_CLK		((clk_clk_t)77)
#define TIMER1_7_CLK		((clk_clk_t)78)
#define TIMER1_8_CLK		((clk_clk_t)79)
#define TIMER2_1_CLK		((clk_clk_t)80)
#define TIMER2_2_CLK		((clk_clk_t)81)
#define TIMER2_3_CLK		((clk_clk_t)82)
#define TIMER2_4_CLK		((clk_clk_t)83)
#define TIMER2_5_CLK		((clk_clk_t)84)
#define TIMER2_6_CLK		((clk_clk_t)85)
#define TIMER2_7_CLK		((clk_clk_t)86)
#define TIMER2_8_CLK		((clk_clk_t)87)
#define TSENSOR0_CLK		((clk_clk_t)92)
#define TSENSOR1_CLK		((clk_clk_t)93)
#define TSENSOR2_CLK		((clk_clk_t)94)
#define TSENSOR3_CLK		((clk_clk_t)95)

#define NR_OUTPUT_CLKS		(TSENSOR3_CLK + 1)

/* CLK_EN_CFG0 */
#define dma_clk			clkid(CLK_OUTPUT, DMA_CLK)
#define ddr_clk			clkid(CLK_OUTPUT, DDR_CLK)
#define ddr_bypass_pclk		clkid(CLK_OUTPUT, DDR_BYPASS_PCLK)
#define pcie_clk		clkid(CLK_OUTPUT, PCIE_CLK)
#define sysfab_dbg_clk		clkid(CLK_OUTPUT, SYSFAB_DBG_CLK)
#define sysfab_tic_clk		clkid(CLK_OUTPUT, SYSFAB_TIC_CLK)
#define cluster0_ahb_clk	clkid(CLK_OUTPUT, CLUSTER0_AHB_CLK)
#define cluster1_ahb_clk	clkid(CLK_OUTPUT, CLUSTER1_AHB_CLK)
#define cluster2_ahb_clk	clkid(CLK_OUTPUT, CLUSTER2_AHB_CLK)
#define cluster3_ahb_clk	clkid(CLK_OUTPUT, CLUSTER3_AHB_CLK)
#define cohfab_ahb_clk		clkid(CLK_OUTPUT, COHFAB_AHB_CLK)
#define scsr_clk		clkid(CLK_OUTPUT, SCSR_CLK)
#define tlmm_clk		clkid(CLK_OUTPUT, TLMM_CLK)
#define plic_clk		clkid(CLK_OUTPUT, PLIC_CLK)
/* #define tic_clk		clkid(CLK_OUTPUT, TIC_CLK) */
#define coresight_clk		clkid(CLK_OUTPUT, CORESIGHT_CLK)
#define timer3_clk		clkid(CLK_OUTPUT, TIMER3_CLK)
#define wdt0_clk		clkid(CLK_OUTPUT, WDT0_CLK)
#define wdt1_clk		clkid(CLK_OUTPUT, WDT1_CLK)

/* CLK_EN_CFG1 */
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
#define spi0_clk		clkid(CLK_OUTPUT, SPI0_CLK)
#define spi1_clk		clkid(CLK_OUTPUT, SPI1_CLK)
#define spi2_clk		clkid(CLK_OUTPUT, SPI2_CLK)
#define spi3_clk		clkid(CLK_OUTPUT, SPI3_CLK)
#define spi4_clk		clkid(CLK_OUTPUT, SPI4_CLK)
#define sd_clk			clkid(CLK_OUTPUT, SD_CLK)

/* CLK_EN_CFG2 */
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
#define tsensor0_clk		clkid(CLK_OUTPUT, TSENSOR0_CLK)
#define tsensor1_clk		clkid(CLK_OUTPUT, TSENSOR1_CLK)
#define tsensor2_clk		clkid(CLK_OUTPUT, TSENSOR2_CLK)
#define tsensor3_clk		clkid(CLK_OUTPUT, TSENSOR3_CLK)

#define CLK_DIV			((clk_cat_t)3)
#define SOC_PLL_DIV4		((clk_clk_t)0)
#define SOC_PLL_DIV8		((clk_clk_t)1)
#define SOC_PLL_DIV10		((clk_clk_t)2)
#define SD_TM_CLK		((clk_clk_t)3)
#define DDR_PLL_DIV4		((clk_clk_t)4)
#define NR_DIV_CLKS		(DDR_PLL_DIV4 + 1)
#define soc_pll_div4		clkid(CLK_DIV, SOC_PLL_DIV4)
#define soc_pll_div8		clkid(CLK_DIV, SOC_PLL_DIV8)
#define soc_pll_div10		clkid(CLK_DIV, SOC_PLL_DIV10)
#define sd_tm_clk		clkid(CLK_DIV, SD_TM_CLK)
#define ddr_pll_div4		clkid(CLK_DIV, DDR_PLL_DIV4)
#define sysfab_clk_250		soc_pll_div4
#define sysfab_clk_125		soc_pll_div8
#define pcie_ref_clk		soc_pll_div10
#define ddr_high_clk		ddr_pll
#define ddr_low_clk		ddr_pll_div4
#define apb_clk			soc_pll

/* APIs here must be invoked w clock tree core enabled */
void clk_hw_select_boot(clk_t clkid); /* select boot source */
void clk_hw_select_run(clk_t clkid); /* select runtime source */
/* Enable clock tree core */
void clk_hw_ctrl_init(void);

#endif /* __CLK_DUOWEN_H_INCLUDE__ */
