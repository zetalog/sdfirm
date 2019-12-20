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
 * @(#)percpu.h: per-cpu variables definitions
 * $Id: percpu.h,v 1.1 2019-12-17 17:00:00 zhenglv Exp $
 */

#ifndef __PERCPU_H_INCLUDE__
#define __PERCPU_H_INCLUDE__

#include <target/smp.h>

#define PERCPU_INPUT(cacheline)	\
	. = ALIGN(cacheline);	\
	__percpu_start = .;	\
	*(.data..percpu)	\
	. = ALIGN(cacheline);	\
	__percpu_end = .;

#ifdef __ASSEMBLY__
#ifndef LINKER_SCRIPT
.macro asm_get_percpu_addr _var:req, _tmp:req, _res=x0
	asm_smp_processor_id \_tmp, \_res
	ldr	\_tmp, =__percpu_offset
	ldr	\_tmp, [\_tmp, \_res, lsl #3]
	ldr     \_res, =\_var
	add     \_res, \_res, \_tmp
.endm
#endif
#else
extern uintptr_t __percpu_start[];
extern uintptr_t __percpu_end[];

#define PERCPU_START		((uint64_t)&__percpu_start)
#define PERCPU_END		((uint64_t)&__percpu_end)

#ifdef CONFIG_SMP
#define DEFINE_PERCPU(type, name)			\
	__attribute__((__section__(".data..percpu")))	\
	__cache_aligned __typeof__(type) name
#define percpu_offset(__cpu)	(__percpu_offset[__cpu])
#define percpu_reloc_ptr(__ptr, __cpu)	\
	RELOC_HIDE((__ptr), percpu_offset(__cpu))
#define percpu_reloc_var(__var, __cpu)	\
	(*percpu_reloc_ptr(&(__var), __cpu))
#define per_cpu_var(__var, __cpu)	percpu_reloc_var(__var, __cpu)
#define per_cpu_ptr(__ptr, __cpu)	percpu_reloc_ptr(__ptr, __cpu)
#define this_cpu_var(__var)		per_cpu_var(__var, smp_processor_id())
#define this_cpu_ptr(__ptr)		per_cpu_ptr(__ptr, smp_processor_id())

extern uint64_t __percpu_offset[NR_CPUS];
void percpu_init(void);
#else
#define DEFINE_PERCPU(type, name)	\
	__typeof__(type) name
#define this_cpu_var(var)		(var)
#define per_cpu_var(var, cpu)		(var)
#define this_cpu_ptr(ptr)		(ptr)
#define per_cpu_ptr(ptr, cpu)		(ptr)
#define percpu_init()		do { } while (0)
#endif
#endif /* __ASSEMBLY__ */

#endif /* __PERCPU_H_INCLUDE__ */
