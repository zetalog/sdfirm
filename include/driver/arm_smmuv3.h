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
 * @(#)arm_smmuv3.h: ARM system memory management unit v3 (SMMUv3) interfaces
 * $Id: arm_smmuv3.h,v 1.1 2023-11-12 16:54:00 zhenglv Exp $
 */

#ifndef __ARM_SMMUv3_H_INCLUDE__
#define __ARM_SMMUv3_H_INCLUDE__

#include <target/arch.h>
#include <driver/smmu.h>
#include <target/dma.h>

/* MMIO registers */
#define ARM_SMMU_IDR0			0x0
#define IDR0_ST_LVL			GENMASK(28, 27)
#define IDR0_ST_LVL_2LVL		1
#define IDR0_STALL_MODEL		GENMASK(25, 24)
#define IDR0_STALL_MODEL_STALL		0
#define IDR0_STALL_MODEL_FORCE		2
#define IDR0_TTENDIAN			GENMASK(22, 21)
#define IDR0_TTENDIAN_MIXED		0
#define IDR0_TTENDIAN_LE		2
#define IDR0_TTENDIAN_BE		3
#define IDR0_CD2L			(1 << 19)
#define IDR0_VMID16			(1 << 18)
#define IDR0_PRI			(1 << 16)
#define IDR0_SEV			(1 << 14)
#define IDR0_MSI			(1 << 13)
#define IDR0_ASID16			(1 << 12)
#define IDR0_ATS			(1 << 10)
#define IDR0_HYP			(1 << 9)
#define IDR0_COHACC			(1 << 4)
#define IDR0_TTF			GENMASK(3, 2)
#define IDR0_TTF_AARCH64		2
#define IDR0_TTF_AARCH32_64		3
#define IDR0_S1P			(1 << 1)
#define IDR0_S2P			(1 << 0)

#define ARM_SMMU_IDR1			0x4
#define IDR1_TABLES_PRESET		(1 << 30)
#define IDR1_QUEUES_PRESET		(1 << 29)
#define IDR1_REL			(1 << 28)
#define IDR1_CMDQS			GENMASK(25, 21)
#define IDR1_EVTQS			GENMASK(20, 16)
#define IDR1_PRIQS			GENMASK(15, 11)
#define IDR1_SSIDSIZE			GENMASK(10, 6)
#define IDR1_SIDSIZE			GENMASK(5, 0)

#define ARM_SMMU_IDR5			0x14
#define IDR5_STALL_MAX			GENMASK(31, 16)
#define IDR5_GRAN64K			(1 << 6)
#define IDR5_GRAN16K			(1 << 5)
#define IDR5_GRAN4K			(1 << 4)
#define IDR5_OAS			GENMASK(2, 0)
#define IDR5_OAS_32_BIT			0
#define IDR5_OAS_36_BIT			1
#define IDR5_OAS_40_BIT			2
#define IDR5_OAS_42_BIT			3
#define IDR5_OAS_44_BIT			4
#define IDR5_OAS_48_BIT			5
#define IDR5_OAS_52_BIT			6
#define IDR5_VAX			GENMASK(11, 10)
#define IDR5_VAX_52_BIT			1

#define ARM_SMMU_CR0			0x20
#define CR0_ATSCHK			(1 << 4)
#define CR0_CMDQEN			(1 << 3)
#define CR0_EVTQEN			(1 << 2)
#define CR0_PRIQEN			(1 << 1)
#define CR0_SMMUEN			(1 << 0)

#define ARM_SMMU_CR0ACK			0x24

#define ARM_SMMU_CR1			0x28
#define CR1_TABLE_SH			GENMASK(11, 10)
#define CR1_TABLE_OC			GENMASK(9, 8)
#define CR1_TABLE_IC			GENMASK(7, 6)
#define CR1_QUEUE_SH			GENMASK(5, 4)
#define CR1_QUEUE_OC			GENMASK(3, 2)
#define CR1_QUEUE_IC			GENMASK(1, 0)
/* CR1 cacheability fields don't quite follow the usual TCR-style encoding */
#define CR1_CACHE_NC			0
#define CR1_CACHE_WB			1
#define CR1_CACHE_WT			2

#define ARM_SMMU_CR2			0x2c
#define CR2_PTM				(1 << 2)
#define CR2_RECINVSID			(1 << 1)
#define CR2_E2H				(1 << 0)

#define ARM_SMMU_GBPA			0x44
#define GBPA_UPDATE			(1 << 31)
#define GBPA_ABORT			(1 << 20)

#define ARM_SMMU_IRQ_CTRL		0x50
#define IRQ_CTRL_EVTQ_IRQEN		(1 << 2)
#define IRQ_CTRL_PRIQ_IRQEN		(1 << 1)
#define IRQ_CTRL_GERROR_IRQEN		(1 << 0)

#define ARM_SMMU_IRQ_CTRLACK		0x54

#define ARM_SMMU_GERROR			0x60
#define GERROR_SFM_ERR			(1 << 8)
#define GERROR_MSI_GERROR_ABT_ERR	(1 << 7)
#define GERROR_MSI_PRIQ_ABT_ERR		(1 << 6)
#define GERROR_MSI_EVTQ_ABT_ERR		(1 << 5)
#define GERROR_MSI_CMDQ_ABT_ERR		(1 << 4)
#define GERROR_PRIQ_ABT_ERR		(1 << 3)
#define GERROR_EVTQ_ABT_ERR		(1 << 2)
#define GERROR_CMDQ_ERR			(1 << 0)
#define GERROR_ERR_MASK			0xfd

#define ARM_SMMU_GERRORN		0x64

#define ARM_SMMU_GERROR_IRQ_CFG0	0x68
#define ARM_SMMU_GERROR_IRQ_CFG1	0x70
#define ARM_SMMU_GERROR_IRQ_CFG2	0x74

#define ARM_SMMU_STRTAB_BASE		0x80
#define STRTAB_BASE_RA			(1UL << 62)
#define STRTAB_BASE_ADDR_MASK		GENMASK_ULL(51, 6)

#define ARM_SMMU_STRTAB_BASE_CFG	0x88
#define STRTAB_BASE_CFG_FMT		GENMASK(17, 16)
#define STRTAB_BASE_CFG_FMT_LINEAR	0
#define STRTAB_BASE_CFG_FMT_2LVL	1
#define STRTAB_BASE_CFG_SPLIT		GENMASK(10, 6)
#define STRTAB_BASE_CFG_LOG2SIZE	GENMASK(5, 0)

#define ARM_SMMU_CMDQ_BASE		0x90
#define ARM_SMMU_CMDQ_PROD		0x98
#define ARM_SMMU_CMDQ_CONS		0x9c

#define ARM_SMMU_EVTQ_BASE		0xa0
#define ARM_SMMU_EVTQ_PROD		0x100a8
#define ARM_SMMU_EVTQ_CONS		0x100ac
#define ARM_SMMU_EVTQ_IRQ_CFG0		0xb0
#define ARM_SMMU_EVTQ_IRQ_CFG1		0xb8
#define ARM_SMMU_EVTQ_IRQ_CFG2		0xbc

#define ARM_SMMU_PRIQ_BASE		0xc0
#define ARM_SMMU_PRIQ_PROD		0x100c8
#define ARM_SMMU_PRIQ_CONS		0x100cc
#define ARM_SMMU_PRIQ_IRQ_CFG0		0xd0
#define ARM_SMMU_PRIQ_IRQ_CFG1		0xd8
#define ARM_SMMU_PRIQ_IRQ_CFG2		0xdc

/* Common MSI config fields */
#define MSI_CFG0_ADDR_MASK		GENMASK_ULL(51, 2)
#define MSI_CFG2_SH			GENMASK(5, 4)
#define MSI_CFG2_MEMATTR		GENMASK(3, 0)

/* Common memory attribute values */
#define ARM_SMMU_SH_NSH			0
#define ARM_SMMU_SH_OSH			2
#define ARM_SMMU_SH_ISH			3
#define ARM_SMMU_MEMATTR_DEVICE_nGnRE	0x1
#define ARM_SMMU_MEMATTR_OIWB		0xf

#define Q_IDX(llq, p)			((p) & ((1 << (llq)->max_n_shift) - 1))
#define Q_WRP(llq, p)			((p) & (1 << (llq)->max_n_shift))
#define Q_OVERFLOW_FLAG			(1U << 31)
#define Q_OVF(p)			((p) & Q_OVERFLOW_FLAG)
#define Q_ENT(q, p)			((q)->base +			\
					 Q_IDX(&((q)->llq), p) *	\
					 (q)->ent_dwords)

