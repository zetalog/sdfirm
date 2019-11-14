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
 * @(#)clk.h: DUOWEN clock/reset controller definitions
 * $Id: clk.h,v 1.1 2019-11-12 12:25:00 zhenglv Exp $
 */

#ifndef __CRCNTL_DUOWEN_H_INCLUDE__
#define __CLCNTL_DUOWEN_H_INCLUDE__

#include <target/arch.h>
#include <target/generic.h>

#define DW_PLL5GHZ_REFCLK_FREQ		XO_CLK_FREQ
#define DW_PLL5GHZ_CFG0(pll)		CRCNTL_PLL_CFG0(pll)
#define DW_PLL5GHZ_CFG1(pll)		CRCNTL_PLL_CFG1(pll)
#define DW_PLL5GHZ_CFG2(pll)		CRCNTL_PLL_CFG2(pll)
#define DW_PLL5GHZ_STATUS(pll)		CRCNTL_PLL_STATUS(pll)

#include <driver/dw_pll5ghz_tsmc12ffc.h>

/* XXX: This implementation is based on undocumented PLL RTL
 *      sequence, and may subject to change.
 * NOTE: all CRCNTL registers are only 32-bit accessiable.
 */

#define CRCNTL_REG(offset)		(CRCNTL_BASE + (offset))

/* power control */
#define CRCNTL_WARM_RESET_DETECT_TIME	CRCNTL_REG(0x00)
#define CRCNTL_PWR_FSM_DELAY_TIME	CRCNTL_REG(0x04)
#define CRCNTL_PWR_SHUTDOWN		CRCNTL_REG(0x08)
#define CRCNTL_PS_HOLD			CRCNTL_REG(0x0C)

/* reset control */
#define CRCNTL_RST_CAUSE		CRCNTL_REG(0x50)
#define CRCNTL_SW_RST_CFG(n)		CRCNTL_REG(0x60 + ((n) << 2))

/* clock control */
#define CRCNTL_CLK_EN_CFG(n)		CRCNTL_REG(0x80 + ((n) << 2))
#define CRCNTL_CLK_SEL_CFG		CRCNTL_REG(0x94)
#define CRCNTL_SW_GLOBAL_RST		CRCNTL_REG(0xA0)

/* PLL control */
#define CRCNTL_PLL_REG(pll, offset)	\
	CRCNTL_REG(((pll) << 5) + 0x10 + (offset))
#define CRCNTL_PLL_CFG0(pll)		CRCNTL_PLL_REG(pll, 0x00)
#define CRCNTL_PLL_CFG1(pll)		CRCNTL_PLL_REG(pll, 0x04)
#define CRCNTL_PLL_CFG2(pll)		CRCNTL_PLL_REG(pll, 0x08)
#define CRCNTL_PLL_STATUS(pll)		CRCNTL_PLL_REG(pll, 0x0C)
#define CRCNTL_PLL_REG_ACCESS(pll)	CRCNTL_PLL_REG(pll, 0x10)
#define CTCNTL_PLL_REG_TIMING(pll)	CRCNTL_PLL_REG(pll, 0x14)

/* power control */
/* CRCNTL_WARM_RESET_DETECT_TIME */
#define PWR_WARM_RESET_DETECT_OFFSET	0
#define PWR_WARM_RESET_DETECT_MASK	REG_8BIT_MASK
#define PWR_WARM_RESET_DETECT(value)	_SET_FV(PWR_WARM_RESET_DETECT, value)
/* CRCNTL_FSM_DELAY_TIME */
#define PWR_WARM_RESET_DELAY_OFFSET	0
#define PWR_WARM_RESET_DELAY_MASK	REG_16BIT_MASK
#define PWR_WARM_RESET_DELAY(value)	_SET_FV(PWR_WARM_RESET_DELAY, value)
#define PWR_SHUTDOWN_DELAY_OFFSET	16
#define PWR_SHUTDOWN_DELAY_MASK		REG_16BIT_MASK
#define PWR_SHUTDOWN_DELAY(value)	_SET_FV(PWR_SHUTDOWN_DELAY, value)

/* CRCNTL_SHUTDOWN */
#define PWR_SHUT_DN		_BV(0)
/* CRCNTL_PS_HOLD */
#define PWR_PS_HOLD		_BV(0)
/* CRCNTL_SW_GLOBAL_RST */
#define PWR_GLOBAL_RST		_BV(0)

/* PLL_CFG0 */
#define PLL_MFRAC_OFFSET	0
#define PLL_MFRAC_MASK		REG_16BIT_MASK
#define PLL_MFRAC(value)	_SET_FV(PLL_MFRAC, value)
#define PLL_MINT_OFFSET		16
#define PLL_MINT_MASK		REG_10BIT_MASK
#define PLL_MINT(value)		_SET_FV(PLL_MINT, value)
#define PLL_PREDIV_OFFSET	26
#define PLL_PREDIV_MASK		REG_5BIT_MASK
#define PLL_PREDIV(value)	_SET_FV(PLL_PREDIV, value)

/* PLL_CFG1 */
#define PLL_DIVVCOR_OFFSET	0
#define PLL_DIVVCOR_MASK	REG_4BIT_MASK
#define PLL_DIVVCOR(value)	_SET_FV(PLL_DIVVCOR, value)
#define PLL_DIVVCOP_OFFSET	4
#define PLL_DIVVCOP_MASK	REG_4BIT_MASK
#define PLL_DIVVCOP(value)	_SET_FV(PLL_DIVVCOP, value)
#define PLL_R_OFFSET		8
#define PLL_R_MASK		REG_6BIT_MASK
#define PLL_R(value)		_SET_FV(PLL_R, value)
#define PLL_P_OFFSET		14
#define PLL_P_MASK		REG_6BIT_MASK
#define PLL_P(value)		_SET_FV(PLL_P, value)
#define PLL_VCO_MODE		_BV(20)
#define PLL_STANDBY		_BV(21)
#define PLL_BYPASS		_BV(22)
#define PLL_ENR			_BV(23)
#define PLL_ENP			_BV(24)
#define PLL_LOWFREQ		_BV(25)
#define PLL_GEAR_SHIFT		_BV(26)
#define PLL_RESET		_BV(27)
#define PLL_PWRON		_BV(28)
#define PLL_TEST_RESET		_BV(29)
#if 0 /* obtained from RTL */
#define PLL_TEST_BYPASS		_BV(30)
#endif

/* PLL_CFG2 */
#define PLL_LOCK_TIME_OFFSET	0
#define PLL_LOCK_TIME_MASK	REG_16BIT_MASK
#define PLL_LOCK_TIME(value)	_SET_FV(PLL_LOCK_TIME, value)

/* PLL_STATUS */
#define PLL_LOCKED		_BV(0)
#define PLL_STANDBYEFF		_BV(1)
#define PLL_CNT_LOCKED		_BV(2)

/* CLK_SEL_CFG */
#define DDR_DFI_CLK_SEL		_BV(29)
#define SPI_SLV_CLK_SEL		_BV(28)
#define SPI_MST3_CLK_SEL	_BV(27)
#define SPI_MST2_CLK_SEL	_BV(26)
#define SPI_MST1_CLK_SEL	_BV(25)
#define SPI_MST0_CLK_SEL	_BV(24)
#define I2C_SLV3_CLK_SEL	_BV(23)
#define I2C_SLV2_CLK_SEL	_BV(22)
#define I2C_SLV1_CLK_SEL	_BV(21)
#define I2C_SLV0_CLK_SEL	_BV(20)
#define I2C_MST7_CLK_SEL	_BV(19)
#define I2C_MST6_CLK_SEL	_BV(18)
#define I2C_MST5_CLK_SEL	_BV(17)
#define I2C_MST4_CLK_SEL	_BV(16)
#define I2C_MST3_CLK_SEL	_BV(15)
#define I2C_MST2_CLK_SEL	_BV(14)
#define I2C_MST1_CLK_SEL	_BV(13)
#define I2C_MST0_CLK_SEL	_BV(12)
#define UART3_CLK_SEL		_BV(11)
#define UART2_CLK_SEL		_BV(10)
#define UART1_CLK_SEL		_BV(9)
#define UART0_CLK_SEL		_BV(8)
#define SD_CCLK_RX_CLK_SEL	_BV(6)
#define SD_CCLK_TX_CLK_SEL	_BV(5)
#define SD_BCLK_CLK_SEL		_BV(4)
#define IMC_CLK_SEL		_BV(2)
#define CFGFAB_CLK_SEL		_BV(1)
#define SYSFAB_CLK_SEL		_BV(0)

/* SW_RST_CFG0/CLK_EN_CFG0 */
#define CRCNTL_DMA_MST		0
#define CRCNTL_DMA_AHB		1
#define CRCNTL_PCIE_M		6
#define CRCNTL_PCIE_S		7
#define CRCNTL_PCIE_PHY_APB	8
#define CRCNTL_PCIE_BUTTON	9   /* SW_RST only */
#define CRCNTL_PCIE_POWER_UP	10  /* SW_RST only */
#define CRCNTL_PCIE_TEST	11  /* SW_RST only */
#define CRCNTL_DDRC0_SLAVE	14
#define CRCNTL_DDRC0_CORE	15
#define CRCNTL_DDRC0_SBR	16
#define CRCNTL_DDRP0_DFI	17
#define CRCNTL_DDRP0		18  /* SW_RST only */
#define CRCNTL_DDRP0_BYPASS_P	18  /* CLK_EN only */
#define CRCNTL_DDRC0_APB	19
#define CRCNTL_DDRP0_APB	20
#define CRCNTL_DDRP0_PWROKIN	21  /* SW_RST only */
#define CRCNTL_DDRP0_DFICTL	21  /* CLK_EN only */
#define CRCNTL_DDRP0_SCAN	22

#define CRCNTL_DDRC1_SLAVE	23
#define CRCNTL_DDRC1_CORE	24
#define CRCNTL_DDRC1_SBR	25
#define CRCNTL_DDRP1_DFI	26
#define CRCNTL_DDRP1		27  /* SW_RST only */
#define CRCNTL_DDRP1_BYPASS_P	27  /* CLK_EN only */
#define CRCNTL_DDRC1_APB	28
#define CRCNTL_DDRP1_APB	29
#define CRCNTL_DDRP1_PWROKIN	30  /* SW_RST only */
#define CRCNTL_DDRP1_DFICTL	30  /* CLK_EN only */
#define CRCNTL_DDRP1_SCAN	31

/* SW_RST_CFG1/CLK_EN_CFG1 */
#define CRCNTL_COHFAB		32
#define CRCNTL_SYSFAB		33
#define CRCNTL_CFGFAB		34
#define CRCNTL_SYSFAB_DBG	36
#define CRCNTL_SYSFAB_TIC	37
#define CRCNTL_COHFAB_CFG	38
#define CRCNTL_COHFAB_PLL_CFG	39
#define CRCNTL_CLUSTER0_AHB	40
#define CRCNTL_CLUSTER1_AHB	41
#define CRCNTL_CLUSTER2_AHB	42
#define CRCNTL_CLUSTER3_AHB	43

/* SW_RST_CFG2/CLK_EN_CFG2 */
#define CRCNTL_CRCNTL_APB	64
#define CRCNTL_WDT0_APB		65
#define CRCNTL_WDT1_APB		66
#define CRCNTL_UART0_APB	67
#define CRCNTL_UART1_APB	68
#define CRCNTL_UART2_APB	69
#define CRCNTL_UART3_APB	70
#define CRCNTL_I2C_MST0_APB	71
#define CRCNTL_I2C_MST1_APB	72
#define CRCNTL_I2C_MST2_APB	73
#define CRCNTL_I2C_MST3_APB	74
#define CRCNTL_I2C_MST4_APB	75
#define CRCNTL_I2C_MST5_APB	76
#define CRCNTL_I2C_MST6_APB	77
#define CRCNTL_I2C_MST7_APB	78
#define CRCNTL_I2C_SLV0_APB	79
#define CRCNTL_I2C_SLV1_APB	80
#define CRCNTL_I2C_SLV2_APB	81
#define CRCNTL_I2C_SLV3_APB	82
#define CRCNTL_SPI_MST0_APB	83
#define CRCNTL_SPI_MST1_APB	84
#define CRCNTL_SPI_MST2_APB	85
#define CRCNTL_SPI_MST3_APB	86
#define CRCNTL_SPI_SLV_APB	87
#define CRCNTL_GPIO_APB		88
#define CRCNTL_SCSR_APB		89
#define CRCNTL_TLMM_APB		90
#define CRCNTL_PLIC		91
#define CRCNTL_BROM		92
#define CRCNTL_TIC		93
#define CRCNTL_CORESIGHT	94
#define CRCNTL_IMC		95

/* SW_RST_CFG3/CLK_EN_CFG3 */
#define CRCNTL_CRCNTL		96
#define CRCNTL_WDT0		97
#define CRCNTL_WDT1		98
#define CRCNTL_UART0		99
#define CRCNTL_UART1		100
#define CRCNTL_UART2		101
#define CRCNTL_UART3		102
#define CRCNTL_I2C_MST0		103
#define CRCNTL_I2C_MST1		104
#define CRCNTL_I2C_MST2		105
#define CRCNTL_I2C_MST3		106
#define CRCNTL_I2C_MST4		107
#define CRCNTL_I2C_MST5		108
#define CRCNTL_I2C_MST6		109
#define CRCNTL_I2C_MST7		110
#define CRCNTL_I2C_SLV0		111
#define CRCNTL_I2C_SLV1		112
#define CRCNTL_I2C_SLV2		113
#define CRCNTL_I2C_SLV3		114
#define CRCNTL_SPI_MST0		115
#define CRCNTL_SPI_MST1		116
#define CRCNTL_SPI_MST2		117
#define CRCNTL_SPI_MST3		118
#define CRCNTL_SPI_SLV		119
#define CRCNTL_SD_HCLK		121
#define CRCNTL_SD_BCLK		122
#define CRCNTL_SD_CCLK_TX	123
#define CRCNTL_SD_CCLK_RX	124
#define CRCNTL_SD_TM_CLK	125

/* SW_RST_CFG4/CLK_EN_CFG4 */
#define CRCNTL_TIMER0		128
#define CRCNTL_TIMER1		129
#define CRCNTL_TIMER2		130
#define CRCNTL_TIMER3		131
#define CRCNTL_TIMER4		132
#define CRCNTL_TIMER5		133
#define CRCNTL_TIMER6		134
#define CRCNTL_TIMER7		135
#define CRCNTL_TIMER8		136
#define CRCNTL_TIMER9		137
#define CRCNTL_TIMER10		138
#define CRCNTL_TIMER11		139
#define CRCNTL_TIMER12		140
#define CRCNTL_TIMER13		141
#define CRCNTL_TIMER14		142
#define CRCNTL_TIMER15		143
#define CRCNTL_TIMER16		144
#define CRCNTL_TIMER17		145
#define CRCNTL_TIMER18		146
#define CRCNTL_TIMER19		147
#define CRCNTL_TIMER20		148
#define CRCNTL_TIMER21		149
#define CRCNTL_TIMER22		150
#define CRCNTL_TIMER23		151
#define CRCNTL_TIMER24		152
#define CRCNTL_TIMER_7_0_APB	153
#define CRCNTL_TIMER_15_8_APB	154
#define CRCNTL_TIMER_23_16_APB	155
#define CRCNTL_TIMER_24_APB	156

/* TODO: Wait imc_rst_n, wait PS_HOLD? */
#define crcntl_power_up()						\
	do {								\
		__raw_setl(PWR_PS_HOLD, CRCNTL_PS_HOLD);		\
		while (!(__raw_readl(CRCNTL_PS_HOLD) & PWR_PS_HOLD));	\
	} while (0)
#define crcntl_power_down()						\
	__raw_writel(PWR_SHUT_DN, CRCNTL_SHUTDOWN)
#define crcntl_global_reset()						\
	__raw_writel(PWR_GLOBAL_RST, CRCNTL_SW_GLOBAL_RST)
#define crcntl_config_timing(wrst_detect, wrst_delay, shutdown_delay)	\
	do {								\
		__raw_writel_mask(PWR_WARM_RESET_DETECT(wrst_detect),	\
				  CRCNTL_WARM_RESET_DETECT_TIME);	\
		__raw_writel(PWR_WARM_RESET_DELAY(wrst_delay) |		\
			     PWR_SHUTDOWN_DELAY(shutdown_delay),	\
			     CRCNTL_FSM_DELAY_TIME);			\
	} while (0)

#define pll_select()		__raw_writel(0x0, CRCNTL_PLLSEL)

#define enable_all_clocks()					\
	do {							\
		__raw_writel(0xFFFFFFFF, CRCNTL_CLKGRP0);	\
		__raw_writel(0xFFFFFFFF, CRCNTL_CLKGRP1);	\
		__raw_writel(0xFFFFFFFF, CRCNTL_CLKGRP2);	\
		__raw_writel(0xFFFFFFFF, CRCNTL_CLKGRP3);	\
		__raw_writel(0xFFFFFFFF, CRCNTL_CLKGRP4);	\
	} while (0)

#define crcntl_pll_enable(pll, freq)		\
	dwc_pll5ghz_tmffc12_enable(pll, (uint64_t)freq)
#define crcntl_pll_disable(pll)			\
	dwc_pll5ghz_tmffc12_disable(pll)

#endif /* __CRCNTL_DUOWEN_H_INCLUDE__ */
