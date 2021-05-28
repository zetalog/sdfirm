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

#define DW_PLL_F_REFCLK(pll)			XO_CLK_FREQ
#define __dw_pll_read(pll, reg)			duowen_pll_reg_read(pll, reg)
#define __dw_pll_write(pll, reg, val)		\
	duowen_pll_reg_write(pll, reg, val)
#define __dw_pll_wait_cmpclk(pll, cycles)	\
	duowen_pll_wait_cmpclk(pll, cycles)
#define __dw_pll_wait_timing(pll, timing)	\
	duowen_pll_wait_timing(pll, timing)
#define DW_PLL_T_SPO			22 /* until enp/enr can be set */
#define DW_PLL_T_GS			21 /* until gearshif can be unset */
#define DW_PLL_T_PRST			20 /* during PRESET */
#define DW_PLL_T_TRST			19 /* until pwron can be set */
#define DW_PLL_T_PWRON			18 /* until rst_n can be set */
#define DW_PLL_T_PWRSTB			17 /* until test_rst_n can be set */

extern phys_addr_t duowen_pll_reg_base[];
#define DW_PLL_BASE(pll)		duowen_pll_reg_base[pll]
#define DW_PLL_REG(pll, offset)		(DW_PLL_BASE(pll) + (offset))
#define DW_PLL_CFG0(pll)		DW_PLL_REG(pll, 0x00)
#define DW_PLL_CFG1(pll)		DW_PLL_REG(pll, 0x04)
#define DW_PLL_CFG2(pll)		DW_PLL_REG(pll, 0x08)
#define DW_PLL_STATUS(pll)		DW_PLL_REG(pll, 0x0C)
#define DW_PLL_REG_ACCESS(pll)		DW_PLL_REG(pll, 0x10)
#define DW_PLL_REG_TIMING(pll)		DW_PLL_REG(pll, 0x14)
#define DW_PLL_HW_CFG(pll)		DW_PLL_REG(pll, 0x18)
#define DW_PLL_REFCLK_FREQ(pll)		DW_PLL_REG(pll, 0x1C)
#define DW_PLL_CLK_P_FREQ(pll)		DW_PLL_REG(pll, 0x20)
#define DW_PLL_CLK_R_FREQ(pll)		DW_PLL_REG(pll, 0x24)
#define DW_PLL_CNT(pll)			DW_PLL_REG(pll, 0x28)

/* PLL_STATUS
 * Vendor specific bits
 */
#define PLL_CNT_LOCKED		_BV(2)

/* DW_PLL_CNT */
#define PLL_CNT_EN		_BV(16)
#define PLL_CNT_COUNTER_OFFSET	0
#define PLL_CNT_COUNTER_MASK	REG_10BIT_MASK
#define PLL_CNT_COUNTER(value)	_SET_FV(PLL_CNT_COUNTER, value)
#define pll_cnt_counter(value)	_GET_FV(PLL_CNT_COUNTER, value)

#include <driver/dw_pll5ghz_tsmc12ffc.h>

#ifdef CONFIG_DUOWEN_SBI_DUAL
#define __CLUSTER_PLL_BASE(n)		(__CLUSTER0_BASE + ((n) << 20))
#define __DUOWEN_CLK_BASE(soc)		(__SOC_BASE(soc) + __CRCNTL_BASE)
#define __DUOWEN_CFAB_CLK_BASE(soc)	(__SOC_BASE(soc) + __COHFAB_PLL_BASE)
#define __DUOWEN_APC_CLK_BASE(soc, n)	\
	(__SOC_BASE(soc) + __CLUSTER_PLL_BASE(n))