#define Q_BASE_RWA			(1UL << 62)
#define Q_BASE_ADDR_MASK		GENMASK_ULL(51, 5)
#define Q_BASE_LOG2SIZE			GENMASK(4, 0)

/* Ensure DMA allocations are naturally aligned */
#ifdef CONFIG_CMA_ALIGNMENT
#define Q_MAX_SZ_SHIFT			(PAGE_SHIFT + CONFIG_CMA_ALIGNMENT)
#else
#define Q_MAX_SZ_SHIFT			(PAGE_SHIFT + MAX_ORDER - 1)
#endif

/*
 * Stream table.
 *
 * Linear: Enough to cover 1 << IDR1.SIDSIZE entries
 * 2lvl: 128k L1 entries,
 *       256 lazy entries per table (each table covers a PCI bus)
 */
#define STRTAB_L1_SZ_SHIFT		20
#define STRTAB_SPLIT			8

#define STRTAB_L1_DESC_DWORDS		1
#define STRTAB_L1_DESC_SPAN		GENMASK_ULL(4, 0)
#define STRTAB_L1_DESC_L2PTR_MASK	GENMASK_ULL(51, 6)

#define STRTAB_STE_DWORDS		8
#define STRTAB_STE_0_V			(1UL << 0)
#define STRTAB_STE_0_CFG		GENMASK_ULL(3, 1)
#define STRTAB_STE_0_CFG_ABORT		0
#define STRTAB_STE_0_CFG_BYPASS		4
#define STRTAB_STE_0_CFG_S1_TRANS	5
#define STRTAB_STE_0_CFG_S2_TRANS	6

