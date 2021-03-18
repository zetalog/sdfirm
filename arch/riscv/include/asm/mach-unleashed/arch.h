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
 * @(#)arch.h: FU540 (unleashed) machine specific definitions
 * $Id: arch.h,v 1.1 2019-10-16 10:02:00 zhenglv Exp $
 */

#ifndef __ARCH_UNLEASHED_H_INCLUDE__
#define __ARCH_UNLEASHED_H_INCLUDE__

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#include <asm/mach/pwm.h>
#include <asm/mach/msel.h>

#if defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
#ifdef CONFIG_UNLEASHED_U54
	.macro get_arch_smpid reg
	beqz	\reg, 7770f
	addi	\reg, \reg, -1
	j	7771f
7770:
	li	\reg, 4
7771:
	.endm
	.macro get_arch_hartmask reg
	li	\reg, HART_ALL
	.endm
	/* Boot hart is the first U54 CPU */
	.macro get_arch_hartboot reg
	li	\reg, 1
	.endm
#endif /* CONFIG_UNLEASHED_U54 */
#endif

#ifndef __ASSEMBLY__
void board_init_clock(void);

#ifdef CONFIG_UNLEASHED_SPINOR
int board_spinor_init(uint8_t spi);
#else
#define board_spinor_init(spi)		0
#endif
#ifdef CONFIG_SIFIVE_DDR
void board_ddr_init(void);
#else
#define board_ddr_init()		do { } while (0)
#endif
#ifdef CONFIG_SIFIVE_CACHE
void board_cache_init(void);
#else
#define board_cache_init()		do { } while (0)
#endif

#ifdef CONFIG_MMU
void sifive_mmu_map_prci(void);
void sifive_mmu_map_gpio(void);
void sifive_mmu_map_uart(int n);
void sifive_mmu_dump_maps(void);
#endif
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_UNLEASHED_H_INCLUDE__ */
