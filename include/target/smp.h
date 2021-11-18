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
 * @(#)smp.h: symmetric multi-processing interfaces
 * $Id: smp.h,v 1.0 2019-12-18 15:33:00 zhenglv Exp $
 */

#ifndef __SMP_H_INCLUDE__
#define __SMP_H_INCLUDE__

#include <target/generic.h>
#include <target/arch.h>

#ifdef CONFIG_SMP
#define smp_processor_id()		__smp_processor_id()
#define NR_CPUS				MAX_CPU_NUM
#else
#define smp_processor_id()		0
#define NR_CPUS				1
#endif

#ifndef __ASSEMBLY__
#if NR_CPUS > 65535
typedef uint32_t cpu_t;
#elif NR_CPUS > 255
typedef uint16_t cpu_t;
#else
typedef uint8_t cpu_t;
#endif
#endif /* __ASSEMBLY__ */

#include <asm/smp.h>

#ifdef CONFIG_SMP
#define SMP_CACHE_BYTES			__SMP_CACHE_BYTES
#ifndef __ASSEMBLY__
typedef struct cpu_mask { DECLARE_BITMAP(bits, NR_CPUS); } cpu_mask_t;

extern cpu_t smp_boot_cpu;
extern cpu_mask_t smp_online_cpus;
extern bool smp_initialized;

void smp_boot_secondary_cpus(caddr_t context);

#define cpumask_bits(maskp)		((maskp)->bits)
#define cpumask_set_cpu(cpu, maskp)	set_bit((cpu), cpumask_bits(maskp))
#define cpumask_clear_cpu(cpu, maskp)	clear_bit((cpu), cpumask_bits(maskp))
#define cpumask_test_cpu(cpu, maskp)	test_bit((cpu), cpumask_bits(maskp))

#define cpumask_first(srcp)		find_first_set_bit(cpumask_bits(srcp), NR_CPUS)
#define cpumask_next(n, srcp)		find_next_set_bit(cpumask_bits(srcp), NR_CPUS, (n)+ 1)
#define for_each_cpu(cpu, mask)		\
	for ((cpu) = cpumask_first(mask); (cpu) = cpumask_next((cpu), (mask)), (cpu) < NR_CPUS;)
#endif
#define smp_cpu_on(cpu, ep)		smp_hw_cpu_on(cpu, ep)
#define smp_cpu_off(cpu)		smp_hw_cpu_off(cpu)
#else /* CONFIG_SMP */
#ifndef __SMP_CACHE_BYTES
#define SMP_CACHE_BYTES			1
#else
#define SMP_CACHE_BYTES			__SMP_CACHE_BYTES
#endif

#ifndef __ASSEMBLY__
#define smp_boot_cpu			0
#define smp_initialized			true

typedef int cpu_mask_t;
extern cpu_mask_t smp_online_cpus;

#define cpumask_bits(maskp)		(maskp)
#define cpumask_set_cpu(cpu, maskp)	(*(maskp) = 1)
#define cpumask_clear_cpu(cpu, maskp)	(*(maskp) = 0)
#define cpumask_test_cpu(cpu, maskp)	(C(cpu) == *(maskp))
#define cpumask_first(srcp)		(0)
/* Valid inputs for n are -1 and 0. */
#define cpumask_next(n, srcp)		((n)+1)
#define for_each_cpu(cpu, mask)		\
	for ((cpu) = 0; (cpu) < 1; (cpu)++, (void)(mask))

#define smp_cpu_on(cpu, ep, context)		do { } while (0)
#define smp_cpu_off(cpu)			do { } while (0)
#define smp_boot_secondary_cpus(context)	do { } while (0)
#endif /* __ASSEMBLY__ */
#endif /* CONFIG_SMP */
#define __cache_aligned			__align(SMP_CACHE_BYTES)

#ifndef __ASSEMBLY__
void smp_init(void);
#endif /* __ASSEMBLY__ */

#endif /* __SMP_H_INCLUDE__ */
