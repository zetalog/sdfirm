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
#define CSR_MDCEC	0x5cd /* D-cache error control */
#define CSR_MDCES	0x5ce /* D-cache error status */
#define CSR_MICEC	0x5d0 /* I-cache error control */
#define CSR_MICES	0x5d1 /* I-cache error status */

/* L2 indirect register access CSR */
#define CSR_ML2CSRS	0x5d3 /* L2 CSR select register */
#define CSR_ML2CSRD	0x5d2 /* L2 CSR data register */

/* L2, accessed via L2CSR */ 
#define L2CSR_L2EC0	0x200 /* L2-cache error control 0 */
#define L2CSR_L2EC1	0x201 /* L2-cache error control 1 */
#define L2CSR_L2ES_GC	0x204 /* L2-cache error status (get/clear) */
#define L2CSR_L2ES_S	0x205 /* L2-cache error status (set) */

/* DCEC/DCES */
#define DCE_L1VTPE	0 /* L1-dcache virtual tag parity error */
#define DCE_L1PTPE	1 /* L1-dcache physical tag parity error */
#define DCE_L1DPE	2 /* L1-dcache data parity error */
#define DCE_S1FTLBTPE	3 /* Stage-1 final TLB tag parity error */
#define DCE_S1FTLBDPE	4 /* Stage-1 final TLB data parity error */

#define MAX_DCES	5

/* ICEC/ICES */
#define ICE_L0TPE	0 /* L0-icache tag parity error */
#define ICE_L0DPE	1 /* L0-icache data parity error */
#define ICE_L1TPE	2 /* L1-icache tag parity error */
#define ICE_L1DPE	3 /* L1-icache data parity error */
#define ICE_V0ICPEO	4 /* Interleave-0 I-cache parity error observed */
#define ICE_V1ICPEO	5 /* Interleave-1 I-cache parity error observed */

#define MAX_ICES	6

/* L2CSR */
#define L2CSR_CLS	_AC(0x00000700, UL)
#define L2CSR_REG	_AC(0x000000ff, UL)

/* L2EC/L2ES */
#define L2E_TSBEC0AD	0  /* Tag single bit error cpu0 abort or discard */
#define L2E_TSBEC0NA	1  /* Tag single bit error cpu0 no-abort */
#define L2E_TSBEC1AD	2  /* Tag single bit error cpu1 abort or discard */
#define L2E_TSBEC1NA	3  /* Tag single bit error cpu1 no-abort */
#define L2E_TSBESNP	4  /* Tag single bit error snoop */
#define L2E_TSBEL2I	5  /* Tag single bit error L2 initiated */
#define L2E_TDBEC0AD	6  /* Tag double bit error cpu0 abort or discard */
#define L2E_TDBEC0NA	7  /* Tag double bit error cpu0 no-abort */
#define L2E_TDBEC1AD	8  /* Tag double bit error cpu1 abort or discard */
#define L2E_TDBEC1NA	9  /* Tag double bit error cpu1 no-abort */
#define L2E_TDBESNP	10 /* Tag double bit error snoop */
#define L2E_TDBEL2I	11 /* Tag double bit error L2 initiated */
#define L2E_DSBEC0AD	12 /* Data single bit error cpu0 abort or discard */
#define L2E_DSBEC0NA	13 /* Data single bit error cpu0 no-abort */
#define L2E_DSBEC1AD	14 /* Data single bit error cpu1 abort or discard */
#define L2E_DSBEC1NA	15 /* Data single bit error cpu1 no-abort */
#define L2E_DSBESNP	16 /* Data single bit error snoop */
#define L2E_DSBEL2I	17 /* Data single bit error L2 initiated */
#define L2E_DDBEC0AD	18 /* Data double bit error cpu0 abort or discard */
#define L2E_DDBEC0NA	19 /* Data double bit error cpu0 no-abort */
#define L2E_DDBEC1AD	20 /* Data double bit error cpu1 abort or discard */
#define L2E_DDBEC1NA	21 /* Data double bit error cpu1 no-abort */
#define L2E_DDBESNP	22 /* Data double bit error snoop */
#define L2E_DDBEL2I	23 /* Data double bit error L2 initiated */
#define L2E_BMPCEC0AD	24 /* Bus master port command error cpu0 abort or discard */
#define L2E_BMPCEC0NA	25 /* Bus master port command error cpu0 no-abort */
#define L2E_BMPCEC1AD	26 /* Bus master port command error cpu1 abort or discard */
#define L2E_BMPCEC1NA	27 /* Bus master port command error cpu1 no-abort */
#define L2E_BMPCESNP	28 /* Bus master port command error snoop */
#define L2E_BMPCEL2I	29 /* Bus master port command error L2 initiated */
#define L2E_BMPDEC0AD	30 /* Bus master port data error cpu0 abort or discard */
#define L2E_BMPDEC0NA	31 /* Bus master port data error cpu0 no-abort */
#define L2E_BMPDEC1AD	32 /* Bus master port data error cpu1 abort or discard */
#define L2E_BMPDEC1NA	33 /* Bus master port data error cpu1 no-abort */

#define MAX_L2ES	34

#define ERR_S(err)	_BV(err)
#define ERR_ESEI(err)	_BV(((err) << 1) + 1)
#define ERR_ENMI(err)	_BV((err) << 1)

#ifndef __ASSEMBLER__
void l2csr_write(uint16_t csr, uint64_t val);
uint64_t l2csr_read(uint16_t csr);

#define err_nmi_enabled(csr, err)		(!!((csr) & ERR_ENMI(err)))
#define err_irq_enabled(csr, err)		(!!((csr) & ERR_ESEI(err)))
#define err_indicated(csr, err)			(!!((csr) & ERR_S(err)))

#if defined(CONFIG_VAISRA_RAS) && defined(CONFIG_SBI)
void sbi_process_vaisra_nmi(void);
#else
#define sbi_process_vaisra_nmi()	do { } while (0)
#endif
#endif /* __ASSEMBLER__ */

#endif /* __RAS_VAISRA_H_INCLUDE__ */
