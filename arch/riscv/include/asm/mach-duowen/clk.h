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

#ifdef CONFIG_CRCNTL
#ifndef ARCH_HAVE_CLK
#define ARCH_HAVE_CLK		1
#else
#error "Multiple CLK controller defined"
#endif
#endif

#define NR_FREQPLANS		1
#define FREQPLAN_RUN		0
#define INVALID_FREQPLAN	NR_FREQPLANS

#define invalid_clk		clkid(0xFF, 0xFF)

#ifdef CONFIG_DUOWEN_ASIC
#define CLK_INPUT		((clk_cat_t)0)
#define XO_CLK			((clk_clk_t)0)
#define TIC_CLK			((clk_clk_t)1)
#define JTAG_CLK		((clk_clk_t)2)
#define NR_INPUT_CLKS		(JTAG_CLK + 1)
#define xo_clk			clkid(CLK_INPUT, XO_CLK)
#define tic_clk			clkid(CLK_INPUT, TIC_CLK)
#define jtag_clk		clkid(CLK_INPUT, JTAG_CLK)

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

#define CLK_SELECT		((clk_cat_t)2)
/* CLK_SEL_CFG */
#define SYSFAB_CLK_SEL		((clk_clk_t)0)
#define DDR_CLK_SEL		((clk_clk_t)1)
#define DDR_CLK_DIV4_SEL	((clk_clk_t)2)
#define NR_SELECT_CLKS		(DDR_CLK_DIV4_SEL + 1)
#define sysfab_clk_sel		clkid(CLK_SELECT, SYSFAB_CLK_SEL)
#define ddr_clk_sel		clkid(CLK_SELECT, DDR_CLK_SEL)
#define ddr_clk_div4_sel	clkid(CLK_SELECT, DDR_CLK_DIV4_SEL)
#define soc_clk			sysfab_clk_sel
#endif

#ifdef CONFIG_DUOWEN_FPGA
#define CLK_INPUT		((clk_cat_t)0)
#define CLK_PLL			CLK_INPUT
#define CLK_SELECT		CLK_INPUT
#define XO_CLK			((clk_clk_t)0)
#define TIC_CLK			((clk_clk_t)1)
#define JTAG_CLK		((clk_clk_t)2)
#define DDR_PLL			((clk_clk_t)3)
#define SOC_PLL			((clk_clk_t)4)
#define COHFAB_PLL		((clk_clk_t)5)
#define CL0_PLL			((clk_clk_t)6)
#define CL1_PLL			((clk_clk_t)7)
#define CL2_PLL			((clk_clk_t)8)
#define CL3_PLL			((clk_clk_t)9)
#define NR_INPUT_CLKS		(CL3_PLL + 1)
#define xo_clk			clkid(CLK_INPUT, XO_CLK)
#define tic_clk			clkid(CLK_INPUT, TIC_CLK)
#define jtag_clk		clkid(CLK_INPUT, JTAG_CLK)
#define ddr_pll			clkid(CLK_PLL, DDR_PLL)
#define soc_pll			clkid(CLK_PLL, SOC_PLL)
#define cohfab_pll		clkid(CLK_PLL, COHFAB_PLL)
#define cl0_pll			clkid(CLK_PLL, CL0_PLL)
#define cl1_pll			clkid(CLK_PLL, CL1_PLL)
#define cl2_pll			clkid(CLK_PLL, CL2_PLL)
#define cl3_pll			clkid(CLK_PLL, CL3_PLL)
#define sysfab_clk_sel		soc_pll
#define ddr_clk_sel		ddr_pll
#define soc_clk			sysfab_clk_sel
#define ddr_clk_div4_sel	ddr_pll
#endif

