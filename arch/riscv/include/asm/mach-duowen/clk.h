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
#define C0_PLL_FREQ		UL(2000000000)
#define C1_PLL_FREQ		UL(2000000000)
#define C2_PLL_FREQ		UL(2000000000)
#define C3_PLL_FREQ		UL(2000000000)
#define FAB_PLL_FREQ		UL(1800000000)
#define SOC_PLL_FREQ		UL(1000000000)
#define SOC_PLL_DIV4_FREQ	UL(250000000)
#define SOC_PLL_DIV8_FREQ	UL(125000000)
#define SOC_PLL_DIV10_FREQ	UL(100000000)
#define DDR_PLL_FREQ		UL(800000000)
#define DDR_PLL_DIV4_FREQ	UL(200000000)
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
#define FAB_PLL			((clk_clk_t)2)
#define C0_PLL			((clk_clk_t)3)
#define C1_PLL			((clk_clk_t)4)
#define C2_PLL			((clk_clk_t)5)
#define C3_PLL			((clk_clk_t)6)
#define NR_PLL_CLKS		(C3_PLL + 1)
#define ddr_pll			clkid(CLK_PLL, DDR_PLL)
#define soc_pll			clkid(CLK_PLL, SOC_PLL)
#define fab_pll			clkid(CLK_PLL, FAB_PLL)
#define c0_pll			clkid(CLK_PLL, C0_PLL)
#define c1_pll			clkid(CLK_PLL, C1_PLL)
#define c2_pll			clkid(CLK_PLL, C2_PLL)
#define c3_pll			clkid(CLK_PLL, C3_PLL)
#define axi_clk			soc_pll

#define CLK_OUTPUT		((clk_cat_t)2)

/* CLK_EN_CFG2 */
#define CRCNTL_APB_CLK		((clk_clk_t)64)
#define WDT0_APB_CLK		((clk_clk_t)65)
#define WDT1_APB_CLK		((clk_clk_t)66)
#define UART0_APB_CLK		((clk_clk_t)67)
#define UART1_APB_CLK		((clk_clk_t)68)
#define UART2_APB_CLK		((clk_clk_t)69)
#define UART3_APB_CLK		((clk_clk_t)70)
#define I2C_MST0_APB_CLK	((clk_clk_t)71)
#define I2C_MST1_APB_CLK	((clk_clk_t)72)
#define I2C_MST2_APB_CLK	((clk_clk_t)73)
#define I2C_MST3_APB_CLK	((clk_clk_t)74)
#define I2C_MST4_APB_CLK	((clk_clk_t)75)
#define I2C_MST5_APB_CLK	((clk_clk_t)76)
#define I2C_MST6_APB_CLK	((clk_clk_t)77)
#define I2C_MST7_APB_CLK	((clk_clk_t)78)
#define I2C_SLV0_APB_CLK	((clk_clk_t)79)
#define I2C_SLV1_APB_CLK	((clk_clk_t)80)
#define I2C_SLV2_APB_CLK	((clk_clk_t)81)
#define I2C_SLV3_APB_CLK	((clk_clk_t)82)
#define SPI_MST0_APB_CLK	((clk_clk_t)83)
#define SPI_MST1_APB_CLK	((clk_clk_t)84)
#define SPI_MST2_APB_CLK	((clk_clk_t)85)
#define SPI_MST3_APB_CLK	((clk_clk_t)86)
#define SPI_SLV_APB_CLK		((clk_clk_t)87)
#define GPIO_APB_CLK		((clk_clk_t)88)
#define SCSR_APB_CLK		((clk_clk_t)89)
#define TLMM_APB_CLK		((clk_clk_t)90)
#define PLIC_CLK		((clk_clk_t)91)
#define BROM_CLK		((clk_clk_t)92)
/* #define TIC_CLK			((clk_clk_t)93)  */
#define CORESIGHT_CLK		((clk_clk_t)94)
#define IMC_CLK			((clk_clk_t)95)

