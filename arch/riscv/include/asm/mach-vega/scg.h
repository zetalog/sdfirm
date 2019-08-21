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

#include <errno.h>
#include <stdbool.h>
#include <target/types.h>
#include <target/bitops.h>
#include <target/arch.h>

/* system power modes */
#define SCG_CLKOUT			NR_SYS_MODES
#define NR_SCG_MODES			(SCG_CLKOUT + 1)
/* system clock source IDs */
#define SCG_SCS_EXT			0
#define SCG_SCS_SOSC			1
#define SCG_SCS_SIRC			2
#define SCG_SCS_FIRC			3
#define SCG_SCS_ROSC			4
#define SCG_SCS_LPFLL			5
#define NR_SCG_CLOCKS			6
/* system clock divider IDs */
#define SCG_DIVSLOW			0
#define SCG_DIVBUS			1
#define SCG_DIVEXT			2
#define SCG_DIVCORE			4
/* clock divider IDs */
#define SCG_DIV1			0
#define SCG_DIV2			1
#define SCG_DIV3			2

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

/* system power mode registers */
#define SCG_MODEREG(mode)		(SCG_RCCR + (mode) * 4)
/* system clock source registers */
#define SCG_SCSREG(scs, off)		(SCG_REG(0x100 * (scs)) + (off))
#define SCG_SCSCSR(scs)			SCG_SCSREG(scs, 0x00)
#define SCG_SCSDIV(scs)			SCG_SCSREG(scs, 0x04)
#define SCG_SCSCFG(scs)			SCG_SCSREG(scs, 0x08)
#define SCG_SCSTCFG(scs)		SCG_SCSREG(scs, 0x0C)

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
#define SCG_CCR_DIV2VAL(div)		((div) - 1)
#define SCG_CCR_VAL2DIV(val)		((val) + 1)
#define SCG_CCR_DIV_MASK		REG_4BIT_MASK
/* DIV values */
#define SCG_CCR_DIV_MAX			16
#define SCG_CCR_DIV_SET(id, div)	\
	(((div) & SCG_CCR_DIV_MASK) << ((id) * 4))
#define SCG_CCR_DIV_GET(id, val)	\
	(((val) >> ((id) * 4)) & SCG_CCR_DIV_MASK)
#define SCG_CCR_DIV_MSK(id)		\
	(SCG_CCR_DIV_MASK << ((id) * 4))

/* use input clock IDs */
#define SCG_SCS_OFFSET			24
#define SCG_SCS_MASK			REG_4BIT_MASK
#define SCG_GET_SCS(value)		_GET_FV(SCG_SCS, value)
#define SCG_SET_SCS(value)		_SET_FV(SCG_SCS, value)

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
#define SCG_DIV_DIV2VAL(div)		((div) ? __ilog2_u16(div) + 1 : 0)
#define SCG_DIV_VAL2DIV(val)		((val) ? (1 << ((val) - 1)) : 0)
#define SCG_DIV_DIV_MASK		REG_3BIT_MASK
/* DIV values */
#define SCG_DIV_DIV_MAX			64
#define SCG_DIV_DIV_DISABLED		0
#define SCG_DIV_DIV_SET(id, div)	\
	(((div) & SCG_DIV_DIV_MASK) << ((id) * 8))
#define SCG_DIV_DIV_GET(id, val)	\
	(((val) >> ((id) * 8)) & SCG_DIV_DIV_MASK)
#define SCG_DIV_DIV_MSK(id)		\
	(SCG_DIV_DIV_MASK << ((id) * 8))

/* 29.3.1.13 Slow IRC Configuration Register (SIRCCFG)
 * 29.3.1.16 Fast IRC Configuration Register (FIRCCFG)
 * 29.3.1.22 Low Power FLL Configuration Register (LPFLLCFG)
 */
#define SCG_RANGE_OFFSET		0
#define SCG_RANGE_MASK			REG_1BIT_MASK
#define SCG_GET_RANGE(value)		_GET_FV(SCG_RANGE, value)
#define SCG_SET_RANGE(value)		_SET_FV(SCG_RANGE, value)
#define SCG_SIRC_2MHZ			0
#define SCG_SIRC_8MHZ			1
#define SCG_SIRC_RANGE_MAX		2
#define SCG_FIRC_48MHZ			0
#define SCG_FIRC_52MHZ			1
#define SCG_FIRC_56MHZ			2
#define SCG_FIRC_60MHZ			3
#define SCG_FIRC_RANGE_MAX		4
#define SCG_LPFLL_48MHZ			0
#define SCG_LPFLL_72MHZ			1
#define SCG_LPFLL_FSEL_MAX		2

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

/* external crystals */
#ifdef CONFIG_SCG_SOSC_32MHZ
#define SCG_SOSC_FREQ			320000000
#endif
#ifdef CONFIG_SCG_SOSC_26MHZ
#define SCG_SOSC_FREQ			260000000
#endif
#ifndef SCG_SOSC_FREQ
#define SCG_SOSC_FREQ			0
#endif
#ifdef CONFIG_SCG_ROSC
#define SCG_ROSC_FREQ			32768
#else
#define SCG_ROSC_FREQ			0
#endif

#define scg_clock_valid(scs)		\
	(!!(__raw_readl(SCG_SCSCSR(scs)) & SCG_VLD))
#define scg_clock_selected(scs)		\
	(!!(__raw_readl(SCG_SCSCSR(scs)) & SCG_SEL))
#define scg_clock_locked(scs)		\
	(!!(__raw_readl(SCG_SCSCSR(scs)) & SCG_LK))
#define scg_clock_get_range(scs)	\
	SCG_GET_RANGE(__raw_readl(SCG_SCSCFG(scs)))
#define scg_clock_set_range(scs, range)			\
	__raw_writel_mask(SCG_SET_RANGE(range),		\
			  SCG_SET_RANGE(SCG_RANGE_MASK),\
			  SCG_SCSCFG(scs))
#define scg_clock_set_source(mode, scs)			\
	__raw_writel_mask(SCG_SET_SCS(scs),		\
			  SCG_SET_SCS(SCG_SCS_MASK),	\
			  SCG_MODEREG(mode))
#define scg_clock_get_source(mode)			\
	SCG_GET_SCS(__raw_readl(SCG_MODEREG(mode)))
#define __scg_clock_get_source()			\
	SCG_GET_SCS(__raw_readl(SCG_CSR))

int scg_input_disable(uint8_t scs);
int scg_input_enable(uint8_t scs, uint32_t freq, uint32_t flags);
uint32_t scg_input_get_freq(uint8_t scs);
void scg_input_set_freq(uint8_t scs, uint32_t freq);

void scg_output_disable(uint8_t scs, uint8_t id);
void scg_output_enable(uint8_t scs, uint8_t id, uint8_t div);
uint32_t scg_output_get_freq(uint8_t scs, uint8_t id);
void scg_output_set_freq(uint8_t scs, uint8_t id, uint32_t freq);

void scg_clock_select(uint8_t mode, uint8_t src);
void scg_system_set_freq(uint8_t mode, uint8_t id, uint32_t freq);
uint32_t scg_system_get_freq(uint8_t mode, uint8_t id);

#endif /* __SCG_VEGA_H_INCLUDE__ */
