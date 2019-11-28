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
 * @(#)counters.h: RISCV counters architecture extension interface
 * $Id: counters.h,v 1.1 2019-08-14 09:42:00 zhenglv Exp $
 */

#ifndef __COUNTERS_RISCV_H_INCLUDE__
#define __COUNTERS_RISCV_H_INCLUDE__

#include <target/types.h>

/* This file uses hardware specification naming - RISCV counters extension,
 * can be used in the tsc/pmu implementation.
 */

#ifdef CONFIG_RISCV_COUNTERS
#ifdef CONFIG_64BIT
static inline uint64_t rdtime(void)
{
	uint64_t n;

	asm volatile("rdtime	%0" : "=r" (n));
	return n;
}

static inline uint64_t rdcycle(void)
{
	uint64_t n;

	asm volatile("rdcycle	%0" : "=r" (n));
	return n;
}

static inline uint64_t rdinstret(void)
{
	uint64_t n;

	asm volatile("rdinstret	%0" : "=r" (n));
	return n;
}
#else
static inline uint64_t rdtime(void)
{
	uint32_t hi, lo, tmp;

        asm volatile(
		"1:\n"
		"rdtimeh	%0\n"
		"rdtime		%1\n"
		"rdtimeh	%2\n"
		"bne		%0, %2, 1b"
		: "=&r" (hi), "=&r" (lo), "=&r" (tmp));
	return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdcycle(void)
{
	uint32_t hi, lo, tmp;

        asm volatile(
		"1:\n"
		"rdcycleh	%0\n"
		"rdcycle	%1\n"
		"rdcycleh	%2\n"
		"bne		%0, %2, 1b"
		: "=&r" (hi), "=&r" (lo), "=&r" (tmp));
	return ((uint64_t)hi << 32) | lo;
}

static inline uint64_t rdinstret(void)
{
	uint32_t hi, lo, tmp;

        asm volatile(
		"1:\n"
		"rdinstreth	%0\n"
		"rdinstret	%1\n"
		"rdinstreth	%2\n"
		"bne		%0, %2, 1b"
		: "=&r" (hi), "=&r" (lo), "=&r" (tmp));
	return ((uint64_t)hi << 32) | lo;
}
#endif
#endif /* CONFIG_RISCV_COUNTERS */

#endif /* __COUNTERS_RISCV_H_INCLUDE__ */
