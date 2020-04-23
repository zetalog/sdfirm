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
 * @(#)clint.h: SiFive core local interruptor (CLINT) interface
 * $Id: clint.h,v 1.1 2019-09-05 17:46:00 zhenglv Exp $
 */

#ifndef __CLINT_RISCV_H_INCLUDE__
#define __CLINT_RISCV_H_INCLUDE__

#include <target/smp.h>

#define CLINT_REG(offset)	(CLINT_BASE + (offset))
#define CLINT_MSIP(hart)	CLINT_REG((hart) << 2)
#define CLINT_MTIMECMP(hart)	CLINT_REG(0x4000 + ((hart) << 3))
#define CLINT_MTIME		CLINT_REG(0xBFF8)

#if !defined(__ASSEMBLY__) && !defined(LINKER_SCRIPT)
#ifdef CONFIG_CLINT
void clint_set_mtimecmp(cpu_t cpu, uint64_t cmp);
void clint_unset_mtimecmp(cpu_t cpu);
uint64_t clint_read_mtime(void);
#if defined(CONFIG_SBI) && defined(CONFIG_SMP)
#define clint_set_ipi(cpu)	\
	__raw_writel(1, CLINT_MSIP(smp_hw_cpu_hart(cpu)))
#define clint_clear_ipi(cpu)	\
	__raw_writel(0, CLINT_MSIP(smp_hw_cpu_hart(cpu)))
void clint_sync_ipi(cpu_t cpu);
#else
#define clint_set_ipi(cpu)		do { } while (0)
#define clint_clear_ipi(cpu)		do { } while (0)
#define clint_sync_ipi(cpu)		do { } while (0)
#endif
#else
#define clint_set_mtimecmp(cpu, cmp)	do { } while (0)
#define clint_unset_mtimecmp(cpu)	do { } while (0)
#define clint_read_mtime()		0
#define clint_set_ipi(cpu)		do { } while (0)
#define clint_clear_ipi(cpu)		do { } while (0)
#define clint_sync_ipi(cpu)		do { } while (0)
#endif
#endif

#endif /* __CLINT_RISCV_H_INCLUDE__ */
