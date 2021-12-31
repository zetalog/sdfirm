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
 * @(#)vaisra_cache.h: VAISRA specific dcache interface
 * $Id: vaisra_cache.h,v 1.1 2020-09-05 08:34:00 zhenglv Exp $
 */

#ifndef __VAISRA_CACHE_H_INCLUDE__
#define __VAISRA_CACHE_H_INCLUDE__

#include <target/generic.h>

#ifdef CONFIG_CPU_VAISRA
#define CSR_SSYSCFG		0x5DE /* control guest/user */
#define CSR_HSYSCFG		0x6C0 /* control host/user */
#define CSR_MSYSCFG		0x7E6 /* control machine mode */

#define SCR_DC			_BV(2)	/* Dcache on/off */
#define SCR_IC			_BV(12)	/* Icache on/off */

#ifdef CONFIG_RISCV_EXIT_M
#define CSR_SYSCFG		CSR_MSYSCFG
#endif
#ifdef CONFIG_RISCV_EXIT_S
#define CSR_SYSCFG		CSR_SSYSCFG
#endif

#if !defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
#define vaisra_cache_off()	csr_clear(CSR_SYSCFG, SCR_DC | SCR_IC)
#define vaisra_cache_on()	csr_set(CSR_SYSCFG, SCR_DC | SCR_IC)

/* Data Cache maintenance instructions */
/* Data cache maintenance instructions use vendor specific encodings as
 * there is no CMO standard ratified when vaisra is developed.
 * 31        20        15       0
 * +---------+---------+--------+
 * | funct12 | opcode5 | func15 |
 * +---------+---------+--------+
 * opcode: register ID, 0=x0, 1=x1, ..., 31=x31
 */
#define INSN_DCACHE_FLUSH_ALL		0x9C70802B

#define INSN_DCACHE_FLUSH_ADDR(reg)	(0x9C30002B | ((reg) << 15))
#define INSN_ICACHE_FLUSH_ADDR(reg)	(0x9C90002B | ((reg) << 15))

#define vaisra_flush_dcache_all()	\
	asm volatile(".word %0" : : "i" (INSN_DCACHE_FLUSH_ALL))

static __inline void vaisra_flush_dcache_addr(void *addr)
{
	__unused register uint64_t a0 asm ("x10") = (uint64_t)addr;

	asm volatile (
		"	add	%0, %0, 0\n"
		"	.word	%1"
		: "=r" (a0) : "i" (INSN_DCACHE_FLUSH_ADDR(10)) : "memory");
}

static __inline void vaisra_flush_icache_addr(void *addr)
{
	__unused register uint64_t a0 asm ("x10") = (uint64_t)addr;

	asm volatile (
		"	add	%0, %0, 0\n"
		"	.word	%1"
		: "=r" (a0) : "i" (INSN_ICACHE_FLUSH_ADDR(10)) : "memory");
}
#endif
#else
#define vaisra_cache_off()		do { } while (0)
#define vaisra_cache_on()		do { } while (0)
#define vaisra_flush_dcache_all()	do { } while (0)
#define vaisra_flush_dcache_addr(addr)	do { } while (0)
#define vaisra_flush_icache_addr(addr)	do { } while (0)
#endif

#endif /* __VAISRA_CACHE_H_INCLUDE__ */
