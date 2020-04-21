/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)arch.h: DPU machine specific definitions
 * $Id: arch.h,v 1.1 2020-03-02 16:11:00 zhenglv Exp $
 */

#ifndef __ARCH_DPU_H_INCLUDE__
#define __ARCH_DPU_H_INCLUDE__

#define ARCH_HAVE_INT_IRQS	1
#define __VEC

#include <asm/mach/tcsr.h>
#include <asm/mach/flash.h>
#include <asm/mach/pe.h>
#include <asm/mach/apc.h>

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#define XIN_FREQ		UL(25000000)	/* 25MHz */
#define PCIE_PHY_CLK_FREQ	UL(25000000)	/* 25MHz */
#define PLL0_VCO_FREQ		ULL(3200000000)	/* 3.2GHz */
#define PLL1_VCO_FREQ		ULL(4000000000)	/* 4GHz */
#define PLL2_VCO_FREQ		ULL(3200000000)	/* 3.2GHz */
#define PLL3_VCO_FREQ		ULL(3200000000)	/* 3.2GHz */
#define PLL4_VCO_FREQ		ULL(4000000000)	/* 4GHz */
#define PLL5_VCO_FREQ		ULL(2800000000) /* 2.8GHz */
#define PLL0_P_FREQ		UL(800000000)	/* 800MHz */
#define PLL1_P_FREQ		UL(1000000000)	/* 1GHz */
#define PLL2_P_FREQ		UL(800000000)	/* 800MHz */
#define PLL2_R_FREQ		UL(50000000)	/* 50MHz */
#define PLL3_P_FREQ		UL(800000000)	/* 800MHz */
#define PLL3_R_FREQ		UL(100000000)	/* 100MHz */
#define PLL4_P_FREQ		UL(2000000000)	/* 2GHz */
#define PLL5_P_FREQ		UL(100000000)	/* 100MHz */
#define IMC_CLK_FREQ		PLL0_P_FREQ
#define PE_CLK_FREQ		PLL1_P_FREQ
#define AXI_CLK_FREQ		PLL3_P_FREQ
#define APB_CLK_FREQ		PLL3_R_FREQ
#define CPU_CLK_FREQ		PLL4_P_FREQ
#define PCIE_REF_CLK_FREQ	PLL5_P_FREQ

#ifndef __ASSEMBLY__
#ifdef CONFIG_CLK
void board_init_clock(void);
#else
#define board_init_clock()	do { } while (0)
#endif
void board_init_timestamp(void);
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_DPU_H_INCLUDE__ */
