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
 * @(#)smp.h: ARM64 specific symmetric multi-processing interfaces
 * $Id: smp.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __ARM64_SMP_H_INCLUDE__
#define __ARM64_SMP_H_INCLUDE__

#include <asm/mach/smp.h>

#ifdef CONFIG_SMP
#ifdef __ASSEMBLY__
#ifndef LINKER_SCRIPT
.macro asm_smp_processor_id _tmp:req, _res=x0
	mov	\_res, sp
	ldr	\_tmp, =PERCPU_STACKS_START
	sub	\_res, \_res, \_tmp
	ubfx	\_res, \_res, #PERCPU_STACK_SHIFT, #(32 - PERCPU_STACK_SHIFT)
.endm
#endif /* LINKER_SCRIPT */
#else /* __ASSEMBLY__ */
static inline cpu_t __smp_processor_id(void)
{
	unsigned int t;

	asm volatile ("mov %0, sp\n\t" : "=r" (t));
	t -= (PERCPU_STACKS_START + 1);
	return (uint8_t)(t >> 12);
}

static inline uintptr_t __smp_processor_stack_top(void)
{
	uintptr_t t;

	asm volatile ("mov %0, sp\n\t" : "=r" (t));
	return ALIGN(t, PERCPU_STACK_SIZE);
}

cpu_t smp_hw_cpu_id(void);
void smp_hw_map_init(void);
void smp_hw_cpu_boot(cpu_t cpu, caddr_t ep);
#define smp_hw_ctrl_init()		do { } while (0)
#endif /* __ASSEMBLY__ */
#endif /* CONFIG_SMP */

#endif /* __ARM64_SMP_H_INCLUDE__ */
