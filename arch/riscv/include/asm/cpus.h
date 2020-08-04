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
 * @(#)cpus.h: RISCV CPU number/primitive interfaces
 * $Id: cpus.h,v 1.1 2019-08-14 09:37:00 zhenglv Exp $
 */

#ifndef __RISCV_CPUS_H_INCLUDE__
#define __RISCV_CPUS_H_INCLUDE__

/* This file includes architecture specific CPU hart definitions and CPU
 * primitives used by OS.
 */

/* Instruction naming identical APIs */
#define nop()			asm volatile("nop")
#define wfi()			asm volatile("wfi")

/* Architecture specific CPU primitives */
#define cpu_relax()		nop() /* Non-fairness cpu_relax() */
#if 0
#define cpu_relax()		asm volatile ("" : : : "memory")
#endif
#define wait_irq()		wfi()
/* No WFE supports */
#define wait_cpu()		cpu_relax()
#define wake_cpu()

#ifndef __ASSEMBLY__
#include <asm/assembler.h>
struct scratch {
	unsigned long sp;
} __attribute__((aligned(STACK_ALIGN)));
#endif

#include <asm/mach/cpus.h>

#ifndef BOOT_HART
#define BOOT_HART		0
#endif
#ifndef MAX_HARTS
#define MAX_HARTS		NR_CPUS
#endif
#ifndef HART_ALL
#define HART_ALL		CPU_ALL
#endif

#if defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
#ifndef ARCH_HAVE_SMPID
	.macro get_arch_smpid reg
	.endm
#endif
#endif

#endif /* __RISCV_CPUS_H_INCLUDE__ */
