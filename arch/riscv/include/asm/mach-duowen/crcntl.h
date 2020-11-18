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
#define __CRCNTL_DUOWEN_H_INCLUDE__

#define DW_PLL_REFCLK_FREQ		XO_CLK_FREQ
#define __dw_pll_read(pll, reg)		duowen_pll_reg_read(pll, reg)
#define __dw_pll_write(pll, reg, val)	duowen_pll_reg_write(pll, reg, val)

/* PLL IDs is kept AS IS to make base addresses simpler, see
 * COHFAB_CLK_PLL() and COHFAB_CLK_SEL_PLL() for details.
 */
#define SOC_PLL			((clk_clk_t)0)
#define DDR_BUS_PLL		((clk_clk_t)1)
#define DDR_PLL			((clk_clk_t)2)
#define PCIE_PLL		((clk_clk_t)3)
#define COHFAB_PLL		((clk_clk_t)4)
#define CL0_PLL			((clk_clk_t)5)
#define CL1_PLL			((clk_clk_t)6)
#define CL2_PLL			((clk_clk_t)7)
#define CL3_PLL			((clk_clk_t)8)
#define ETH_PLL			((clk_clk_t)9)
#define DUOWEN_MAX_PLLS			10
extern phys_addr_t duowen_pll_reg_base[];
#define DW_PLL_BASE(pll)		duowen_pll_reg_base[pll]
#define DW_PLL_REG(pll, offset)		(DW_PLL_BASE(pll) + (offset))
#define DW_PLL_CFG0(pll)		DW_PLL_REG(pll, 0x00)
#define DW_PLL_CFG1(pll)		DW_PLL_REG(pll, 0x04)
#define DW_PLL_CFG2(pll)		DW_PLL_REG(pll, 0x08)
#define DW_PLL_STATUS(pll)		DW_PLL_REG(pll, 0x0C)
#define DW_PLL_REG_ACCESS(pll)		DW_PLL_REG(pll, 0x10)
#define DW_PLL_REG_TIMING(pll)		DW_PLL_REG(pll, 0x14)

#include <driver/dw_pll5ghz_tsmc12ffc.h>

#define __DUOWEN_CLK_BASE		CRCNTL_BASE
#define __DUOWEN_CFAB_CLK_BASE		COHFAB_PLL_BASE
#define __DUOWEN_APC_CLK_BASE(n)	(CLUSTER0_BASE + ((n) << 20))
#define __DUOWEN_ETH_CLK_BASE		ETH_PLL_BASE
#ifdef CONFIG_MMU
#define DUOWEN_CLK_BASE			duowen_clk_reg_base
extern caddr_t duowen_clk_reg_base;
#define DUOWEN_CFAB_CLK_BASE		duowen_cfab_clk_reg_base
extern caddr_t duowen_cfab_clk_reg_base;
#define DUOWEN_APC_CLK_BASE(n)		duowen_apc_clk_reg_base[n]
extern caddr_t duowen_apc_clk_reg_base[];
#define DUOWEN_ETH_CLK_BASE		duowen_eth_clk_reg_base
extern caddr_t duowen_eth_clk_reg_base;
#else
#define DUOWEN_CLK_BASE			__DUOWEN_CLK_BASE
#define DUOWEN_CFAB_CLK_BASE		__DUOWEN_CFAB_CLK_BASE
#define DUOWEN_APC_CLK_BASE(n)		__DUOWEN_APC_CLK_BASE(n)
#define DUOWEN_ETH_CLK_BASE		__DUOWEN_ETH_CLK_BASE
#endif

/* XXX: This implementation is based on undocumented PLL RTL
 *      sequence, and may subject to change.
 * NOTE: all CRCNTL registers are only 32-bit accessiable.
 */

#define __CRCNTL_REG(offset)		(__DUOWEN_CLK_BASE + (offset))
#define CRCNTL_REG(offset)		(DUOWEN_CLK_BASE + (offset))
#define COHFAB_PLL_REG(offset)		(DUOWEN_CFAB_CLK_BASE + (offset))
#define CLUSTER_PLL_REG(n, offset)	(DUOWEN_APC_CLK_BASE(n) + (offset))
#define ETH_PLL_REG(offset)		(DUOWEN_ETH_CLK_BASE + (offset))

/* PLL control */
#define __CRCNTL_PLL_REG(pll, offset)	__CRCNTL_REG(((pll) << 6) + (offset))
#define CRCNTL_PLL_REG(pll, offset)	CRCNTL_REG(((pll) << 6) + (offset))

/* COHFAB/CLUSTER PLL clock control */
#define COHFAB_CLK_CFG(pll)		DW_PLL_REG((pll), 0x40)
#define COHFAB_RESET_COHFAB(pll)	DW_PLL_REG((pll), 0x44)
#define COHFAB_RESET_CLUSTER(pll)	DW_PLL_REG((pll), 0x50)

