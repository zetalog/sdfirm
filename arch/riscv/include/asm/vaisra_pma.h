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

#define CSR_PMACFG(n)		(0x7C0 + (n)) /* 0..3 */
#define CSR_PMAADDR(n)		(0x7D0 + (n)) /* 0..15 */

/* Address mode */
#define PMA_A			_AC(0x00000C00, UL)
#define PMA_A_TOR		_AC(0x00000400, UL)
#define PMA_A_NA4		_AC(0x00000800, UL)
#define PMA_A_NAPOT		_AC(0x00000C00, UL)
/* Sharable */
#define PMA_S			_AC(0x00000300, UL)
#define PMA_S_NON		_AC(0x00000000, UL)
#define PMA_S_OUTER		_AC(0x00000200, UL)
#define PMA_S_INNER		_AC(0x00000300, UL)
/* Attribute */
/* Device memory attribute */
#define PMA_AT_DEV_nGnRnE	_AC(0x00000000, UL)
#define PMA_AT_DEV_nGnRE	_AC(0x00000004, UL)
#define PMA_AT_DEV_nGRE		_AC(0x00000008, UL)
#define PMA_AT_DEV_GRE		_AC(0x0000000C, UL)
#if defined(CONFIG_VAISRA_PMA_GATHERING)
#define PMA_AT_DEVICE		PMA_AT_DEV_GRE
#elif defined(CONFIG_VAISRA_PMA_RE_ORDERING)
#define PMA_AT_DEVICE		PMA_AT_DEV_nGRE
#elif defined(CONFIG_VAISRA_PMA_EARLY_WRITE_ACK)
#define PMA_AT_DEVICE		PMA_AT_DEV_nGnRE
#else
#define PMA_AT_DEVICE		PMA_AT_DEV_nGnRnE
#endif
/* Normal memory attribute, used by outer[7:4], inner[3:0] */
#define PMA_AT_NORM_WTT		_AC(0x00000003, UL) /* write-through transient */
#define PMA_AT_NOMR_NC		_AC(0x00000004, UL) /* non-cacheable */
#define PMA_AT_NOMR_WBT		_AC(0x00000007, UL) /* write-back transient */
#define PMA_AT_NORM_WTnT	_AC(0x0000000B, UL) /* write-through non-transient */
#define PMA_AT_NORM_WBnT	_AC(0x0000000F, UL) /* write-back non-transient */
#define PMA_AT_NORM(outer, ineer)	((outer) << 4 | (ineer))
#define PMA_AT_NORM_NC		PMA_AT_NORM(PMA_AT_NORM_NC, PMA_AT_NORM_NC)
#ifdef CONFIG_VAISRA_PMA_TRANSIENT
#define PMA_AT_NORM_WT		PMA_AT_NORM(PMA_AT_NORM_WTT, PMA_AT_NORM_WTT)
#define PMA_AT_NORM_WB		PMA_AT_NORM(PMA_AT_NORM_WBT, PMA_AT_NORM_WBT)
#else
#define PMA_AT_NORM_WT		PMA_AT_NORM(PMA_AT_NORM_WTnT, PMA_AT_NORM_WTnT)
#define PMA_AT_NORM_WB		PMA_AT_NORM(PMA_AT_NORM_WBnT, PMA_AT_NORM_WBnT)
#endif
#ifdef CONFIG_VAISRA_PMA_WRITE_THROUGH
#define PMA_AT_NORMAL		PMA_AT_NORM_WT
#else
#define PMA_AT_NORMAL		PMA_AT_NORM_WB
#endif

#define PMA_COUNT		16
#define PMA_SHIFT		2

#ifndef __ASSEMBLY__
int pma_set(int n, unsigned long attr, phys_addr_t addr, unsigned long log2len);
#endif

#endif /* __PMA_VAISRA_H_INCLUDE__ */