#define STRTAB_STE_0_S1FMT		GENMASK_ULL(5, 4)
#define STRTAB_STE_0_S1FMT_LINEAR	0
#define STRTAB_STE_0_S1FMT_64K_L2	2
#define STRTAB_STE_0_S1CTXPTR_MASK	GENMASK_ULL(51, 6)
#define STRTAB_STE_0_S1CDMAX		GENMASK_ULL(63, 59)

#define STRTAB_STE_1_S1DSS		GENMASK_ULL(1, 0)
#define STRTAB_STE_1_S1DSS_TERMINATE	0x0
#define STRTAB_STE_1_S1DSS_BYPASS	0x1
#define STRTAB_STE_1_S1DSS_SSID0	0x2

#define STRTAB_STE_1_S1C_CACHE_NC	0UL
#define STRTAB_STE_1_S1C_CACHE_WBRA	1UL
#define STRTAB_STE_1_S1C_CACHE_WT	2UL
#define STRTAB_STE_1_S1C_CACHE_WB	3UL
#define STRTAB_STE_1_S1CIR		GENMASK_ULL(3, 2)
#define STRTAB_STE_1_S1COR		GENMASK_ULL(5, 4)
#define STRTAB_STE_1_S1CSH		GENMASK_ULL(7, 6)

#define STRTAB_STE_1_S1STALLD		(1UL << 27)

#define STRTAB_STE_1_EATS		GENMASK_ULL(29, 28)
#define STRTAB_STE_1_EATS_ABT		0UL
#define STRTAB_STE_1_EATS_TRANS		1UL
#define STRTAB_STE_1_EATS_S1CHK		2UL

#define STRTAB_STE_1_STRW		GENMASK_ULL(31, 30)
#define STRTAB_STE_1_STRW_NSEL1		0UL
#define STRTAB_STE_1_STRW_EL2		2UL

