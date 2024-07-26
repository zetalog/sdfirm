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
 * @(#)arch.h: K1Matrix machine specific definitions
 * $Id: arch.h,v 1.1 2023-09-06 15:19:00 zhenglv Exp $
 */

#ifndef __ARCH_K1MATRIX_H_INCLUDE__
#define __ARCH_K1MATRIX_H_INCLUDE__

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#include <asm/mach/cpus.h>
#ifdef CONFIG_K1MATRIX_APU
#include <asm/x100.h>
#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
	.macro	k1matrix_cpu_smp_init
	x100_smp_init
	.endm
	.macro	k1matrix_cpu_up_init
	x100_init
	.endm
#endif
#endif
#ifdef CONFIG_K1MATRIX_RMU
#include <asm/c910.h>
#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
	.macro	k1matrix_cpu_smp_init
	c910_smp_init
	.endm
	.macro	k1matrix_cpu_up_init
	c910_init
	.endm
#endif
#endif
#include <asm/mach/sysreg.h>
#include <asm/mach/d2d.h> /* D2D ROM registers */

#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
	.macro	boot0_hook
	k1matrix_cpu_up_init
	.endm
	.macro	boot1_hook
	jal	ra, k1matrix_cpu_reset
	.endm
	.macro	boot2_hook
	k1matrix_cpu_smp_init
	.endm

	/* SMP ID <-> HART ID conversions on APC */
	.macro get_arch_smpid reg
	.endm
	/* TODO: Use SYSREG/D2DREG */
#if defined(CONFIG_K1MATRIX_NUMA)
	.macro get_arch_hartboot reg
	li	\reg, BOOT_HART
	.endm
	.macro get_arch_hartmask reg
	li	\reg, BOOT_MASK
	.endm
#else
	.macro get_arch_hartboot reg
	li	\reg, 0x0  //FIXME
//	li	\reg, BOOT_HART  //FIXME
//	li	t0, PAD_IN_STATUS
//	lw	t0, 0(t0)
//	andi	t0, t0, __PAD_DIE_ID
//	beqz	t0, 4444f
//	addi	\reg, \reg, DIE1_HART
//4444:
	.endm
	.macro get_arch_hartmask reg
	li	\reg, 0x1  //FIXME
//	li	\reg, BOOT_MASK
//	li	t0, PAD_IN_STATUS
//	lw	t0, 0(t0)
//	andi	t0, t0, __PAD_DIE_ID
//	beqz	t0, 5555f
//	slli	\reg, \reg, DIE1_HART
//5555:
	.endm
#endif
#define ARCH_HAVE_BOOT_SMP	1
#endif /* __ASSEMBLY__ && !__DTS__ && !LINKER_SCRIPT */

#ifndef __ASSEMBLY__
void board_init_clock(void);

#ifdef CONFIG_MMU
void k1matrix_mmu_init(void);
#else /* CONFIG_MMU */
#define k1matrix_mmu_init()	do { } while (0)
#endif /* CONFIG_MMU */
#ifdef CONFIG_CLK
void k1matrix_clk_init(void);
#else /* CONFIG_CLK */
#define k1matrix_clk_init()	do { } while (0)
#endif /* CONFIG_CLK */
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_K1MATRIX_H_INCLUDE__ */
