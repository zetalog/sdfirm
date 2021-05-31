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
 * @(#)cpus.h: DUOWEN specific CPU definitions
 * $Id: cpus.h,v 1.1 2019-09-02 11:08:00 zhenglv Exp $
 */

#ifndef __CPUS_DUOWEN_H_INCLUDE__
#define __CPUS_DUOWEN_H_INCLUDE__

#ifdef CONFIG_DUOWEN_APC_16
#define MAX_APC_NUM		16
#endif /* CONFIG_DUOWEN_APC_16 */
#ifdef CONFIG_DUOWEN_APC_4
#define MAX_APC_NUM		4
#endif /* CONFIG_DUOWEN_APC_4 */
#ifdef CONFIG_DUOWEN_APC_1
#define MAX_APC_NUM		1
#endif /* CONFIG_DUOWEN_APC_1 */

/* APC 4 Cores Usage:
 *
 * In this configuration, mhartid always returns 0/1/2/3, while partial
 * goods is actually different, definining natural partial goods as
 * __GOOD_CPU_MASK, and the actual partial goods as GOOD_CPU_MASK here.
 */
#define __GOOD_CPU_MASK		(CPU_TO_MASK(MAX_APC_NUM) - 1)
#ifdef CONFIG_DUOWEN_APC_4_3100
#define GOOD_CPU_MASK		0x0017
#endif /* CONFIG_DUOWEN_APC_4_3100 */
#ifdef CONFIG_DUOWEN_APC_4_1111
#define GOOD_CPU_MASK		0x1111
#endif /* CONFIG_DUOWEN_APC_4_1111 */
#ifndef GOOD_CPU_MASK
#define GOOD_CPU_MASK		__GOOD_CPU_MASK
#endif /* GOOD_CPU_MASK */

/* MAX_CPU_NUM: Used by SMP subsystem to determine the number of CPUs
 * MAX_CPU_CLUSTERS:   Used by NoC code to determine the maximum number of
 *                     clusters. However, the actual clusters are masked
 *                     by partial good result.
 *                     Also used by SMP bench to determine cluster/CPU
 *                     relationship.
 * __MAX_CPU_CORES:    Used only by APC clock driver base address array.
 * __MAX_CPU_CLUSTERS: Used only by SCSR ROM driver to indicate per-socket
 *                     ROM information.
 */
#define __MAX_CPU_CLUSTERS	4
#define __MAX_CPU_CORES		16

#ifdef CONFIG_DUOWEN_IMC
#define MAX_CPU_NUM		1
#define MAX_CPU_CLUSTERS	1
#endif /* CONFIG_DUOWEN_IMC */

#ifdef CONFIG_DUOWEN_APC
#ifdef CONFIG_SMP
#ifdef CONFIG_SBI
#define MAX_CPU_NUM		(2 * MAX_APC_NUM)
#define MAX_CPU_CLUSTERS	(2 * __MAX_CPU_CLUSTERS)
#else /* CONFIG_SBI */
#define MAX_CPU_NUM		MAX_APC_NUM
#define MAX_CPU_CLUSTERS	__MAX_CPU_CLUSTERS
#endif /* CONFIG_SBI */
#else /* CONFIG_SMP */
#ifdef CONFIG_DUOWEN_SBI_DUAL
#error "Do not support dual socket as SMP is not configured!"
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#define MAX_CPU_NUM		1
#define MAX_CPU_CLUSTERS	1
#endif /* CONFIG_SMP */
#endif /* CONFIG_DUOWEN_APC */

#define CPUS_PER_APC		2
#define CPU_TO_APC(cpu)		((cpu) / CPUS_PER_APC)

/* hartid bases */
#ifdef CONFIG_DUOWEN_IMC
#define SOC0_HART		0
#define SOC1_HART		0
#endif /* CONFIG_DUOWEN_IMC */
#ifdef CONFIG_DUOWEN_APC
#define SOC0_HART		0
#define SOC1_HART		16
#endif /* CONFIG_DUOWEN_APC */

#ifndef __ASSEMBLY__
extern unsigned long duowen_hart_base;
#endif
#define HART_BASE		duowen_hart_base

#ifdef CONFIG_SBI
#ifdef CONFIG_DUOWEN_SBI_DUAL_SPARSE
/* FIXME: This actually makes sbi_processor_id() returned as hartid */
#define MAX_HARTS		32
#define HART_ALL				\
	((CPU_TO_MASK(MAX_CPU_NUM>>1)-1) |	\
	 ((CPU_TO_MASK(MAX_CPU_NUM>>1)-1) << SOC1_HART))
#endif /* CONFIG_DUOWEN_SBI_DUAL_SPARSE */
#else /* CONFIG_SBI */
/* Local programs uses special hart mask */
#define HART_ALL		((CPU_TO_MASK(MAX_CPU_NUM)-1) << HART_BASE)
#endif /* CONFIG_SBI */

#endif /* __CPUS_DUOWEN_H_INCLUDE__ */
