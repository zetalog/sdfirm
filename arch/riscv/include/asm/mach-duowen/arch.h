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

#include <asm/mach/scsr.h>
#include <asm/mach/flash.h>
#include <asm/mach/msg.h>

#if defined(CONFIG_DUOWEN_SCSR) || defined(CONFIG_VAISRA_PMA)
#define VAISRA_PMA_G		19
#include <asm/vaisra_pma.h>
#endif

#ifdef CONFIG_VAISRA_RAS
#include <asm/vaisra_ras.h>
#endif

#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
	.macro	boot0_hook
#ifdef CONFIG_DUOWEN_APC_BOOT_HOOK
#ifdef CONFIG_DUOWEN_LOAD_SRAM
	jal	ra, duowen_load_sram
#endif
#ifdef CONFIG_DUOWEN_APC_INIT
	jal	ra, vaisra_cpu_init
#endif
#endif
	.endm
	.macro	boot1_hook
	jal	ra, duowen_dual_init
#ifdef CONFIG_DUOWEN_APC_BOOT_HOOK
#ifdef CONFIG_DUOWEN_PMA
	jal	ra, duowen_pma_soc_init
#endif
#if defined(CONFIG_DUOWEN_NOC) && defined(CONFIG_DUOWEN_APC)
	jal	ra, duowen_noc_init
#endif
#endif
	.endm
	.macro	boot2_hook
#ifdef CONFIG_DUOWEN_APC_BOOT_HOOK
#ifdef CONFIG_DUOWEN_PMA
	jal	ra, duowen_pma_cpu_init
#endif
#endif
	.endm
#ifdef CONFIG_DUOWEN_SOC_DUAL
#ifdef CONFIG_DUOWEN_BBL
	.macro get_arch_smpid reg
	andi	t0, \reg, 0xF0
	andi	\reg, \reg, 0xF
	beqz	t0, 2222f
	addi	\reg, \reg, (MAX_CPU_NUM / 2)
2222:
	.endm
	.macro get_arch_hartboot reg
	li	\reg, SOC0_HART
	.endm
	.macro get_arch_hartmask reg
	li	\reg, HART_ALL
	.endm
#else /* CONFIG_DUOWEN_BBL */
	.macro get_arch_smpid reg
	li	t1, SOC0_HART
	li	t0, SCSR_SOCKET_ID
	lw	t0, 0(t0)
	andi	t0, t0, 2
	beqz	t0, 3333f
	addi	t1, t1, SOC1_HART
3333:
	sub	\reg, \reg, t1
	.endm
	.macro get_arch_hartboot reg
	li	\reg, SOC0_HART
	li	t0, SCSR_SOCKET_ID
	lw	t0, 0(t0)
	andi	t0, t0, 2
	beqz	t0, 4444f
	addi	\reg, \reg, SOC1_HART
4444:
	.endm
	.macro get_arch_hartmask reg
	li	\reg, CPU_ALL
	li	t0, SCSR_SOCKET_ID
	lw	t0, 0(t0)
	andi	t0, t0, 2
	beqz	t0, 5555f
	slli	\reg, \reg, SOC1_HART
5555:
	.endm
#endif /* CONFIG_DUOWEN_BBL */
#endif /* CONFIG_DUOWEN_SOC_DUAL */
#endif

#ifndef __ASSEMBLY__
void duowen_dual_init(void);
void duowen_pll_init(void);
#ifdef CONFIG_MMU
void duowen_mmu_init(void);
#else
#define duowen_mmu_init()	do { } while (0)
#endif
void board_init_clock(void);
void board_init_timestamp(void);
#ifdef CONFIG_DUOWEN_APC_INIT
void vaisra_cpu_init(void);
#else
#define vaisra_cpu_init()	do { } while (0)
#endif
void duowen_smmu_init(void);
#ifdef CONFIG_DUOWEN_PMA
void duowen_pma_cpu_init(void);
void duowen_pma_soc_init(void);
#else
#define duowen_pma_cpu_init()	do { } while (0)
#define duowen_pma_soc_init()	do { } while (0)
#endif
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_DUOWEN_H_INCLUDE__ */
