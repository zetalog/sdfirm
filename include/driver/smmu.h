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
 * @(#)arm_smmu.h: ARM system memory management unit (SMMU) interfaces
 * $Id: arm_smmu.h,v 1.1 2020-02-20 16:54:00 zhenglv Exp $
 */

#ifndef __ARM_SMMU_H_INLCUDE__
#define __ARM_SMMU_H_INCLUDE__

#include <target/generic.h>
#include <target/irq.h>

/* Architecture specific definition needs to provide base address and
 * address space layout:
 * SMMU_BASE(n)
 * SMMU_HW_MAX_CTRLS
 * SMMU_HW_PAGESIZE
 * SMMU_HW_NUMPAGENDXB, NUMPAGES = 2^(NUMPAGENDXB+1)
 * SMMU_HW_NUMCB
 * SMMU_HW_IAS
 *
 * sdfirm implements inlined drivers, which requires soc specific
 * configurations via Kconfig, and ID register is useless then.
 */
#if SMMU_HW_PAGESIZE == 0x1000
#define SMMU_PAGESHIFT			12
#elif SMMU_HW_PAGESIZE == 0x10000
#define SMMU_PAGESHIFT			16
#else
#error "Unsupported SMMU_HW_PAGESIZE."
#endif
#define SMMU_NUMPAGES			(ULL(1) << (SMMU_HW_NUMPAGENDXB + 1))
#define SMMU_GLOBAL_SIZE		\
	(SMMU_HW_PAGESIZE << (SMMU_HW_NUMPAGENDXB + 1))
#define SMMU_CB_SIZE			\
	(SMMU_HW_PAGESIZE << (SMMU_HW_NUMPAGENDXB + 1))
#define __SMMU_CB_BASE(smmu)		(SMMU_BASE(smmu) + SMMU_GLOBAL_SIZE)
/* Context Bank */
#define SMMU_CB_BASE(smmu, n)		\
	(__SMMU_CB_BASE(smmu) + (n) * SMMU_HW_PAGESIZE)

/* Global Register Space 0 */
#define SMMU_GR0_BASE(smmu)		SMMU_BASE(smmu)
/* Global Register Space 1 */
#define SMMU_GR1_BASE(smmu)		(SMMU_GR0_BASE(smmu) + SMMU_HW_PAGESIZE)
/* Implementation Defined */
#define SMMU_GID_BASE(smmu)		(SMMU_GR1_BASE(smmu) + SMMU_HW_PAGESIZE)
/* Performance Monitor */
#define SMMU_PM_BASE(smmu)		(SMMU_GID_BASE(smmu) + SMMU_HW_PAGESIZE)
/* Security State Determination */
#define SMMU_SSD_BASE(smmu)		(SMMU_PM_BASE(smmu) + SMMU_HW_PAGESIZE)

#define SMMU_GR0_REG(smmu, offset)	(SMMU_GR0_BASE(smmu) + (offset))
#define SMMU_GR1_REG(smmu, offset)	(SMMU_GR1_BASE(smmu) + (offset))
#define SMMU_GID_REG(smmu, offset)	(SMMU_GID_BASE(smmu) + (offset))
#define SMMU_PM_REG(smmu, offset)	(SMMU_PM_BASE(smmu) + (offset))
#define SMMU_SSD_REG(smmu, offset)	(SMMU_SSD_BASE(smmu) + (offset))
#define SMMU_CB_REG(smmu, n, offset)	(SMMU_CB_BASE(smmu, n) + (offset))

/* Configuration Register 0 */
#define SMMU_sCR0(smmu)			SMMU_GR0_REG(smmu, 0x000)
/* Secure Configuration Register 1 */
#define SMMU_SCR1(smmu)			SMMU_GR0_REG(smmu, 0x004)
/* Configuration Register 2 */
#define SMMU_sCR1(smmu)			SMMU_GR0_REG(smmu, 0x008)
/* Auxiliary Configuration Register */
#define SMMU_sACR(smmu)			SMMU_GR0_REG(smmu, 0x010)
/* Identification Register 0 */
#define SMMU_IDR0(smmu)			SMMU_GR0_REG(smmu, 0x020)
/* Identification Register 1 */
#define SMMU_IDR1(smmu)			SMMU_GR0_REG(smmu, 0x024)
/* Identification Register 2 */
#define SMMU_IDR2(smmu)			SMMU_GR0_REG(smmu, 0x028)
/* Identification Register 3 */
#define SMMU_IDR3(smmu)			SMMU_GR0_REG(smmu, 0x02C)
/* Identification Register 4 */
#define SMMU_sIDR4(smmu)		SMMU_GR0_REG(smmu, 0x030)
/* Identification Register 5 */
#define SMMU_sIDR5(smmu)		SMMU_GR0_REG(smmu, 0x034)
/* Identification Register 6 */
#define SMMU_IDR6(smmu)			SMMU_GR0_REG(smmu, 0x038)
/* Identification Register 7 */
#define SMMU_IDR7(smmu)			SMMU_GR0_REG(smmu, 0x03C)
/* Global Fault Address Register */
#define SMMU_sGFAR(smmu)		SMMU_GR0_REG(smmu, 0x040) /* quad access */
/* Global Fault Status Register */
#define SMMU_sGFSR(smmu)		SMMU_GR0_REG(smmu, 0x048)
/* Global Fault Status Restore Register */
#define SMMU_sGFSRRESTORE(smmu)		SMMU_GR0_REG(smmu, 0x04C)
/* Global Fault Syndrome Register 0 */
#define SMMU_sGFSYNR0(smmu)		SMMU_GR0_REG(smmu, 0x050)
/* Global Fault Syndrome Register 1 */
#define SMMU_sGFSYNR1(smmu)		SMMU_GR0_REG(smmu, 0x054)
/* Global Fault Syndrome Register 2 */
#define SMMU_sGFSYNR2(smmu)		SMMU_GR0_REG(smmu, 0x058)
/* TLB Invalidate All */
#define SMMU_STLBIALL(smmu)		SMMU_GR0_REG(smmu, 0x060)
/* TLB Invalidate by VMID */
#define SMMU_TLBIVMID(smmu)		SMMU_GR0_REG(smmu, 0x064)
/* TLB Invalidate All Non-Secure Non-Hyp */
#define SMMU_TLBIALLNSNH(smmu)		SMMU_GR0_REG(smmu, 0x068)
/* TLB Invalidate All Hyp */
#define SMMU_TLBIALLH(smmu)		SMMU_GR0_REG(smmu, 0x06C)
/* Global Synchronize TLB Invalidate */
#define SMMU_sTLBGSYNC(smmu)		SMMU_GR0_REG(smmu, 0x070)
/* Global TLB Status register */
#define SMMU_sTLBGSTATUS(smmu)		SMMU_GR0_REG(smmu, 0x074)
/* Invalidate Hyp TLB by VA */
#define SMMU_TLBIVAH(smmu)		SMMU_GR0_REG(smmu, 0x078)
/* GAT Stage 1 Unprivileged Read */
#define SMMU_sGATS1UR(smmu)		SMMU_GR0_REG(smmu, 0x100)
/* GAT Stage 1 Unprivileged Write */
#define SMMU_sGATS1UW(smmu)		SMMU_GR0_REG(smmu, 0x108)
/* GAT Stage 1 Privileged Read */
#define SMMU_sGATS1PR(smmu)		SMMU_GR0_REG(smmu, 0x110)
/* GAT Stage 1 Privileged Write */
#define SMMU_sGATS1PW(smmu)		SMMU_GR0_REG(smmu, 0x118)
/* GAT Stage 1 and 2 Unprivileged Read */
#define SMMU_sGATS12UR(smmu)		SMMU_GR0_REG(smmu, 0x120)
/* GAT Stage 1 and 2 Unprivileged Write */
#define SMMU_sGATS12UW(smmu)		SMMU_GR0_REG(smmu, 0x128)
/* GAT Stage 1 and 2 Privileged Read */
#define SMMU_sGATS12PR(smmu)		SMMU_GR0_REG(smmu, 0x130)
/* GAT Stage 1 and 2 Privileged Write */
#define SMMU_sGATS12PW(smmu)		SMMU_GR0_REG(smmu, 0x138)
/* Global Physical Address Register */
#define SMMU_sGPAR(smmu)		SMMU_GR0_REG(smmu, 0x180) /* quad access */
/* Global Address Translation Status Register */
#define SMMU_sGATSR(smmu)		SMMU_GR0_REG(smmu, 0x188)
/* Secure alias for Non-secure copy */
#define SMMU_NSCR0(smmu)		SMMU_GR0_REG(smmu, 0x400)
#define SMMU_NSCR2(smmu)		SMMU_GR0_REG(smmu, 0x408)
#define SMMU_NSACR(smmu)		SMMU_GR0_REG(smmu, 0x410)
#define SMMU_NSGFAR(smmu)		SMMU_GR0_REG(smmu, 0x440) /* quad access */
#define SMMU_NSGFSR(smmu)		SMMU_GR0_REG(smmu, 0x448)
#define SMMU_NSGFSRRESTORE(smmu)	SMMU_GR0_REG(smmu, 0x44C)
#define SMMU_NSGFSYNR0(smmu)		SMMU_GR0_REG(smmu, 0x450)
#define SMMU_NSGFSYNR1(smmu)		SMMU_GR0_REG(smmu, 0x454)
#define SMMU_NSGFSYNR2(smmu)		SMMU_GR0_REG(smmu, 0x458)
#define SMMU_NSTLBGSYNC(smmu)		SMMU_GR0_REG(smmu, 0x470)
#define SMMU_NSTLBGSTATUS(smmu)		SMMU_GR0_REG(smmu, 0x474)
#define SMMU_NSGATS1UR(smmu)		SMMU_GR0_REG(smmu, 0x500)
#define SMMU_NSGATS1UW(smmu)		SMMU_GR0_REG(smmu, 0x508)
#define SMMU_NSGATS1PR(smmu)		SMMU_GR0_REG(smmu, 0x510)
#define SMMU_NSGATS1PW(smmu)		SMMU_GR0_REG(smmu, 0x518)
#define SMMU_NSGATS12UR(smmu)		SMMU_GR0_REG(smmu, 0x520)
#define SMMU_NSGATS12UW(smmu)		SMMU_GR0_REG(smmu, 0x528)
#define SMMU_NSGATS12PR(smmu)		SMMU_GR0_REG(smmu, 0x530)
#define SMMU_NSGATS12PW(smmu)		SMMU_GR0_REG(smmu, 0x538)
#define SMMU_NSGPAR(smmu)		SMMU_GR0_REG(smmu, 0x580) /* quad access */
#define SMMU_NSGTSR(smmu)		SMMU_GR0_REG(smmu, 0x584)
/* Stream Match Register */
#define SMMU_SMR(smmu, n)		SMMU_GR0_REG(smmu, 0x800 + ((n) << 2))
/* Stream-to-Context Register */
#define SMMU_S2CR(smmu, n)		SMMU_GR0_REG(smmu, 0xC00 + ((n) << 2))
/* Context Bank Attribute Registers */
#define SMMU_CBAR(smmu, n)		SMMU_GR1_REG(smmu, 0x000 + ((n) << 2))
/* Context Bank Fault Restricted Syndrome Register A */
#define SMMU_CBFRSYNRA(smmu, n)		SMMU_GR1_REG(smmu, 0x400 + ((n) << 2))
/* System Control Register */
#define SMMU_CB_SCTLR(smmu, n)		SMMU_CB_REG(smmu, n, 0x000)
/* Auxiliary Control Register */
#define SMMU_CB_ACTLR(smmu, n)		SMMU_CB_REG(smmu, n, 0x004)
/* Transaction Resume register */
#define SMMU_CB_RESUME(smmu, n)		SMMU_CB_REG(smmu, n, 0x008)
/* Translation Table Base Register 0 */
#define SMMU_CB_TTBR0(smmu, n)		SMMU_CB_REG(smmu, n, 0x020) /* quad access */
/* Translation Table Base Register 1 */
#define SMMU_CB_TTBR1(smmu, n)		SMMU_CB_REG(smmu, n, 0x028) /* quad access */
/* Translation Table Base Control Register */
#define SMMU_CB_TTBCR(smmu, n)		SMMU_CB_REG(smmu, n, 0x030)
#define SMMU_CB_TCR(smmu, n)		SMMU_CB_TTBCR(smmu, n)
/* Context Identification Register */
#define SMMU_CB_CONTEXTIDR(smmu, n)	SMMU_CB_REG(smmu, n, 0x034)
/* Memory Attribute Indirection Registers */
#define SMMU_CB_MAIR(smmu, n, attr)	\
	REG_4BIT_ADDR(SMMU_CB_BASE(smmu, n), attr)
/* Primary Region Remap Register */
#define SMMU_CB_PRRR(smmu, n)		SMMU_CB_REG(smmu, n, 0x038)
/* Normal Memory Remap Register */
#define SMMU_CB_NMRR(smmu, n)		SMMU_CB_REG(smmu, n, 0x03C)
/* Physical Address Register */
#define SMMU_CB_PAR(smmu, n)		SMMU_CB_REG(smmu, n, 0x050) /* quad access */
/* Fault Status Register */
#define SMMU_CB_FSR(smmu, n)		SMMU_CB_REG(smmu, n, 0x058)
/* Fault Status Restore Register */
#define SMMU_CB_FSRRESTORE(smmu, n)	SMMU_CB_REG(smmu, n, 0x05C)
/* Fault Address Register */
#define SMMU_CB_FAR(smmu, n)		SMMU_CB_REG(smmu, n, 0x060) /* quad access */
/* Fault Syndrome Register 0 */
#define SMMU_CB_FSYNR0(smmu, n)		SMMU_CB_REG(smmu, n, 0x068)
/* Fault Syndrome Register 1 */
#define SMMU_CB_FSYNR1(smmu, n)		SMMU_CB_REG(smmu, n, 0x06C)
/* Invalidate TLB by VA */
#define SMMU_CB_TLBIVA(smmu, n)		SMMU_CB_REG(smmu, n, 0x600)
/* TLB Invalidate by VA All ASID */
#define SMMU_CB_TLBIVAA(smmu, n)	SMMU_CB_REG(smmu, n, 0x608)
/* TLB Invalidate by ASID */
#define SMMU_CB_TLBIASID(smmu, n)	SMMU_CB_REG(smmu, n, 0x610)
/* TLB Invalidate All */
#define SMMU_CB_TLBIALL(smmu, n)	SMMU_CB_REG(smmu, n, 0x618)
/* TLB Invalidate by VA, Last level */
#define SMMU_CB_TLBIVAL(smmu, n)	SMMU_CB_REG(smmu, n, 0x620)
/* TLB Invalidate by VA, All ASID, Last level */
#define SMMU_CB_TLBIVAAL(smmu, n)	SMMU_CB_REG(smmu, n, 0x628)
/* TLB Synchronize Invalidate */
#define SMMU_CB_TLBSYNC(smmu, n)	SMMU_CB_REG(smmu, n, 0x7F0)
/* TLB Status */
#define SMMU_CB_TLBSTATUS(smmu, n)	SMMU_CB_REG(smmu, n, 0x7F4)
/* Address Translation Stage 1 Privileged Read */
#define SMMU_CB_ATS1PR(smmu, n)		SMMU_CB_REG(smmu, n, 0x800)
/* Address Translation Stage 1 Privileged Write */
#define SMMU_CB_ATS1PW(smmu, n)		SMMU_CB_REG(smmu, n, 0x808)
/* Address Translation Stage 1 Unprivileged Read */
#define SMMU_CB_ATS1UR(smmu, n)		SMMU_CB_REG(smmu, n, 0x810)
/* Address Translation Stage 1 Unprivileged Write */
#define SMMU_CB_ATS1UW(smmu, n)		SMMU_CB_REG(smmu, n, 0x818)
/* Address Translation Status Register */
#define SMMU_CB_ATSR(smmu, n)		SMMU_CB_REG(smmu, n, 0x8F0)

/* 10.6.1 SMMU_IDR0-7, Identification registers */
/* SMMU_IDR0 */
#define SMMU_SES			_BV(31)
#define SMMU_S1TS			_BV(30)
#define SMMU_S2TS			_BV(29)
#define SMMU_NTS			_BV(28)
#define SMMU_SMS			_BV(27)
#define SMMU_NUMIRPT_OFFSET		16
#define SMMU_NUMIRPT_MASK		REG_8BIT_MASK
#define SMMU_NUMIRPT(value)		_GET_FV(SMMU_NUMIRPT, value)
#define SMMU_CTTW			_BV(14)
#define SMMU_BTM			_BV(13)
#define SMMU_NUMSIDB_OFFSET		9
#define SMMU_NUMSIDB_MASK		REG_4BIT_MASK
#define SMMU_NUMSIDB(value)		_GET_FV(SMMU_NUMSIDB, value)
#define SMMU_NUMSMRG_OFFSET		0
#define SMMU_NUMSMRG_MASK		REG_8BIT_MASK
#define SMMU_NUMSMRG(value)		_GET_FV(SMMU_NUMSMRG, value)
/* SMMU_IDR1 */
#define SMMU_PAGESIZE			_BV(31)
#define SMMU_NUMPAGENDXB_OFFSET		28
#define SMMU_NUMPAGENDXB_MASK		REG_3BIT_MASK
#define SMMU_NUMPAGENDXB(value)		_GET_FV(SMMU_NUMPAGENDXB, value)
#define SMMU_NUMS2CB_OFFSET		16
#define SMMU_NUMS2CB_MASK		REG_8BIT_MASK
#define SMMU_NUMS2CB(value)		_GET_FV(SMMU_NUMS2CB, value)
#define SMMU_SMCD			_BV(15)
#define SMMU_SSDTP			_BV(12)
#define SMMU_NUMSSDNDXB_OFFSET		8
#define SMMU_NUMSSDNDXB_MASK		REG_4BIT_MASK
#define SMMU_NUMSSDNDXB(value)		_GET_FV(SMMU_NUMSSDNDXB, value)
#define SMMU_NUMCB_OFFSET		0
#define SMMU_NUMCB_MASK			REG_8BIT_MASK
#define SMMU_NUMCB(value)		_GET_FV(SMMU_NUMCB, value)
/* SMMU_IDR2 */
#define SMMU_IAS_OFFSET			0
#define SMMU_IAS_MASK			REG_4BIT_MASK
#define SMMU_IAS(value)			_GET_FV(SMMU_IAS, value)
#define SMMU_OAS_OFFSET			4
#define SMMU_OAS_MASK			REG_4BIT_MASK
#define SMMU_OAS(value)			_GET_FV(SMMU_OAS, value)
/* SMMU_IDR7 */
#define SMMU_MINOR_OFFSET		0
#define SMMU_MINOR_MASK			REG_4BIT_MASK
#define SMMU_MINOR(value)		_GET_FV(SMMU_MINOR, value)
#define SMMU_MAJOR_OFFSET		4
#define SMMU_MAJOR_MASK			REG_4BIT_MASK
#define SMMU_MAJOR(value)		_GET_FV(SMMU_MAJOR, value)

/* 10.6.2 SMMU_sACR, Auxiliary Configuration Register */

/* 10.6.3 SMMU_sCR0, Configuration Register 0 */
#define SMMU_NSCFG_OFFSET		28
#define SMMU_NSCFG_MASK			REG_2BIT_MASK
#define SMMU_NSCFG(value)		_SET_FV(SMMU_NSCFG, value) /* S */
#define SMMU_WACFG_OFFSET		26
#define SMMU_WACFG_MASK			REG_2BIT_MASK
#define SMMU_WACFG(value)		_SET_FV(SMMU_WACFG, value)
#define SMMU_RACFG_OFFSET		24
#define SMMU_RACFG_MASK			REG_2BIT_MASK
#define SMMU_RACFG(value)		_SET_FV(SMMU_RACFG, value)
#define SMMU_SHCFG_OFFSET		22
#define SMMU_SHCFG_MASK			REG_2BIT_MASK
#define SMMU_SHCFG(value)		_SET_FV(SMMU_SHCFG, value)
#define SMMU_SMCFCFG			_BV(21)
#define SMMU_MTCFG			_BV(20)
#define SMMU_MEMATTR_OFFSET		16
#define SMMU_MEMATTR_MASK		REG_4BIT_MASK
#define SMMU_MEMATTR(value)		_SET_FV(SMMU_MEMATTR, value)
#define SMMU_BSU_OFFSET			14
#define SMMU_BSU_MASK			REG_2BIT_MASK
#define SMMU_BSU(value)			_SET_FV(SMMU_BSU, value)
#define SMMU_BSU_NO_EFFECT		0
#define SMMU_BSU_INNER_SHAREABLE	1
#define SMMU_BSU_OUTER_SHAREABLE	2
#define SMMU_BSU_FULL_SYSTEM		3
#define SMMU_FB				_BV(13)
#define SMMU_PTM			_BV(12)
#define SMMU_VMIDPNE			_BV(11) /* NS */
#define SMMU_USFCFG			_BV(10)
#define SMMU_GSE			_BV(9)
#define SMMU_STALLD			_BV(8)
#define SMMU_TRANSIENTCFG_OFFSET	6
#define SMMU_TRANSIENTCFG_MASK		REG_2BIT_MASK
#define SMMU_TRANSIENTCFG(value)	_SET_FV(SMMU_TRANSIENTCFG, value)
#define SMMU_GCFGFIE			_BV(5)
#define SMMU_GCFGFRE			_BV(4)
#define SMMU_GFIE			_BV(2)
#define SMMU_GFRE			_BV(1)
#define SMMU_CLIENTPD			_BV(0)

/* 10.6.4 SMMU_sCR2, Configuration Register 2 */
#define SMMU_BPVMID_OFFSET		0
#define SMMU_BPVMID_MASK		REG_8BIT_MASK
#define SMMU_BPVMID(value)		_SET_FV(SMMU_BPVMID, value)

/* 10.6.5 SMMU_sGATS1PR, GAT Stage 1 Privileged Read
 * 10.6.6 SMMU_sGATS1PW, GAT Stage 1 Privileged Write
 * 10.6.7 SMMU_sGATS1UR, GAT Stage 1 Unprivileged Read
 * 10.6.8 SMMU_sGATS1UW, GAT Stage 1 Unprivileged Write
 * 10.6.9 SMMU_sGATS12PR, GAT Stages 1 and 2 Privileged Read
 * 10.6.10 SMMU_sGATS12PW, GAT Stages 1 and 2 Privileged Write
 * 10.6.11 SMMU_sGATS12UR, GAT Stages 1 and 2 Unprivileged Read
 * 10.6.12 SMMU_sGATS12UW, GAT Stages 1 and 2 Unprivileged Write
 * 15.5.2 SMMU_CBn_ATS1PR, Address Translation Stage 1 Privileged Read
 * 15.5.3 SMMU_CBn_ATS1PW, Address Translation Stage 1 Privileged Write
 * 15.5.4 SMMU_CBn_ATS1UR, Address Translation Stage 1 Unprivileged Read
 * 15.5.5 SMMU_CBn_ATS1UW, Address Translation Stage 1 Unprivileged Write
 */
#define SMMU_NDX_OFFSET			0
#define SMMU_NDX_MASK			REG_8BIT_MASK
#define SMMU_NDX(value)			_SET_FV(SMMU_NDX, value)
#define SMMU_ADDR_OFFSET		12
#define SMMU_ADDR_MASK			REG_20BIT_MASK
#define SMMU_ADDR(value)		_SET_FV(SMMU_ADDR, value)

/* 10.6.13 SMMU_sGATSR, Global Address Translation Status Register
 * 15.5.6 SMMU_CBn_ATSR, Address Translation Status Register
 */
#define SMMU_ACTIVE			_BV(0)

/* 10.6.14 SMMU_sGFAR, Global Fault Address Register
 * 15.5.8 SMMU_CBn_FAR, Fault Address Register
 */
#define SMMU_FADDR_OFFSET		0
#if SMMU_HW_IAS == 32
#define SMMU_FADDR_MASK			REG_32BIT_MASK
#elif SMMU_HW_IAS == 36
#define SMMU_FADDR_MASK			REG_36BIT_MASK
#elif SMMU_HW_IAS == 40
#define SMMU_FADDR_MASK			REG_40BIT_MASK
#elif SMMU_HW_IAS == 42
#define SMMU_FADDR_MASK			REG_42BIT_MASK
#elif SMMU_HW_IAS == 44
#define SMMU_FADDR_MASK			REG_44BIT_MASK
#elif SMMU_HW_IAS == 48
#define SMMU_FADDR_MASK			REG_48BIT_MASK
#else
#error "Missing SMMU_HW_IAS"
#endif
#define SMMU_FADDR(value)		_GET_FV_ULL(SMMU_FADDR, value)

/* 10.6.15 SMMU_sGFSR, Global Fault Status Register
 * 10.6.16 SMMU_sGFSRRESTORE, Global Fault Status Restore Register
 */
#define SMMU_MULTI			_BV(31)
#define SMMU_PF				_BV(7)
#define SMMU_EF				_BV(6)
#define SMMU_CAF			_BV(5)
#define SMMU_UCIF			_BV(4)
#define SMMU_UCBF			_BV(3)
#define SMMU_SMCF			_BV(2)
#define SMMU_USF			_BV(1)
#define SMMU_ICF			_BV(0)

/* 10.6.17 SMMU_sGFSYNR0, Global Fault Syndrome Register 0 */
#define SMMU_ATS			_BV(6)
#define SMMU_NSATTR			_BV(5)
#define SMMU_NSSTATE			_BV(4)
#define SMMU_IND			_BV(3)
#define SMMU_PNU			_BV(2)
#define SMMU_WNR			_BV(1)
#define SMMU_NESTED			_BV(0)

/* 10.6.19 SMMU_sGFSYNR2, Global Fault Syndrome Register 2 */

/* 10.6.20 SMMU_sGPAR, Global Physical Address Register
 * 15.5.14 SMMU_CBn_PAR, Physical Address Register
 */
/* Short-descriptor translation table format */
#define SMMU_S_PA_OFFSET		12
#define SMMU_S_PA_MASK			REG_20BIT_MASK
#define SMMU_S_PA(value)		_GET_FV_ULL(SMMU_S_PA, value)
#define SMMU_S_NOS			_BV_ULL(10)
#define SMMU_S_NS			_BV_ULL(9)
#define SMMU_S_IMP			_BV_ULL(8)
#define SMMU_S_SH			_BV_ULL(7)
#define SMMU_S_INNER_OFFSET		4
#define SMMU_S_INNER_MASK		REG_2BIT_MASK
#define SMMU_S_INNER(value)		_GET_FV_ULL(SMMU_S_INNER, value)
#define SMMU_S_OUTER_OFFSET		2
#define SMMU_S_OUTER_MASK		REG_2BIT_MASK
#define SMMU_S_OUTER(value)		_GET_FV_ULL(SMMU_S_OUTER, value)
#define SMMU_S_SS			_BV_ULL(1)
#define SMMU_S_F0			_BV_ULL(0)
/* Long-descriptor translation table format */
#define SMMU_L_MATTR_OFFSET		56
#define SMMU_L_MATTR_MASK		REG_8BIT_MASK
#define SMMU_L_MATTR(value)		_GET_FV_ULL(SMMU_L_MATTR, value)
#define SMMU_L_IMP			_BV_ULL(10)
#define SMMU_L_NS			_BV_ULL(9)
#define SMMU_L_SH_OFFSET		7
#define SMMU_L_SH_MASK			REG_2BIT_MASK
#define SMMU_L_SH(value)		_GET_FV_ULL(SMMU_L_SH, value)
#define SMMU_L_F0			_BV_ULL(0)
/* Fault format */
#define SMMU_F_STAGE			_BV_ULL(35)
#define SMMU_F_PLVL_OFFSET		32
#define SMMU_F_PLVL_MASK		REG_2BIT_MASK
#define SMMU_F_PLVL(value)		_GET_FV_ULL(SMMU_F_PLVL, value)
#define SMMU_F_ATOT			_BV_ULL(31)
#define SMMU_F_UCBF			_BV_ULL(30)
#define SMMU_F_ICF			_BV_ULL(29)
#define SMMU_F_TLBLKF			_BV_ULL(6)
#define SMMU_F_TLBMCF			_BV_ULL(5)
#define SMMU_F_EF			_BV_ULL(4)
#define SMMU_F_PF			_BV_ULL(3)
#define SMMU_F_AFF			_BV_ULL(2)
#define SMMU_F_TF			_BV_ULL(1)
#define SMMU_F_F1			_BV_ULL(0)

/* 10.6.21 SMMU_sTLBGSTATUS, Global TLB Status register
 * 15.5.36 SMMU_CBn_TLBSTATUS, TLB Status
 */
#define SMMU_SACTIVE			_BV(0)

/* 10.6.22 SMMU_sTLBGSYNC, Global Synchronize TLB Invalidate */

/* 10.6.23 SMMU_S2CRn, Stream-to-Context Register */
#define SMMU_S2CR_TRANSIENTCFG_OFFSET	28
#define SMMU_S2CR_TRANSIENTCFG_MASK	REG_2BIT_MASK
#define SMMU_S2CR_TRANSIENTCFG(value)	_SET_FV(SMMU_S2CR_TRANSIENTCFG, value)
#define SMMU_S2CR_TRANSIENT_DEFAULT	0
#define SMMU_S2CR_TRANSIENT_NON_TRAN	2
#define SMMU_S2CR_TRANSIENT_TRAN	3
#define SMMU_S2CR_INSTCFG_OFFSET	26
#define SMMU_S2CR_INSTCFG_MASK		REG_2BIT_MASK
#define SMMU_S2CR_INSTCFG(value)	_SET_FV(SMMU_S2CR_INSTCFG, value)
#define SMMU_S2CR_INST_DEFAULT		0
#define SMMU_S2CR_INST_DATA		2
#define SMMU_S2CR_INST_INSTRUCTION	3
#define SMMU_S2CR_PRIVCFG_OFFSET	24
#define SMMU_S2CR_PRIVCFG_MASK		REG_2BIT_MASK
#define SMMU_S2CR_PRIVCFG(value)	_SET_FV(SMMU_S2CR_PRIVCFG, value)
#define SMMU_S2CR_PRIV_DEFAULT		0
#define SMMU_S2CR_PRIV_UNPRIV		2
#define SMMU_S2CR_PRIV_PRIV		3
#define SMMU_S2CR_WACFG_OFFSET		22
#define SMMU_S2CR_WACFG_MASK		REG_2BIT_MASK
#define SMMU_S2CR_WACFG(value)		_SET_FV(SMMU_S2CR_WACFG, value)
#define SMMU_S2CR_RACFG_OFFSET		20
#define SMMU_S2CR_RACFG_MASK		REG_2BIT_MASK
#define SMMU_S2CR_RACFG(value)		_SET_FV(SMMU_S2CR_RACFG, value)
#define SMMU_S2CR_NSCFG_OFFSET		18
#define SMMU_S2CR_NSCFG_MASK		REG_2BIT_MASK
#define SMMU_S2CR_NSCFG(value)		_SET_FV(SMMU_S2CR_NSCFG, value)
#define SMMU_S2CR_TYPE_OFFSET		16
#define SMMU_S2CR_TYPE_MASK		REG_2BIT_MASK
#define SMMU_S2CR_TYPE(value)		_SET_FV(SMMU_S2CR_TYPE, value)
#define SMMU_S2CR_TYPE_TRANS		0
#define SMMU_S2CR_TYPE_BYPASS		1
#define SMMU_S2CR_TYPE_FAULT		2
#define SMMU_S2CR_MEMATTR_OFFSET	12
#define SMMU_S2CR_MEMATTR_MASK		REG_4BIT_MASK
#define SMMU_S2CR_MEMATTR(value)	_SET_FV(SMMU_S2CR_MEMATTR, value)
#define SMMU_S2CR_MTCFG			11
#define SMMU_S2CR_SHCFG_OFFSET		8
#define SMMU_S2CR_SHCFG_MASK		REG_2BIT_MASK
#define SMMU_S2CR_SHCFG(value)		_SET_FV(SMMU_S2CR_SHCFG, value)
#define SMMU_S2CR_CBNDX_OFFSET		0
#define SMMU_S2CR_CBNDX_MASK		REG_8BIT_MASK
#define SMMU_S2CR_CBNDX(value)		_SET_FV(SMMU_S2CR_CBNDX, value)
#define SMMU_S2CR_FB			_BV(26)
#define SMMU_S2CR_BSU_OFFSET		24
#define SMMU_S2CR_BSU_MASK		REG_2BIT_MASK
#define SMMU_S2CR_BSU(value)		_SET_FV(SMMU_S2CR_BSU, value)
#define SMMU_S2CR_VMID_OFFSET		0
#define SMMU_S2CR_VMID_MASK		REG_8BIT_MASK
#define SMMU_S2CR_VMID(value)		_SET_FV(SMMU_S2CR_VMID, value)
#ifdef CONFIG_SMMU_DISABLE_BYPASS
#define SMMU_S2CR_TYPE_INIT		SMMU_S2CR_TYPE_FAULT
#else
#define SMMU_S2CR_TYPE_INIT		SMMU_S2CR_TYPE_BYPASS
#endif

/* 10.6.24 SMMU_SCR1, Secure Configuration Register 1 */
#define SMMU_SPMEN			_BV(27)
#define SMMU_SIF			_BV(26)
#define SMMU_GASRAE			_BV(24)
#define SMMU_NSNUMIRPTO_OFFSET		16
#define SMMU_NSNUMIRPTO_MASK		REG_8BIT_MASK
#define SMMU_NSNUMIRPTO(value)		_SET_FV(SMMU_NSNUMIRPTO, value)
#define SMMU_NSNUMSMRGO_OFFSET		8
#define SMMU_NSNUMSMRGO_MASK		REG_8BIT_MASK
#define SMMU_NSNUMSMRGO(value)		_SET_FV(SMMU_NSNUMSMRGO, value)
#define SMMU_NSNUMCBO_OFFSET		0
#define SMMU_NSNUMCBO_MASK		REG_8BIT_MASK
#define SMMU_NSNUMCBO(value)		_SET_FV(SMMU_NSNUMCBO, value)

/* 10.6.25 SMMU_SMRn, Stream Match Register */
#define SMMU_SMR_VALID			_BV(31)
#define SMMU_SMR_MASK_OFFSET		16
#define SMMU_SMR_MASK_MASK		REG_15BIT_MASK
#define SMMU_SMR_MASK(value)		_SET_FV(SMMU_SMR_MASK, value)
#define smmu_smr_mask(value)		_GET_FV(SMMU_SMR_MASK, value)
#define SMMU_SMR_ID_OFFSET		0
#define SMMU_SMR_ID_MASK		REG_15BIT_MASK
#define SMMU_SMR_ID(value)		_SET_FV(SMMU_SMR_ID, value)
#define smmu_smr_id(value)		_GET_FV(SMMU_SMR_ID, value)

/* 10.6.26 SMMU_STLBIALL, TLB Invalidate All */

/* 10.6.27 SMMU_TLBIALLH, TLB Invalidate All Hyp */

/* 10.6.28 SMMU_TLBIALLNSNH, TLB Invalidate All Non-Secure Non-Hyp */

/* 10.6.29 SMMU_TLBIVAH, Invalidate Hyp TLB by VA */
#define SMMU_ADDRESS_OFFSET		12
#define SMMU_ADDRESS_MASK		REG_20BIT_MASK
#define SMMU_ADDRESS(value)		_SET_FV(SMMU_ADDRESS, value)_

/* 11.2.1 SMMU_CBARn, Context Bank Attribute Registers */
#define SMMU_CBAR_IRPTNDX_OFFSET	24
#define SMMU_CBAR_IRPTNDX_MASK		REG_8BIT_MASK
#define SMMU_CBAR_IRPTNDX(value)	_SET_FV(SMMU_CBAR_IRPTNDX, value)
#define SMMU_CBAR_SBZ_OFFSET		18
#define SMMU_CBAR_SBZ_MASK		REG_2BIT_MASK
#define SMMU_CBAR_SBZ(value)		_SET_FV(SMMU_CBAR_SBZ, value)
#define SMMU_CBAR_TYPE_OFFSET		16
#define SMMU_CBAR_TYPE_MASK		REG_2BIT_MASK
#define SMMU_CBAR_TYPE(value)		_SET_FV(SMMU_CBAR_TYPE, value)
#define SMMU_CBAR_TYPE_S2_TRANS			0
#define SMMU_CBAR_TYPE_S1_TRANS_S2_BYPASS	1
#define SMMU_CBAR_TYPE_S1_TRANS_S2_FAULT	2
#define SMMU_CBAR_TYPE_S1_TRANS_S2_TRANS	3
#define SMMU_CBAR_VMID_OFFSET		0
#define SMMU_CBAR_VMID_MASK		REG_8BIT_MASK
#define SMMU_CBAR_VMID(value)		_SET_FV(SMMU_CBAR_VMID, value)
#define SMMU_CBAR_WACFG_OFFSET		22
#define SMMU_CBAR_WACFG_MASK		REG_2BIT_MASK
#define SMMU_CBAR_WACFG(value)		_SET_FV(SMMU_CBAR_WACFG, value)
#define SMMU_CBAR_RACFG_OFFSET		20
#define SMMU_CBAR_RACFG_MASK		REG_2BIT_MASK
#define SMMU_CBAR_RACFG(value)		_SET_FV(SMMU_CBAR_RACFG, value)
#define SMMU_CBAR_BSU_OFFSET		18
#define SMMU_CBAR_BSU_MASK		REG_2BIT_MASK
#define SMMU_CBAR_BSU(value)		_SET_FV(SMMU_CBAR_BSU, value)
#define SMMU_CBAR_MEMATTR_OFFSET	12
#define SMMU_CBAR_MEMATTR_MASK		REG_4BIT_MASK
#define SMMU_CBAR_MEMATTR(value)	_SET_FV(SMMU_CBAR_MEMATTR, value)
#define SMMU_MEM_DEVICE			0
#define SMMU_MEM_NORMAL_NC		1
#define SMMU_MEM_NORMAL_WT		2
#define SMMU_MEM_NORMAL_WB		3
#define SMMU_CBAR_FB			_BV(11)
#define SMMU_CBAR_HYPC			_BV(10)
#define SMMU_CBAR_BPSHCFG_OFFSET	8
#define SMMU_CBAR_BPSHCFG_MASK		REG_2BIT_MASK
#define SMMU_CBAR_BPSHCFG(value)	_SET_FV(SMMU_CBAR_BPSHCFG, value)
#define SMMU_OUTER_SHAREABLE		1
#define SMMU_INNER_SHAREABLE		2
#define SMMU_NON_SHAREABLE		3
#define SMMU_CBAR_CBNDX_OFFSET		8
#define SMMU_CBAR_CBNDX_MASK		REG_8BIT_MASK
#define SMMU_CBAR_CBNDX(value)		_SET_FV(SMMU_CBAR_CBNDX, value)

/* 15.5.1 SMMU_CBn_ACTLR, Auxiliary Control Register */

/* 15.5.7 SMMU_CBn_CONTEXTIDR, Context Identification Register */
/* SMMU_CBn_TTBCR.EAE=0 */
#define SMMU_ASID_OFFSET		0
#define SMMU_ASID_MASK			REG_8BIT_MASK
#define SMMU_ASID(value)		_SET_FV(SMMU_ASID, value)
#define SMMU_PROCID_OFFSET		8
#define SMMU_PROCID_MASK		REG_24BIT_MASK
#define SMMU_PROCID(value)		_SET_FV(SMMU_PROCID, value)
/* SMMU_CBn_TTBCR.EAE=1 */
/*
#define SMMU_PROCID_OFFSET		0
#define SMMU_PROCID_MASK		REG_32BIT_MASK
#define SMMU_PROCID(value)		_SET_FV(SMMU_PROCID, value)
*/

/* 15.5.9 SMMU_CBn_FSR, Fault Status Register
 * 15.5.10 SMMU_CBn_FSRRESTORE, Fault Status Restore Register
 */
#define SMMU_C_MULTI			_BV(31)
#define SMMU_C_SS			_BV(30)
#define SMMU_C_TLBLKF			_BV(6)
#define SMMU_C_TLBMCF			_BV(5)
#define SMMU_C_EF			_BV(4)
#define SMMU_C_PF			_BV(3)
#define SMMU_C_AFF			_BV(2)
#define SMMU_C_TF			_BV(1)

#define SMMU_FSR_IGN						\
	(SMMU_C_AFF | SMMU_C_ASF | SMMU_C_TLBMCF | SMMU_C_TLBLKF)
#define SMMU_FSR_FAULT						\
	(SMMU_C_MULTI | SMMU_C_SS | SMMU_C_UUT | SMMU_C_EF |	\
	 SMMU_C_PF | SMMU_C_TF | SMMU_FSR_IGN)

/* 15.5.11 SMMU_CBn_FSYNRm, Fault Syndrome Registers */
#define SMMU_C_S1CBNDX_OFFSET		16
#define SMMU_C_S1CBNDX_MASK		REG_8BIT_MASK
#define SMMU_C_S1CBNDX(value)		_GET_FV(SMMU_C_S1CBNDX, value)
#define SMMU_C_AFR			_BV(11)
#define SMMU_C_PTWF			_BV(10)
#define SMMU_C_NSATTR			_BV(8)
#define SMMU_C_IND			_BV(6)
#define SMMU_C_PNU			_BV(5)
#define SMMU_C_WNR			_BV(4)
#define SMMU_C_PLVL_OFFSET		0
#define SMMU_C_PLVL_MASK		REG_2BIT_MASK
#define SMMU_C_PLVL(value)		_GET_FV(SMMU_C_PLVL, value)

/* 15.5.12 SMMU_CBn_MAIRm, Memory Attribute Indirection Registers */
#define SMMU_ATTR_MASK			REG_4BIT_MASK
#define SMMU_ATTR_OFFSET(attr)		REG_4BIT_OFFSET(attr)
#define SMMU_ATTR(attr, value)		\
	_SET_FVn(attr, SMMU_ATTR, (value) & SMMU_ATTR_MASK)

/* 15.5.13 SMMU_CBn_NMRR, Normal Memory Remap Register */
#define SMMU_IR_MASK			REG_2BIT_MASK
#define SMMU_IR_OFFSET(ir)		REG_2BIT_OFFSET(ir)
#define SMMU_IR(__ir, value)		\
	_SET_FVn(__ir, SMMU_IR, (value) & SMMU_IR_MASK)
#define SMMU_OR(__or, value)		SMMU_IR(__or + 8, value)

/* 15.5.27 SMMU_CBn_PRRR, Primary Region Remap Register */
#define SMMU_TR_MASK			REG_2BIT_MASK
#define SMMU_TR_OFFSET(tr)		REG_2BIT_OFFSET(tr)
#define SMMU_TR(__tr, value)		\
	_SET_FVn(__tr, SMMU_TR, (value) & SMMU_TR_MASK)
#define SMMU_DS(__ds)			_BV((__ds) + 16)
#define SMMU_NS(__ns)			_BV((__ns) + 18)
#define SMMU_NOS(__nos)			_BV((__nos) + 24)

/* 15.5.28 SMMU_CBn_RESUME, Transaction Resume register */
#define SMMU_TNR			_BV(0)

/* 15.5.29 SMMU_CBn_SCTLR, System Control Register */
#define SMMU_C_NSCFG_OFFSET		28
#define SMMU_C_NSCFG_MASK		REG_2BIT_MASK
#define SMMU_C_NSCFG(value)		_SET_FV(SMMU_NSCFG, value)
#define SMMU_C_WACFG_OFFSET		26
#define SMMU_C_WACFG_MASK		REG_2BIT_MASK
#define SMMU_C_WACFG(value)		_SET_FV(SMMU_WACFG, value)
#define SMMU_C_RACFG_OFFSET		24
#define SMMU_C_RACFG_MASK		REG_2BIT_MASK
#define SMMU_C_RACFG(value)		_SET_FV(SMMU_RACFG, value)
#define SMMU_C_SHCFG_OFFSET		22
#define SMMU_C_SHCFG_MASK		REG_2BIT_MASK
#define SMMU_C_SHCFG(value)		_SET_FV(SMMU_SHCFG, value)
#define SMMU_C_MTCFG			_BV(20)
#define SMMU_C_MEMATTR_OFFSET		16
#define SMMU_C_MEMATTR_MASK		REG_4BIT_MASK
#define SMMU_C_MEMATTR(value)		_SET_FV(SMMU_MEMATTR, value)
#define SMMU_C_TRANSIENTCFG_OFFSET	14
#define SMMU_C_TRANSIENTCFG_MASK	REG_2BIT_MASK
#define SMMU_C_TRANSIENTCFG(value)	_SET_FV(SMMU_C_TRANSIENTCFG, value)
#define SMMU_C_ASIDPNE			_BV(12)
#define SMMU_C_UWXN			_BV(10)
#define SMMU_C_WXN			_BV(9)
#define SMMU_C_HUPCF			_BV(8)
#define SMMU_C_CFCFG			_BV(7)
#define SMMU_C_CFIE			_BV(6)
#define SMMU_C_CFRE			_BV(5)
#define SMMU_C_E			_BV(4)
#define SMMU_C_AFFD			_BV(3)
#define SMMU_C_AFE			_BV(2)
#define SMMU_C_TRE			_BV(1)
#define SMMU_C_M			_BV(0)

/* 15.5.30 SMMU_CBn_TLBIALL, TLB Invalidate All */

/* 15.5.31 SMMU_CBn_TLBIASID, TLB Invalidate by ASID
 * 15.5.32 SMMU_CBn_TLBIVA, Invalidate TLB by VA
 * 15.5.33 SMMU_CBn_TLBIVAA, TLB Invalidate by VA All ASID
 * 15.5.34 SMMU_CBn_TLBIVAAL, TLB Invalidate by VA, All ASID, Last level
 * 15.5.35 SMMU_CBn_TLBIVAL, TLB Invalidate by VA, Last level
 */
#define SMMU_32_ASID_OFFSET		0
#define SMMU_32_ASID_MASK		REG_8BIT_MASK
#define SMMU_32_ASID(value)		_SET_FV(SMMU_32_ASID, value)
#define SMMU_32_VA_OFFSET		SMMU_CB_VA_SHIFT
#define SMMU_32_VA_MASK			REG_20BIT_MASK
#define SMMU_32_VA(value)		_SET_FV(SMMU_32_VA, value)
#define SMMU_CB_VA_SHIFT		12

/* 15.5.38 SMMU_CBn_TTBCR, Translation Table Base Control Register */
/* SMMU_CBn_TTBCR.EAE=0 */
#define SMMU_C_EAE			_BV(31)
#define SMMU_C_NSCFG1			_BV(30)
#define SMMU_C_NSCFG0			_BV(14)
#define SMMU_C_PD1			_BV(5)
#define SMMU_C_PD0			_BV(4)
#define SMMU_C_SBZ			_BV(3)
#define SMMU_C_T0SZ_OFFSET		0
#define SMMU_C_T0SZ_MASK		REG_3BIT_MASK
#define SMMU_C_T0SZ(value)		_SET_FV(SMMU_C_T0SZ, value)
/* SMMU_CBn_TTBCR.EAE=1 */
#define SMMU_C_SH1_OFFSET		28
#define SMMU_C_SH1_MASK			REG_2BIT_MASK
#define SMMU_C_SH1(value)		_SET_FV(SMMU_C_SH1, value)
#define SMMU_C_ORGN1_OFFSET		26
#define SMMU_C_ORGN1_MASK		REG_2BIT_MASK
#define SMMU_C_ORGN1(value)		_SET_FV(SMMU_C_ORGN1, value)
#define SMMU_C_IRGN1_OFFSET		24
#define SMMU_C_IRGN1_MASK		REG_2BIT_MASK
#define SMMU_C_IRGN1(value)		_SET_FV(SMMU_C_IRGN1, value)
#define SMMU_C_EPD1			_BV(23)
#define SMMU_C_A1			_BV(22)
#define SMMU_C_T1SZ_OFFSET		16
#define SMMU_C_T1SZ_MASK		REG_3BIT_MASK
#define SMMU_C_T1SZ(value)		_SET_FV(SMMU_C_T1SZ, value)
#define SMMU_C_SH0_OFFSET		12
#define SMMU_C_SH0_MASK			REG_2BIT_MASK
#define SMMU_C_SH0(value)		_SET_FV(SMMU_C_SH0, value)
#define SMMU_C_ORGN0_OFFSET		10
#define SMMU_C_ORGN0_MASK		REG_2BIT_MASK
#define SMMU_C_ORGN0(value)		_SET_FV(SMMU_C_ORGN0, value)
#define SMMU_C_IRGN0_OFFSET		8
#define SMMU_C_IRGN0_MASK		REG_2BIT_MASK
#define SMMU_C_IRGN0(value)		_SET_FV(SMMU_C_IRGN0, value)
#define SMMU_C_EPD0			_BV(7)

/* 15.5.39 SMMU_CBn_TTBRm, Translation Table Base Registers */
/* Short-descriptor translation table format */

/* Long-descriptor translation table format */

#define smmu_enable()				\
	__raw_clearl(SMMU_CLIENTPD, SMMU_sCR0(iommu_dev))
#define smmu_disable()				\
	__raw_setl(SMMU_CLIENTPD, SMMU_sCR0(iommu_dev))

#define smmu_get_global_fault()			\
	__raw_readl(SMMU_sGFSR(iommu_dev))
#define smmu_set_global_fault(fault)		\
	__raw_writel(fault, SMMU_sGFSR(iommu_dev))
#define smmu_clear_global_fault()		\
	smmu_set_global_fault(smmu_get_global_fault())

#define smmu_write_s2cr(type, cbndx, priv, valid)			\
	do {								\
		uint32_t reg = SMMU_S2CR_TYPE(type) |			\
			       SMMU_S2CR_CBNDX(cbndx) |			\
			       SMMU_S2CR_PRIVCFG(priv);			\
		if (valid &&						\
		    smmu_device_ctrl.features &	SMMU_FEAT_EXIDS)	\
			reg |= SMMU_S2CR_EXIDVALID;			\
		__raw_writel(reg, SMMU_S2CR(iommu_dev, smmu_gr));	\
	} while (0)
#ifdef CONFIG_ARCH_IS_SMMU_SM
#ifdef SMMU_HW_NUMSMRG
#define smmu_max_smrgs(id)						\
	do {								\
		smmu_device_ctrl.features |= SMMU_FEAT_STREAM_MATCH;	\
		smmu_device_ctrl.max_streams = SMMU_HW_NUMSMRG;		\
	} while (0)
#else
#define smmu_max_smrgs(id)						\
	do {								\
		if ((id) & SMMU_SMS) {					\
			smmu_device_ctrl.features |=			\
				SMMU_FEAT_STREAM_MATCH;			\
			smmu_device_ctrl.max_streams = SMMU_NUMSMRG(id);\
		}							\
	} while (0)
#endif
#ifdef CONFIG_SMMU_SMCD
#define smmu_probe_smcd(id)						\
	do {								\
		smmu_device_ctrl.features |= SMMU_FEAT_SMCD;		\
	} while (0)
#else
#define smmu_probe_smcd(id)		do { } while (0)
#endif
#define smmu_write_smr(id, mask, valid)					\
	do {								\
		uint32_t reg = SMMU_SMR_ID(id) | SMMU_SMR_MASK(mask);	\
		if (valid &&						\
		    smmu_device_ctrl.features & SMMU_FEAT_EXIDS)	\
			reg |= SMMU_SMR_VALID;				\
		__raw_writel(reg, SMMU_SMR(iommu_dev, smmu_gr));	\
	} while (0)
/* The width of SMR's mask field depends on sCR0_EXIDENABLE, so this function
 * should be called after sCR0 is written.
 *
 * SMR.ID bits may not be preserved if the corresponding MASK bits are set,
 * so check each one separately. We can reject masters later if they try to
 * claim IDs outside these masks.
 */
#define smmu_test_smr(smr)						\
	do {								\
		__raw_writel(smr, SMMU_SMR(iommu_dev, smmu_gr));	\
		smr = __raw_readl(SMMU_SMR(iommu_dev, smmu_gr));	\
	} while (0)
#define smmu_test_smr_masks()						\
	do {								\
		uint32_t smr;						\
		iommu_grp_t sgrp;					\
		sgrp = iommu_group_save(NR_IOMMU_GROUPS);		\
		smr = SMMU_SMR_ID(smmu_device_ctrl.streamid_mask);	\
		smmu_test_smr(smr);					\
		smmu_device_ctrl.streamid_mask = smmu_smr_id(smr);	\
		smr = SMMU_SMR_MASK(smmu_device_ctrl.streamid_mask);	\
		smmu_test_smr(smr);					\
		smmu_device_ctrl.smr_mask_mask = smmu_smr_mask(smr);	\
		iommu_group_restore(sgrp);				\
	} while (0)
#else
#define smmu_max_smrgs(id)		do { } while (0)
#define smmu_probe_smcd(id)		do { } while (0)
#define smmu_write_smr(id, mask, valid)	do { } while (0)
#define smmu_test_smr_masks()		do { } while (0)
#endif

typedef uint16_t smmu_gr_t; /* smmu stream id */
typedef uint8_t smmu_cb_t; /* smmu context bank id */
typedef iommu_t smmu_sme_t; /* iommu device and smmu stream mapping group */
#define SMMU_SME_MASK(iommu, gr, sm)	MAKELLONG(MAKELONG(gr, sm), iommu)
#define SMMU_SME(iommu, gr)		SMMU_SME_MASK(iommu, gr, 0)
#define smmu_sme_dev(sme)		((iommu_dev_t)HIDWORD(sme))
#define smmu_sme_gr(sme)		LOWORD(LODWORD(sme))
#define smmu_sme_sm(sme)		HIWORD(LODWORD(sme))

#define SMMU_MAX_CBS			128
#define INVALID_SMMU_CB			SMMU_MAX_CBS
#define INVALID_SMMU_SME		SMMU_SME(INVALID_IOMMU_DEV, 0)

struct smmu_device {
	uint32_t features;
#define SMMU_FEAT_TRANS_S1		(1 << 0)
#define SMMU_FEAT_TRANS_S2		(1 << 1)
#define SMMU_FEAT_TRANS_NESTED		(1 << 2)
#define SMMU_FEAT_TRANS_OPS		(1 << 3)
#ifdef CONFIG_ARCH_HAS_SMMU_PTFS_v8_4KB
#define SMMU_FEAT_PTFS_ARCH64_4K	(1 << 4)
#else
#define SMMU_FEAT_PTFS_ARCH64_4K	0
#endif
#ifdef CONFIG_ARCH_HAS_SMMU_PTFS_v8_16KB
#define SMMU_FEAT_PTFS_ARCH64_16K	(1 << 5)
#else
#define SMMU_FEAT_PTFS_ARCH64_16K	0
#endif
#if defined(CONFIG_ARCH_HAS_SMMUv1_64KB) || \
    defined(CONFIG_ARCH_HAS_SMMU_PTFS_v8_64KB)
#define SMMU_FEAT_PTFS_ARCH64_64K	(1 << 6)
#else
#define SMMU_FEAT_PTFS_ARCH64_64K	0
#endif
#ifdef CONFIG_ARCH_HAS_SMMU_PTFS_LONG
#define SMMU_FEAT_PTFS_ARCH32_L		(1 << 7)
#else
#define SMMU_FEAT_PTFS_ARCH32_L		0
#endif
#ifdef CONFIG_ARCH_HAS_SMMU_PTFS_SHORT
#define SMMU_FEAT_PTFS_ARCH32_S		(1 << 8)
#else
#define SMMU_FEAT_PTFS_ARCH32_S		0
#endif
#ifdef CONFIG_ARCH_HAS_SMMU_PTFS_SV48
#define SMMU_FEAT_PTFS_RISCV_SV48	(1 << 9)
#else
#define SMMU_FEAT_PTFS_RISCV_SV48	0
#endif
#ifdef CONFIG_ARCH_HAS_SMMU_PTFS_SV39
#define SMMU_FEAT_PTFS_RISCV_SV39	(1 << 10)
#else
#define SMMU_FEAT_PTFS_RISCV_SV39	0
#endif
#ifdef CONFIG_ARCH_HAS_SMMU_PTFS_SV32
#define SMMU_FEAT_PTFS_RISCV_SV32	(1 << 11)
#else
#define SMMU_FEAT_PTFS_RISCV_SV32	0
#endif
#define SMMU_FEAT_EXIDS			(1 << 12)
#define SMMU_FEAT_STREAM_MATCH		(1 << 13)
#define SMMU_FEAT_COHERENT_WALK		(1 << 14)
#define SMMU_FEAT_VMID16		(1 << 15)
#define SMMU_FEAT_HARDWARE_ACCESS	(1 << 16)
#define SMMU_FEAT_HARDWARE_DIRTY	(1 << 17)
	int max_streams;
	uint16_t streamid_mask;
	uint16_t smr_mask_mask;
	unsigned int numpage;
	unsigned int pgshift;
	uint32_t max_s1_cbs;
	uint32_t max_s2_cbs;
	uint32_t max_context_irqs;
	uint8_t ipa_size;
	uint8_t opa_size;
	uint8_t va_size;

	smmu_gr_t gr;
	smmu_cb_t cb;
	DECLARE_BITMAP(context_map, SMMU_MAX_CBS);
#if 0
	struct arm_smmu_cb		*cbs;
	atomic_t			irptndx;
	u32				num_global_irqs;
#endif
};

struct smmu_stream {
	iommu_grp_t grp;

	/* S2CR entry */
	int count;
	uint32_t s2cr_type : 2;
	uint32_t s2cr_priv : 2;
/*	smmu_cb_t cb; */

	/* SMR entry */
	smmu_sme_t sme;
};

struct arm_smmu_cb {
	uint64_t ttbr[2];
	uint32_t tcr[2];
	uint32_t mair[2];
};

struct smmu_context {
	uint8_t fmt;
#define SMMU_CONTEXT_NONE	0
#define SMMU_CONTEXT_AARCH64	(1 << 0)
#define SMMU_CONTEXT_AARCH32_L	(1 << 1)
#define SMMU_CONTEXT_AARCH32_S	(1 << 2)
#define SMMU_CONTEXT_RISCV_SV48	(1 << 3)
#define SMMU_CONTEXT_RISCV_SV39	(1 << 4)
#define SMMU_CONTEXT_RISCV_SV32	(1 << 5)
	uint8_t stage;
#define SMMU_DOMAIN_BYPASS	0
#define SMMU_DOMAIN_S1		1
#define SMMU_DOMAIN_S2		2
#define SMMU_DOMAIN_NESTED	3
	union {
		uint16_t asid;
#ifdef CONFIG_ARCH_HAS_SMMU_S2
		uint16_t vmid;
#endif
	};
	smmu_cb_t cb;
	irq_t irpt;
	/* CBAR */
	uint8_t type;
};

#if 0
#define ARM_SMMU_CB_TCR2		0x10
#define TCR2_SEP			GENMASK(17, 15)
#define TCR2_SEP_UPSTREAM		0x7
#define TCR2_AS				_BV(4)
#define ARM_SMMU_CB_TCR			0x30

#define INVALID_IRPTNDX			0xff

struct arm_smmu_domain {
	bool non_strict;
	uint8_t irptndx;
};
#endif

#ifdef SMMU_HW_TRANS
#define smmu_trans_regimes(id)						\
	(smmu_device_ctrl.features |= SMMU_HW_TRANS)
#else
#define smmu_trans_regimes(id)						\
	do {								\
		if ((id) & SMMU_S1TS)					\
			smmu_device_ctrl.features |= SMMU_FEAT_TRANS_S1;\
		if ((id) & SMMU_S2TS)					\
			smmu_device_ctrl.features |= SMMU_FEAT_TRANS_S2;\
		if ((id) & SMMU_NTS)					\
			smmu_device_ctrl.features |=			\
				SMMU_FEAT_TRANS_NESTED;			\
	} while (0)
#endif
#ifdef CONFIG_SMMU_CTTW
#define smmu_probe_cttw(id)						\
	do {								\
		if ((id) & SMMU_CTTW)					\
			smmu_device_ctrl.features |=			\
				SMMU_FEAT_COHERENT_WALK;		\
	} while (0)
#else
#define smmu_probe_cttw(id)						\
	do { } while (0)
#endif
#ifdef SMMU_HW_PAGESIZE
#define smmu_page_size(id)						\
	do {								\
		smmu_device_ctrl.pgshift = SMMU_PAGESHIFT;		\
	} while (0)
#else
#error "Firmware shouldn't support dynamic PAGE_SIZE."
#define smmu_page_size(id)						\
	do {								\
		smmu_device_ctrl.pgshift =				\
			((id) & SMMU_PAGESIZE) ? 16 : 12;		\
	} while (0)
#endif
#ifdef SMMU_HW_NUMPAGENDXB
#define smmu_max_pages(id)						\
	do {								\
		smmu_page_size(id);					\
		smmu_device_ctrl.numpage = SMMU_NUMPAGES;		\
	} while (0)
#else
#error "Firmware shouldn't support dynamic PAGE_INDEX."
#define smmu_max_pages(id)						\
	do {								\
		smmu_page_size(id);					\
		smmu_device_ctrl.numpage =				\
			_BV(SMMU_NUMPAGENDXB(id) + 1);			\
	} while (0)
#endif
#ifdef SMMU_HW_NUMCB
#define smmu_max_s1cbs(id)						\
	do {								\
		smmu_device_ctrl.max_s1_cbs = SMMU_HW_NUMCB;		\
	} while (0)
#else
#error "Firmware shouldn't support dynamic S1CB."
#define smmu_max_s1cbs(id)						\
	do {								\
		smmu_device_ctrl.max_s1_cbs = SMMU_NUMCB(id);		\
	} while (0)
#endif
#ifdef SMMU_HW_NUMS2CB
#define smmu_max_s2cbs(id)						\
	do {								\
		smmu_device_ctrl.max_s2_cbs = SMMU_HW_NUMS2CB;		\
	} while (0)
#else
#define smmu_max_s2cbs(id)						\
	do {								\
		smmu_device_ctrl.max_s2_cbs = SMMU_NUMS2CB(id);		\
	} while (0)
#endif
#define smmu_max_cbs(id)						\
	do {								\
		smmu_max_s1cbs(id);					\
		smmu_max_s2cbs(id);					\
	} while (0)
#define smmu_size2bits(as)						\
	((as) < 3 : 32 + (as) << 2 ? ((as) > 3 ? 28 + (as) << 2 : 42))
#ifdef SMMU_HW_IAS
#define smmu_ipa_size(id)						\
	do {								\
		smmu_device_ctrl.ipa_size = SMMU_HW_IAS;		\
	} while (0)
#else
#define smmu_ipa_size(id)						\
	do {								\
		smmu_device_ctrl.ipa_size =				\
			smmu_size2bits(SMMU_IAS(id));			\
	} while (0)
#endif
#ifdef SMMU_HW_OAS
#define smmu_opa_size(id)						\
	do {								\
		smmu_device_ctrl.opa_size = SMMU_HW_OAS;		\
	} while (0)
#else
#define smmu_opa_size(id)						\
	do {								\
		smmu_device_ctrl.opa_size =				\
			smmu_size2bits(SMMU_OAS(id));			\
	} while (0)
#endif
#define smmu_pa_sizes(id)						\
	do {								\
		smmu_ipa_size(id);					\
		smmu_opa_size(id);					\
	} while (0)

#define arm_smmu_32_tlb_inv_range_s1(idx, asid, iova, size, gran, reg)	\
	do {								\
		(iova) = SMMU_32_VA((iova) >> SMMU_CB_VA_SHIFT) |	\
			 SMMU_32_ASID(asid);				\
		do {							\
			__raw_writel(iova,				\
				     reg((iommu_dev), (smmu_cb)));	\
			(iova) += (gran);				\
		} while ((size) -= (gran));				\
	} while (0)

#define arm_smmu_32_tlb_inv_range_s2(iova, size, gran, reg)		\
	do {								\
		(iova) >>= SMMU_CB_VA_SHIFT;				\
		do {							\
			__raw_writel((iova),				\
				     reg((iommu_dev), (smmu_cb)));	\
			(iova) += (gran) >> 12;				\
		} while ((size) -= (gran));				\
	} while (0)

void smmu_gr_restore(smmu_gr_t gr);
smmu_gr_t smmu_gr_save(smmu_gr_t gr);
#define smmu_gr			smmu_device_ctrl.gr
void smmu_cb_restore(smmu_cb_t cb);
smmu_cb_t smmu_cb_save(smmu_cb_t cb);
#define smmu_cb			smmu_device_ctrl.cb

extern struct smmu_device smmu_devices[];
extern struct smmu_stream smmu_streams[];
extern struct smmu_context smmu_contexts[];
#define smmu_device_ctrl		smmu_devices[iommu_dev]
#define smmu_stream_ctrl		smmu_streams[iommu_grp]
#define smmu_context_ctrl		smmu_contexts[iommu_dom]

void smmu_tlb_sync_global(void);
void smmu_tlb_sync_context(void);
void smmu_tlb_inv_context_s1(void);
void smmu_tlb_inv_walk_s1(unsigned long iova, size_t size, size_t granule);
void smmu_tlb_inv_leaf_s1(unsigned long iova, size_t size, size_t granule);
void smmu_tlb_add_page_s1(unsigned long iova, size_t granule);
#ifdef CONFIG_ARCH_HAS_SMMU_S2
void smmu_tlb_inv_context_s2(void);
void smmu_tlb_inv_walk_s2(unsigned long iova, size_t size, size_t granule);
void smmu_tlb_inv_leaf_s2(unsigned long iova, size_t size, size_t granule);
void smmu_tlb_add_page_s2(unsigned long iova, size_t granule);
void smmu_tlb_inv_any_s2_v1(unsigned long iova, size_t size, size_t granule);
void smmu_tlb_add_page_s2_v1(unsigned long iova, size_t granule);
#endif

#ifdef CONFIG_SMMU
iommu_grp_t smmu_alloc_sme(smmu_sme_t sme);
void smmu_free_sme(iommu_grp_t grp);
smmu_cb_t smmu_alloc_cb(smmu_cb_t start, smmu_cb_t end);
void smmu_free_cb(smmu_cb_t cb);
#else
#define smmu_alloc_sme(grp, sme)		INVALID_IOMMU_GRP
#define smmu_free_sme(grp)			do { } while (0)
#endif

void smmu_group_select(void);
void smmu_domain_select(void);

void smmu_device_exit(void);
void smmu_device_init(void);

#endif /* __ARM_SMMU_H_INCLUDE__ */
