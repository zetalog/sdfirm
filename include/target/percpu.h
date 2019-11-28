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
 * $Id: percpu.h,v 1.1 2019-11-06 15:56:00 zhenglv Exp $
 */

#ifndef __PERCPU_H_INCLUDE__
#define __PERCPU_H_INCLUDE__

#include <target/smp.h>

#define PERCPU_INPUT(cacheline)	\
	. = ALIGN(cacheline);	\
	__percpu_start = .;	\
	*(.sysdata..percpu)	\
	. = ALIGN(cacheline);	\
	__percpu_end = .;

#ifdef __ASSEMBLY__
#ifndef LINKER_SCRIPT
.macro asm_get_percpu_addr _var:req, _tmp:req, _res=x0
	asm_smp_processor_id \_tmp, \_res
	ldr	\_tmp, =__percpu_offset
	ldr	\_tmp, [\_tmp, \_res, lsl #3]
	ldr     \_res, =percpu_\_var
	add     \_res, \_res, \_tmp
.endm
#endif
#else
extern uintptr_t __percpu_start[];
extern uintptr_t __percpu_end[];

#define PERCPU_START		((uint64_t)&__percpu_start)
#define PERCPU_END		((uint64_t)&__percpu_end)

#ifdef CONFIG_SMP
#ifdef CONFIG_PERCPU_INTERLEAVE
#define DEFINE_PERCPU(type, name)	\
	__attribute__((__section__(".sysdata..percpu"))) \
	__typeof__(type) percpu_##name[NR_CPUS]
#define get_percpu(var)		(percpu_##var[smp_processor_id()])
#define get_percpu_hmp(var)	get_percpu(var)
#else
#define DEFINE_PERCPU(type, name)	\
	__attribute__((__section__(".sysdata..percpu"))) \
	__typeof__(type) percpu_##name

extern uint64_t __percpu_offset[NR_CPUS];
#define percpu_offset(x)	(__percpu_offset[x])
#define thiscpu_offset		percpu_offset(smp_processor_id())

#define percpu_reloc(var, cpu)	(*RELOC_HIDE(&percpu_##var, percpu_offset(cpu)))
#define get_percpu(var)		percpu_reloc(var, smp_processor_id())
#define get_percpu_hmp(var)	percpu_reloc(var, hmp_processor_id())
#endif
#else
#define DEFINE_PERCPU(type, name)	\
	__typeof__(type) percpu_##name

#define get_percpu(var)		percpu_##var
#define get_percpu_hmp(var)	get_percpu(var)
#endif

caddr_t percpu_get_free_area(uint8_t cpu);
int percpu_init(void);
#endif /* __ASSEMBLY__ */

#endif /* __PERCPU_H_INCLUDE__ */