/* CLUSTER PLL clock control */
#define CLUSTER_CLK_CG_CFG(apc)		CLUSTER_PLL_REG(apc, 0x44)
#define CLUSTER_RESET_CTRL(apc)		CLUSTER_PLL_REG(apc, 0x50)

/* reset control */
#define CRCNTL_RST_CAUSE		CRCNTL_REG(0x120)
#define CRCNTL_SW_RST_CFG(n)		CRCNTL_REG(0x130 + ((n) << 2))

/* clock control */
#define CRCNTL_CLK_EN_CFG(n)		CRCNTL_REG(0x140 + ((n) << 2))
#define CRCNTL_CLK_SEL_CFG		CRCNTL_REG(0x150)

/* COHFAB/CLUSTER clock control */
#define COHFAB_CLOCK_SEL		_BV(0)
#define COHFAB_CLOCK_ON			_BV(1)

/* COHFAB/CLUSTER reset control */
#define COHFAB_RESET			_BV(0)

/* CLUSTER clock control */
#define CLUSTER_SWALLOW_BYPASS		_BV(2)

/* CLUSTER clock gate */
#define CLUSTER_APC0_CPU0_CG		_BV(0)
#define CLUSTER_APC0_CPU1_CG		_BV(1)
#define CLUSTER_APC1_CPU0_CG		_BV(2)
#define CLUSTER_APC1_CPU1_CG		_BV(3)
#define CLUSTER_APC0_L2_CG		_BV(4)
#define CLUSTER_APC1_L2_CG		_BV(5)

/* CLUSTER reset control */
#define CLUSTER_POR_RST			_BV(0)
#define CLUSTER_APC0_CPU0_DBG_RESET	_BV(12)
#define CLUSTER_APC0_CPU1_DBG_RESET	_BV(13)
#define CLUSTER_APC1_CPU0_DBG_RESET	_BV(14)
#define CLUSTER_APC1_CPU1_DBG_RESET	_BV(15)
#define CLUSTER_APC0_L2_DBG_RESET	_BV(16)
#define CLUSTER_APC1_L2_DBG_RESET	_BV(17)

/* COHFAB/CLUSTER clock ID conversions */
#define CLUSTER_CLOCKS			6
#define CLUSTER_RESET_OFFSET		4 /* convert cg to rst */
#define CLUSTER_RESET(cg)		((cg) + CLUSTER_RESET_OFFSET)
#define CLUSTER_DBG_RESET_OFFSET	12 /* convert cg to dbg rst */
#define CLUSTER_DBG_RESET(cg)		((cg) + CLUSTER_DBG_RESET_OFFSET)
/* Macros are correct given PLL indexes are fixed */
#define COHFAB_CLK_PLL(clk)		((clk) - COHFAB_CLK + COHFAB_PLL)
#define COHFAB_CLK_SEL_PLL(clk)		((clk) - COHFAB_CLK_SEL + COHFAB_PLL)
#define CLUSTER_CLK_APC(clk)		\
	(((clk) - CLUSTER0_APC0_CPU0_CLK) / CLUSTER_CLOCKS)
#define CLUSTER_CLK_CG(clk)		\
	(((clk) - CLUSTER0_APC0_CPU0_CLK) % CLUSTER_CLOCKS)

/* DW_PLL_REG_ACCESS */
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

/* APIs here can be invoked w/o enabling clock tree core */
bool crcntl_clk_asserted(clk_clk_t clk);
void crcntl_clk_assert(clk_clk_t clk);
void crcntl_clk_deassert(clk_clk_t clk);
bool crcntl_clk_enabled(clk_clk_t clk);
void crcntl_clk_enable(clk_clk_t clk);
void crcntl_clk_disable(clk_clk_t clk);
bool cohfab_clk_asserted(clk_clk_t clk);
void cohfab_clk_assert(clk_clk_t clk);
void cohfab_clk_deassert(clk_clk_t clk);
bool cohfab_clk_enabled(clk_clk_t clk);
void cohfab_clk_enable(clk_clk_t clk);
void cohfab_clk_disable(clk_clk_t clk);
bool cluster_clk_asserted(clk_clk_t clk);
void cluster_clk_assert(clk_clk_t clk);
void cluster_clk_deassert(clk_clk_t clk);
bool cluster_clk_enabled(clk_clk_t clk);
void cluster_clk_enable(clk_clk_t clk);
void cluster_clk_disable(clk_clk_t clk);
#ifdef CONFIG_CRCNTL_MUX
bool crcntl_clk_selected(clk_clk_t clk);
void crcntl_clk_select(clk_clk_t clk);
void crcntl_clk_deselect(clk_clk_t clk);
bool cohfab_clk_selected(clk_clk_t clk);
void cohfab_clk_select(clk_clk_t clk);
void cohfab_clk_deselect(clk_clk_t clk);
#endif
#ifdef CONFIG_CRCNTL_TRACE
void crcntl_trace_enable(void);
void crcntl_trace_disable(void);
void crcntl_trace(bool enabling, const char *name);
#else
#define crcntl_trace_enable()		do { } while (0)
#define crcntl_trace_disable()		do { } while (0)
#define crcntl_trace(enabling, name)	do { } while (0)
#endif