#define STRTAB_STE_1_SHCFG		GENMASK_ULL(45, 44)
#define STRTAB_STE_1_SHCFG_INCOMING	1UL

#define STRTAB_STE_2_S2VMID		GENMASK_ULL(15, 0)
#define STRTAB_STE_2_VTCR		GENMASK_ULL(50, 32)
#define STRTAB_STE_2_VTCR_S2T0SZ	GENMASK_ULL(5, 0)
#define STRTAB_STE_2_VTCR_S2SL0		GENMASK_ULL(7, 6)
#define STRTAB_STE_2_VTCR_S2IR0		GENMASK_ULL(9, 8)
#define STRTAB_STE_2_VTCR_S2OR0		GENMASK_ULL(11, 10)
#define STRTAB_STE_2_VTCR_S2SH0		GENMASK_ULL(13, 12)
#define STRTAB_STE_2_VTCR_S2TG		GENMASK_ULL(15, 14)
#define STRTAB_STE_2_VTCR_S2PS		GENMASK_ULL(18, 16)
#define STRTAB_STE_2_S2AA64		(1UL << 51)
#define STRTAB_STE_2_S2ENDI		(1UL << 52)
#define STRTAB_STE_2_S2PTW		(1UL << 54)
#define STRTAB_STE_2_S2R		(1UL << 58)

#define STRTAB_STE_3_S2TTB_MASK		GENMASK_ULL(51, 4)

/*
 * Context descriptors.
 *
 * Linear: when less than 1024 SSIDs are supported
 * 2lvl: at most 1024 L1 entries,
 *       1024 lazy entries per table.
 */
#define CTXDESC_SPLIT			10
#define CTXDESC_L2_ENTRIES		(1 << CTXDESC_SPLIT)

#define CTXDESC_L1_DESC_DWORDS		1
#define CTXDESC_L1_DESC_V		(1UL << 0)
#define CTXDESC_L1_DESC_L2PTR_MASK	GENMASK_ULL(51, 12)

#define CTXDESC_CD_DWORDS		8
#define CTXDESC_CD_0_TCR_T0SZ		GENMASK_ULL(5, 0)
#define CTXDESC_CD_0_TCR_TG0		GENMASK_ULL(7, 6)
#define CTXDESC_CD_0_TCR_IRGN0		GENMASK_ULL(9, 8)
#define CTXDESC_CD_0_TCR_ORGN0		GENMASK_ULL(11, 10)
#define CTXDESC_CD_0_TCR_SH0		GENMASK_ULL(13, 12)
#define CTXDESC_CD_0_TCR_EPD0		(1ULL << 14)
#define CTXDESC_CD_0_TCR_EPD1		(1ULL << 30)

#define CTXDESC_CD_0_ENDI		(1UL << 15)
#define CTXDESC_CD_0_V			(1UL << 31)

#define CTXDESC_CD_0_TCR_IPS		GENMASK_ULL(34, 32)
#define CTXDESC_CD_0_TCR_TBI0		(1ULL << 38)

#define CTXDESC_CD_0_AA64		(1UL << 41)
#define CTXDESC_CD_0_S			(1UL << 44)
#define CTXDESC_CD_0_R			(1UL << 45)
#define CTXDESC_CD_0_A			(1UL << 46)
#define CTXDESC_CD_0_ASET		(1UL << 47)
#define CTXDESC_CD_0_ASID		GENMASK_ULL(63, 48)

#define CTXDESC_CD_1_TTB0_MASK		GENMASK_ULL(51, 4)

/*
 * When the SMMU only supports linear context descriptor tables, pick a
 * reasonable size limit (64kB).
 */
#define CTXDESC_LINEAR_CDMAX		ilog2(SZ_64K / (CTXDESC_CD_DWORDS << 3))

/* Command queue */
#define CMDQ_ENT_SZ_SHIFT		4
#define CMDQ_ENT_DWORDS			((1 << CMDQ_ENT_SZ_SHIFT) >> 3)
#define CMDQ_MAX_SZ_SHIFT		(Q_MAX_SZ_SHIFT - CMDQ_ENT_SZ_SHIFT)

