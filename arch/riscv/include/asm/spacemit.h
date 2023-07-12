/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)spacemit.h: SpacemiT programming model definitions
 * $Id: spacemit.h,v 1.1 2023-5-30 14:43:00 zhenglv Exp $
 */

#ifndef __CSR_SPACEMIT_H_INCLUDE__
#define __CSR_SPACEMIT_H_INCLUDE__

/* spacemit programming model */

#define CSR_MSETUP	0x7C0
#define CSR_MCPM	0x7C1
#define CSR_MRAOP	0x7C2
#define CSR_MSWITCH	0x7C3
#define CSR_MBUSERR	0x7CC
#define CSR_ML2SETUP	0x7F0
#define CSR_ML2BUSERR	0x7F1
#define CSR_ML2HINT     0x7F7
#define CSR_MPCFIFO	0xFD0
#define CSR_MDBGINFO1	0xFD1
#define CSR_MDBGINFO2	0xFD2
#define CSR_MDBGINFO3	0xFD3

#define CSR_SL2PID	0x5F0
#define CSR_SL2PART	0x5F1

/* MSETUP */
#define STP_DE		_AC(0x00000001, UL) /* D-cache enable */
#define STP_IE		_AC(0x00000002, UL) /* I-cache enable */
#define STP_BPE		_AC(0x00000010, UL) /* branch predictor enable */
#define STP_PRFE	_AC(0x00000020, UL) /* prefetcher enable */
#define STP_MM		_AC(0x00000040, UL) /* unalignment memory access */
#define STP_ECC		_AC(0x00010000, UL) /* ECC enable */

/* MCPM */
#define CPM_AIEE	_AC(0x00000001, UL) /* AI extension */
#define CPM_VECEE	_AC(0x00000002, UL) /* vector extension */
#define CPM_BF16	_AC(0x0000000100000000, UL) /* BF16 extension */
#define CPM_ZKTE	_AC(0x0000000200000000, UL) /* Zkt extension */
#define CPM_ZICONDEE	_AC(0x0000000400000000, UL) /* Zicond extension */

/* MRAOP */
#define RAOP_DC		_AC(0x00000001, UL) /* D-cache all clean */
#define RAOP_DI		_AC(0x00000002, UL) /* D-cache all invalidate */
#define RAOP_DF		(RAOP_DC | RAOP_DI) /* D-cache all flush */
#define RAOP_BI		_AC(0x00000010, UL) /* BHT/BTB invalidate */
#define RAOP_II		_AC(0x00000011, UL) /* I-cache invalidate */

/* MSWITCH */
#define SWT_SL2PART	_AC(0x00000001, UL) /* S-mode allow L2 partition */

/* MBUSERR */
#define BER_ERR		_AC(0x8000000000000000, UL) /* bus error valid */
#define BER_ST		_AC(0x0200000000000000, UL) /* store error valid */
#define BER_LD		_AC(0x0100000000000000, UL) /* load error valid */

/* ML2SETUP */
#define L2S_SMP(cpu)	(_AC(0x1, UL) << (cpu)) /* snoop enable */
#define L2S_SMP_MASK	(L2S_SMP(MAX_CPU_NUM) - 1)
#define L2S_ECC		_AC(0x00000100, UL) /* ECC enable */
#define L2S_PAE		_AC(0x00000200, UL)
#define L2S_IPRF	_AC(0x00030000, UL)
#define L2S_TPRF	_AC(0x00040000, UL)

/* ML2BUSERR */
#define L2BER_ERR	_AC(0x8000000000000000, UL) /* bus error valid */
#define L2BER_CORE	_AC(0x0300000000000000, UL)
#define L2BER_INFO	_AC(0x00000003, UL)

#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
	.macro	spacemit_smp_init
	/* spacemit specific */
	li	x3, STP_DE | STP_IE | STP_BPE | STP_PRFE | STP_MM
	csrs	CSR_MSETUP, x3
	.endm
	.macro	spacemit_init
#ifdef CONFIG_K1M_S2C_SPEEDUP_QUIRK_RDTIME
	li	x3, 0
	csrs	CSR_MCOUNTEREN, x3
#endif
	li	x3, ECR_PBMTE | ECR_STCE
	csrs	CSR_MENVCFG, x3
	/* MSTATUS */
	li	x3, (SR_FS | SR_VS)
	csrs	CSR_MSTATUS, x3
	/* spacemit specific */
	li	x3, L2S_SMP_MASK
	csrs	CSR_ML2SETUP, x3
	.endm
#endif /* __ASSEMBLY__ */

#endif /* __CSR_SPACEMIT_H_INCLUDE__ */
