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
 * @(#)muldiv.h: RISCV specific multiplication/division interface
 * $Id: muldiv.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __MULDIV_RISCV_H_INCLUDE__
#define __MULDIV_RISCV_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>

#ifdef CONFIG_RISCV_M
static inline uint16_t div16u(uint16_t x, uint16_t y)
{
        uint16_t val;
	asm volatile("divu	%0, %1, %2\n"
		     : "=r"(val) : "r"(x), "r"(y));
	return val;
}
#define ARCH_HAVE_DIV16U 1

static inline uint32_t div32u(uint32_t x, uint32_t y)
{
	uint32_t val;
	asm volatile("divu	%0, %1, %2\n"
		     : "=r"(val) : "r"(x), "r"(y));
	return val;
}
#define ARCH_HAVE_DIV32U 1

#ifdef CONFIG_64BIT
static inline uint64_t div64u(uint64_t x, uint64_t y)
{
	uint64_t val;
	asm volatile("divu	%0, %1, %2\n"
		     : "=r"(val) : "r"(x), "r"(y));
	return val;
}
#define ARCH_HAVE_DIV64U 1
#endif

static inline uint16_t mod16u(uint16_t x, uint16_t y)
{
        uint16_t val;
	asm volatile("remu	%0, %1, %2\n"
		     : "=r"(val) : "r"(x), "r"(y));
        return val;
}
#define ARCH_HAVE_MOD16U 1

static inline uint32_t mod32u(uint32_t x, uint32_t y)
{
        uint32_t val;
	asm volatile("remu	%0, %1, %2\n"
		     : "=r"(val) : "r"(x), "r"(y));
        return val;
}
#define ARCH_HAVE_MOD32U 1

#ifdef CONFIG_64BIT
static inline uint64_t mod64u(uint64_t x, uint64_t y)
{
        uint64_t val;
	asm volatile("remu	%0, %1, %2\n"
		     : "=r"(val) : "r"(x), "r"(y));
        return val;
}
#define ARCH_HAVE_MOD64U 1
#endif

static inline uint32_t mul16u(uint16_t x, uint16_t y)
{
        uint32_t val;
        asm volatile("mul	%0, %1, %2\n"
		     : "=r"(val) : "r"(x), "r"(y));
        return val;
}
#define ARCH_HAVE_MUL16U 1

#ifdef CONFIG_64BIT
static inline uint64_t mul32u(uint32_t x, uint32_t y)
{
        uint32_t val;
        asm volatile("mul	%0, %1, %2\n"
		     : "=r"(val) : "r"(x), "r"(y));
        return val;
}
#define ARCH_HAVE_MUL32U 1

#ifdef HAVE_UINT128_T
static inline uint128_t mul64u(uint64_t x, uint64_t y)
{
        uint64_t lo, hi;
        asm volatile("mul	%0, %1, %2\n"
		     : "=r"(lo) : "r"(x), "r"(y));
        asm volatile("mul	%0, %1, %2\n"
		     : "=r"(hi) : "r"(x), "r"(y));
        return (uint128_t)hi << 64 | lo;
}
#define ARCH_HAVE_MUL64U 1
#endif
#else
static inline uint64_t mul32u(uint32_t x, uint32_t y)
{
        uint32_t lo, hi;
        asm volatile("mul	%0, %1, %2\n"
		     : "=r"(lo) : "r"(x), "r"(y));
        asm volatile("mulhu	%0, %1, %2\n"
		     : "=r"(hi) : "r"(x), "r"(y));
        return (uint64_t)hi << 32 | lo;
}
#define ARCH_HAVE_MUL32U 1
#endif
#endif /* CONFIG_RISCV_M */

#endif /* __MULDIV_RISCV_H_INCLUDE__ */
