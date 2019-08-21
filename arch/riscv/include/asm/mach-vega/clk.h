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
 * @(#)clk.h: RV32M1 (VEGA) specific clock tree defintions
 * $Id: clk.h,v 1.1 2019-08-14 15:59:00 zhenglv Exp $
 */

#ifndef __CLK_VEGA_H_INCLUDE__
#define __CLK_VEGA_H_INCLUDE__

#include <asm/mach/scg.h>
#include <asm/mach/pcc.h>

#define NR_FREQPLANS		3
#define FREQPLAN_RUN		0
#define FREQPLAN_VLPR		1
#define FREQPLAN_HSRUN		2
#define INVALID_FREQPLAN	NR_FREQPLANS

#define invalid_clk		clkid(0xFF, 0xFF)

/* Padring reference clocks */
/* clk_cat_t */
#define CLK_CONST		((clk_cat_t)0)
/* clk_clk_t */
#define LPOSC_CLK		((clk_clk_t)0)
#define NR_CONST_CLKS		(LPOSC_CLK + 1)
#define FREQ_LPOSC_CLK		1000
#define lposc_clk		clkid(CLK_CONST, LPOSC_CLK)

#define CLK_INPUT		((clk_cat_t)1)
#define SOSC_CLK		((clk_clk_t)0)
#define SIRC_CLK		((clk_clk_t)1)
#define FIRC_CLK		((clk_clk_t)2)
#define ROSC_CLK		((clk_clk_t)3)
#define LPFLL_CLK		((clk_clk_t)4)
#define NR_INPUT_CLKS		(LPFLL_CLK + 1)
#define CLK_SCG_SCS(clk)	((clk) + 1)
#define XTAL_CLK		SOSC_CLK
#define XTAL32_CLK		ROSC_CLK
#define RFOSC_CLK		SOSC_CLK
#define RTCOSC_CLK		ROSC_CLK
#define sosc_clk		clkid(CLK_INPUT, SOSC_CLK)
#define sirc_clk		clkid(CLK_INPUT, SIRC_CLK)
#define firc_clk		clkid(CLK_INPUT, FIRC_CLK)
#define rosc_clk		clkid(CLK_INPUT, ROSC_CLK)
#define lpfll_clk		clkid(CLK_INPUT, LPFLL_CLK)
#define xtal_clk		sosc_clk
#define xtal32_clk		rosc_clk
#define rfosc_clk		sosc_clk
#define rtcosc_clk		rosc_clk

#define CLK_SYSTEM		((clk_cat_t)2)
#define SYS_CLK_SRC		((clk_clk_t)0)
#define CLKOUT_CLK		((clk_clk_t)1)
#define NR_SYSTEM_CLKS		(CLKOUT_CLK + 1)
#define CLK_SCG_CCR(clk)	SCG_REG(SCG_RCCR + 4 * (clk))
#define sys_clk_src		clkid(CLK_SYSTEM, SYS_CLK_SRC)
#define clkout_clk		clkid(CLK_SYSTEM, CLKOUT_CLK)

#define CLK_OUTPUT		((clk_cat_t)3)
/* CCR clocks */
#define SLOW_CLK		((clk_clk_t)0)
#define BUS_CLK			((clk_clk_t)1)
#define EXT_CLK			((clk_clk_t)2)
#define CORE_CLK		((clk_clk_t)3)
#define NR_OUTPUT_CCRS		4
#define CLK_CCR_DIVID(clk)	((clk) == CORE_CLK ? 4 : (clk))
/* DIV clocks */
#define SOSCDIV1_CLK		((clk_clk_t)4)
#define SOSCDIV2_CLK		((clk_clk_t)5)
#define SOSCDIV3_CLK		((clk_clk_t)6)
#define SIRCDIV1_CLK		((clk_clk_t)8)
#define SIRCDIV2_CLK		((clk_clk_t)9)
#define SIRCDIV3_CLK		((clk_clk_t)10)
#define FIRCDIV1_CLK		((clk_clk_t)12)
#define FIRCDIV2_CLK		((clk_clk_t)13)
#define FIRCDIV3_CLK		((clk_clk_t)14)
#define LPFLLDIV1_CLK		((clk_clk_t)20)
#define LPFLLDIV2_CLK		((clk_clk_t)21)
#define LPFLLDIV3_CLK		((clk_clk_t)22)
#define NR_OUTPUT_CLKS		23
#define CLK_OUTPUT_VALID	UL(0x0070F77F)
#define CLK_DIV_CLK(clk)	((clk_clk_t)(((clk) >> 2) - 1))
#define CLK_DIV_CLKID(clk)	clkid(CLK_INPUT, CLK_DIV_CLK(clk))
#define CLK_DIV_SCS(clk)	CLK_SCG_SCS(CLK_DIV_CLK(clk))
#define CLK_DIV_DIVID(clk)	((clk) & 0x03)
#define SYS_CLK			CORE_CLK
#define PLAT_CLK		CORE_CLK
#define slow_clk		clkid(CLK_OUTPUT, SLOW_CLK)
#define bus_clk			clkid(CLK_OUTPUT, BUS_CLK)
#define ext_clk			clkid(CLK_OUTPUT, EXT_CLK)
#define core_clk		clkid(CLK_OUTPUT, CORE_CLK)
#define soscdiv1_clk		clkid(CLK_OUTPUT, SOSCDIV1_CLK)
#define soscdiv2_clk		clkid(CLK_OUTPUT, SOSCDIV2_CLK)
#define soscdiv3_clk		clkid(CLK_OUTPUT, SOSCDIV3_CLK)
#define sircdiv1_clk		clkid(CLK_OUTPUT, SIRCDIV1_CLK)
#define sircdiv2_clk		clkid(CLK_OUTPUT, SIRCDIV2_CLK)
#define sircdiv3_clk		clkid(CLK_OUTPUT, SIRCDIV3_CLK)
#define fircdiv1_clk		clkid(CLK_OUTPUT, FIRCDIV1_CLK)
#define fircdiv2_clk		clkid(CLK_OUTPUT, FIRCDIV2_CLK)
#define fircdiv3_clk		clkid(CLK_OUTPUT, FIRCDIV3_CLK)
#define lpflldiv1_clk		clkid(CLK_OUTPUT, LPFLLDIV1_CLK)
#define lpflldiv2_clk		clkid(CLK_OUTPUT, LPFLLDIV2_CLK)
#define lpflldiv3_clk		clkid(CLK_OUTPUT, LPFLLDIV3_CLK)
#define sys_clk			core_clk
#define plat_clk		core_clk

#define CORE_CLK_FREQ_RUN	48000000
#define CORE_CLK_FREQ_VLPR	4000000
#define CORE_CLK_FREQ_HSRUN	72000000

clk_t freqplan_scs2clk(void);
uint8_t freqplan_clk2scs(void);
#ifdef CONFIG_VEGA_BOOT_CPU
void freqplan_config_boot(void);
void freqplan_config_run(void);
void freqplan_config_vlpr(void);
void freqplan_config_hsrun(void);
#else
#define freqplan_config_boot()		do { } while (0)
#define freqplan_config_run()		do { } while (0)
#define freqplan_config_vlpr()		do { } while (0)
#define freqplan_config_hsrun()		do { } while (0)
#endif

int clk_hw_ctrl_init(void);

#endif /* __CLK_VEGA_H_INCLUDE__ */