#define CMDQ_CONS_ERR			GENMASK(30, 24)
#define CMDQ_ERR_CERROR_NONE_IDX	0
#define CMDQ_ERR_CERROR_ILL_IDX		1
#define CMDQ_ERR_CERROR_ABT_IDX		2
#define CMDQ_ERR_CERROR_ATC_INV_IDX	3

#define CMDQ_PROD_OWNED_FLAG		Q_OVERFLOW_FLAG

/*
 * This is used to size the command queue and therefore must be at least
 * BITS_PER_LONG so that the valid_map works correctly (it relies on the
 * total number of queue entries being a multiple of BITS_PER_LONG).
 */
#define CMDQ_BATCH_ENTRIES		BITS_PER_LONG

#define CMDQ_0_OP			GENMASK_ULL(7, 0)
#define CMDQ_0_SSV			(1UL << 11)

#define CMDQ_PREFETCH_0_SID		GENMASK_ULL(63, 32)
#define CMDQ_PREFETCH_1_SIZE		GENMASK_ULL(4, 0)
#define CMDQ_PREFETCH_1_ADDR_MASK	GENMASK_ULL(63, 12)

#define CMDQ_CFGI_0_SSID		GENMASK_ULL(31, 12)
#define CMDQ_CFGI_0_SID			GENMASK_ULL(63, 32)
#define CMDQ_CFGI_1_LEAF		(1UL << 0)
#define CMDQ_CFGI_1_RANGE		GENMASK_ULL(4, 0)

#define CMDQ_TLBI_0_VMID		GENMASK_ULL(47, 32)
#define CMDQ_TLBI_0_ASID		GENMASK_ULL(63, 48)
#define CMDQ_TLBI_1_LEAF		(1UL << 0)
#define CMDQ_TLBI_1_VA_MASK		GENMASK_ULL(63, 12)
#define CMDQ_TLBI_1_IPA_MASK		GENMASK_ULL(51, 12)

#define CMDQ_ATC_0_SSID			GENMASK_ULL(31, 12)
#define CMDQ_ATC_0_SID			GENMASK_ULL(63, 32)
#define CMDQ_ATC_0_GLOBAL		(1UL << 9)
#define CMDQ_ATC_1_SIZE			GENMASK_ULL(5, 0)
#define CMDQ_ATC_1_ADDR_MASK		GENMASK_ULL(63, 12)

#define CMDQ_PRI_0_SSID			GENMASK_ULL(31, 12)
#define CMDQ_PRI_0_SID			GENMASK_ULL(63, 32)
#define CMDQ_PRI_1_GRPID		GENMASK_ULL(8, 0)
#define CMDQ_PRI_1_RESP			GENMASK_ULL(13, 12)

#define CMDQ_SYNC_0_CS			GENMASK_ULL(13, 12)
#define CMDQ_SYNC_0_CS_NONE		0
#define CMDQ_SYNC_0_CS_IRQ		1
#define CMDQ_SYNC_0_CS_SEV		2
#define CMDQ_SYNC_0_MSH			GENMASK_ULL(23, 22)
#define CMDQ_SYNC_0_MSIATTR		GENMASK_ULL(27, 24)
#define CMDQ_SYNC_0_MSIDATA		GENMASK_ULL(63, 32)
#define CMDQ_SYNC_1_MSIADDR_MASK	GENMASK_ULL(51, 2)

/* Event queue */
#define EVTQ_ENT_SZ_SHIFT		5
#define EVTQ_ENT_DWORDS			((1 << EVTQ_ENT_SZ_SHIFT) >> 3)
#define EVTQ_MAX_SZ_SHIFT		(Q_MAX_SZ_SHIFT - EVTQ_ENT_SZ_SHIFT)

#define EVTQ_0_ID			GENMASK_ULL(7, 0)

/* PRI queue */
#define PRIQ_ENT_SZ_SHIFT		4
#define PRIQ_ENT_DWORDS			((1 << PRIQ_ENT_SZ_SHIFT) >> 3)
#define PRIQ_MAX_SZ_SHIFT		(Q_MAX_SZ_SHIFT - PRIQ_ENT_SZ_SHIFT)