/* CLK_EN_CFG3 */
#define CRCNTL_CLK		((clk_clk_t)96)
#define WDT0_CLK		((clk_clk_t)97)
#define WDT1_CLK		((clk_clk_t)98)
#define UART0_CLK		((clk_clk_t)99)
#define UART1_CLK		((clk_clk_t)100)
#define UART2_CLK		((clk_clk_t)101)
#define UART3_CLK		((clk_clk_t)102)
#define I2C_MST0_CLK		((clk_clk_t)103)
#define I2C_MST1_CLK		((clk_clk_t)104)
#define I2C_MST2_CLK		((clk_clk_t)105)
#define I2C_MST3_CLK		((clk_clk_t)106)
#define I2C_MST4_CLK		((clk_clk_t)107)
#define I2C_MST5_CLK		((clk_clk_t)108)
#define I2C_MST6_CLK		((clk_clk_t)109)
#define I2C_MST7_CLK		((clk_clk_t)110)
#define I2C_SLV0_CLK		((clk_clk_t)111)
#define I2C_SLV1_CLK		((clk_clk_t)112)
#define I2C_SLV2_CLK		((clk_clk_t)113)
#define I2C_SLV3_CLK		((clk_clk_t)114)
#define SPI_MST0_CLK		((clk_clk_t)115)
#define SPI_MST1_CLK		((clk_clk_t)116)
#define SPI_MST2_CLK		((clk_clk_t)117)
#define SPI_MST3_CLK		((clk_clk_t)118)
#define SPI_SLV_CLK		((clk_clk_t)119)
#define SD_HCLK_CLK		((clk_clk_t)121)
#define SD_BCLK_CLK		((clk_clk_t)122)
#define SD_CCLK_TX_CLK		((clk_clk_t)123)
#define SD_CCLK_RX_CLK		((clk_clk_t)124)
#define SD_TM_RX_CLK		((clk_clk_t)125)

/* CLK_EN_CFG4 */
#define TIMER0_CLK		((clk_clk_t)128)
#define TIMER1_CLK		((clk_clk_t)129)
#define TIMER2_CLK		((clk_clk_t)130)
#define TIMER3_CLK		((clk_clk_t)131)
#define TIMER4_CLK		((clk_clk_t)132)
#define TIMER5_CLK		((clk_clk_t)133)
#define TIMER6_CLK		((clk_clk_t)134)
#define TIMER7_CLK		((clk_clk_t)135)
#define TIMER8_CLK		((clk_clk_t)136)
#define TIMER9_CLK		((clk_clk_t)137)
#define TIMER10_CLK		((clk_clk_t)138)
#define TIMER11_CLK		((clk_clk_t)139)
#define TIMER12_CLK		((clk_clk_t)140)
#define TIMER13_CLK		((clk_clk_t)141)
#define TIMER14_CLK		((clk_clk_t)142)
#define TIMER15_CLK		((clk_clk_t)143)
#define TIMER16_CLK		((clk_clk_t)144)
#define TIMER17_CLK		((clk_clk_t)145)
#define TIMER18_CLK		((clk_clk_t)146)
#define TIMER19_CLK		((clk_clk_t)147)
#define TIMER20_CLK		((clk_clk_t)148)
#define TIMER21_CLK		((clk_clk_t)149)
#define TIMER22_CLK		((clk_clk_t)150)
#define TIMER23_CLK		((clk_clk_t)151)
#define TIMER24_CLK		((clk_clk_t)152)
#define TIMER_0_7_APB_CLK	((clk_clk_t)153)
#define TIMER_15_8_APB_CLK	((clk_clk_t)154)
#define TIMER_23_16_APB_CLK	((clk_clk_t)155)
#define TIMER_24_APB_CLK	((clk_clk_t)156)

#define NR_OUTPUT_CLKS		(TIMER_24_APB_CLK + 1)

#define dma_mst_clk		clkid(CLK_OUTPUT, CRCNTL_DMA_MST)
#define dma_ahb_clk		clkid(CLK_OUTPUT, CRCNTL_DMA_AHB)
#define pcie_m_clk		clkid(CLK_OUTPUT, CRCNTL_PCIE_M)
#define pcie_s_clk		clkid(CLK_OUTPUT, CRCNTL_PCIE_S)
#define pcie_phy_apb_clk	clkid(CLK_OUTPUT, CRCNTL_PCIE_PHY_APB)
#define ddrc0_slave_clk		clkid(CLK_OUTPUT, CRCNTL_DDRC0_SLAVE)
#define ddrc0_core_clk		clkid(CLK_OUTPUT, CRCNTL_DDRC0_CORE)
#define ddrc0_sbr_clk		clkid(CLK_OUTPUT, CRCNTL_DDRC0_SBR)
#define ddrp0_dfi_clk		clkid(CLK_OUTPUT, CRCNTL_DDRP0_DFI)
#define ddrp0_bypass_pclk	clkid(CLK_OUTPUT, CRCNTL_DDRP0_BYPASS_P)
#define ddrc0_apb_clk		clkid(CLK_OUTPUT, CRCNTL_DDRC0_APB)
#define ddrp0_apb_clk		clkid(CLK_OUTPUT, CRCNTL_DDRP0_APB)
#define ddrp0_dfictl_clk	clkid(CLK_OUTPUT, CRCNTL_DDRP0_DFICTL)
#define ddrc1_slave_clk		clkid(CLK_OUTPUT, CRCNTL_DDRC1_SLAVE)
#define ddrc1_core_clk		clkid(CLK_OUTPUT, CRCNTL_DDRC1_CORE)
#define ddrc1_sbr_clk		clkid(CLK_OUTPUT, CRCNTL_DDRC1_SBR)
#define ddrp1_dfi_clk		clkid(CLK_OUTPUT, CRCNTL_DDRP1_DFI)
#define ddrp1_bypass_pclk	clkid(CLK_OUTPUT, CRCNTL_DDRP1_BYPASS_P)
#define ddrc1_apb_clk		clkid(CLK_OUTPUT, CRCNTL_DDRC1_APB)
#define ddrp1_apb_clk		clkid(CLK_OUTPUT, CRCNTL_DDRP1_APB)
#define ddrp1_dfictl_clk	clkid(CLK_OUTPUT, CRCNTL_DDRP1_DFICTL)

