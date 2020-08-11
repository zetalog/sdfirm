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
 * @(#)crcntl.h: DUOWEN clock/reset controller definitions
 * $Id: crcntl.h,v 1.2 2020-01-20 17:50:00 zhenglv Exp $
 */

#ifndef __CRCNTL_DUOWEN_H_INCLUDE__
#define __CLCNTL_DUOWEN_H_INCLUDE__

#define DW_PLL_REFCLK_FREQ		XO_CLK_FREQ
#define DW_PLL_CFG0(pll)		CRCNTL_PLL_CFG0(pll)
#define DW_PLL_CFG1(pll)		CRCNTL_PLL_CFG1(pll)
#define DW_PLL_CFG2(pll)		CRCNTL_PLL_CFG2(pll)
#define DW_PLL_STATUS(pll)		CRCNTL_PLL_STATUS(pll)
#define __dw_pll_read(pll, reg)		crcntl_pll_reg_read(pll, reg)
#define __dw_pll_write(pll, reg, val)	crcntl_pll_reg_write(pll, reg, val)

#include <driver/dw_pll5ghz_tsmc12ffc.h>

#define __DUOWEN_CLK_BASE		CRCNTL_BASE
#ifdef CONFIG_MMU
#define DUOWEN_CLK_BASE			duowen_clk_reg_base
extern caddr_t duowen_clk_reg_base;
#else
#define DUOWEN_CLK_BASE			__DUOWEN_CLK_BASE
#endif

/* XXX: This implementation is based on undocumented PLL RTL
 *      sequence, and may subject to change.
 * NOTE: all CRCNTL registers are only 32-bit accessiable.
 */

#define CRCNTL_REG(offset)		(DUOWEN_CLK_BASE + (offset))

/* PLL control */
#define CRCNTL_PLL_REG(pll, offset)	CRCNTL_REG(((pll) << 6) + (offset))
#define CRCNTL_PLL_CFG0(pll)		CRCNTL_PLL_REG(pll, 0x00)
#define CRCNTL_PLL_CFG1(pll)		CRCNTL_PLL_REG(pll, 0x04)
#define CRCNTL_PLL_CFG2(pll)		CRCNTL_PLL_REG(pll, 0x08)
#define CRCNTL_PLL_STATUS(pll)		CRCNTL_PLL_REG(pll, 0x0C)
#define CRCNTL_PLL_REG_ACCESS(pll)	CRCNTL_PLL_REG(pll, 0x10)
#define CTCNTL_PLL_REG_TIMING(pll)	CRCNTL_PLL_REG(pll, 0x14)

/* reset control */
#define CRCNTL_RST_CAUSE		CRCNTL_REG(0x120)
#define CRCNTL_SW_RST_CFG(n)		CRCNTL_REG(0x130 + ((n) << 2))

/* clock control */
#define CRCNTL_CLK_EN_CFG(n)		CRCNTL_REG(0x140 + ((n) << 2))
#define CRCNTL_CLK_SEL_CFG		CRCNTL_REG(0x150)

/* power control */
#define CRCNTL_WARM_RESET_DETECT_TIME	CRCNTL_REG(0x100)
#define CRCNTL_PWR_FSM_DELAY_TIME	CRCNTL_REG(0x104)
#define CRCNTL_PWR_SHUTDOWN		CRCNTL_REG(0x108)
#define CRCNTL_PS_HOLD			CRCNTL_REG(0x10C)
#define CRCNTL_SW_GLOBAL_RST		CRCNTL_REG(0x110)

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

/* CRCNTL_RST_CAUSE */
#define RST_SW_GLBAL		_BV(0)
#define RST_WDT0		_BV(1)
#define RST_WDT1		_BV(2)
#define RST_WARM		_BV(3)

/* CRCNTL_PLL_REG_ACCESS */
#define PLL_REG_INVALID		_BV(25)
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
#define crcntl_div_enable(pll, fvco, freq, r)	\
	dw_pll5ghz_tsmc12ffc_enable(pll, fvco, freq, r)
#define crcntl_div_disable(pll, r)		\
	dw_pll5ghz_tsmc12ffc_disable(pll, r)

void crcntl_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val);
uint8_t crcntl_pll_reg_read(uint8_t pll, uint8_t reg);

/* APIs here can be invoked w/o enabling clock tree core */
bool crcntl_clk_asserted(clk_clk_t clk);
void crcntl_clk_assert(clk_clk_t clk);
void crcntl_clk_deassert(clk_clk_t clk);
bool crcntl_clk_enabled(clk_clk_t clk);
void crcntl_clk_enable(clk_clk_t clk);
void crcntl_clk_disable(clk_clk_t clk);
bool crcntl_clk_selected(clk_clk_t clk);
void crcntl_clk_select(clk_clk_t clk);
void crcntl_clk_deselect(clk_clk_t clk);
#ifdef CONFIG_CRCNTL_TRACE
void crcntl_trace_enable(void);
void crcntl_trace_disable(void);
void crcntl_trace(bool enabling, const char *name);
#else
#define crcntl_trace_enable()		do { } while (0)
#define crcntl_trace_disable()		do { } while (0)
#define crcntl_trace(enabling, name)	do { } while (0)
#endif
void crcntl_init(void);

#endif /* __CRCNTL_DUOWEN_H_INCLUDE__ */
