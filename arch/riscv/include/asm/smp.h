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
 * @(#)smp.h: RISCV specific symmetric multi-processing interfaces
 * $Id: smp.h,v 1.1 2019-08-14 14:01:00 zhenglv Exp $
 */

#ifndef __RISCV_SMP_H_INCLUDE__
#define __RISCV_SMP_H_INCLUDE__

#ifdef CONFIG_SMP
#ifndef __ASSEMBLY__
static inline uint8_t __smp_processor_id(void)
{
	unsigned int t;

	asm volatile ("add	%0, sp, zero\n" : "=r" (t));
	t -= (ABI_PERCPU_STACKS_START + 1);
	return (uint8_t)(t >> PERCPU_STACK_SHIFT);
}

static inline uintptr_t __smp_processor_stack_top(void)
{
	uintptr_t t;

	asm volatile ("add	%0, sp, zero\n" : "=r" (t));
	return ALIGN(t, PERCPU_STACK_SIZE);
}

cpu_t smp_hw_cpu_id(void);
void smp_hw_cpu_boot(void);
void smp_hw_cpu_on(cpu_t cpu, caddr_t ep);
#endif
#endif

#endif /* __RISCV_SMP_H_INCLUDE__ */
