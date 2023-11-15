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
 * @(#)arm_smmuv1.h: ARM system mmu v1 (SMMUv1) interfaces
 * $Id: arm_smmuv1.h,v 1.1 2020-02-21 08:37:00 zhenglv Exp $
 */

#ifndef __ARM_SMMUv1_H_INLCUDE__
#define __ARM_SMMUv1_H_INCLUDE__

#include <target/generic.h>
#include <driver/smmu.h>

/* 10.6.1 SMMU_IDR0-7, Identification registers */
#define SMMU_PTFS			_BV(24)
#define SMMU_PTFS_NO_ARCH32_L		_BV(24)

/* 10.6.18 SMMU_sGFSYNR1, Global Fault Syndrome Register 1
 * 11.2.2 SMMU_CBFRSYNRAn, Context Bank Fault Restricted Syndrome Register A
 */
#define SMMU_STREAMID_OFFSET		0
#define SMMU_STREAMID_MASK		REG_15BIT_MASK
#define SMMU_STREAMID(value)		_GET_FV(SMMU_STREAMID, value)
#define SMMU_SSD_INDEX_OFFSET		16
#define SMMU_SSD_INDEX_MASK		REG_15BIT_MASK
#define SMMU_SSD_INDEX(value)		_GET_FV(SMMU_SSD_INDEX, value)

/* 10.6.20 SMMU_sGPAR, Global Physical Address Register */
/* Long-descriptor translation table format */
#define SMMU_L_PA_OFFSET		12
#define SMMU_L_PA_MASK			REG_28BIT_MASK
#define SMMU_L_PA(value)		_GET_FV_ULL(SMMU_L_PA, value)

/* 10.6.24 SMMU_SCR1, Secure Configuration Register 1 */
#define SMMU_GEFRO			_BV(25)

/* 10.6.30 SMMU_TLBIVMID, TLB Invalidate by VMID */
#define SMMU_VMID_OFFSET		0
#define SMMU_VMID_MASK			REG_8BIT_MASK
#define SMMU_VMID(value)		_SET_FV(SMMU_VMID, value)

#define SMMU_FSR_IGN						\
	(SMMU_C_AFF | SMMU_C_TLBMCF | SMMU_C_TLBLKF)
#define SMMU_FSR_FAULT						\
	(SMMU_C_MULTI | SMMU_C_SS |				\
	 SMMU_C_EF | SMMU_C_PF | SMMU_C_TF | SMMU_FSR_IGN)

#ifdef SMMU_HW_TRANS
#define smmu_trans_ops(id)						\
	(smmu_device_ctrl.features |= SMMU_HW_TRANS)
#else
#define smmu_trans_ops(id)						\
	do {								\
		if ((id) & SMMU_S1TS)					\
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
		if (!((id) & SMMU_PTFS_NO_ARCH32_L)) 			\
			smmu_device_ctrl.features |=			\
				SMMU_FEAT_PTFS_ARCH32_L;		\
	} while (0)
#endif
#ifdef CONFIG_ARM_SMMUv1_64KB
#define smmu_ptfs_arch64(id)						\
	do {								\
		smmu_device_ctrl.features |= SMMU_FEAT_PTFS_AARCH64_64K;\
	} while (0)
#else
#define smmu_ptfs_arch64(id)						\
	do { } while (0)
#endif
#ifdef SMMU_HW_NUMSIDB
#define smmu_max_streams(id)						\
	do {								\
		smmu_device_ctrl.max_streams = _BV(SMMU_HW_NUMSIDB);	\
		smmu_device_ctrl.streamid_mask =			\
			smmu_device_ctrl.max_streams - 1;		\
	} while (0)
#else
#define smmu_max_streams(id)						\
	do {								\
		smmu_device_ctrl.max_streams = _BV(SMMU_NUMSIDB(id));	\
		smmu_device_ctrl.streamid_mask =			\
			smmu_device_ctrl.max_streams - 1;		\
	} while (0)
#endif
#define smmu_probe_hafdbs(id)						\
	do { } while (0)
#define smmu_va_size(id)						\
	do {								\
		smmu_device_ctrl.va_size = smmu_device_ctrl.ipa_size;	\
	} while (0)
#define smmu_probe_vmid(id)						\
	do { } while (0)

#define arm_smmu_tlb_inv_range_s1(asid, iova, size, gran, reg)		\
	do {								\
		if (smmu_device_ctrl.features & SMMU_FEAT_COHERENT_WALK)\
			wmb();						\
		arm_smmu_32_tlb_inv_range_s1(asid, iova, size,		\
					     gran, reg);	  	\
	} while (0)

#define arm_smmu_tlb_inv_range_s2(iova, size, gran, reg)		\
	do {								\
		if (smmu_device_ctrl.features & SMMU_FEAT_COHERENT_WALK)\
			wmb();						\
		arm_smmu_32_tlb_inv_range_s2(iova, size, gran, reg);	\
	} while (0)

#endif /* __ARM_SMMUv1_H_INCLUDE__ */
