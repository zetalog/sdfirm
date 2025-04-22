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
 * @(#)clk.h: FU540 (unleashed) specific clock tree defintions
 * $Id: clk.h,v 1.1 2019-10-16 14:06:00 zhenglv Exp $
 */

#ifndef __CLK_UNLEASHED_H_INCLUDE__
#define __CLK_UNLEASHED_H_INCLUDE__

#include <target/arch.h>

#define ARCH_HAVE_CLK_TYPES	1
typedef uint16_t clk_t;
typedef uint8_t clk_cat_t;
typedef uint8_t clk_clk_t;
#define clkid(cat, clk)		((clk_t)MAKEWORD(clk, cat))
#define invalid_clk		clkid(0xFF, 0xFF)
#define clk_clk(clkid)		LOBYTE(clkid)
#define clk_cat(clkid)		HIBYTE(clkid)

#include <asm/mach/prci.h>

#ifdef CONFIG_PRCI
#ifndef ARCH_HAVE_CLK
#define ARCH_HAVE_CLK		1
#else
#error "Multiple CLK controller defined"
#endif
#endif

#define NR_FREQPLANS		1
#define FREQPLAN_RUN		0
#define INVALID_FREQPLAN	NR_FREQPLANS

#define FREQ_33MHZ		UL(33333333)
#define HFCLK_FREQ		FREQ_33MHZ
#ifdef CONFIG_UNLEASHED_CORECLK_FREQ_10GHZ
#define CORECLK_FREQ		UL(1000000000)
#endif
#ifdef CONFIG_UNLEASHED_CORECLK_FREQ_15GHZ
#define CORECLK_FREQ		UL(1500000000)
#endif
#define TLCLK_FREQ_FINAL	(CORECLK_FREQ/2)
#define RTCCLK_FREQ		UL(1000000)
#define DDRCTRLCLK_FREQ		UL(933333333)
#define GEMGXLCLK_FREQ		UL(125000000)

#define I2C_FREQ		UL(400000)
#define SPI_HW_FREQ		UL(50000000)

#define CLK_INPUT		((clk_cat_t)0)
#define HFCLK			((clk_clk_t)0)
#define RTCCLK			((clk_clk_t)1)
#define NR_INPUT_CLKS		(RTCCLK + 1)
#define hfclk			clkid(CLK_INPUT, HFCLK)
#define rtcclk			clkid(CLK_INPUT, RTCCLK)

#define CLK_PLL			((clk_cat_t)1)
#define COREPLL			((clk_clk_t)PRCI_COREPLL)
#define DDRPLL			((clk_clk_t)PRCI_DDRPLL)
#define GEMGXLPLL		((clk_clk_t)PRCI_GEMGXLPLL)
#define NR_PLL_CLKS		(GEMGXLPLL + 1)
#define corepll			clkid(CLK_PLL, COREPLL)
#define ddrpll			clkid(CLK_PLL, DDRPLL)
#define gemgxlpll		clkid(CLK_PLL, GEMGXLPLL)
#define ddrctrlclk		ddrpll
#define gemgxlclk		gemgxlpll

#define CLK_OUTPUT		((clk_cat_t)2)
#define CORECLK			((clk_clk_t)0)
#define TLCLK			((clk_clk_t)1)
#define NR_OUTPUT_CLKS		(TLCLK + 1)
#define coreclk			clkid(CLK_OUTPUT, CORECLK)
#define tlclk			clkid(CLK_OUTPUT, TLCLK)

void clk_hw_ctrl_init(void);
#ifdef CONFIG_MMU
#define clk_hw_mmu_init()	sifive_mmu_map_prci()
#endif

#endif /* __CLK_UNLEASHED_H_INCLUDE__ */
