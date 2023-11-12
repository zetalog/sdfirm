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
 * @(#)arm_smmuv2.h: ARM system mmu v2 (SMMUv2) interfaces
 * $Id: arm_smmuv2.h,v 1.1 2020-02-21 08:29:00 zhenglv Exp $
 */

#ifndef __ARM_SMMUv2_H_INLCUDE__
#define __ARM_SMMUv2_H_INCLUDE__

#include <target/generic.h>
#include <driver/arm_smmu.h>

/* Secure TLB Invalidate MONC by VA, Last level */
#define SMMU_STLBIVALM(smmu)		SMMU_GR0_REG(smmu, 0x0A0)
/* Secure TLB Invalidate MONC by VA */
#define SMMU_STLBIVAM(smmu)		SMMU_GR0_REG(smmu, 0x0A8)
/* Invalidate Hyp TLB by VA, Last level, AArch64 */
#define SMMU_TLBIVALH64(smmu)		SMMU_GR0_REG(smmu, 0x0B0)
/* TLB Invalidate Stage 1 by VMID */
#define SMMU_TLBIVMIDS1(smmu)		SMMU_GR0_REG(smmu, 0x0B8)
/* Secure TLB Invalidate All MONC */
#define SMMU_STLBIALLM(smmu)		SMMU_GR0_REG(smmu, 0x0BC)
/* Invalidate Hyp TLB by VA, AArch64 */
#define SMMU_TLBIVAH64(smmu)		SMMU_GR0_REG(smmu, 0x0C0)
/* Context Bank Attribute Registers */
#define SMMU_CBA2R(smmu, n)		SMMU_GR1_REG(smmu, 0x800 + ((n) << 2))
/* Translation Control Register 2 */
#define SMMU_TCR2(smmu, n)		SMMU_CB_REG(smmu, n, 0x010)

/* 9.6.1 SMMU_IDR0-7, Identification registers */
/* SMMU_IDR0 */
#define SMMU_ATOSNS			_BV(26)
#define SMMU_PTFS_OFFSET		24
#define SMMU_PTFS_MASK			REG_2BIT_MASK
#define SMMU_PTFS(value)		_GET_FV(SMMU_PTFS, value)
#define SMMU_PTFS_ARCH32_S_L		0x00
#define SMMU_PTFS_ARCH32_S		0x01
#define SMMU_PTFS_ARCH32_N		0x02
#define SMMU_PTFS_NO_ARCH32_L		_BV(24)
#define SMMU_PTFS_NO_ARCH32_S		_BV(25)
#define SMMU_EXSMRGS_OFFSET		16
#define SMMU_EXSMRGS_MASK		REG_8BIT_MASK
#define SMMU_EXSMRGS(value)		_GET_FV(SMMU_EXSMRGS, value)
#define SMMU_EXIDS			_BV(8)
/* SMMU_IDR1 */
#define SMMU_HAFDBS_OFFSET		24
#define SMMU_HAFDBS_MASK		REG_2BIT_MASK
#define SMMU_HAFDBS(value)		_GET_FV(SMMU_HAFDBS, value)
#define SMMU_HAFDBS_ACCESS		0x1
#define SMMU_HAFDBS_DIRTY		0x2 /* set if SMMU_HAFDBS_ACCESS set */
/* SMMU_IDR2 */
#define SMMU_DIPANS			_BV(30)
#define SMMU_COMPINDEXS			_BV(29)
#define SMMU_HADS			_BV(28)
#define SMMU_E2HS			_BV(27)
#define SMMU_EXNUMSMRG_OFFSET		16
#define SMMU_EXNUMSMRG_MASK		REG_11BIT_MASK
#define SMMU_EXNUMSMRG(value)		_GET_FV(SMMU_EXNUMSMRG, value)
#define SMMU_VMID16S			_BV(15)
#define SMMU_PTFSV8_64KB		_BV(14)
#define SMMU_PTFSV8_16KB		_BV(13)
#define SMMU_PTFSV8_4KB			_BV(12)
#define SMMU_UBS_OFFSET			8
#define SMMU_UBS_MASK			REG_4BIT_MASK
#define SMMU_UBS(value)			_GET_FV(SMMU_UBS, value)

/* 9.6.3 SMMU_sCR0, Configuration Register 0 */
#define SMMU_VMID16EN			_BV(31) /* NS */
#define SMMU_HYPMODE			_BV(30) /* NS */
#define SMMU_EXIDENABLE			_BV(3)

/* 9.6.4 SMMU_sCR2, Configuration Register 2 */
#define SMMU_EXSMRGENABLE		_BV(31)
#define SMMU_EXNSSMRGDISABLE		_BV(30) /* S */
#define SMMU_COMPINDEXENABLE		_BV(29)
#define SMMU_EXNSNUMSMRGO_OFFSET	16
#define SMMU_EXNSNUMSMRGO_MASK		REG_11BIT_MASK
#define SMMU_EXNSNUMSMRGO(value)	\
	_SET_FV(SMMU_EXNSNUMSMRGO, value) /* S */

/* 9.6.15 SMMU_sGFSR, Global Fault Status Register
 * 9.6.16 SMMU_sGFSRRESTORE, Global Fault Status Restore Register
 */
#define SMMU_UUT			_BV(8)

/* 9.6.18 SMMU_sGFSYNR1, Global Fault Syndrome Register 1
 * 10.2.3 SMMU_CBFRSYNRAn, Context Bank Fault Restricted Syndrome Register A
 */
#define SMMU_STREAMID_OFFSET		0
#define SMMU_STREAMID_MASK		REG_16BIT_MASK
#define SMMU_STREAMID(value)		_GET_FV(SMMU_STREAMID, value)
#define SMMU_SSD_INDEX_OFFSET		16
#define SMMU_SSD_INDEX_MASK		REG_16BIT_MASK
#define SMMU_SSD_INDEX(value)		_GET_FV(SMMU_SSD_INDEX, value)

/* 9.6.20 SMMU_sGPAR, Global Physical Address Register */
/* AArch32 Long-descriptor and AArch64 translation schemes */
#define SMMU_L_PA_OFFSET		12
#define SMMU_L_PA_MASK			REG_36BIT_MASK
#define SMMU_L_PA(value)		_GET_FV_ULL(SMMU_L_PA, value)
/* Fault format */
#define SMMU_F_FORMAT_OFFSET		8
#define SMMU_F_FORMAT_MASK		REG_2BIT_MASK
#define SMMU_F_FORMAT(value)		_SET_FV_ULL(SMMU_F_FORMAT, value)
#define SMMU_F_ASF			_BV(7)

/* 9.6.23 SMMU_S2CRn, Stream-to-Context Register */
/* Translation context */
#define SMMU_S2CR_EXIDVALID		_BV(10)
#define SMMU_S2CR_PRIV_DIPAN		1

/* 9.6.24 SMMU_SCR1, Secure Configuration Register 1 */
#define SMMU_NSCOMPINDEXDISABLE		_BV(29)
#define SMMU_NSCAFNFS			_BV(28)
#define SMMU_GEFFS			_BV(25)
#define SMMU_GEFRO			SMMU_GEFFS

/* 9.6.25 SMMU_SMRn, Stream Match Register */
/* SMMU_sCR0.EXIDENABLE==1 */
#define SMMU_SMR_EXID_OFFSET		0
#define SMMU_SMR_EXID_MASK		REG_16BIT_MASK
#define SMMU_SMR_EXID(value)		_SET_FV(SMMU_SMR_EXID, value)
#define SMMU_SMR_EXMASK_OFFSET		16
#define SMMU_SMR_EXMASK_MASK		REG_16BIT_MASK
#define SMMU_SMR_EXMASK(value)		_SET_FV(SMMU_SMR_EXMASK, value)

/* 9.6.27 SMMU_STLBIALLM, Secure TLB Invalidate All MONC */

/* 9.6.30 SMMU_STLBIVALM, Secure TLB Invalidate MONC by VA, Last level
 * 9.6.31 SMMU_STLBIVAM, Secure TLB Invalidate MONC by VA
 * 9.6.33 SMMU_TLBIVAH64, Invalidate Hyp TLB by VA, AArch64
 * 9.6.34 SMMU_TLBIVALH64, Invalidate Hyp TLB by VA, Last level, AArch64
 */
#define SMMU_64_ADDRESS_OFFSET		0
#define SMMU_64_ADDRESS_MASK		REG_44BIT_MASK
#define SMMU_64_ADDRESS(value)		_SET_FV_ULL(SMMU_64_ADDRESS, value)
#define SMMU_64_ASID_OFFSET		48
#define SMMU_64_ASID_MASK		REG_16BIT_MASK
#define SMMU_64_ASID(value)		_SET_FV_ULL(SMMU_64_ASID, value)

/* 9.6.35 SMMU_TLBIVMID, TLB Invalidate by VMID
 * 9.6.36 SMMU_TLBIVMIDS1, TLB Invalidate Stage 1 by VMID
 */
#define SMMU_VMID_OFFSET		0
#define SMMU_VMID_MASK			REG_16BIT_MASK
#define SMMU_VMID(value)		_SET_FV(SMMU_VMID, value)

/* 10.2.1 SMMU_CBARn, Context Bank Attribute Registers */
#define SMMU_CBAR_E2HC			_BV(10)

/* 10.2.2 SMMU_CBA2Rn, Context Bank Attribute Registers */
#define SMMU_CBA2R_VMID16_OFFSET	16
#define SMMU_CBA2R_VMID16_MASK		REG_16BIT_MASK
#define SMMU_CBA2R_VMID16(value)	_SET_FV(SMMU_CBA2R_VMID16, value)
#define SMMU_CBA2R_MONC			_BV(1)
#define SMMU_CBA2R_VA64			_BV(0)

/* 16.5.9 SMMU_CBn_FSR, Fault Status Register */
#define SMMU_C_FORMAT_OFFSET		9
#define SMMU_C_FORMAT_MASK		REG_2BIT_MASK
#define SMMU_C_FORMAT(value)		_SET_FV(SMMU_C_FORMAT, value)
#define SMMU_C_UUT			_BV(8)
#define SMMU_C_ASF			_BV(7)

#define SMMU_FSR_IGN						\
	(SMMU_C_AFF | SMMU_C_ASF | SMMU_C_TLBMCF | SMMU_C_TLBLKF)
#define SMMU_FSR_FAULT						\
	(SMMU_C_MULTI | SMMU_C_SS | SMMU_C_UUT | SMMU_C_EF |	\
	 SMMU_C_PF | SMMU_C_TF | SMMU_FSR_IGN)

#ifdef SMMU_HW_TRANS
#define smmu_trans_ops(id)						\
	(smmu_device_ctrl.features |= SMMU_HW_TRANS)
#else
#define smmu_trans_ops(id)						\
	do {								\
		if (((id) & SMMU_S1TS) && !((id) & SMMU_ATOSNS))	\
			smmu_device_ctrl.features |=			\
				SMMU_FEAT_TRANS_OPS;			\
	} while (0)
#endif
#ifdef SMMU_HW_PTFS
#define smmu_ptfs_arch32(id)						\
	(smmu_device_ctrl.features |= SMMU_HW_PTFS)
#else
#define smmu_ptfs_arch32(id)						\
	do {								\
		if (!((id) & SMMU_PTFS_NO_ARCH32_L)) {			\
			smmu_device_ctrl.features |=			\
				SMMU_FEAT_PTFS_ARCH32_L;		\
			if (!((id) & SMMU_PTFS_NO_ARCH32_S))		\
				smmu_device_ctrl.features |=		\
					SMMU_FEAT_PTFS_ARCH32_S;	\
		}							\
	} while (0)
#endif
#ifdef SMMU_HW_PTFS
#define smmu_ptfs_arch64(id)						\
	(smmu_device_ctrl.features |= SMMU_HW_PTFS)
#else
#define smmu_ptfs_arch64(id)						\
	do {								\
		if ((id) & SMMU_PTFSV8_4KB)				\
			smmu_device_ctrl.features |=			\
				SMMU_FEAT_PTFS_ARCH64_4K;		\
		if ((id) & SMMU_PTFSV8_16KB)				\
			smmu_device_ctrl.features |=			\
				SMMU_FEAT_PTFS_ARCH64_16K;		\
		if ((id) & SMMU_PTFSV8_64KB)				\
			smmu_device_ctrl.features |=			\
				SMMU_FEAT_PTFS_ARCH64_64K;		\
	} while (0)
#endif
#ifdef SMMU_HW_NUMSIDB
#if SMMU_HW_NUMSIDB == 16
#define smmu_max_streams(id)						\
	do {								\
		smmu_device_ctrl.features |= SMMU_FEAT_EXIDS;		\
		smmu_device_ctrl.max_streams = _BV(16);			\
		smmu_device_ctrl.streamid_mask =			\
			smmu_device_ctrl.max_streams - 1;		\
	} while (0)
#else
#define smmu_max_streams(id)						\
	do {								\
		smmu_device_ctrl.max_streams = _BV(SMMU_HW_NUMSIDB);	\
		smmu_device_ctrl.streamid_mask =			\
			smmu_device_ctrl.max_streams - 1;		\
	} while (0)
#endif
#else
#define smmu_max_streams(id)						\
	do {								\
		if ((id) & SMMU_EXIDS) {				\
			smmu_device_ctrl.features |= SMMU_FEAT_EXIDS;	\
			smmu_device_ctrl.max_streams = _BV(16);		\
		} else							\
			smmu_device_ctrl.max_streams =			\
				_BV(SMMU_NUMSIDB(id));			\
		smmu_device_ctrl.streamid_mask =			\
			smmu_device_ctrl.max_streams - 1;		\
	} while (0)
#endif
#ifdef CONFIG_SMMU_HFADBS
#define smmu_probe_hafdbs(id)						\
	do {								\
		uint8_t hafdbs = SMMU_HAFDBS(id);			\
		if (hafdbs & SMMU_HAFDBS_ACCESS) {			\
			smmu_device_ctrl.features |=			\
				SMMU_FEAT_HARDWARE_ACCESS;		\
			if (hafdbs & SMMU_HAFDBS_DIRTY)			\
				smmu_device_ctrl.features |=		\
					SMMU_FEAT_HARDWARE_DIRTY;	\
		}							\
	} while (0)
#else
#define smmu_probe_hafdbs(id)						\
	do { } while (0)
#endif
#ifdef SMMU_HW_UBS
#define smmu_va_size(id)						\
	do {								\
		smmu_device_ctrl.va_size = SMMU_HW_UBS;			\
	} while (0)
#else
#define smmu_va_size(id)						\
	do {								\
		smmu_device_ctrl.va_size =				\
			arm_smmu_size2bits(SMMU_UBS(id));		\
	} while (0)
#endif
#ifdef CONFIG_ARCH_HAS_SMMU_VMID16
#define smmu_probe_vmid(id)						\
	do {								\
		smmu_device_ctrl.features |= SMMU_FEAT_VMID16;		\
	} while (0)
#else
#define smmu_probe_vmid(id)						\
	do {								\
		if ((id) & SMMU_VMID16S)				\
			smmu_device_ctrl.features |= SMMU_FEAT_VMID16;	\
	} while (0)
#endif

#define arm_smmu_64_tlb_inv_range_s1(asid, iova, size, gran, reg)	\
	do {								\
		(iova) = SMMU_64_ADDRESS((iova) >> SMMU_CB_VA_SHIFT) |	\
			 SMMU_64_ASID(asid);				\
		do {							\
			__raw_writeq((iova),				\
				     reg((iommu_dev), (smmu_cb)));	\
			(iova) += (gran);				\
		} while ((size) -= (gran));				\
	} while (0)

#define arm_smmu_64_tlb_inv_range_s2(iova, size, gran, reg)		\
	do {								\
		(iova) >>= SMMU_CB_VA_SHIFT;				\
		do {							\
			__raw_writeq((iova),				\
				     reg((iommu_dev), (smmu_cb)));	\
			(iova) += (gran) >> 12;				\
		} while ((size) -= (gran));				\
	} while (0)

#ifdef CONFIG_SMMU_ARCH32
#define arm_smmu_tlb_inv_range_s1(asid, iova, size, gran, reg)	 \
	do {								 \
		if (smmu_device_ctrl.features & SMMU_FEAT_COHERENT_WALK) \
			wmb();						 \
		arm_smmu_32_tlb_inv_range_s1((asid), (iova), (size),	 \
					     (gran), reg);		 \
	} while (0)
#define arm_smmu_tlb_inv_range_s2(iova, size, gran, reg)		 \
	do {								 \
		if (smmu_device_ctrl.features & SMMU_FEAT_COHERENT_WALK) \
			wmb();						 \
		arm_smmu_32_tlb_inv_range_s2((iova), (size),		 \
					     (gran), reg);		 \
	} while (0)
#endif

#ifdef CONFIG_SMMU_ARCH64
#define arm_smmu_tlb_inv_range_s1(asid, iova, size, gran, reg)		 \
	do {								 \
		if (smmu_device_ctrl.features & SMMU_FEAT_COHERENT_WALK) \
			wmb();						 \
		arm_smmu_64_tlb_inv_range_s1((asid), (iova), (size),	 \
					     (gran), reg);		 \
	} while (0)
#define arm_smmu_tlb_inv_range_s2(iova, size, gran, reg)		 \
	do {								 \
		if (smmu_device_ctrl.features & SMMU_FEAT_COHERENT_WALK) \
			wmb();						 \
		arm_smmu_64_tlb_inv_range_s2((iova), (size),		 \
					     (gran), reg);		 \
	} while (0)
#endif

#endif /* __ARM_SMMUv2_H_INCLUDE__ */
