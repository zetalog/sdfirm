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
 * @(#)iommu.h: K1Matrix input-output (IO) MMU interface
 * $Id: iommu.h,v 1.1 2023-10-17 07:40:00 zhenglv Exp $
 */

#ifndef __RV_IOMMU_K1MATRIX_H_INCLUDE__
#define __RV_IOMMU_K1MATRIX_H_INCLUDE__

#include <target/arch.h>

#define SMMU_HW_MAX_CTRLS	1
#define SMMU_HW_TRANS		SMMU_FEAT_TRANS_S1
#define SMMU_HW_PTFS		(SMMU_FEAT_PTFS_RISCV_SV39 | \
				 SMMU_FEAT_PTFS_RISCV_SV48 | \
				 SMMU_FEAT_PTFS_ARCH64_4K | \
				 SMMU_FEAT_PTFS_ARCH64_64K)
#define SMMU_HW_NUMSIDB		15
#define SMMU_HW_NUMSMRG		32
#define SMMU_HW_PAGESIZE	0x1000
#define SMMU_HW_NUMPAGENDXB	3
#define SMMU_HW_NUMCB		16
#define SMMU_HW_NUMS2CB		0
#define SMMU_HW_IAS		48
#define SMMU_HW_OAS		48
#define SMMU_HW_UBS		48

/* iommu_dev_t */
#define IOMMU_DMAC		0

#if defined(CONFIG_K1MATRIX_DMAR)
#ifndef ARCH_HAVE_IOMMU
#define ARCH_HAVE_IOMMU		1

#define SMMU_HW_NUMSMES(n)	smmu_num_sms[n]

/* iommu_t */
#define IOMMU_DMA_TBU0		IOMMU(IOMMU_DMAC, 0)
#define IOMMU_DMA_TBU1		IOMMU(IOMMU_DMAC, 1)
#define IOMMU_DMA_TBU2		IOMMU(IOMMU_DMAC, 2)
#define IOMMU_DMA_TBU3		IOMMU(IOMMU_DMAC, 3)
#define NR_DMA_IOMMUS		4

#define NR_IOMMUS		(NR_DMA_IOMMUS)
#else
#error "Multiple IOMMU controller defined"
#endif
#endif


#include <driver/arm_smmuv3.h>
#include <target/iommu_armv8.h>

#ifdef ARCH_HAVE_IOMMU
#define iommu_hw_ctrl_init()				riscv_iommu_device_init()
#define iommu_hw_domain_select()			do { } while (0)
#define iommu_hw_group_init()				do { } while (0)
#define iommu_hw_domain_init()				do { } while (0)
#define iommu_hw_domain_exit()				do { } while (0)
#define iommu_hw_group_attach()				do { } while (0)
#ifdef SYS_REALTIME
// #define iommu_hw_poll_irqs()				smmu_poll_irqs()
#define iommu_hw_poll_irqs()				
#endif


#if 0
#define iommu_hw_tlb_flush_all()			\
	smmuv3_tlb_inv_context()
#define iommu_hw_tlb_flush_walk(iova, size, granule)	\
	smmuv3_tlb_inv_walk(iova, size, granule)
#define iommu_hw_tlb_flush_leaf(iova, size, granule)	\
	smmuv3_tlb_inv_leaf(iova, size, granule)
#define iommu_hw_tlb_add_page(gather, iova, granule)	\
	smmuv3_tlb_inv_page_nosync(gather, iova, granule)
#define iommu_hw_iotlb_sync(gather)			\
	smmuv3_iotlb_sync(gather)
#define iommu_hw_handle_stm()				\
	//smmuv3_handle_stm()
#define iommu_hw_handle_seq()				\
	smmuv3_handle_seq()
#endif

#define iommu_hw_tlb_flush_all()			do { } while (0)
#define iommu_hw_tlb_flush_walk(iova, size, granule)	do { } while (0)
#define iommu_hw_tlb_flush_leaf(iova, size, granule)	do { } while (0)
#define iommu_hw_tlb_add_page(gather, iova, granule)	do { } while (0)
#define iommu_hw_iotlb_sync(gather)			do { } while (0)
#define iommu_hw_handle_stm()				do { } while (0)
#define iommu_hw_handle_seq()				do { } while (0)

#define smmu_hw_num_pasid_bits				20

#if 0
#define iommu_hw_map(iova, paddr, pgsize, prot)		\
	arm_lpae_map(iova, paddr, pgsize, prot)
#define iommu_hw_unmap(iova, pgsize, gather)		\
	arm_lpae_unmap(iova, pgsize, gather)
#define iommu_hw_alloc_table(cfg)			\
	arm_lpae_pgtable_alloc(cfg)
#define iommu_hw_free_table()				\
	arm_lpae_pgtable_free()
#endif

#define iommu_hw_map(iova, pgsize, paddr, prot)		\
	riscv_iommu_map_pages(iova, paddr, pgsize, 1, prot)
#define iommu_hw_unmap(iova, pgsize, gather)		\
	riscv_iommu_unmap_pages(iova, pgsize, 1, gather)
#define iommu_hw_alloc_table(cfg)			\
	riscv_iommu_alloc_pgtable(cfg)

#define iommu_hw_iotlb_sync(gather)			do { } while (0)

#define iommu_hw_free_table()				do { } while (0)


#define smmu_hw_ctrl_reset(reg)				(reg)

extern unsigned long smmu_hw_pgsize_bitmap;
extern smmu_gr_t smmu_num_sms[];
#endif /* ARCH_HAVE_IOMMU */

#endif /* __IOMMU_K1MATRIX_H_INCLUDE__ */
