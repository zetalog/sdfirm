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
 * @(#)arch.h: DUOWEN machine specific definitions
 * $Id: arch.h,v 1.1 2019-09-02 10:56:00 zhenglv Exp $
 */

#ifndef __ARCH_DUOWEN_H_INCLUDE__
#define __ARCH_DUOWEN_H_INCLUDE__

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#define __VEC

#ifdef CONFIG_DUOWEN_ASIC
#define XO_CLK_FREQ		UL(25000000)
#define SOC_VCO_FREQ		ULL(4000000000) /* 4GHz */
#define DDR_BUS_VCO_FREQ	ULL(3200000000) /* 3.2GHz */
#define DDR_VCO_FREQ		ULL(3200000000) /* 3.2GHz */
#define PCIE_VCO_FREQ		ULL(2800000000) /* 2.8GHz */
#define COHFAB_VCO_FREQ		ULL(4000000000) /* 4GHz */
#define CL_VCO_FREQ		ULL(5000000000) /* 5GHz */
#define SOC_PLL_FREQ		UL(1000000000)
#define DDR_BUS_PLL_FREQ	UL(800000000)
#define DDR_PLL_FREQ		UL(800000000)
#define PCIE_PLL_FREQ		UL(100000000)
#define CFAB_PLL_FREQ		UL(2000000000)
#define CL_PLL_FREQ		UL(2500000000)
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
#endif
#ifdef CONFIG_DUOWEN_ZEBU
#define XO_CLK_FREQ		UL(25000000)
#define SOC_PLL_FREQ		UL(1000000000)
#define DDR_BUS_PLL_FREQ	UL(800000000)
#define DDR_PLL_FREQ		UL(800000000)
#define PCIE_PLL_FREQ		UL(100000000)
#define CFAB_PLL_FREQ		UL(2000000000)
#define CL_PLL_FREQ		UL(2500000000)
#endif

#define APC_CLK_FREQ		CL_PLL_FREQ
#define IMC_CLK_FREQ		SFAB_CLK_FREQ
#define AHB_CLK_FREQ		SFAB_CLK_FREQ
#define APB_CLK_FREQ		SFAB_HALF_CLK_FREQ
#define CFAB_CLK_FREQ		CFAB_PLL_FREQ
#define SOC_HALF_CLK_FREQ	(SOC_PLL_FREQ/2)
#define SFAB_CLK_FREQ		(SOC_PLL_FREQ/4) /* Used by IMC/RAM/ROM/PLIC */
#define SFAB_HALF_CLK_FREQ	(SFAB_CLK_FREQ/2)

#define SD_TM_CLK_FREQ		(SOC_PLL_FREQ/10/100)

#include <asm/mach/scsr.h>
#include <asm/mach/flash.h>
#include <asm/mach/msg.h>

#ifdef CONFIG_VAISRA_PMA
#define VAISRA_PMA_G		19
#include <asm/vaisra_pma.h>
#endif

#ifdef CONFIG_VAISRA_RAS
#include <asm/vaisra_ras.h>
#endif

#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
#ifdef CONFIG_DUOWEN_APC_BOOT_HOOK
	.macro	boot0_hook
#ifdef CONFIG_DUOWEN_APC_INIT
	jal	ra, vaisra_cpu_init
#endif
	.endm
	.macro	boot1_hook
#if defined(CONFIG_DUOWEN_NOC) && defined(CONFIG_DUOWEN_APC)
	jal	ra, duowen_noc_init
#endif
	.endm
	.macro	boot2_hook
#ifdef CONFIG_DUOWEN_PMA
	jal	ra, duowen_pma_init
#endif
	.endm
#endif
#endif

#ifndef __ASSEMBLY__
void board_init_clock(void);
void board_init_timestamp(void);
#ifdef CONFIG_DUOWEN_APC_INIT
void vaisra_cpu_init(void);
#else
#define vaisra_cpu_init()	do { } while (0)
#endif
#ifdef CONFIG_DUOWEN_PMA
void duowen_pma_init(void);
#else
#define duowen_pma_init()	do { } while (0)
#endif
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_DUOWEN_H_INCLUDE__ */