#define PRIQ_0_SID			GENMASK_ULL(31, 0)
#define PRIQ_0_SSID			GENMASK_ULL(51, 32)
#define PRIQ_0_PERM_PRIV		(1UL << 58)
#define PRIQ_0_PERM_EXEC		(1UL << 59)
#define PRIQ_0_PERM_READ		(1UL << 60)
#define PRIQ_0_PERM_WRITE		(1UL << 61)
#define PRIQ_0_PRG_LAST			(1UL << 62)
#define PRIQ_0_SSID_V			(1UL << 63)

#define PRIQ_1_PRG_IDX			GENMASK_ULL(8, 0)
#define PRIQ_1_ADDR_MASK		GENMASK_ULL(63, 12)

/* High-level queue structures */
#define ARM_SMMU_POLL_TIMEOUT_US	1000000 /* 1s! */
#define ARM_SMMU_POLL_SPIN_COUNT	10

#define MSI_IOVA_BASE			0x8000000
#define MSI_IOVA_LENGTH			0x100000

enum pri_resp {
	PRI_RESP_DENY = 0,
	PRI_RESP_FAIL = 1,
	PRI_RESP_SUCC = 2,
};

enum arm_smmu_msi_index {
	EVTQ_MSI_INDEX,
	GERROR_MSI_INDEX,
	PRIQ_MSI_INDEX,
	ARM_SMMU_MAX_MSIS,
};

struct arm_smmu_cmdq_ent {
	/* Common fields */
	uint8_t				opcode;
	bool				substream_valid;

	/* Command-specific fields */
	union {
		#define CMDQ_OP_PREFETCH_CFG	0x1
		struct {
			uint32_t			sid;
			uint8_t			size;
			uint64_t			addr;
		} prefetch;

		#define CMDQ_OP_CFGI_STE	0x3
		#define CMDQ_OP_CFGI_ALL	0x4
		#define CMDQ_OP_CFGI_CD		0x5
		#define CMDQ_OP_CFGI_CD_ALL	0x6
		struct {
			uint32_t			sid;
			uint32_t			ssid;
			union {
				bool		leaf;
				uint8_t		span;
			};
		} cfgi;

		#define CMDQ_OP_TLBI_NH_ASID	0x11
		#define CMDQ_OP_TLBI_NH_VA	0x12
		#define CMDQ_OP_TLBI_EL2_ALL	0x20
		#define CMDQ_OP_TLBI_S12_VMALL	0x28
		#define CMDQ_OP_TLBI_S2_IPA	0x2a
		#define CMDQ_OP_TLBI_NSNH_ALL	0x30
		struct {
			uint16_t			asid;
			uint16_t			vmid;
			bool			leaf;
			uint64_t			addr;
		} tlbi;

		#define CMDQ_OP_ATC_INV		0x40
		#define ATC_INV_SIZE_ALL	52
		struct {
			uint32_t			sid;
			uint32_t			ssid;
			uint64_t			addr;
			uint8_t			size;
			bool			global;
		} atc;

		#define CMDQ_OP_PRI_RESP	0x41
		struct {
			uint32_t			sid;
			uint32_t			ssid;
			uint16_t			grpid;
			enum pri_resp		resp;
		} pri;

		#define CMDQ_OP_CMD_SYNC	0x46
		struct {
			uint64_t			msiaddr;
		} sync;
	};
};

/* High-level stream table and context descriptor structures */
struct arm_smmu_strtab_l1_desc {
	uint8_t				span;

	uint64_t				*l2ptr;
	dma_addr_t			l2ptr_dma;
};

struct arm_smmu_ctx_desc {
	uint16_t				asid;
	uint64_t				ttbr;
	uint64_t				tcr;
	uint64_t				mair;
};

struct arm_smmu_l1_ctx_desc {
	uint64_t				*l2ptr;
	dma_addr_t			l2ptr_dma;
};

#include <driver/smmu_common.h>

#endif /* __ARM_SMMUv3_H_INCLUDE__ */
