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
 * @(#)sysop.h: ARM64 system opcode/instruction defintions
 * $Id: sysop.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __ARM64_SYSOP_H_INCLUDE__
#define __ARM64_SYSOP_H_INCLUDE__

#include <stdint.h>

/* Define function for simple system instruction */
#define SYSOP_FUNC(_op)					\
static inline void _op(void)				\
{							\
	__asm__ (#_op);					\
}

/* Define function for system instruction with type specifier */
#define SYSOP_FUNC_TYPE(_op, _type)			\
static inline void _op ## _ ## _type(void)		\
{							\
	__asm__ (#_op " " #_type);			\
}

/* Define function for system instruction with register parameter */
#define SYSOP_FUNC_TYPE_PARAM(_op, _type)		\
static inline void _op ## _ ## _type(uint64_t v)	\
{							\
	 __asm__ (#_op " " #_type ", %0" : : "r" (v));	\
}

#ifndef __ASSEMBLY__

SYSOP_FUNC(yield)

/*
 * System instructions for cache maintenance
 */
SYSOP_FUNC_TYPE_PARAM(dc, cisw)
SYSOP_FUNC_TYPE_PARAM(dc, civac)
SYSOP_FUNC_TYPE_PARAM(dc, csw)
SYSOP_FUNC_TYPE_PARAM(dc, cvac)
SYSOP_FUNC_TYPE_PARAM(dc, cvau)
SYSOP_FUNC_TYPE_PARAM(dc, isw)
SYSOP_FUNC_TYPE_PARAM(dc, ivac)
SYSOP_FUNC_TYPE_PARAM(dc, zva)
SYSOP_FUNC_TYPE_PARAM(ic, iallu)
SYSOP_FUNC_TYPE_PARAM(ic, ialluis)
SYSOP_FUNC_TYPE_PARAM(ic, ivau)

/*
 * System instructions for address translation
 */
SYSOP_FUNC_TYPE_PARAM(at, s12e0r)
SYSOP_FUNC_TYPE_PARAM(at, s12e0w)
SYSOP_FUNC_TYPE_PARAM(at, s12e1r)
SYSOP_FUNC_TYPE_PARAM(at, s12e1w)
SYSOP_FUNC_TYPE_PARAM(at, s1e0r)
SYSOP_FUNC_TYPE_PARAM(at, s1e0w)
SYSOP_FUNC_TYPE_PARAM(at, s1e1r)
SYSOP_FUNC_TYPE_PARAM(at, s1e1w)
SYSOP_FUNC_TYPE_PARAM(at, s1e2r)
SYSOP_FUNC_TYPE_PARAM(at, s1e2w)
SYSOP_FUNC_TYPE_PARAM(at, s1e3r)
SYSOP_FUNC_TYPE_PARAM(at, s1e3w)

#endif /* __ASSEMBLY__ */

#endif /* __ARM64_SYSOP_H_INCLUDE__ */
