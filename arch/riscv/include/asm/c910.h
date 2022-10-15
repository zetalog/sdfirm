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
 * @(#)c910.h: C910 CSR and initialization definitions
 * $Id: c910.h,v 1.1 2023-20-20 18:48:00 zhenglv Exp $
 */

#ifndef __CSR_C910_H_INCLUDE__
#define __CSR_C910_H_INCLUDE__

#define CSR_MXSTATUS	0x7C0
#define CSR_MHCR	0x7C1
#define CSR_MCOR	0x7C2
#define CSR_MCCR2	0x7C3
#define CSR_MHINT	0x7C5
#define CSR_MSMPR	0x7F3

/* 18.1.9.1 MXSTATUS */
#define XSR_PMDU	_AC(0x00000400, UL) /* user pmcount disable */
#define XSR_PMDS	_AC(0x00000800, UL) /* supervisor pmcount disable */
#define XSR_PMDM	_AC(0x00002000, UL) /* machine pmcount disable */
#define XSR_PMP4K	_AC(0x00004000, UL) /* PMP granularity, fixed 4K for c910 */
#define XSR_MM		_AC(0x00008000, UL) /* unaligned memory access */
#define XSR_UCME	_AC(0x00010000, UL) /* user cache management enable */
#ifdef CONFIG_CLINT_THEAD
#define XSR_CLINTEE	_AC(0x00020000, UL) /* supervisor clint SI/TI */
#else /* CONFIG_CLINT_THEAD */
#define XSR_CLINTEE	_AC(0x00000000, UL)
#endif /* CONFIG_CLINT_THEAD */
#define XSR_MHRD	_AC(0x00040000, UL) /* hardware TLB refill disable */
#define XSR_INSDE	_AC(0x00080000, UL) /* snoop d$ on I$ miss disable */
#define XSR_MAEE	_AC(0x00200000, UL) /* MMU attribute ext. enable */
#define XSR_THEADISAEE	_AC(0x00400000, UL) /* T-head C908 ISA ext. enable */
#define XSR_PM_U	_AC(0x00000000, UL) /* current user mode */
#define XSR_PM_S	_AC(0x40000000, UL) /* current supervisor mode */
#define XSR_PM_M	_AC(0xC0000000, UL) /* current machine mode */

/* 18.1.9.2 MHCR */
#define HCR_IE		_AC(0x00000001, UL) /* ICache enable */
#define HCR_DE		_AC(0x00000002, UL) /* DCache enable */
#define HCR_WA		_AC(0x00000004, UL) /* write allocate */
#define HCR_WB		_AC(0x00000008, UL) /* write back */
#define HCR_RS		_AC(0x00000010, UL) /* address return stack */
#define HCR_BPE		_AC(0x00000020, UL) /* branch predictor */
#define HCR_BTB		_AC(0x00000040, UL) /* branch target buffer */
#define HCR_IBPE	_AC(0x00000080, UL) /* indirect branch predictor */
#define HCR_WBR		_AC(0x00000100, UL) /* write burst */
#define HCR_L0BTB	_AC(0x00001000, UL) /* L0 BTB */
#define HCR_SCK		_AC(0x00004000, UL) /* system processor clock ratio */

#define HCR_C_EN	(HCR_IE | HCR_DE | HCR_WA | HCR_WB)
#define HCR_BP_EN	(HCR_RS | HCR_BPE | HCR_BTB | HCR_IBPE | HCR_L0BTB)

/* 18.1.9.3 MCOR */
#define COR_IS		_AC(0x00000001, UL) /* select ICache */
#define COR_DS		_AC(0x00000002, UL) /* select DCache */
#define COR_INV		_AC(0x00000010, UL) /* invalidate cache */
#define COR_CLR		_AC(0x00000020, UL) /* clear cache */
#define COR_BHT_INV	_AC(0x00010000, UL) /* BHT invalidate */
#define COR_BTB_INV	_AC(0x00020000, UL) /* BTB invalidate */
#define COR_IBP_INV	_AC(0x00040000, UL) /* IBP invalidate */
#define COR_C_INV	(COR_IS | COR_DS | COR_INV | COR_CLR)
#define COR_BP_INV	(COR_BHT_INV | COR_BTB_INV | COR_IBP_INV)

/* 18.1.9.4 MCCR2 */
#define CCR2_RFE	_AC(0x00000001, UL) /* read L2Cache refill enable */
#define CCR2_ECCEN	_AC(0x00000002, UL) /* L2Cache ECC enable */
#define CCR2_L2EN	_AC(0x00000008, UL) /* L2Cache enable */
#define CCR2_DLTNCY(x)	(_AC(x, UL) << 16)  /* data ram latency */
#define CCR2_DSETUP	_AC(0x00080000, UL) /* data ram extra setup cycle */
#define CCR2_TLTNCY(x)	(_AC(x, UL) << 22)  /* tag ram latency */
#define CCR2_TSETUP	_AC(0x02000000, UL) /* tag ram extra setup cycle */
#define CCR2_IPRF	_AC(0x60000000, UL) /* instruction prefech lines */
#define CCR2_IPRF_OFF	_AC(0x00000000, UL)
#define CCR2_IPRF_1	_AC(0x20000000, UL)
#define CCR2_IPRF_2	_AC(0x40000000, UL)
#define CCR2_IPRF_3	_AC(0x60000000, UL)
#define CCR2_TPRF	_AC(0x80000000, UL) /* TLB prefetch */

/* 18.1.9.6 MHINT */
#define HNT_DPLD	_AC(0x00000004, UL) /* DCache prefetch enable */
#define HNT_AMR		_AC(0x00000018, UL) /* Auto write allocate policy */
#define HNT_AMR_WA	_AC(0x00000000, UL) /* on WA */
#define HNT_AMR_3	_AC(0x00000008, UL) /* on 3 cache-lines */
#define HNT_AMR_15	_AC(0x00000010, UL) /* on 15 cache-lines */
#define HNT_AMR_63	_AC(0x00000018, UL) /* on 63 cache-lines */
#define HNT_IPLD	_AC(0x00000100, UL) /* ICache prefetch enable */
#define HNT_LPE		_AC(0x00000200, UL) /* loop accel enable */
#define HNT_IWPE	_AC(0x00000400, UL) /* ICache way predictor enable */
#define HNT_SRE		_AC(0x00000800, UL) /* single retire enable */
#define HNT_D_DIS	_AC(0x00006000, UL) /* DCache prefech lines */
#define HNT_D_DIS_2	_AC(0x00000000, UL)
#define HNT_D_DIS_4	_AC(0x00002000, UL)
#define HNT_D_DIS_8	_AC(0x00004000, UL)
#define HNT_D_DIS_16	_AC(0x00006000, UL)
#define HNT_L2PLD	_AC(0x00008000, UL) /* L2 prefech enable */
#define HNT_L2_DIS	_AC(0x00030000, UL)
#define HNT_L2_DIS_8	_AC(0x00000000, UL)
#define HNT_L2_DIS_16	_AC(0x00010000, UL)
#define HNT_L2_DIS_32	_AC(0x00020000, UL)
#define HNT_L2_DIS_64	_AC(0x00030000, UL)
#define HNT_NO_SPEC	_AC(0x00040000, UL) /* SPEC fail prefetch enable */
#define HNT_ECC		_AC(0x00080000, UL) /* L1 ECC enable */
#define HNT_L2STPLD	_AC(0x00100000, UL) /* L2 cache store prefetch enable */
#define HNT_TLB_BRAOD_DIS	_AC(0x00200000, UL)
#define HNT_FENCERW_BROAD_DIS	_AC(0x00400000, UL)
#define HNT_FENCEI_BROAD_DIS	_AC(0x00800000, UL)

#define HNT_PLD		(HNT_DPLD | HNT_IPLD | HNT_L2PLD)

/* 18.1.14.1 MSMPR */
#define SMPR_SMPEN	_AC(0x00000001, UL) /* SMP snpper enable */

#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
	.macro	c910_smp_init
	li	x3, (COR_C_INV | COR_BP_INV)
	csrs	CSR_MCOR, x3
	li	x3, (HCR_C_EN | HCR_BP_EN | HCR_WBR)
	csrs	CSR_MHCR, x3
#if 0
	li	x3, 0x7E30C
	li	x3, 0x6E308
#endif
	li	x3, (HNT_PLD | HNT_LPE | HNT_AMR_3 | HNT_D_DIS_16 | HNT_L2_DIS_32 | HNT_NO_SPEC)
	csrs	CSR_MHINT, x3
	li	x3, (CCR2_TPRF | CCR2_IPRF_3 | CCR2_L2EN | CCR2_RFE)
	csrs	CSR_MCCR2, x3
	csrsi	CSR_MSMPR, SMPR_SMPEN
	.endm
	.macro	c910_init
	li	x3, ECR_PBMTE
	csrs	CSR_MENVCFG, x3
	/* MSTATUS */
	li	x3, (SR_FS | SR_VS)
	csrs	CSR_MSTATUS, x3
	/* MXSTATUS */
	li	x3, (XSR_MM | XSR_CLINTEE)
	csrs	CSR_MXSTATUS, x3
	li	x3, (XSR_MAEE | XSR_THEADISAEE)
	csrc	CSR_MXSTATUS, x3
	.endm
#endif /* __ASSEMBLY__ */

#endif /* __CSR_C910_H_INCLUDE__ */
