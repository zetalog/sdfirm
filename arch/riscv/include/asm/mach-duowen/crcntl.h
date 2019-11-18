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

#define DW_PLL_REFCLK_FREQ		XO_CLK_FREQ
#define DW_PLL_CFG0(pll)		CRCNTL_PLL_CFG0(pll)
#define DW_PLL_CFG1(pll)		CRCNTL_PLL_CFG1(pll)
#define DW_PLL_CFG2(pll)		CRCNTL_PLL_CFG2(pll)
#define DW_PLL_STATUS(pll)		CRCNTL_PLL_STATUS(pll)
#define dw_pll_read(pll, reg)		crcntl_pll_reg_read(pll, reg)
#define dw_pll_write(pll, reg, val)	crcntl_pll_reg_write(pll, reg, val)

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

/* CRCNTL_PLL_REG_ACCESS */
#define PLL_REG_IDLE		_BV(24)
#define PLL_REG_RDATA_OFFSET	16
#define PLL_REG_RDATA_MASK	REG_8BIT_MASK
#define PLL_REG_RDATA(value)	_GET_FV(PLL_REG_RDATA, value)
#define PLL_REG_WDATA_OFFSET	8
#define PLL_REG_WDATA_MASK	REG_8BIT_MASK
#define PLL_REG_WDATA(value)	_SET_FV(PLL_REG_WDATA, value)
#define PLL_REG_SEL_OFFSET	2
#define PLL_REG_SEL_MASK	REG_6BIT_MASK
#define PLL_REG_SEL(value)	_SET_FV(PLL_REG_SEL, value)
#define PLL_REG_WRITE		_BV(1)
#define PLL_REG_READ		_BV(0)

/* TODO: Wait imc_rst_n, drive PS_HOLD, and wait for PMIC? */
#define crcntl_power_up()						\
	__raw_setl(PWR_PS_HOLD, CRCNTL_PS_HOLD)
#define crcntl_power_down()						\
	__raw_setl(PWR_SHUT_DN, CRCNTL_SHUTDOWN)
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

#define crcntl_pll_enable(pll, freq)		\
	dw_pll5ghz_tsmc12ffc_pwron(pll, (uint64_t)freq)
#define crcntl_pll_disable(pll)			\
	dw_pll5ghz_tsmc12ffc_pwrdn(pll)

void crcntl_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val);
uint8_t crcntl_pll_reg_read(uint8_t pll, uint8_t reg);

/* APIs here can be invoked w/o enabling clock tree core */
bool crcntl_clk_selected(clk_clk_t clk);
bool crcntl_clk_enabled(clk_clk_t clk);
void crcntl_clk_enable(clk_clk_t clk);
void crcntl_clk_disable(clk_clk_t clk);
bool crcntl_clk_asserted(clk_clk_t clk);
void crcntl_clk_assert(clk_clk_t clk);
void crcntl_clk_deassert(clk_clk_t clk);

#endif /* __CRCNTL_DUOWEN_H_INCLUDE__ */