#define __DUOWEN_ETH_CLK_BASE(soc)	(__SOC_BASE(soc) + __ETH_PLL_BASE)
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define __DUOWEN_CLK_BASE(soc)		CRCNTL_BASE
#define __DUOWEN_CFAB_CLK_BASE(soc)	COHFAB_PLL_BASE
#define __DUOWEN_APC_CLK_BASE(soc, n)	(CLUSTER0_BASE + ((n) << 20))
#define __DUOWEN_ETH_CLK_BASE(soc)	ETH_PLL_BASE
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#ifdef CONFIG_MMU
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define DUOWEN_CLK_BASE(soc)		duowen_clk_reg_base[soc]
extern caddr_t duowen_clk_reg_base[];
#define DUOWEN_CFAB_CLK_BASE(soc)	duowen_cfab_clk_reg_base[soc]
extern caddr_t duowen_cfab_clk_reg_base[];
#define DUOWEN_APC_CLK_BASE(soc, n)	duowen_apc_clk_reg_base[soc][n]
extern caddr_t duowen_apc_clk_reg_base[][4];
#define DUOWEN_ETH_CLK_BASE(soc)	duowen_eth_clk_reg_base[soc]
extern caddr_t duowen_eth_clk_reg_base[];
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define DUOWEN_CLK_BASE(soc)		duowen_clk_reg_base
extern caddr_t duowen_clk_reg_base;
#define DUOWEN_CFAB_CLK_BASE(soc)	duowen_cfab_clk_reg_base
extern caddr_t duowen_cfab_clk_reg_base;
#define DUOWEN_APC_CLK_BASE(soc, n)	duowen_apc_clk_reg_base[n]
extern caddr_t duowen_apc_clk_reg_base[];
#define DUOWEN_ETH_CLK_BASE(soc)	duowen_eth_clk_reg_base
extern caddr_t duowen_eth_clk_reg_base;
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#else
#define DUOWEN_CLK_BASE(soc)		__DUOWEN_CLK_BASE(soc)
#define DUOWEN_CFAB_CLK_BASE(soc)	__DUOWEN_CFAB_CLK_BASE(soc)
#define DUOWEN_APC_CLK_BASE(soc, n)	__DUOWEN_APC_CLK_BASE(soc, n)
#define DUOWEN_ETH_CLK_BASE(soc)	__DUOWEN_ETH_CLK_BASE(soc)
#endif

/* XXX: This implementation is based on undocumented PLL RTL
 *      sequence, and may subject to change.
 * NOTE: all CRCNTL registers are only 32-bit accessiable.
 */

#define __CRCNTL_REG(soc, offset)	(__DUOWEN_CLK_BASE(soc) + (offset))
#define CRCNTL_REG(soc, offset)		(DUOWEN_CLK_BASE(soc) + (offset))
#define COHFAB_PLL_REG(soc, offset)	(DUOWEN_CFAB_CLK_BASE(soc) + (offset))
#define CLUSTER_PLL_REG(soc, n, offset)	(DUOWEN_APC_CLK_BASE(soc, n) + (offset))
#define ETH_PLL_REG(soc, offset)	(DUOWEN_ETH_CLK_BASE(soc) + (offset))

/* PLL control */
#define __CRCNTL_PLL_REG(soc, pll, offset)	\
	__CRCNTL_REG(soc, ((pll) << 6) + (offset))
#define CRCNTL_PLL_REG(soc, pll, offset)	\
	CRCNTL_REG(soc, ((pll) << 6) + (offset))

/* COHFAB/CLUSTER PLL clock control */
#define COHFAB_CLK_CFG(soc, pll)	DW_PLL_REG(socpll(pll, soc), 0x40)
#define COHFAB_RESET_COHFAB(soc, pll)	DW_PLL_REG(socpll(pll, soc), 0x44)
#define COHFAB_RESET_CLUSTER(soc, pll)	DW_PLL_REG(socpll(pll, soc), 0x50)

/* CLUSTER PLL clock control */
#define CLUSTER_CLK_CG_CFG(soc, apc)	CLUSTER_PLL_REG(soc, apc, 0x44)
#define CLUSTER_RESET_CTRL(soc, apc)	CLUSTER_PLL_REG(soc, apc, 0x50)

/* reset control */
#define CRCNTL_SW_RST_CFG(soc, n)	CRCNTL_REG(soc, 0x130 + ((n) << 2))

/* clock control */
#define CRCNTL_CLK_EN_CFG(soc, n)	CRCNTL_REG(soc, 0x140 + ((n) << 2))
#define CRCNTL_CLK_SEL_CFG(soc)		CRCNTL_REG(soc, 0x150)

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

