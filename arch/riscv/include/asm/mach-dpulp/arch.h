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
 * @(#)arch.h: DPU-LP machine specific definitions
 * $Id: arch.h,v 1.1 2021-11-01 14:28:00 zhenglv Exp $
 */

#ifndef __ARCH_DPULP_H_INCLUDE__
#define __ARCH_DPULP_H_INCLUDE__

#define __VEC
#define __VEC_ALIGN		8

#include <asm/mach/tcsr.h>
#include <asm/mach/flash.h>
#include <asm/vaisra_pma.h>
#ifdef CONFIG_VAISRA_RAS
#include <asm/vaisra_ras.h>
#endif /* CONFIG_VAISRA_RAS */

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
	.macro	boot0_hook
#ifdef CONFIG_DPU_BOOT_CPU
	jal	ra, vaisra_cpu_init
#endif /* CONFIG_DPU_BOOT_CPU */
	.endm
	.macro	boot1_hook
	.endm
	.macro	boot2_hook
#ifdef CONFIG_DPU_BOOT_PMA
	jal	ra, dpulp_pma_cpu_init
#endif /* CONFIG_DPU_BOOT_PMA */
	.endm
#endif /* __ASSEMBLY__ && !__DTS__ && !LINKER_SCRIPT */

#ifndef __ASSEMBLY__
#ifdef CONFIG_CLK
void board_init_clock(void);
#else
#define board_init_clock()	do { } while (0)
#endif
void board_init_timestamp(void);
void dpulp_pma_cpu_init(void);
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_DPULP_H_INCLUDE__ */
