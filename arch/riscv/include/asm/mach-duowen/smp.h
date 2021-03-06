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
 * @(#)smp.h: DUOWEN specific SMP id <-> HART id conversion
 * $Id: smp.h,v 1.1 2020-03-15 11:59:00 zhenglv Exp $
 */

#ifndef __SMP_DUOWEN_H_INCLUDE__
#define __SMP_DUOWEN_H_INCLUDE__

#ifdef CONFIG_SBI
#ifdef CONFIG_DUOWEN_SBI_DUAL_SPARSE
#define SOC_CPU_HALF		(MAX_CPU_NUM >> 1)
#define SOC_CPU_MASK		(SOC_CPU_HALF - 1)
#define SOC_CPU_BITS		(ilog2_const(SOC_CPU_MASK) + 1)
#define smp_hw_cpu_hart(cpu)	((cpu & SOC_CPU_MASK) +	\
				 ((!!(cpu & SOC_CPU_HALF)) << 4))
#define smp_hw_hart_cpu(hart)   (((hart) & 0xF) +	\
				 (!!((hart) & (0xF0)) << SOC_CPU_BITS))
#else /* CONFIG_DUOWEN_SBI_DUAL_SPARSE */
#define smp_hw_cpu_hart(cpu)	((cpu))
#define smp_hw_hart_cpu(hart)	((hart))
#endif /* CONFIG_DUOWEN_SBI_DUAL_SPARSE */
#else /* CONFIG_SBI */
#define smp_hw_cpu_hart(cpu)	((cpu) + HART_BASE)
#define smp_hw_hart_cpu(hart)	((hart) - HART_BASE)
#endif /* CONFIG_SBI */

#endif /* __SMP_DUOWEN_H_INCLUDE__ */
