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
 * @(#)clk.h: DUOWEN clock controller definitions
 * $Id: clk.h,v 1.1 2019-11-06 10:36:00 zhenglv Exp $
 */

#ifndef __CLK_DUOWEN_H_INCLUDE__
#define __CLK_DUOWEN_H_INCLUDE__

#include <target/arch.h>
#include <asm/mach/crcntl.h>

#ifdef CONFIG_CRCNTL
#ifndef ARCH_HAVE_CLK
#define ARCH_HAVE_CLK		1
#else
#error "Multiple CLK controller defined"
#endif
#endif

#define NR_FREQPLANS		1
#define FREQPLAN_RUN		0
#define INVALID_FREQPLAN	NR_FREQPLANS

#define clk_freq_t		uint64_t
#define invalid_clk		clkid(0xFF, 0xFF)

#ifdef CONFIG_DUOWEN_ASIC
#define XO_CLK_FREQ		UL(25000000)
#define SOC_VCO_FREQ		ULL(4000000000) /* 4GHz */
#define DDR_BUS_VCO_FREQ	ULL(3200000000) /* 3.2GHz */
#define DDR_VCO_FREQ		ULL(3200000000) /* 3.2GHz */
#define PCIE_VCO_FREQ		ULL(2800000000) /* 2.8GHz */
#define COHFAB_VCO_FREQ		ULL(4000000000) /* 4GHz */
#define CL_VCO_FREQ		ULL(5000000000) /* 5GHz */
#define ETH_VCO_FREQ		ULL(2500000000) /* 2.5GHz */
#define SOC_PLL_FREQ		UL(1000000000)
#define DDR_BUS_PLL_FREQ	UL(800000000)
#define DDR_PLL_FREQ		UL(800000000)
#define PCIE_PLL_FREQ		UL(100000000)
#define CFAB_PLL_FREQ		UL(2000000000)
#define CL_PLL_FREQ		UL(2500000000)
#define ETH_PLL_FREQ		UL(156250000) /* 156.25MHz */
#endif
#ifdef CONFIG_DUOWEN_FPGA
#define FPGA_CLK_FREQ		UL(10000000) /* 10MHz */
#define XO_CLK_FREQ		FPGA_CLK_FREQ
#define SOC_PLL_FREQ		FPGA_CLK_FREQ
#define DDR_BUS_PLL_FREQ	FPGA_CLK_FREQ
#define DDR_PLL_FREQ		FPGA_CLK_FREQ
#define PCIE_PLL_FREQ		FPGA_CLK_FREQ
#define CFAB_PLL_FREQ		FPGA_CLK_FREQ
#define CL_PLL_FREQ		FPGA_CLK_FREQ
#define ETH_PLL_FREQ		FPGA_CLK_FREQ
#endif
#ifdef CONFIG_DUOWEN_ZEBU
#ifdef CONFIG_DUOWEN_XO_25M
#define XO_CLK_FREQ		UL(25000000)
#endif
#ifdef CONFIG_DUOWEN_XO_125M
#define XO_CLK_FREQ		UL(125000000)
#endif
#define SOC_PLL_FREQ		UL(1000000000)
#define DDR_BUS_PLL_FREQ	UL(800000000)
#define DDR_PLL_FREQ		UL(800000000)
#define PCIE_PLL_FREQ		UL(100000000)
#define CFAB_PLL_FREQ		UL(2000000000)
#define CL_PLL_FREQ		UL(2500000000)
#define ETH_PLL_FREQ		UL(156250000) /* 156.25MHz */
#endif

#define APC_CLK_FREQ		CL_PLL_FREQ
#define AHB_CLK_FREQ		SFAB_CLK_FREQ
#define CFAB_CLK_FREQ		CFAB_PLL_FREQ

#ifdef CONFIG_DUOWEN_SOCv1
#define IMC_CLK_FREQ		SFAB_CLK_FREQ
#define APB_CLK_FREQ		SFAB_HALF_CLK_FREQ
#define SOC_HALF_CLK_FREQ	(SOC_PLL_FREQ/2)
#define SFAB_CLK_FREQ		(SOC_PLL_FREQ/4) /* Used by IMC/RAM/ROM/PLIC */
#define SFAB_HALF_CLK_FREQ	(SFAB_CLK_FREQ/2)
#define SD_TM_CLK_FREQ		(SOC_PLL_FREQ/10/100)
#include <asm/mach/clk_socv1.h>
#endif
#ifdef CONFIG_DUOWEN_SOCv2
#define SFAB_PLL_FREQ		UL(100000000)
#define APB_CLK_FREQ		SFAB_CLK_FREQ
#define SOC_CLK_FREQ		SOC_PLL_FREQ
#define IMC_CLK_FREQ		(SOC_CLK_FREQ/2) /* 500MHz IMC/RAM/ROM */
#define SFAB_CLK_FREQ		SFAB_PLL_FREQ /* 100MHz SFAB peripherals */
#include <asm/mach/clk_socv2.h>
#endif

/* Clock flags, used by clk drivers to indicate clock features */
#define CLK_CLK_SEL_F		_BV(0)
#define CLK_CLK_EN_F		_BV(1)
#define CLK_SW_RST_F		_BV(2)
#define CLK_COHFAB_CFG_F	_BV(3) /* cohfab/cluster clock/reset */
#define CLK_CLUSTER_CFG_F	_BV(4) /* APC internal clock/reset */
#define CLK_REVERSE_DEP_F	_BV(5)
#define CLK_HOMOLOG_SRC_F	_BV(6)
#define CLK_CR			(CLK_CLK_EN_F | CLK_SW_RST_F)
#define CLK_C			CLK_CLK_EN_F
#define CLK_R			CLK_SW_RST_F

#ifdef CONFIG_CONSOLE_COMMAND
void clk_pll_dump(void);
#else
#define clk_pll_dump()				do { } while (0)
#endif
void clk_pll_init(void);

/* Enable clock tree core */
void clk_hw_ctrl_init(void);
#ifdef CONFIG_MMU
void clk_hw_mmu_init(void);
#endif
#ifdef CONFIG_DUOWEN_ASIC
void clk_apply_vco(clk_clk_t clk, clk_freq_t freq);
void clk_apply_pll(clk_clk_t clk, clk_freq_t freq);
#else
#define clk_apply_vco(clk, freq)		do { } while (0)
#define clk_apply_pll(clk, freq)		do { } while (0)
#endif

void duowen_clk_init(void);
void duowen_clk_imc_init(void);
void duowen_clk_apc_init(void);

#endif /* __CLK_DUOWEN_H_INCLUDE__ */
