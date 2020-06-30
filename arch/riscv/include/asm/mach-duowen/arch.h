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
#define SOC_PLL_FREQ		UL(1000000000)
#define DDR_PLL_FREQ		UL(800000000)
#define CL_PLL_FREQ		UL(2000000000)
#define CFAB_PLL_FREQ		UL(1800000000)
#endif
#ifdef CONFIG_DUOWEN_FPGA
#define XO_CLK_FREQ		UL(5000000)
#define SOC_PLL_FREQ		UL(5000000)
#define DDR_PLL_FREQ		UL(5000000)
#define CL_PLL_FREQ		UL(5000000)
#define CFAB_PLL_FREQ		UL(5000000)
#endif

#define APC_CLK_FREQ		CL_PLL_FREQ
#define IMC_CLK_FREQ		SFAB_CLK_FREQ
#define AHB_CLK_FREQ		SFAB_CLK_FREQ
#define APB_CLK_FREQ		SFAB_HALF_CLK_FREQ

#define CFAB_CLK_FREQ		CFAB_PLL_FREQ
#define SFAB_CLK_FREQ		(SOC_PLL_FREQ/4) /* Used by IMC/RAM/ROM */
#define SFAB_HALF_CLK_FREQ	(SFAB_CLK_FREQ/2)

#define SD_TM_CLK_FREQ		(SOC_PLL_FREQ/10/100)

#include <asm/mach/imc.h>
#include <asm/mach/flash.h>

#ifndef __ASSEMBLY__
void board_init_clock(void);
void board_init_timestamp(void);
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_DUOWEN_H_INCLUDE__ */