/* CLK_EN_CFG1 */
#define cohfab_clk		clkid(CLK_OUTPUT, CRCNTL_COHFAB)
#define sysfab_clk		clkid(CLK_OUTPUT, CRCNTL_SYSFAB)
#define cfgfab_clk		clkid(CLK_OUTPUT, CRCNTL_CFGFAB)
#define sysfab_dbg_clk		clkid(CLK_OUTPUT, CRCNTL_SYSFAB_DBG)
#define sysfab_tic_clk		clkid(CLK_OUTPUT, CRCNTL_SYSFAB_TIC)
#define cfgfab_cfg_clk		clkid(CLK_OUTPUT, CRCNTL_CFGFAB_CFG)
#define cohfab_pll_cfg_clk	clkid(CLK_OUTPUT, CRCNTL_COHFAB_PLL_CFG)
#define cluster0_ahb_clk	clkid(CLK_OUTPUT, CRCNTL_CLUSTER0_AHB)
#define cluster1_ahb_clk	clkid(CLK_OUTPUT, CRCNTL_CLUSTER1_AHB)
#define cluster2_ahb_clk	clkid(CLK_OUTPUT, CRCNTL_CLUSTER2_AHB)
#define cluster3_ahb_clk	clkid(CLK_OUTPUT, CRCNTL_CLUSTER3_AHB)

/* CLK_EN_CFG2 */
#define crcntl_apb_clk		clkid(CLK_OUTPUT, CRCNTL_CRCNTL_APB)
#define wdt0_apb_clk		clkid(CLK_OUTPUT, CRCNTL_WDT0_APB)
#define wdt1_apb_clk		clkid(CLK_OUTPUT, CRCNTL_WDT1_APB)
#define uart0_apb_clk		clkid(CLK_OUTPUT, CRCNTL_UART0_APB)
#define uart1_apb_clk		clkid(CLK_OUTPUT, CRCNTL_UART1_APB)
#define uart2_apb_clk		clkid(CLK_OUTPUT, CRCNTL_UART2_APB)
#define uart3_apb_clk		clkid(CLK_OUTPUT, CRCNTL_UART3_APB)
#define i2c_mst0_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_MST0_APB)
#define i2c_mst1_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_MST1_APB)
#define i2c_mst2_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_MST2_APB)
#define i2c_mst3_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_MST3_APB)
#define i2c_mst4_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_MST4_APB)
#define i2c_mst5_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_MST5_APB)
#define i2c_mst6_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_MST6_APB)
#define i2c_mst7_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_MST7_APB)
#define i2c_slv0_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_SLV0_APB)
#define i2c_slv1_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_SLV1_APB)
#define i2c_slv2_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_SLV2_APB)
#define i2c_slv3_apb_clk	clkid(CLK_OUTPUT, CRCNTL_I2C_SLV3_APB)
#define spi_mst0_apb_clk	clkid(CLK_OUTPUT, CRCNTL_SPI_MST0_APB)
#define spi_mst1_apb_clk	clkid(CLK_OUTPUT, CRCNTL_SPI_MST1_APB)
#define spi_mst2_apb_clk	clkid(CLK_OUTPUT, CRCNTL_SPI_MST2_APB)
#define spi_mst3_apb_clk	clkid(CLK_OUTPUT, CRCNTL_SPI_MST3_APB)
#define spi_slv_apb_clk		clkid(CLK_OUTPUT, CRCNTL_SPI_SLV_APB)
#define gpio_apb_clk		clkid(CLK_OUTPUT, CRCNTL_GPIO_APB)
#define scsr_apb_clk		clkid(CLK_OUTPUT, CRCNTL_SCSR_APB)
#define tlmm_apb_clk		clkid(CLK_OUTPUT, CRCNTL_TLMM_APB)
#define plic_clk		clkid(CLK_OUTPUT, CRCNTL_PLIC)
#define brom_clk		clkid(CLK_OUTPUT, CRCNTL_BROM)
/* #define tic_clk			clkid(CLK_OUTPUT, CRCNTL_TIC) */
#define coresight_clk		clkid(CLK_OUTPUT, CRCNTL_CORESIGHT)
#define imc_clk			clkid(CLK_OUTPUT, CRCNTL_IMC)

