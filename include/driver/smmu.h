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

#ifndef __SMMU_H_INLCUDE__
#define __SMMU_H_INCLUDE__

#include <target/generic.h>
#include <target/irq.h>

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
	struct arm_smmu_cb *cbs;
	atomic_t irptndx;
	uint32_t num_global_irqs;
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

#ifdef CONFIG_SMMU
iommu_grp_t smmu_alloc_sme(smmu_sme_t sme);
void smmu_free_sme(iommu_grp_t grp);
smmu_cb_t smmu_alloc_cb(smmu_cb_t start, smmu_cb_t end);
void smmu_free_cb(smmu_cb_t cb);
#else
#define smmu_alloc_sme(sme)			INVALID_IOMMU_GRP
#define smmu_free_sme(grp)			do { } while (0)
#endif

void smmu_group_select(void);
void smmu_domain_select(void);

void smmu_device_exit(void);
void smmu_device_init(void);

#endif /* __SMMU_H_INCLUDE__ */
