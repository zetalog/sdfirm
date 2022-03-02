/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)vaisra_pma.h: VAISRA physical memory attributes (PMA) interface
 * $Id: vaisra_pma.h,v 1.1 2020-07-05 23:41:00 zhenglv Exp $
 */

#ifndef __PMA_VAISRA_H_INCLUDE__
#define __PMA_VAISRA_H_INCLUDE__

#ifndef VAISRA_PMA_G
/* VAISRA_PMA_G is not defined, use default 19 (2MB). */
#define VAISRA_PMA_G		19
#endif /* VAISRA_PMA_SIZE */

#define PMA_GRAIN_SHIFT		(VAISRA_PMA_G + 2)
#define PMA_GRAIN_ALIGN		(ULL(1) << PMA_GRAIN_SHIFT)

#define CSR_PMACFG(n)		(0x7C0 + (n)) /* 0..3 */
#define CSR_PMAADDR(n)		(0x7D0 + (n)) /* 0..15 */

/* Address mode [11:10] */
#define PMA_A			_AC(0x00000C00, UL)
#define PMA_A_TOR		_AC(0x00000400, UL)
#if VAISRA_PMA_G >= 1
#define PMA_A_NA4		PMA_A_NAPOT
#else
#define PMA_A_NA4		_AC(0x00000800, UL)
#endif
#define PMA_A_NAPOT		_AC(0x00000C00, UL)
/* Sharable [9:8] */
#define PMA_S			_AC(0x00000300, UL)
#define PMA_S_NON		_AC(0x00000000, UL)
#define PMA_S_OUTER		_AC(0x00000200, UL)
#define PMA_S_INNER		_AC(0x00000300, UL)
/* Attribute [7:4] [3:0]*/
/* Device memory attribute */
#define PMA_AT_DEVICE_nGnRnE	_AC(0x00000000, UL)
#define PMA_AT_DEVICE_nGnRE	_AC(0x00000004, UL)
#define PMA_AT_DEVICE_nGRE	_AC(0x00000008, UL)
#define PMA_AT_DEVICE_GRE	_AC(0x0000000C, UL)
#if defined(CONFIG_VAISRA_PMA_GATHERING)
#define PMA_AT_DEVICE		PMA_AT_DEVICE_GRE
#elif defined(CONFIG_VAISRA_PMA_RE_ORDERING)
#define PMA_AT_DEVICE		PMA_AT_DEVICE_nGRE
#elif defined(CONFIG_VAISRA_PMA_EARLY_WRITE_ACK)
#define PMA_AT_DEVICE		PMA_AT_DEVICE_nGnRE
#else
#define PMA_AT_DEVICE		PMA_AT_DEVICE_nGnRnE
#endif
/* Normal memory attribute, used by outer[7:4], inner[3:0] */
/* 1.2.3 ARM like MemAttributes descriptors */
#define PMA_AT_NORM_WTTWA	_AC(0x00000001, UL) /* write-through transient write-allocate */
#define PMA_AT_NORM_WTTRA	_AC(0x00000002, UL) /* write-through transient read-allocate */
#define PMA_AT_NORM_WTTRWA	_AC(0x00000003, UL) /* write-through transient read/write-allocate */
#define PMA_AT_NORM_NC		_AC(0x00000004, UL) /* non-cacheable */
#define PMA_AT_NORM_WBTWA	_AC(0x00000005, UL) /* write-back transient write-allocate */
#define PMA_AT_NORM_WBTRA	_AC(0x00000006, UL) /* write-back transient read-allocate */
#define PMA_AT_NORM_WBTRWA	_AC(0x00000007, UL) /* write-back transient read/write-allocate */
#define PMA_AT_NORM_WTnTRWA	_AC(0x0000000B, UL) /* write-through non-transient */
#define PMA_AT_NORM_WTnTWA	_AC(0x00000009, UL) /* write-through non-transient write-allocate */
#define PMA_AT_NORM_WTnTRA	_AC(0x0000000A, UL) /* write-through non-transient read-allocate */
#define PMA_AT_NORM_WTnT	_AC(0x0000000B, UL) /* write-through non-transient read/write-allocate */
#define PMA_AT_NORM_WBnT	_AC(0x0000000D, UL) /* write-back non-transient */
#define PMA_AT_NORM_WBnTWA	_AC(0x0000000D, UL) /* write-back non-transient write-allocate */
#define PMA_AT_NORM_WBnTRA	_AC(0x0000000E, UL) /* write-back non-transient read-allocate */
#define PMA_AT_NORM_WBnTRWA	_AC(0x0000000F, UL) /* write-back non-transient read/write-allocate*/
#define PMA_AT_NORM(outer, ineer)	((outer) << 4 | (ineer))
#define PMA_AT_NORMAL_NC	PMA_AT_NORM(PMA_AT_NORM_NC,      PMA_AT_NORM_NC)
#define PMA_AT_NORMAL_WTTWA	PMA_AT_NORM(PMA_AT_NORM_WTTWA,   PMA_AT_NORM_WTTWA)
#define PMA_AT_NORMAL_WTTRA	PMA_AT_NORM(PMA_AT_NORM_WTTRA,   PMA_AT_NORM_WTTRA)
#define PMA_AT_NORMAL_WTTRWA	PMA_AT_NORM(PMA_AT_NORM_WTTRWA,  PMA_AT_NORM_WTTRWA)
#define PMA_AT_NORMAL_WBTWA	PMA_AT_NORM(PMA_AT_NORM_WBTWA,   PMA_AT_NORM_WBTWA)
#define PMA_AT_NORMAL_WBTRA	PMA_AT_NORM(PMA_AT_NORM_WBTRA,   PMA_AT_NORM_WBTRA)
#define PMA_AT_NORMAL_WBTRWA	PMA_AT_NORM(PMA_AT_NORM_WBTRWA,  PMA_AT_NORM_WBTRWA)
#ifdef CONFIG_VAISRA_PMA_TRANSIENT
#define PMA_AT_NORMAL_WT	PMA_AT_NORMAL_WTTRWA
#define PMA_AT_NORMAL_WTWA	PMA_AT_NORMAL_WTTWA
#define PMA_AT_NORMAL_WTRA	PMA_AT_NORMAL_WTTRA
#define PMA_AT_NORMAL_WTRWA	PMA_AT_NORMAL_WTTRWA
#define PMA_AT_NORMAL_WB	PMA_AT_NORMAL_WBTRWA
#define PMA_AT_NORMAL_WBWA	PMA_AT_NORMAL_WBTWA
#define PMA_AT_NORMAL_WBRA	PMA_AT_NORMAL_WBTRA
#define PMA_AT_NORMAL_WBRWA	PMA_AT_NORMAL_WBTRWA
#else
#ifdef CONFIG_VAISRA_PMA_NON_ALLOCATE
#define PMA_AT_NORMAL_WT	PMA_AT_NORM(PMA_AT_NORM_WTnT,    PMA_AT_NORM_WTnT)
#else
#define PMA_AT_NORMAL_WT	PMA_AT_NORMAL_WTRWA
#endif
#define PMA_AT_NORMAL_WTWA	PMA_AT_NORM(PMA_AT_NORM_WTnTWA,  PMA_AT_NORM_WTnTWA)
#define PMA_AT_NORMAL_WTRA	PMA_AT_NORM(PMA_AT_NORM_WTnTRA,  PMA_AT_NORM_WTnTRA)
#define PMA_AT_NORMAL_WTRWA	PMA_AT_NORM(PMA_AT_NORM_WTnTRWA, PMA_AT_NORM_WTnTRWA)
#ifdef CONFIG_VAISRA_PMA_NON_ALLOCATE
#define PMA_AT_NORMAL_WB	PMA_AT_NORM(PMA_AT_NORM_WBnT,    PMA_AT_NORM_WBnT)
#else
#define PMA_AT_NORMAL_WB	PMA_AT_NORMAL_WBRWA
#endif
#define PMA_AT_NORMAL_WBWA	PMA_AT_NORM(PMA_AT_NORM_WBnTWA,  PMA_AT_NORM_WBnTWA)
#define PMA_AT_NORMAL_WBRA	PMA_AT_NORM(PMA_AT_NORM_WBnTWA,  PMA_AT_NORM_WBnTRA)
#define PMA_AT_NORMAL_WBRWA	PMA_AT_NORM(PMA_AT_NORM_WBnTRWA, PMA_AT_NORM_WBnTRWA)
#endif
#ifdef CONFIG_VAISRA_PMA_WRITE_THROUGH
#define PMA_AT_NORMAL_WA	PMA_AT_NORMAL_WTWA
#define PMA_AT_NORMAL_RA	PMA_AT_NORMAL_WTRA
#define PMA_AT_NORMAL		PMA_AT_NORMAL_WT
#else
#define PMA_AT_NORMAL_WA	PMA_AT_NORMAL_WBWA
#define PMA_AT_NORMAL_RA	PMA_AT_NORMAL_WBRA
#define PMA_AT_NORMAL		PMA_AT_NORMAL_WB
#endif

#define PMA_COUNT		16
#define PMA_SHIFT		2

#ifndef __ASSEMBLY__
int pma_set(int n, unsigned long attr, phys_addr_t addr,
	    unsigned long log2len);
#endif

#endif /* __PMA_VAISRA_H_INCLUDE__ */
