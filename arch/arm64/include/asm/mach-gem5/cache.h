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
 * @(#)cache.h: GEM5 specific cache maintenance defintions
 * $Id: cache.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __GEM5_CACHE_H_INCLUDE__
#define __GEM5_CACHE_H_INCLUDE__

#include <asm/mach/arch.h>

#define L1_LINE_SHIFT		6
#define L1_CACHE_BYTES		(1 << L1_LINE_SHIFT)
#define L1_INTERLEAVE_BYTES	128
#define L1_CACHE_WAYS		8
#define L1_CACHE_SIZE		0x8000

#define L2_LINE_SHIFT		7
#define L2_CACHE_BYTES		(1 << L2_LINE_SHIFT)
#define L2_INTERLEAVE_BYTES	128
#define L2_CACHE_WAYS		8

#define L3_LINE_SHIFT		6
#define L3_CACHE_BYTES		(1 << L3_LINE_SHIFT)
#define L3_INTERLEAVE_BYTES	128
#define L3_CACHE_WAYS		20
#define L2_CACHE_SIZE		0x80000

/* 2.5MB per interleave */
#define L3_INTERLEAVE_SIZE		0x280000
#define L3_INTERLEAVES_PER_BLOCK	2
#define L3_CACHE_INTERLEAVES		MAX_LLC_NUM
#define L3_CACHE_BLOCKS			\
	(L3_CACHE_INTERLEAVES / L3_INTERLEAVES_PER_BLOCK)
#define L3_BLOCK_SIZE			\
	(L3_INTERLEAVE_SIZE * L3_INTERLEAVES_PER_BLOCK)
#define L3_BLOCK_SHIFT			8
#define L3_BLOCK_MASK			(0xF)
#define L3_BLOCK(phys)			\
	(((phys) & (L3_BLOCK_MASK << L3_BLOCK_SHIFT)) >> L3_BLOCK_SHIFT)
#define L3_CACHE_SIZE			(L3_CACHE_BLOCKS * L3_BLOCK_SIZE)

#define __SMP_CACHE_SHIFT	7

#endif /* __GEM5_CACHE_H_INCLUDE__ */
