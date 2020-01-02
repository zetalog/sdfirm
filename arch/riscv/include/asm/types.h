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
 * @(#)types.h: RISCV architecture specific types interface
 * $Id: types.h,v 1.1 2019-08-14 14:24:00 zhenglv Exp $
 */

#ifndef __TYPES_RISCV_H_INCLUDE__
#define __TYPES_RISCV_H_INCLUDE__

#define __text__	const
#define __near__
#define __far__

#ifdef CONFIG_32BIT
#define __WORDSIZE	32
#define UINTPTR_MAX	UINT32_MAX
#endif
#ifdef CONFIG_64BIT
#define __WORDSIZE	64
#define UINTPTR_MAX	UINT64_MAX
#endif

#define __TEXT_TYPE__(type, name)	\
	typedef type name

typedef unsigned long		caddr_t;
#define ARCH_HAVE_CADDR_T	1
typedef unsigned long		size_t;
#define ARCH_HAVE_SIZE_T	1
typedef long			ssize_t;
#define ARCH_HAVE_SSIZE_T	1
typedef caddr_t			uintptr_t;
#define ARCH_HAVE_UINTPTR_T	1
typedef unsigned long long	phys_addr_t;

#endif /* __TYPES_RISCV_H_INCLUDE__ */
