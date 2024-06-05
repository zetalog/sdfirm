/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)clk.h: K1Matrix specific clock tree definitions
 * $Id: clk.h,v 1.1 2023-09-06 18:54:00 zhenglv Exp $
 */

#ifndef __CLK_K1MATRIX_H_INCLUDE__
#define __CLK_K1MATRIX_H_INCLUDE__

#include <target/arch.h>
#include <asm/mach/crg.h>

#ifdef CONFIG_K1MATRIX_CRG
#ifndef ARCH_HAVE_CLK
#define ARCH_HAVE_CLK		1
#else
#error "Multiple CLK controller defined"
#endif
#endif

#include <dt-bindings/clock/sbi-clock-k1matrix.h>

#define NR_FREQPLANS		5
#define FREQPLAN_RUN		0
#define INVALID_FREQPLAN	(-1)

#define osc_clk			clkid(CLK_INPUT, OSC_CLK)

#define com_pll_foutpostdiv	clkid(CLK_PLL, COM_PLL)
#define mesh_pll_foutpostdiv	clkid(CLK_PLL, MESH_PLL)
#define peri_pll_foutpostdiv	clkid(CLK_PLL, PERI_PLL)
#define ddr0_pll_foutpostdiv	clkid(CLK_PLL, DDR0_PLL)
#define ddr1_pll_foutpostdiv	clkid(CLK_PLL, DDR1_PLL)
#define cpu0_pll_foutpostdiv	clkid(CLK_PLL, CPU0_PLL)
#define cpu1_pll_foutpostdiv	clkid(CLK_PLL, CPU1_PLL)

#define ddr_clksel		clkid(CLK_SEL, DDR_CLKSEL)
#define cpu_clksel		clkid(CLK_SEL, CPU_CLKSEL)

#define ddr_clk			clkid(CLK_DYN, DDR_CLK)
#define cpu_clk			clkid(CLK_DYN, CPU_CLK)

#define clk_freq_t		uint64_t
#define invalid_clk		clkid(0xFF, 0xFF)

void clk_hw_ctrl_init(void);

#endif /* __CLK_K1MATRIX_H_INCLUDE__ */