/* CLK_EN_CFG3 */
#define crcntl_clk		clkid(CLK_OUTPUT, CRCNTL_CRCNTL)
#define wdt0_clk		clkid(CLK_OUTPUT, CRCNTL_WDT0)
#define wdt1_clk		clkid(CLK_OUTPUT, CRCNTL_WDT1)
#define uart0_clk		clkid(CLK_OUTPUT, CRCNTL_UART0)
#define uart1_clk		clkid(CLK_OUTPUT, CRCNTL_UART1)
#define uart2_clk		clkid(CLK_OUTPUT, CRCNTL_UART2)
#define uart3_clk		clkid(CLK_OUTPUT, CRCNTL_UART3)
#define i2c_mst0_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_MST0)
#define i2c_mst1_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_MST1)
#define i2c_mst2_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_MST2)
#define i2c_mst3_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_MST3)
#define i2c_mst4_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_MST4)
#define i2c_mst5_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_MST5)
#define i2c_mst6_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_MST6)
#define i2c_mst7_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_MST7)
#define i2c_slv0_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_SLV0)
#define i2c_slv1_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_SLV1)
#define i2c_slv2_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_SLV2)
#define i2c_slv3_clk		clkid(CLK_OUTPUT, CRCNTL_I2C_SLV3)
#define spi_mst0_clk		clkid(CLK_OUTPUT, CRCNTL_SPI_MST0)
#define spi_mst1_clk		clkid(CLK_OUTPUT, CRCNTL_SPI_MST1)
#define spi_mst2_clk		clkid(CLK_OUTPUT, CRCNTL_SPI_MST2)
#define spi_mst3_clk		clkid(CLK_OUTPUT, CRCNTL_SPI_MST3)
#define spi_slv_clk		clkid(CLK_OUTPUT, CRCNTL_SPI_SLV)
#define sd_hclk_clk		clkid(CLK_OUTPUT, CRCNTL_SD_HCLK)
#define sd_bclk_clk		clkid(CLK_OUTPUT, CRCNTL_SD_BCLK)
#define sd_cclk_tx_clk		clkid(CLK_OUTPUT, CRCNTL_SD_CCLK_TX)
#define sd_cclk_rx_clk		clkid(CLK_OUTPUT, CRCNTL_SD_CCLK_RX)
#define sd_tm_rx_clk		clkid(CLK_OUTPUT, CRCNTL_SD_TM_RX)

/* CLK_EN_CFG4 */
#define timer0_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER0)
#define timer1_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER1)
#define timer2_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER2)
#define timer3_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER3)
#define timer4_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER4)
#define timer5_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER5)
#define timer6_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER6)
#define timer7_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER7)
#define timer8_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER8)
#define timer9_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER9)
#define timer10_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER10)
#define timer11_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER11)
#define timer12_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER12)
#define timer13_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER13)
#define timer14_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER14)
#define timer15_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER15)
#define timer16_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER16)
#define timer17_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER17)
#define timer18_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER18)
#define timer19_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER19)
#define timer20_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER20)
#define timer21_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER21)
#define timer22_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER22)
#define timer23_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER23)
#define timer24_clk		clkid(CLK_OUTPUT, CRCNTL_TIMER24)
#define timer_7_0_apb_clk	clkid(CLK_OUTPUT, CRCNTL_TIMER_7_0_APB)
#define timer_15_8_apb_clk	clkid(CLK_OUTPUT, CRCNTL_TIMER_15_8_APB)
#define timer_23_16_apb_clk	clkid(CLK_OUTPUT, CRCNTL_TIMER_23_16_APB)
#define timer_24_apb_clk	clkid(CLK_OUTPUT, CRCNTL_TIMER_24_APB)
#define apb_clk			sysfab_clk

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

/* APIs here must be invoked w clock tree core enabled */
void clk_hw_select_boot(clk_t clkid); /* select boot source */
void clk_hw_select_run(clk_t clkid); /* select runtime source */
/* Enable clock tree core */
void clk_hw_ctrl_init(void);

#endif /* __CLK_DUOWEN_H_INCLUDE__ */
