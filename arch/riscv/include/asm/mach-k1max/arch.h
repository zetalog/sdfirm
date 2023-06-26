/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2022
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
 * @(#)arch.h: K1MAX machine specific definitions
 * $Id: arch.h,v 1.1 2022-10-15 15:19:00 zhenglv Exp $
 */

#ifndef __ARCH_K1MAX_H_INCLUDE__
#define __ARCH_K1MAX_H_INCLUDE__

#include <asm/mach/cpus.h>
#include <asm/x100.h>
#include <asm/mach/sysreg.h>

#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
	.macro	boot0_hook
	x100_init
	.endm
	.macro	boot1_hook
#ifdef CONFIG_K1M_CCI
	jal	ra, k1max_cci_init
#endif
#ifdef CONFIG_K1M_SOC
	jal	ra, k1max_cpu_reset
#endif
	.endm
	.macro	boot2_hook
	x100_smp_init
	.endm
#endif

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#ifndef __ASSEMBLY__
#ifdef CONFIG_MMU
void k1max_mmu_init(void);
#else /* CONFIG_MMU */
#define k1max_mmu_init()	do { } while (0)
#endif /* CONFIG_MMU */
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_K1MAX_H_INCLUDE__ */
