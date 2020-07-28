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
 * @(#)vaisra_ras.h: VAISRA specific RAS (reli/avail/service-ability) interface
 * $Id: vaisra_ras.h,v 1.1 2020-07-28 09:09:00 zhenglv Exp $
 */

#ifndef __RAS_VAISRA_H_INCLUDE__
#define __RAS_VAISRA_H_INCLUDE__

#define IRQ_M_NMI	16

/* dcache and icache, accessed via CSR */
#define CSR_DCEC	0x5cd /* D-cache error control */
#define CSR_DCES	0x5ce /* D-cache error status */
#define CSR_ICEC	0x5d0 /* I-cache error control */
#define CSR_ICES	0x5d1 /* I-cache error status */

/* L2, accessed via L2CSR */ 
#define L2CSR_L2EC0	0x208 /* L2-cache error control 0 */
#define L2CSR_L2EC1	0x210 /* L2-cache error control 1 */
#define L2CSR_L2ES_GC	0x218 /* L2-cache error status (get/clear) */
#define L2CSR_L2ES_S	0x220 /* L2-cache error status (set) */

/* DCEC/DCES */
#define DCE_L1VTPE	0 /* L1-dcache virtual tag parity error */
#define DCE_L1PTPE	1 /* L1-dcache physical tag parity error */
#define DCE_L1DPE	2 /* L1-dcache data parity error */
#define DCE_S1FTLBTPE	3 /* Stage-1 final TLB tag parity error */
#define DCE_S1FTLBDPE	4 /* Stage-1 final TLB data parity error */

/* ICEC/ICES */
#define ICE_L0TPE	0 /* L0-icache tag parity error */
#define ICE_L0DPE	1 /* L0-icache data parity error */
#define ICE_L1TPE	2 /* L1-icache tag parity error */
#define ICE_L1DPE	3 /* L1-icache data parity error */
#define ICE_V0ICPEO	4 /* interleave-0 I-cache parity error observed */
#define ICE_V1ICPEO	5 /* interleave-1 I-cache parity error observed */

/* L2EC/L2ES */

#define ERR_S(err)	_BV(err)
#define ERR_ESEI(err)	_BV(((err) << 1) + 1)
#define ERR_ENMI(err)	_BV((err) << 1)

#if defined(CONFIG_VAISRA_RAS) && defined(CONFIG_SBI)
void sbi_process_vaisra_nmi(void);
#else
#define sbi_process_vaisra_nmi()	do { } while (0)
#endif

#endif /* __RAS_VAISRA_H_INCLUDE__ */
