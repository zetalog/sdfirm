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
 * @(#)cpus.h: FU540 (unleased) specific CPU definitions
 * $Id: cpus.h,v 1.1 2019-10-16 10:03:00 zhenglv Exp $
 */

#ifndef __CPUS_UNLEASHED_H_INCLUDE__
#define __CPUS_UNLEASHED_H_INCLUDE__

#ifdef CONFIG_UNLEASHED_E51
#define MAX_CPU_NUM		1
#endif

#ifdef CONFIG_UNLEASHED_U54
#ifdef CONFIG_UNLEASHED_U54_HART_MASK
#ifdef CONFIG_UNLEASHED_U54_HART1
#define HART1			C(1)
#define CPU0			C(0)
#else
#define HART1			0
#define CPU0			0
#endif
#ifdef CONFIG_UNLEASHED_U54_HART2
#define HART2			C(2)
#define CPU1			C(1)
#else
#define HART2			0
#define CPU1			0
#endif
#ifdef CONFIG_UNLEASHED_U54_HART3
#define HART3			C(3)
#define CPU2			C(2)
#else
#define HART3			0
#define CPU2			0
#endif
#ifdef CONFIG_UNLEASHED_U54_HART4
#define HART4			C(4)
#define CPU3			C(3)
#else
#define HART4			0
#define CPU3			0
#endif
#define CPU_ALL			(CPU0 | CPU1 | CPU2 | CPU3)
#define HART_ALL		(HART1 | HART2 | HART3 | HART4)
#else /* CONFIG_UNLEASHED_U54_HART_MASK */
#define HART_ALL		(C(1) | C(2) | C(3) | C(4))
#endif /* CONFIG_UNLEASHED_U54_HART_MASK */
#ifdef CONFIG_SMP
#define MAX_CPU_NUM		4 /* Excluding E51 */
#else
#define MAX_CPU_NUM		1
#endif
#define BOOT_HART		1
#define MAX_HARTS		5
#if defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
	.macro get_arch_smpid reg
	beqz	\reg, 7770f
	addi	\reg, \reg, -1
	j	7771f
7770:
	li	\reg, 4
7771:
	.endm
#define ARCH_HAVE_SMPID		1
#endif
#endif

#endif /* __CPUS_UNLEASHED_H_INCLUDE__ */
