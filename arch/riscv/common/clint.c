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
 * @(#)clint.c: Core local interruptor (CLINT) implementation
 * $Id: clint.c,v 1.1 2019-09-05 18:04:00 zhenglv Exp $
 */

#include <target/tsc.h>
#include <target/bitops.h>
#include <target/irq.h>
#include <target/smp.h>
#include <target/atomic.h>

#ifdef CONFIG_CLINT_FORCE_FAST_TIMEOUT
#define FAST_TIMEOUT_DIFF  0x08
#endif

#if __riscv_xlen == 64
#ifdef CONFIG_ARCH_IS_MMIO_32BIT
uint64_t clint_read_mtime(void)
{
	caddr_t a = CLINT_MTIME;
	uint32_t hi, lo, tmp;

	do {
		hi = __raw_readl((caddr_t)((uintptr_t)(a) + 4));
		lo = __raw_readl(a);
		tmp = __raw_readl((caddr_t)((uintptr_t)(a) + 4));
	} while (hi != tmp);
	return (uint64_t)hi << 32 | lo;
}

void clint_write_mtimecmp(cpu_t cpu, uint64_t cmp)
{
	caddr_t a = CLINT_MTIMECMP(smp_hw_cpu_hart(cpu));

	__raw_writel(HIDWORD(cmp), (caddr_t)((uintptr_t)(a) + 4));
	__raw_writel(LODWORD(cmp), (a));
}
#else
uint64_t clint_read_mtime(void)
{
	return __raw_readq(CLINT_MTIME);
}

void clint_write_mtimecmp(cpu_t cpu, uint64_t cmp)
{
	__raw_writeq(cmp, CLINT_MTIMECMP(smp_hw_cpu_hart(cpu)));
}
#endif

void clint_set_mtimecmp(cpu_t cpu, uint64_t cmp)
{
#ifdef CONFIG_CLINT_FORCE_FAST_TIMEOUT
	uint64_t val = clint_read_mtime();
	cmp = val + FAST_TIMEOUT_DIFF;
#endif
	clint_write_mtimecmp(cpu, cmp);
}

void clint_unset_mtimecmp(cpu_t cpu)
{
	__raw_writeq(ULL(-1), CLINT_MTIMECMP(smp_hw_cpu_hart(cpu)));
}
#else
uint64_t clint_read_mtime(void)
{
	uint32_t hi1, hi2;
	uint32_t lo;

	do {
	     	hi1 = __raw_readl(CLINT_MTIME + 4);
		lo = __raw_readl(CLINT_MTIME);
		hi2 = __raw_readl(CLINT_MTIME + 4);
	} while (hi1 != hi2);
	return MAKELLONG(lo, hi1);
}

void clint_set_mtimecmp(cpu_t cpu, uint64_t cmp)
{
#ifdef CONFIG_CLINT_FORCE_FAST_TIMEOUT
	uint64_t val;
	uint32_t hi;
	uint32_t lo;
	hi = __raw_readl(CLINT_MTIME + 4);
	lo = __raw_readl(CLINT_MTIME);
	val = MAKELLONG(lo, hi);
	cmp = val + FAST_TIMEOUT_DIFF;
#endif
	__raw_writel(LODWORD(cmp), CLINT_MTIMECMP(smp_hw_cpu_hart(cpu)));
	__raw_writel(HIDWORD(cmp), CLINT_MTIMECMP(smp_hw_cpu_hart(cpu)) + 4);
}

void clint_unset_mtimecmp(cpu_t cpu)
{
	__raw_writel(UL(-1), CLINT_MTIMECMP(smp_hw_cpu_hart(cpu)));
	__raw_writel(UL(-1), CLINT_MTIMECMP(smp_hw_cpu_hart(cpu)) + 4);
}
#endif

#if defined(CONFIG_SBI) && defined(CONFIG_SMP)
#if !defined(CONFIG_CLINT_XCHG_LLSC) && defined(CONFIG_RISCV_A)
#define clint_xchg(ptr, val)		xchg(ptr, val)
#if 0
static uint32_t clint_xchg(volatile uint32_t *ptr, uint32_t newval)
{
	/* The name of GCC built-in macro __sync_lock_test_and_set()
	 * is misleading. A more appropriate name for GCC built-in
	 * macro would be __sync_val_exchange().
	 */
	return __sync_lock_test_and_set(ptr, newval);
}
#endif
#else
static uint32_t clint_xchg(volatile uint32_t *ptr, uint32_t newval)
{
	uint32_t ret;
	register uint32_t rc;

	asm volatile("0:lr.w	%0, %2\n"
		     "	sc.w.rl	%1, %z3, %2\n"
		     "	bnez	%1, 0b\n"
		     "	fence	rw, rw\n"
		     : "=&r"(ret), "=&r"(rc), "+A"(*ptr)
		     : "rJ"(newval)
		     : "memory");
	return ret;
}
#endif

void clint_sync_ipi(cpu_t cpu)
{
	uint32_t ipi, incoming_ipi;
	cpu_t src_cpu = sbi_processor_id();

	incoming_ipi = 0;
	while (1) {
		ipi = __raw_readl(CLINT_MSIP(smp_hw_cpu_hart(cpu)));
		if (!ipi)
			break;
		incoming_ipi |= clint_xchg((uint32_t *)(caddr_t)
			CLINT_MSIP(smp_hw_cpu_hart(cpu)), 0);
	}
	if (incoming_ipi)
		__raw_writel(incoming_ipi,
			CLINT_MSIP(smp_hw_cpu_hart(src_cpu)));
}
#endif