/* DUOWEN unitified PLL (crcntl, cluster, cohfab) API */
#define duowen_pll_enable(pll, freq)			\
	dw_pll5ghz_tsmc12ffc_pwron(pll, (uint64_t)freq)
#define duowen_pll_enable2(pll, fvco, fpclk, frclk)	\
	dw_pll5ghz_tsmc12ffc_pwron2(pll,		\
		(uint64_t)fvco, (uint64_t)fpclk, (uint64_t)frclk)
#define duowen_pll_disable(pll)				\
	dw_pll5ghz_tsmc12ffc_pwrdn(pll)
#define duowen_div_enable(pll, fvco, freq, r)		\
	dw_pll5ghz_tsmc12ffc_enable(pll, fvco, freq, r)
#define duowen_div_disable(pll, r)			\
	dw_pll5ghz_tsmc12ffc_disable(pll, r)
void duowen_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val);
uint8_t duowen_pll_reg_read(uint8_t pll, uint8_t reg);

/* power control */
#ifdef CONFIG_DUOWEN_SOCv1
#define CRCNTL_WARM_RESET_DETECT_TIME	CRCNTL_REG(0x100)
#define CRCNTL_PWR_FSM_DELAY_TIME	CRCNTL_REG(0x104)
#define CRCNTL_PWR_SHUTDOWN		CRCNTL_REG(0x108)
#define CRCNTL_PS_HOLD			CRCNTL_REG(0x10C)
#define CRCNTL_SW_GLOBAL_RST		CRCNTL_REG(0x110)

/* CRCNTL_FSM_DELAY_TIME */
#define PWR_WARM_RESET_DELAY_OFFSET	0
#define PWR_WARM_RESET_DELAY_MASK	REG_16BIT_MASK
#define PWR_WARM_RESET_DELAY(value)	_SET_FV(PWR_WARM_RESET_DELAY, value)
#define PWR_SHUTDOWN_DELAY_OFFSET	16
#define PWR_SHUTDOWN_DELAY_MASK		REG_16BIT_MASK
#define PWR_SHUTDOWN_DELAY(value)	_SET_FV(PWR_SHUTDOWN_DELAY, value)

#define crcntl_config_timing(wrst_detect, wrst_delay, shutdown_delay)	\
	do {								\
		__raw_writel_mask(PWR_WARM_RESET_DETECT(wrst_detect),	\
				  CRCNTL_WARM_RESET_DETECT_TIME);	\
		__raw_writel(PWR_WARM_RESET_DELAY(wrst_delay) |		\
			     PWR_SHUTDOWN_DELAY(shutdown_delay),	\
			     CRCNTL_FSM_DELAY_TIME);			\
	} while (0)
#endif
#ifdef CONFIG_DUOWEN_SOCv2
#define CRCNTL_WARM_RESET_DETECT_TIME	CRCNTL_REG(0x100)
#define CRCNTL_WARM_RESET_DELAY_TIME	CRCNTL_REG(0x104)
#define CRCNTL_SHUTDN_DELAY_TIME	CRCNTL_REG(0x108)
#define CRCNTL_PWR_SHUTDOWN		CRCNTL_REG(0x10C)
#define CRCNTL_PS_HOLD			CRCNTL_REG(0x110)
#define CRCNTL_SW_GLOBAL_RST		CRCNTL_REG(0x114)

#define crcntl_config_timing(wrst_detect, wrst_delay, shutdown_delay)	\
	do {								\
		__raw_writel_mask(PWR_WARM_RESET_DETECT(wrst_detect),	\
				  CRCNTL_WARM_RESET_DETECT_TIME);	\
		__raw_writel(wrst_delay, CRCNTL_WARM_RESET_DELAY_TIME);	\
		__raw_writel(shutdown_delay, CRCNTL_SHUTDN_DELAY_TIME);	\
	} while (0)
#endif

/* power control */
/* CRCNTL_WARM_RESET_DETECT_TIME */
#define PWR_WARM_RESET_DETECT_OFFSET	0
#define PWR_WARM_RESET_DETECT_MASK	REG_8BIT_MASK
#define PWR_WARM_RESET_DETECT(value)	_SET_FV(PWR_WARM_RESET_DETECT, value)

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

/* TODO: Wait imc_rst_n, drive PS_HOLD, and wait for PMIC? */
#define crcntl_power_up()						\
	__raw_setl(PWR_PS_HOLD, CRCNTL_PS_HOLD)
#define crcntl_power_down()						\
	__raw_setl(PWR_SHUT_DN, CRCNTL_SHUTDOWN)
#define crcntl_global_reset()						\
	__raw_writel(PWR_GLOBAL_RST, CRCNTL_SW_GLOBAL_RST)

#endif /* __CRCNTL_DUOWEN_H_INCLUDE__ */