/* PLL_HW_CFG */
#define PLL_WAIT_T(timing)	_BV(timing)
#define PLL_HW_CFG_BUSY		_BV(16)
#define PLL_OPTION_OFFSET	4
#define PLL_OPTION_MASK		REG_4BIT_MASK
#define PLL_OPTION(value)	_SET_FV(PLL_OPTION, value)
/* Pre-defined PLL output options */
#define PLL_OPTION_2500M	1
#define PLL_OPTION_2000M	2
#define PLL_OPTION_1800M	3
#define PLL_OPTION_1000M	4
#define PLL_OPTION_800M		5
#define PLL_OPTION_666M		6
#define PLL_OPTION_600M		7
#define PLL_OPTION_533M		8
#define PLL_OPTION_466M		9
#define PLL_OPTION_400M		10
#define PLL_OPTION_100M		11
#define PLL_OPTION_156250K	12
#define PLL_HW_START		_BV(1)

/* PLL_CNT */
#define PLL_CNT_EN		_BV(16)
#define PLL_COUNTER_OFFSET	0
#define PLL_COUNTER_MASK	REG_10BIT_MASK
#define PLL_COUNTER(value)	_GET_FV(PLL_COUNTER, value)

/* APIs here can be invoked w/o enabling clock tree core */
bool __crcntl_clk_asserted(clk_clk_t clk, uint8_t soc);
void __crcntl_clk_assert(clk_clk_t clk, uint8_t soc);
void __crcntl_clk_deassert(clk_clk_t clk, uint8_t soc);
bool __crcntl_clk_enabled(clk_clk_t clk, uint8_t soc);
void __crcntl_clk_enable(clk_clk_t clk, uint8_t soc);
void __crcntl_clk_disable(clk_clk_t clk, uint8_t soc);
bool __cohfab_clk_asserted(clk_clk_t clk, uint8_t soc);
void __cohfab_clk_assert(clk_clk_t clk, uint8_t soc);
void __cohfab_clk_deassert(clk_clk_t clk, uint8_t soc);
bool __cohfab_clk_enabled(clk_clk_t clk, uint8_t soc);
void __cohfab_clk_enable(clk_clk_t clk, uint8_t soc);
void __cohfab_clk_disable(clk_clk_t clk, uint8_t soc);
bool __cluster_clk_asserted(clk_clk_t clk, uint8_t soc);
void __cluster_clk_assert(clk_clk_t clk, uint8_t soc);
void __cluster_clk_deassert(clk_clk_t clk, uint8_t soc);
bool __cluster_clk_enabled(clk_clk_t clk, uint8_t soc);
void __cluster_clk_enable(clk_clk_t clk, uint8_t soc);
void __cluster_clk_disable(clk_clk_t clk, uint8_t soc);
#define crcntl_clk_asserted(clk)		\
	__crcntl_clk_asserted(clk, imc_socket_id())
#define crcntl_clk_assert(clk)			\
	__crcntl_clk_assert(clk, imc_socket_id())
#define crcntl_clk_deassert(clk)		\
	__crcntl_clk_deassert(clk, imc_socket_id())
#define crcntl_clk_enabled(clk)			\
	__crcntl_clk_enabled(clk, imc_socket_id())
#define crcntl_clk_enable(clk)			\
	__crcntl_clk_enable(clk, imc_socket_id())
#define crcntl_clk_disable(clk)			\
	__crcntl_clk_disable(clk, imc_socket_id())
#define cohfab_clk_asserted(clk)		\
	__cohfab_clk_asserted(clk, imc_socket_id())
#define cohfab_clk_assert(clk)			\
	__cohfab_clk_assert(clk, imc_socket_id())
#define cohfab_clk_deassert(clk)		\
	__cohfab_clk_deassert(clk, imc_socket_id())
#define cohfab_clk_enabled(clk)			\
	__cohfab_clk_enabled(clk, imc_socket_id())
#define cohfab_clk_enable(clk)			\
	__cohfab_clk_enable(clk, imc_socket_id())
#define cohfab_clk_disable(clk)			\
	__cohfab_clk_disable(clk, imc_socket_id())
#define cluster_clk_asserted(clk)		\
	__cluster_clk_asserted(clk, imc_socket_id())
#define cluster_clk_assert(clk)			\
	__cluster_clk_assert(clk, imc_socket_id())
#define cluster_clk_deassert(clk)		\
	__cluster_clk_deassert(clk, imc_socket_id())
#define cluster_clk_enabled(clk)		\
	__cluster_clk_enabled(clk, imc_socket_id())
#define cluster_clk_enable(clk)			\
	__cluster_clk_enable(clk, imc_socket_id())
