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
 * @(#)cache.h: FU540 (unleased) specific cache maintenance defintions
 * $Id: cache.h,v 1.1 2019-10-06 10:06:00 zhenglv Exp $
 */

#ifndef __CACHE_UNLEASHED_H_INCLUDE__
#define __CACHE_UNLEASHED_H_INCLUDE__

#ifdef CONFIG_UNLEASHED_U54
#define L1_LINE_SHIFT		6
#define L1_CACHE_WAYS		8
#define L1_CACHE_SIZE		0x8000
#define L1_CACHE_BYTES		(1 << L1_LINE_SHIFT)
#define __SMP_CACHE_SHIFT	L1_LINE_SHIFT
#endif

/*===========================================================================
 * L2_CC
 *===========================================================================*/
/* Register offsets */
#define CCACHE_INFO		0x000
#define CCACHE_ENABLE		0x008
#define CCACHE_INJECT		0x040
#define CCACHE_META_FIX		0x100
#define CCACHE_DATA_FIX		0x140
#define CCACHE_DATA_FAIL	0x160
#define CCACHE_FLUSH64		0x200
#define CCACHE_FLUSH32		0x240
#define CCACHE_WAYS		0x800

/* Bytes inside the INFO field */
#define CCACHE_INFO_BANKS		0
#define CCACHE_INFO_WAYS		1
#define CCACHE_INFO_LG_SETS		2
#define CCACHE_INFO_LG_BLOCKBYTES	3

/* INJECT types */
#define	CCACHE_ECC_TOGGLE_DATA	0x00000
#define CCACHE_ECC_TOGGLE_META	0x10000

/* Offsets per FIX/FAIL */
#define CCACHE_ECC_ADDR		0x0
#define CCACHE_ECC_COUNT	0x8

/* Interrupt Number offsets from Base */
#define CCACHE_INT_META_FIX	0
#define CCACHE_INT_DATA_FIX	1
#define CCACHE_INT_DATA_FAIL	2

#endif /* __CACHE_UNLEASHED_H_INCLUDE__ */
