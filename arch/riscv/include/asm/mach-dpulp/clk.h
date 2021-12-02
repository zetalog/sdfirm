/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)clk.h: DPU-LP specific clock tree definitions
 * $Id: clk.h,v 1.1 2021-11-01 14:54:00 zhenglv Exp $
 */

#ifndef __CLK_DPULP_H_INCLUDE__
#define __CLK_DPULP_H_INCLUDE__

#include <target/arch.h>
#include <asm/mach/cru.h>

#ifdef CONFIG_DPULP_PLL
#ifndef ARCH_HAVE_CLK
#define ARCH_HAVE_CLK		1
#else
#error "Multiple CLK controller defined"
#endif
#endif

#include <dt-bindings/clock/sbi-clock-dpulp.h>

#define NR_FREQPLANS		5
#define FREQPLAN_RUN		0
#define INVALID_FREQPLAN	(-1)

#define clk_freq_t		uint64_t
#define invalid_clk		clkid(0xFF, 0xFF)

#define CPU_VCO_FREQ		ULL(5000000000) /* 5GHz */
#define GPDPU_BUS_VCO_FREQ	ULL(4800000000)	/* 4.8GHz */
#define GPDPU_CORE_VCO_FREQ	ULL(4800000000)	/* 4.8GHz */
#define DDR_VCO_FREQ		ULL(3200000000) /* 3.2GHz */
#define SOC_VCO_FREQ		ULL(4000000000) /* 4GHz */
#define VPU_B_VCO_FREQ		ULL(4000000000)	/* 4GHz */
#define VPU_C_VCO_FREQ		ULL(4400000000) /* 4.4GHz */
#define RAB_PHY_VCO_FREQ	ULL(2500000000) /* 2.5GHz */
#define ETH_PHY_VCO_FREQ	ULL(2500000000) /* 2.5GHz */

/* Clock flags, used by clk drivers to indicate clock features */
#define CLK_CLKSEL_F		_BV(0)
#define CLK_CLKEN_F		_BV(1)
#define CLK_RESET_F		_BV(2)
/* XXX: CLK_PLL_SEL_F/CLK_DIV_SEL_F:
 *
 * Some select clocks marked by this flag are controlled internally by the
 * PLL clocks.
 */
#define CLK_PLL_SEL_F		_BV(5) /* CLK_SEL for PLL P/R clkout */
#define CLK_DIV_SEL_F		_BV(6) /* CLK_SEL for PLL DIV */
#define CLK_MUX_SEL_F		(CLK_PLL_SEL_F | CLK_DIV_SEL_F)
#define CLK_CR			(CLK_CLKEN_F | CLK_RESET_F)
#define CLK_C			CLK_CLKEN_F
#define CLK_R			CLK_RESET_F

#define CLK_DEC_FLAGS					\
	uint8_t flags;
#define CLK_DEF_FLAGS(__flags)				\
	.flags = __flags,
#define CLK_DEC_FLAGS_RO				\
	uint8_t flags;
#define CLK_DEF_FLAGS_RO(__flags)			\
	.flags = __flags,
#define clk_read_flags(soc, clk)		((clk).flags)
#define clk_write_flags(soc, clk, __flags)	((clk).flags = (__flags))
#define clk_set_flags(soc, clk, __flags)	((clk).flags |= (__flags))
#define clk_clear_flags(soc, clk, __flags)	((clk).flags &= ~(__flags))

/* Enable clock tree core */
void clk_hw_ctrl_init(void);
void clk_apply_vco(clk_clk_t vco, clk_clk_t clk, clk_freq_t freq);
void clk_apply_pll(clk_clk_t pll, clk_clk_t clk, clk_freq_t freq);
/* XXX: Protect Dynamic PLL Change
 *
 * For ASIC environment, CLK_SELECT is managed by the CLK_PLL and CLK_DIV.
 */
#define clk_select_mux(clk)			\
	do {					\
		if (clk != invalid_clk)		\
			clk_enable(clk);	\
	} while (0)
#define clk_deselect_mux(clk)			\
	do {					\
		if (clk != invalid_clk)		\
			clk_disable(clk);	\
	} while (0)

#endif /* __CLK_DPULP_H_INCLUDE__ */