#define cluster_clk_disable(clk)		\
	__cluster_clk_disable(clk, imc_socket_id())
#ifdef CONFIG_CRCNTL_MUX
bool __crcntl_clk_selected(clk_clk_t clk, uint8_t soc);
void __crcntl_clk_select(clk_clk_t clk, uint8_t soc);
void __crcntl_clk_deselect(clk_clk_t clk, uint8_t soc);
bool __cohfab_clk_selected(clk_clk_t clk, uint8_t soc);
void __cohfab_clk_select(clk_clk_t clk, uint8_t soc);
void __cohfab_clk_deselect(clk_clk_t clk, uint8_t soc);
uint32_t __crcntl_clk_sel_read(uint8_t soc);
void __crcntl_clk_sel_write(uint32_t sels, uint8_t soc);
#define crcntl_clk_selected(clk)		\
	__crcntl_clk_selected(clk, imc_socket_id())
#define crcntl_clk_select(clk)			\
	__crcntl_clk_select(clk, imc_socket_id())
#define crcntl_clk_deselect(clk)		\
	__crcntl_clk_deselect(clk, imc_socket_id())
#define cohfab_clk_selected(clk)		\
	__cohfab_clk_selected(clk, imc_socket_id())
#define cohfab_clk_select(clk)			\
	__cohfab_clk_select(clk, imc_socket_id())
#define cohfab_clk_deselect(clk)		\
	__cohfab_clk_deselect(clk, imc_socket_id())
#define crcntl_clk_sel_read()			\
	__crcntl_clk_sel_read(imc_socket_id())
#define crcntl_clk_sel_write(sels)		\
	__crcntl_clk_sel_write(sels, imc_socket_id())
#endif
#ifdef CONFIG_CRCNTL_TRACE
void crcntl_trace(bool enabling, const char *name);
#else
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
void duowen_pll_wait_cmpclk(uint8_t pll, uint16_t cycles);
bool duowen_pll_wait_timing(uint8_t pll, uint8_t timing);

/* global power/reset control */
#define CRCNTL_WARM_RESET_DETECT_TIME	CRCNTL_REG(imc_socket_id(), 0x100)
#define CRCNTL_WARM_RESET_DELAY_TIME	CRCNTL_REG(imc_socket_id(), 0x104)
#define CRCNTL_SHUTDN_DELAY_TIME	CRCNTL_REG(imc_socket_id(), 0x108)
#define CRCNTL_PWR_SHUTDOWN		CRCNTL_REG(imc_socket_id(), 0x10C)
#define CRCNTL_PS_HOLD			CRCNTL_REG(imc_socket_id(), 0x110)
#define CRCNTL_SW_GLOBAL_RST		CRCNTL_REG(imc_socket_id(), 0x114)
#define CRCNTL_WARM_RST_INTR		CRCNTL_REG(imc_socket_id(), 0x118)
#define CRCNTL_WARM_RST_INTR_CLR	CRCNTL_REG(imc_socket_id(), 0x11C)
#define CRCNTL_RST_CAUSE		CRCNTL_REG(imc_socket_id(), 0x120)

#define crcntl_config_timing(wrst_detect, wrst_delay, shutdown_delay)	\
	do {								\
		__raw_writel_mask(PWR_WARM_RESET_DETECT(wrst_detect),	\
				  CRCNTL_WARM_RESET_DETECT_TIME);	\
		__raw_writel(wrst_delay, CRCNTL_WARM_RESET_DELAY_TIME);	\
		__raw_writel(shutdown_delay, CRCNTL_SHUTDN_DELAY_TIME);	\
	} while (0)

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
#define RST_WDT			_BV(1)
#define RST_WARM		_BV(2)

/* TODO: Wait imc_rst_n, drive PS_HOLD, and wait for PMIC? */
#define crcntl_power_up()						\
	__raw_setl(PWR_PS_HOLD, CRCNTL_PS_HOLD)
#define crcntl_power_down()						\
	__raw_setl(PWR_SHUT_DN, CRCNTL_SHUTDOWN)
#define crcntl_global_reset()						\
	__raw_writel(PWR_GLOBAL_RST, CRCNTL_SW_GLOBAL_RST)

#endif /* __CRCNTL_DUOWEN_H_INCLUDE__ */
