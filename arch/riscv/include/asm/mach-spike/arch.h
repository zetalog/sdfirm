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
 * @(#)arch.h: SPIKE machine specific definitions
 * $Id: arch.h,v 1.1 2019-09-05 11:26:00 zhenglv Exp $
 */

#ifndef __ARCH_SPIKE_H_INCLUDE__
#define __ARCH_SPIKE_H_INCLUDE__

#include <asm/htif.h>

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

/* Architecture specific bootloader macros */
#if defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
#ifdef CONFIG_SMP
#ifdef CONFIG_SPIKE_BOOT_CPU
/* This mode is used to simulate U54/APC boots where E51/IMC exist */
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
	/* BOOT_HART is 1 to skip hart0 (E51/IMC) */
	.macro get_arch_hartboot reg
	li	\reg, 1
	.endm
#endif /* CONFIG_SPIKE_BOOT_CPU */
#endif /* CONFIG_SMP */
#endif

#endif /* __ARCH_SPIKE_H_INCLUDE__ */
