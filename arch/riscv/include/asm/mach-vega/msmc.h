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
 * @(#)msmc.h: RV32M1 (VEGA) MSMC (multi system mode controller) driver
 * $Id: msmc.h,v 1.1 2019-08-21 19:04:00 zhenglv Exp $
 */

#ifndef __MSMC_VEGA_H_INCLUDE__
#define __MSMC_VEGA_H_INCLUDE__

#define SMC0_BASE		UL(0x40020000)
#define SMC1_BASE		UL(0x41020000)
#ifdef CONFIG_VEGA_RI5CY
#define SMC_BASE		SMC0_BASE
#endif
#ifdef CONFIG_VEGA_0RISCY
#define SMC_BASE		SMC1_BASE
#endif
#define SMC_REG(offset)		(SMC_BASE + (offset))

#define SMC_VERID		SMC_REG(0x00)
#define SMC_PARAM		SMC_REG(0x04)
#define SMC_PMPROT		SMC_REG(0x08)
#define SMC_PMCTRL		SMC_REG(0x10)
#define SMC_PMSTAT		SMC_REG(0x18)

/* 32.4.1.4 Power Mode Protection register (PMPROT) */
#define SMC_PM_AHSRUN		_BV(7)
#define SMC_PM_AVLP		_BV(5)
#define SMC_PM_ALLS		_BV(3)
#define SMC_PM_AVLLS_0_1	_BV(0)
#define SMC_PM_AVLLS_2_3	_BV(1)
#define SMC_PM_A_ALL		\
	(SMC_PM_AHSRUN | SMC_PM_AVLP | SMC_PM_ALLS | \
	 SMC_PM_AVLLS_0_1 | SMC_PM_AVLLS_2_3)

/* 32.4.1.5 Power Mode Control register (PMCTRL) */
#define SMC_PM_STOPM_OFFSET	0
#define SMC_PM_STOPM_MASK	REG_3BIT_MASK
#define SMC_PM_STOPM(value)	_SET_FV(SMC_PM_STOPM, value)
#define SMC_PM_STOP_NORM	0
#define SMC_PM_STOP_VLPS	2
#define SMC_PM_STOP_LLS		3
#define SMC_PM_STOP_VLLS_2_3	4
#define SMC_PM_STOP_VLLS_0_1	6
#define SMC_PM_RUNM_OFFSET	8
#define SMC_PM_RUNM_MASK	REG_2BIT_MASK
#define SMC_PM_RUNM(value)	_SET_FV(SMC_PM_RUNM, value)
#define SMC_PM_RUN		0
#define SMC_PM_STOP		1
#define SMC_PM_VLPR		2
#define SMC_PM_HSRUN		3
#define SMC_SYS2PM(sys)		\
	((sys) != SYS_MODE_RUN ? (sys) + 1 : (sys))
#define SMC_PM_PSTOPO_OFFSET	0
#define SMC_PM_PSTOPO_MASK	REG_2BIT_MASK
#define SMC_PM_PSTOPO(value)	_SET_FV(SMC_PM_PSTOPO, value)
/* 00b - STOP - Normal Stop mode
 * 01b - PSTOP1 - Partial Stop with system and bus clock disabled
 * 10b - PSTOP2 - Partial Stop with system clock disabled and bus
 *                clock enabled
 * 11b - PSTOP3 - Partial Stop with system clock enabled and bus
 *                clock enabled
 */

/* 32.4.1.6 Power Mode Status register (PMSTAT) */
#define SMC_PM_PMSTAT_OFFSET	0
#define SMC_PM_PMSTAT_MASK	REG_8BIT_MASK
#define SMC_PM_PMSTAT(value)	_GET_FV(SMC_PM_PMSTAT, value)
#define SMC_PM_STOPSTAT_OFFSET	24
#define SMC_PM_STOPSTAT_MASK	REG_8BIT_MASK
#define SMC_PM_STOPSTAT(value)	_GET_FV(SMC_PM_STOPSTAT, value)

#define smc_allow_power_mode(mode)	__raw_writel(mode, SMC_PMPROT)
#define smc_set_run_mode(mode)					\
	__raw_writel_mask(SMC_PM_RUNM(mode),			\
			  SMC_PM_RUNM(SMC_PM_RUNM_MASK),	\
			  SMC_PMCTRL)
#define smc_get_power_status()		\
	SMC_PM_PMSTAT(__raw_readl(SMC_PMSTAT))

#define smc_power_select(mode)					\
	do {							\
		if (mode != SYS_MODE_RUN) {			\
			smc_allow_power_mode(SMC_PM_A_ALL);	\
			smc_set_run_mode(SMC_SYS2PM(mode));	\
			while (smc_get_power_status() !=	\
			       SMC_SYS2PM(mode));		\
		}						\
	} while (0)

#endif /* __MSMC_VEGA_H_INCLUDE__ */
