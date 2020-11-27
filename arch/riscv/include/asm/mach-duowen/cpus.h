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

#ifdef CONFIG_DUOWEN_IMC
#define MAX_CPU_NUM		1
#define MAX_CPU_CLUSTERS	1
#endif /* CONFIG_DUOWEN_IMC */

#ifdef CONFIG_DUOWEN_APC
#ifdef CONFIG_SMP
#ifdef CONFIG_DUOWEN_APC_3
#define MAX_CPU_NUM		3
#define MAX_CPU_CLUSTERS	3
#elif defined(CONFIG_DUOWEN_APC_4)
#define MAX_CPU_NUM		4
#define MAX_CPU_CLUSTERS	4
#else /* CONFIG_DUOWEN_APC_3/4 */
#define MAX_CPU_NUM		16
#define MAX_CPU_CLUSTERS	4
#endif /* CONFIG_DUOWEN_APC_3/4 */
#else /* CONFIG_SMP */
#define MAX_CPU_NUM		1
#define MAX_CPU_CLUSTERS	1
#endif /* CONFIG_SMP */
#endif /* CONFIG_DUOWEN_APC */

#ifdef CONFIG_DUOWEN_SOC0
#define HART_BASE		0
#endif
#ifdef CONFIG_DUOWEN_SOC1
#define HART_BASE		16
#endif
#if defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
	.macro get_arch_smpid reg
	addi	\reg, \reg, -HART_BASE
	.endm
#define ARCH_HAVE_SMPID		1
#endif
#define BOOT_HART		HART_BASE
#define HART_ALL		((CPU_TO_MASK(MAX_CPU_NUM)-1) << HART_BASE)

#endif /* __CPUS_DUOWEN_H_INCLUDE__ */
