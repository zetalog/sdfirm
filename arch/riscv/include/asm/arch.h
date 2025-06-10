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
 * @(#)arch.h: RISCV architecture specific definitions
 * $Id: arch.h,v 1.1 2019-08-14 09:32:00 zhenglv Exp $
 */

#ifndef __ARCH_RISCV_H_INCLUDE__
#define __ARCH_RISCV_H_INCLUDE__

/* This file includes architecture specific CPU primitive definitions:
 * wait_irq, cpu_relax, etc., as long as the CPU/Cache definitions and
 * configurable system bus targets (CPU/Cache, peripheral nodes).
 */
#include <asm/reg.h>
#include <asm/pmp.h>
#include <asm/fp.h>
#include <asm/counters.h>
#include <asm/mach/arch.h>

#ifdef CONFIG_ARCH_HAS_NOVEC
#ifdef VEC_BASE
#error "Arch has no specific vector while VEC_BASE defined!"
#endif
#define VEC_BASE	0
#endif

#define ABI_PERCPU_STACKS_START		PERCPU_STACKS_START
#define ABI_PERCPU_STACKS_END		PERCPU_STACKS_END
#define ABI_PERCPU_STACK_START(x)	PERCPU_STACK_START(x)

#ifdef CONFIG_SBI
#define SBI_PERCPU_STACKS_SIZE		(PERCPU_STACK_SIZE * MAX_HARTS)
#define SBI_PERCPU_STACKS_START		ABI_PERCPU_STACKS_END
#define SBI_PERCPU_STACKS_END		\
	(SBI_PERCPU_STACKS_START + SBI_PERCPU_STACKS_SIZE)
#define SBI_PERCPU_STACK_START(x)	\
	(SBI_PERCPU_STACKS_START + (x) * PERCPU_STACK_SIZE)
#define SBI_PERCPU_HEAPS_SIZE		(PERCPU_STACK_SIZE * (8 + MAX_HARTS))
#else
#define SBI_PERCPU_STACKS_START		ABI_PERCPU_STACKS_START
#define SBI_PERCPU_STACKS_END		ABI_PERCPU_STACKS_END
#define SBI_PERCPU_STACK_START(x)	ABI_PERCPU_STACK_START(x)
#endif

#ifdef CONFIG_SPIKE_SHUTDOWN_OVPSIM
#define _start_hang			write_tohost
#endif /* CONFIG_SPIKE_SHUTDOWN_OVPSIM */

#ifndef __ASSEMBLY__
unsigned long get_sp(void);
unsigned long get_tp(void);
#endif

#if defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
#ifndef ARCH_HAVE_BOOT_SMP
	.macro get_arch_smpid reg
	.endm
	.macro get_arch_hartmask reg
	li	\reg, HART_ALL
	.endm
	.macro get_arch_hartboot reg
	li	\reg, 0
	.endm
#endif
#endif

#endif /* __ARCH_RISCV_H_INCLUDE__ */
