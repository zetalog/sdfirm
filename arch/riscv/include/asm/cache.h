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
 * @(#)cache.h: RISCV cache flush/alignment interfaces
 * $Id: cache.h,v 1.1 2019-08-14 09:37:00 zhenglv Exp $
 */

#ifndef __CACHE_RISCV_H_INCLUDE__
#define __CACHE_RISCV_H_INCLUDE__

/* This file includes architecture specific cache alignment/flush primitive
 * implementations which should be SMP specific.
 */

/* Locate real machine specific SMP cache alignments */
#include <asm/mach/cache.h>

#ifndef __SMP_CACHE_SHIFT
#define __SMP_CACHE_SHIFT	6
#endif
#define __SMP_CACHE_BYTES	(1 << __SMP_CACHE_SHIFT)

#ifndef __ASSEMBLY__
#define fence_i()				\
	asm volatile("fence.i" : : : "memory")

#define __flush_dcache_area(addr, size)		do { } while (0)
#define __clean_dcache_area_poc(addr, size)	do { } while (0)
#define __inval_dcache_area_poc(addr, size)	do { } while (0)
#define local_flush_icache_all()		fence_i()

#define __flush_dcache_addr(addr)		do { } while (0)
#define __clean_dcache_addr(addr)		do { } while (0)
#define __inval_dcache_addr(addr)		do { } while (0)
void flush_icache_all(void);
#endif

#endif /* __CACHE_RISCV_H_INCLUDE__ */
