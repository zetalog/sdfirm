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
 * @(#)scg.h: RV32M1 (VEGA) SCG (system clock generator) driver
 * $Id: scg.h,v 1.1 2019-08-18 12:45:00 zhenglv Exp $
 */

#ifndef __SCG_VEGA_H_INCLUDE__
#define __SCG_VEGA_H_INCLUDE__

#include <target/types.h>
#include <target/bitops.h>

#define SCG_BASE			UL(0x4002C000)
#define SCG_REG(offset)			(SCG_BASE + (offset))

#define SCG_VERID			SCG_REG(0x000)
#define SCG_PARAM			SCG_REG(0x004)
#define SCG_CSR				SCG_REG(0x010)
#define SCG_RCCR			SCG_REG(0x014)
#define SCG_VCCR			SCG_REG(0x018)
#define SCG_HCCR			SCG_REG(0x01C)
#define SCG_CLKOUTCNFG			SCG_REG(0x020)
#define SCG_SOSCCSR			SCG_REG(0x100)
#define SCG_SOSCDIV			SCG_REG(0x104)
#define SCG_SIRCCSR			SCG_REG(0x200)
#define SCG_SIRCDIV			SCG_REG(0x204)
#define SCG_SIRCCFG			SCR_REG(0x208)
#define SCG_FIRCCSR			SCG_REG(0x300)
#define SCG_FIRCDIV			SCG_REG(0x304)
#define SCG_FIRCCFG			SCG_REG(0x308)
#define SCG_FIRCTCFG			SCG_REG(0x30C)
#define SCG_FIRCSTAT			SCG_REG(0x318)
#define SCG_ROSCCSR			SCG_REG(0x400)
#define SCG_LPFLLCSR			SCG_REG(0x500)
#define SCG_LPFLLDIV			SCG_REG(0x504)
#define SCG_LPFLLCFG			SCG_REG(0x508)
#define SCG_LPFLLTCFG			SCG_REG(0x50C)
#define SCG_LPFLLSTAT			SCG_REG(0x514)

/* 29.3.1.3 Parameter Register (PARAM) */
#define SCG_DIVPRES_OFFSET		27
#define SCG_DIVPRES_MASK		REG_5BIT_MASK
#define SCG_DIVPRES(value)		_GET_FV(SCG_DIVPRES, value)
#define SCG_DIVPRES_SLOW		_BV(0)
#define SCG_DIVPRES_BUS			_BV(1)
#define SCG_DIVPRES_EXT			_BV(2)
#define SCG_DIVPRES_CORE		_BV(4)
#define SCG_CLKPRES_OFFSET		0
#define SCG_CLKPRES_MASK		REG_5BIT_MASK
#define SCG_CLKPRES(value)		_GET_FV(SCG_CLKPRES, value)
#define SCG_CLKPRES_SOSC		_BV(1) /* System OSC */
#define SCG_CLKPRES_SIRC		_BV(2)
#define SCG_CLKPRES_FIRC		_BV(3)
#define SCG_CLKPRES_ROSC		_BV(4)
#define SCG_CLKPRES_LPFLL		_BV(5)

/* 29.3.1.4 Clock Status Register (CSR)
 * 29.3.1.5 Run Clock Control Register (RCCR)
 * 29.3.1.6 VLPR Clock Control Register (VCCR)
 * 29.3.1.7 HSRUN Clock Control Register (HCCR)
 * 29.3.1.8 SCG CLKOUT Configuration Register (CLKOUTCNFG)
 */
#define SCG_DIVSLOW_OFFSET		0
#define SCG_DIVSLOW_MASK		REG_4BIT_MASK
#define SCG_GET_DIVSLOW(value)		_GET_FV(SCG_DIVSLOW, value)
#define SCG_SET_DIVSLOW(value)		_SET_FV(SCG_DIVSLOW, value)
#define SCG_DIVBUS_OFFSET		4
#define SCG_DIVBUS_MASK			REG_4BIT_MASK
#define SCG_GET_DIVBUS(value)		_GET_FV(SCG_DIVBUS, value)
#define SCG_SET_DIVBUS(value)		_SET_FV(SCG_DIVBUS, value)
#define SCG_DIVEXT_OFFSET		8
#define SCG_DIVEXT_MASK			REG_4BIT_MASK
#define SCG_GET_DIVEXT(value)		_GET_FV(SCG_DIVEXT, value)
#define SCG_SET_DIVEXT(value)		_SET_FV(SCG_DIVEXT, value)
#define SCG_DIVCORE_OFFSET		16
#define SCG_DIVCORE_MASK		REG_4BIT_MASK
#define SCG_GET_DIVCORE(value)		_GET_FV(SCG_DIVCORE, value)
#define SCG_SET_DIVCORE(value)		_SET_FV(SCG_DIVCORE, value)
#define SCG_CSR_DIV_MAX			16
#define SCG_CSR_DIV2VAL(div)		((div) - 1)
#define SCG_CSR_VAL2DIV(val)		((val) + 1)
#define SCG_SCS_OFFSET			24
#define SCG_SCS_MASK			REG_4BIT_MASK
#define SCG_GET_SCS(value)		_GET_FV(SCG_SCS, value)
#define SCG_SET_SCS(value)		_SET_FV(SCG_SCS, value)
#define SCG_SCS_EXT			0
#define SCG_SCS_SOSC			1
#define SCG_SCS_SIRC			2
#define SCG_SCS_FIRC			3
#define SCG_SCS_ROSC			4
#define SCG_SCS_LPFLL			5

/* 29.3.1.9 System OSC Control Status Register (SOSCCSR)
 * 29.3.1.11 Slow IRC Control Status Register (SIRCCSR)
 * 29.3.1.14 Fast IRC Control Status Register (FIRCCSR)
 * 29.3.1.19 RTC OSC Control Status Register (ROSCCSR)
 * 29.3.1.20 Low Power FLL Control Status Register (LPFLLCSR)
 */
#define SCG_EN				_BV(0)
#define SCG_STEN			_BV(1) /* stop enable */
#define SCG_LPEN			_BV(2) /* low power enable */
#define SCG_REGOFF			_BV(3) /* regulator off */
#define SCG_TREN			_BV(8) /* trim enable */
#define SCG_TRUP			_BV(9) /* trim update */
#define SCG_TRMLOCK			_BV(10)/* trim lock */
#define SCG_CM				_BV(16)/* clock monitor */
#define SCG_CMRE			_BV(17)/* clock monitor reset enable */
#define SCG_LK				_BV(23)
#define SCG_VLD				_BV(24)/* valid */
#define SCG_SEL				_BV(25)/* selected */
#define SCG_ERR				_BV(26)

/* 29.3.1.10 System OSC Divide Register (SOSCDIV)
 * 29.3.1.12 Slow IRC Divide Register (SIRCDIV)
 * 29.3.1.15 Fast IRC Divide Register (FIRCDIV)
 * 29.3.1.21 Low Power FLL Divide Register (LPFLLDIV)
 */
#define SCG_DIV1_OFFSET			0
#define SCG_DIV1_MASK			REG_3BIT_MASK
#define SCG_DIV1(value)			_SET_FV(SCG_DIV1, value)
#define SCG_DIV2_OFFSET			8
#define SCG_DIV2_MASK			REG_3BIT_MASK
#define SCG_DIV2(value)			_SET_FV(SCG_DIV2, value)
#define SCG_DIV3_OFFSET			16
#define SCG_DIV3_MASK			REG_3BIT_MASK
#define SCG_DIV3(value)			_SET_FV(SCG_DIV3, value)
#define SCG_DIV_DIV_MAX			64
#define SCG_DIV_DIV2VAL(div)		ilog2_const(div)
#define SCG_DIV_VAL2DIV(val)		(1 << (val))

/* 29.3.1.13 Slow IRC Configuration Register (SIRCCFG) */
#define SCG_SIRC_RANGE_OFFSET		0
#define SCG_SIRC_RANGE_MASK		REG_1BIT_MASK
#define SCG_SIRC_RANGE(value)		_SET_FV(SCG_SIRC_RANGE, value)
#define SCG_SIRC_2MHZ			0
#define SCG_SIRC_8MHZ			1

/* 29.3.1.16 Fast IRC Configuration Register (FIRCCFG) */
#define SCG_FIRC_RANGE_OFFSET		0
#define SCG_FIRC_RANGE_MASK		1
#define SCG_FIRC_RANGE(value)		_SET_FV(SCG_FIRC_RANGE, value)
#define SCG_FIRC_48MHZ			0
#define SCG_FIRC_52MHZ			1
#define SCG_FIRC_56MHZ			2
#define SCG_FIRC_60MHZ			3

/* 29.3.1.17 Fast IRC Trim Configuration Register (FIRCTCFG) */
#define SCG_FIRC_TRIMSRC_OFFSET		0
#define SCG_FIRC_TRIMSRC_MASK		REG_2BIT_MASK
#define SCG_FIRC_TRIMSRC(value)		_SET_FV(SCG_FIRC_TRIMSRC, value)
/* only if SOSC is divided to a freq slower than 32KHz */
#define SCG_FIRC_TRIMSRC_SOSC		2
#define SCG_FIRC_TRIMSRC_ROSC		3
#define SCG_FIRC_TRIMDIV_OFFSET		8
#define SCG_FIRC_TRIMDIV_MASK		REG_3BIT_MASK
#define SCG_FIRC_TRIMDIV(value)		_SET_FV(SCG_FIRC_TRIMDIV, value)
#define SCG_FIRC_TRIM_DIV_MAX		2048
#define SCG_FIRC_TRIM_DIV2VAL(div)	\
	((div) == 1 ? 0 : ilog2_const(div) - 6)

/* 29.3.1.18 Fast IRC Status Register (FIRCSTAT) */
#define SCG_FIRC_TRIMFINE_OFFSET	0
#define SCG_FIRC_TRIMFINE_MASK		REG_7BIT_MASK
#define SCG_FIRC_TRIMFINE(value)	_SET_FV(SCG_FIRC_TRIMFINE, value)
#define SCG_FIRC_TRIMCOAR_OFFSET	8
#define SCG_FIRC_TRIMCOAR_MASK		REG_6BIT_MASK
#define SCG_FIRC_TRIMCOAR(value)	_SET_FV(SCG_FIRC_TRIMCOAR, value)

/* 29.3.1.22 Low Power FLL Configuration Register (LPFLLCFG) */
#define SCG_LPFLL_FSEL_OFFSET		0
#define SCG_LPFLL_FSEL_MASK		REG_2BIT_MASK
#define SCG_LPFLL_FSEL(value)		_SET_FV(SCG_LPFLL_FSEL, value)
#define SCG_LPFLL_48MHZ			0
#define SCG_LPFLL_72MHZ			1

/* 29.3.1.23 Low Power FLL Trim Configuration Register (LPFLLTCFG) */
#define SCG_LPFLL_TRIMSRC_OFFSET	0
#define SCG_LPFLL_TRIMSRC_MASK		REG_2BIT_MASK
#define SCG_LPFLL_TRIMSRC(value)	_SET_FV(SCG_LPFLL_TRIMSRC, value)
#define SCG_LPFLL_TRIMSRC_SIRC		0
#define SCG_LPFLL_TRIMSRC_FIRC		1
#define SCG_LPFLL_TRIMSRC_SOSC		2
#define SCG_LPFLL_TRIMSRC_ROSC		3
#define SCG_LPFLL_TRIMDIV_OFFSET	8
#define SCG_LPFLL_TRIMDIV_MASK		REG_5BIT_MASK
#define SCG_LPFLL_TRIMDIV(value)	_SET_FV(SCG_LPFLL_TRIMDIV, value)
#define SCG_LPFLL_TRIM_DIV_MAX		32
#define SCG_LPFLL_TRIM_DIV2VAL(div)	((div) - 1)
#define SCG_LPFLL_TRIM_VAL2DIV(div)	((val) + 1)
/* LOCKW2LSB - Lock LPFLL with 2 LSBS
 *  0b0..LPFLL locks within 1LSB (0.4%)
 *  0b1..LPFLL locks within 2LSB (0.8%)
 */
#define SCG_LPFLL_LOCKW2LSB		_BV(16)

/* 29.3.1.24 Low Power FLL Status Register (LPFLLSTAT) */
#define SCG_LPFLL_AUTOTRIM_OFFSET	0
#define SCG_LPFLL_AUTOTRIM_MASK		REG_8BIT_MASK
#define SCG_LPFLL_AUTOTRIM(value)	_SET_FV(SCG_LPFLL_AUTOTRIM, value)

#define BOARD_BOOTCLOCKRUN_CORE_CLOCK		U(48000000)
#define BOARD_BOOTCLOCKHSRUN_CORE_CLOCK		U(72000000)
#define BOARD_BOOTCLOCKVLPR_CORE_CLOCK		U(4000000)

void BOARD_InitBootClocks(void);
void BOARD_BootClockRUN(void);
void BOARD_BootClockHSRUN(void);
void BOARD_BootClockVLPR(void);

#if 0
/* Boot clocks */
extern const scg_sys_clk_config_t g_sysClkConfig_BOARD_BootClockRUN;
extern const scg_sosc_config_t g_scgSysOscConfig_BOARD_BootClockRUN;
extern const scg_sirc_config_t g_scgSircConfig_BOARD_BootClockRUN;
extern const scg_firc_config_t g_scgFircConfigBOARD_BootClockRUN;
extern const scg_lpfll_config_t g_scgLpFllConfigBOARD_BootClockRUN;

/* HS clocks */
extern const scg_sys_clk_config_t g_sysClkConfig_BOARD_BootClockHSRUN;
extern const scg_sosc_config_t g_scgSysOscConfig_BOARD_BootClockHSRUN;
extern const scg_sirc_config_t g_scgSircConfig_BOARD_BootClockHSRUN;
extern const scg_firc_config_t g_scgFircConfigBOARD_BootClockHSRUN;
extern const scg_lpfll_config_t g_scgLpFllConfigBOARD_BootClockHSRUN;

/* LPR clocks */
extern const scg_sys_clk_config_t g_sysClkConfig_BOARD_BootClockVLPR;
extern const scg_sosc_config_t g_scgSysOscConfig_BOARD_BootClockVLPR;
extern const scg_sirc_config_t g_scgSircConfig_BOARD_BootClockVLPR;
extern const scg_firc_config_t g_scgFircConfigBOARD_BootClockVLPR;
extern const scg_lpfll_config_t g_scgLpFllConfigBOARD_BootClockVLPR;
#endif

#endif /* __SCG_VEGA_H_INCLUDE__ */
