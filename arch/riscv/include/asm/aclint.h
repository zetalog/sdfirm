/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2024
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
 * @(#)aclint.h: RISC-V advanced core local interruptor (ACLINT) interface
 * $Id: aclint.h,v 1.1 2024-06-12 17:46:00 zhenglv Exp $
 */

#ifndef __ACLINT_RISCV_H_INCLUDE__
#define __ACLINT_RISCV_H_INCLUDE__

#define ACLINT_MTIMER_REG_ALIGN		0x8
#define ACLINT_MTIMER_MAX_HARTS		4095
#define ACLINT_MTIME_SIZE		ACLINT_MTIMER_REG_ALIGN
#define ACLINT_MTIMECMP_SIZE		\
	(ACLINT_MTIMER_MAX_HARTS * ACLINT_MTIMER_REG_ALIGN)

#ifndef ACLINT_MTIMER_BASE
#define ACLINT_MTIMER_BASE		0x0000
#endif
#ifndef ACLINT_MTIME_BASE
#define ACLINT_MTIME_BASE		\
	(ACLINT_MTIMECMP_BASE + ACLINT_MTIMECMP_SIZE)
#endif
#ifndef ACLINT_MTIMECMP_BASE
#define ACLINT_MTIMECMP_BASE		ACLINT_MTIMER_BASE
#endif
#define ACLINT_MTIMER_SIZE		\
	(ACLINT_MTIME_SIZE + ACLINT_MTIMECMP_SIZE)
#ifndef ACLINT_MSWI_BASE
#define ACLINT_MSWI_BASE		\
	(ACLINT_MTIMER_BASE + ACLINT_MTIMER_SIZE)
#endif

#ifdef CONFIG_ACLINT
#define CLINT_MTIME_BASE		ACLINT_MTIME_BASE
#define CLINT_MTIMECMP_BASE		ACLINT_MTIMECMP_BASE
#define CLINT_MSIP_BASE			ACLINT_MSWI_BASE

#ifdef CONFIG_ARCH_HAS_ACLINT_CTX
#define aclint_ctx(hart)		aclint_hw_ctx(hart)
#else
#define aclint_ctx(hart)		(hart)
#endif
#ifdef CONFIG_ARCH_HAS_CLINT_CTX
#define clint_hw_ctx(hart)		aclint_ctx(hart)
#endif
#define clint_hw_hart_offset(hart)	aclint_hw_hart_offset(hart)

#ifdef CONFIG_ACLINT_MULTI
#define CLINT_BASE(n)			ACLINT_BASE(n)
#define ACLINT_REG_BASE(n)		ACLINT_BASE(n)
#define clint_hw_chip(hart)		aclint_hw_chip(hart)
#else
#define CLINT_BASE			ACLINT_BASE
#define ACLINT_REG_BASE(n)		ACLINT_BASE
#endif

#define ACLINT_REG(n, offset)		(ACLINT_REG_BASE(n) + (offset))
#define ACLINT_MTIME(n)			ACLINT_REG(ACLINT_MTIME_BASE)
#define ACLINT_MTIMECMP(n, m)		ACLINT_REG(ACLINT_MTIMECMP_BASE)
#define ACLINT_MTIMER_REGION_ALIGN	0x1000

#include <asm/clint.h>
#endif

#endif /* __ACLINT_RISCV_H_INCLUDE__ */