#define CLK_OUTPUT		((clk_cat_t)3)
/* CLK_EN_CFG0 */
#define DMA_CLK			((clk_clk_t)0)
#define DDR_CLK			((clk_clk_t)1)
#define DDR_PWROKIN_RST		((clk_clk_t)2) /* SW_RST only */
#define DDR_BYPASS_PCLK		((clk_clk_t)2) /* CLK_EN only */
#define PCIE_CLK		((clk_clk_t)4)
#define PCIE_POWER_UP_RST	((clk_clk_t)5) /* SW_RST only */
#define SYSFAB_DBG_CLK		((clk_clk_t)9)
#define SYSFAB_TIC_CLK		((clk_clk_t)10)
#define CLUSTER0_HCLK		((clk_clk_t)12)
#define CLUSTER1_HCLK		((clk_clk_t)13)
#define CLUSTER2_HCLK		((clk_clk_t)14)
#define CLUSTER3_HCLK		((clk_clk_t)15)
#define COHFAB_HCLK		((clk_clk_t)16)
#define SCSR_CLK		((clk_clk_t)19)
#define TLMM_CLK		((clk_clk_t)20)
#define PLIC_CLK		((clk_clk_t)21)
/* #define TIC_CLK			((clk_clk_t)23)  */
#define CORESIGHT_CLK		((clk_clk_t)24)
#define TIMER0_RST		((clk_clk_t)25) /* SW_RST only */
#define TIMER1_RST		((clk_clk_t)26) /* SW_RST only */
#define TIMER2_RST		((clk_clk_t)27) /* SW_RST only */
#define TIMER3_RST		((clk_clk_t)28) /* SW_RST only */
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
#define TIMER0_1_CLK		((clk_clk_t)64) /* CLK_EN only */
#define TIMER0_2_CLK		((clk_clk_t)65) /* CLK_EN only */
#define TIMER0_3_CLK		((clk_clk_t)66) /* CLK_EN only */
#define TIMER0_4_CLK		((clk_clk_t)67) /* CLK_EN only */
#define TIMER0_5_CLK		((clk_clk_t)68) /* CLK_EN only */
#define TIMER0_6_CLK		((clk_clk_t)69) /* CLK_EN only */
#define TIMER0_7_CLK		((clk_clk_t)70) /* CLK_EN only */
#define TIMER0_8_CLK		((clk_clk_t)71) /* CLK_EN only */
#define TIMER1_1_CLK		((clk_clk_t)72) /* CLK_EN only */
#define TIMER1_2_CLK		((clk_clk_t)73) /* CLK_EN only */
#define TIMER1_3_CLK		((clk_clk_t)74) /* CLK_EN only */
#define TIMER1_4_CLK		((clk_clk_t)75) /* CLK_EN only */
#define TIMER1_5_CLK		((clk_clk_t)76) /* CLK_EN only */
#define TIMER1_6_CLK		((clk_clk_t)77) /* CLK_EN only */
#define TIMER1_7_CLK		((clk_clk_t)78) /* CLK_EN only */
#define TIMER1_8_CLK		((clk_clk_t)79) /* CLK_EN only */
#define TIMER2_1_CLK		((clk_clk_t)80) /* CLK_EN only */
#define TIMER2_2_CLK		((clk_clk_t)81) /* CLK_EN only */
#define TIMER2_3_CLK		((clk_clk_t)82) /* CLK_EN only */
#define TIMER2_4_CLK		((clk_clk_t)83) /* CLK_EN only */
#define TIMER2_5_CLK		((clk_clk_t)84) /* CLK_EN only */
#define TIMER2_6_CLK		((clk_clk_t)85) /* CLK_EN only */
#define TIMER2_7_CLK		((clk_clk_t)86) /* CLK_EN only */
#define TIMER2_8_CLK		((clk_clk_t)87) /* CLK_EN only */
#define TIMER3_CLK		((clk_clk_t)88) /* CLK_EN only */
#define TSENSOR0_CLK		((clk_clk_t)92)
#define TSENSOR1_CLK		((clk_clk_t)93)
#define TSENSOR2_CLK		((clk_clk_t)94)
#define TSENSOR3_CLK		((clk_clk_t)95)

/* CLK_EN_CFG3 */
#define IMC_CLK			((clk_clk_t)96) /* SW_RST only */

#define NR_OUTPUT_CLKS		(IMC_CLK + 1)

/* CLK_EN_CFG0 */
/* Integrated overall clocks */
#define dma_clk			clkid(CLK_OUTPUT, DMA_CLK)
#define ddr_clk			clkid(CLK_OUTPUT, DDR_CLK)
#define ddr_bypass_pclk		clkid(CLK_OUTPUT, DDR_BYPASS_PCLK)
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
/* Alias for AO domain clocks */
#define imc_clk			sysfab_clk
#define ram_clk			sysfab_clk
#define brom_clk		sysfab_clk

/* CLK_EN_CFG1 */
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
#define spi0_clk		clkid(CLK_OUTPUT, SPI0_CLK)
#define spi1_clk		clkid(CLK_OUTPUT, SPI1_CLK)
#define spi2_clk		clkid(CLK_OUTPUT, SPI2_CLK)
#define spi3_clk		clkid(CLK_OUTPUT, SPI3_CLK)
#define spi4_clk		clkid(CLK_OUTPUT, SPI4_CLK)
#define sd_clk			clkid(CLK_OUTPUT, SD_CLK)
/* TODO: dependency clocks:
 * sd_tm_clk: depends sd_clk, sources soc_pll_div10/100
 */

/* CLK_EN_CFG2 */
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
#define tsensor0_clk		clkid(CLK_OUTPUT, TSENSOR0_CLK)
#define tsensor1_clk		clkid(CLK_OUTPUT, TSENSOR1_CLK)
#define tsensor2_clk		clkid(CLK_OUTPUT, TSENSOR2_CLK)
#define tsensor3_clk		clkid(CLK_OUTPUT, TSENSOR3_CLK)

#define CLK_DIV			((clk_cat_t)4)
#define SYSFAB_CLK		((clk_clk_t)0)
#define SYSFAB_HALF_CLK		((clk_clk_t)1)
#define SOC_PLL_DIV10		((clk_clk_t)2)
#define SD_TM_CLK		((clk_clk_t)3)
#define DDR_PLL_DIV4		((clk_clk_t)4)
#define XO_CLK_DIV4		((clk_clk_t)5)
#define NR_DIV_CLKS		(XO_CLK_DIV4 + 1)
#define sysfab_clk		clkid(CLK_DIV, SYSFAB_CLK)
#define sysfab_half_clk		clkid(CLK_DIV, SYSFAB_HALF_CLK)
#define soc_pll_div10		clkid(CLK_DIV, SOC_PLL_DIV10)
#define sd_tm_clk		clkid(CLK_DIV, SD_TM_CLK)
#define ddr_pll_div4		clkid(CLK_DIV, DDR_PLL_DIV4)
#define xo_clk_div4		clkid(CLK_DIV, XO_CLK_DIV4)

/* Clock flags, used by clk drivers to indicate clock features */
#define CLK_CLK_SEL_F	_BV(6)
#define CLK_CLK_EN_F	_BV(7)
#define CLK_SW_RST_F	_BV(8)
#define CLK_CR		(CLK_CLK_EN_F | CLK_SW_RST_F)
#define CLK_C		CLK_CLK_EN_F
#define CLK_R		CLK_SW_RST_F

void clk_pll_dump(void);
void clk_pll_init(void);

/* Enable clock tree core */
void clk_hw_ctrl_init(void);
#ifdef CONFIG_MMU
#define clk_hw_mmu_init()	do { } while (0)
#endif

#endif /* __CLK_DUOWEN_H_INCLUDE__ */
