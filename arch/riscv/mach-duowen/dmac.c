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
 * @(#)dma.c: DUOWEN specific DMAC implementation
 * $Id: dma.c,v 1.1 2020-11-24 23:20:00 zhenglv Exp $
 */

#include <target/dma.h>
#include <target/iommu.h>

iommu_grp_t duowen_dma_iommus[8];

#ifdef CONFIG_DUOWEN_SMMU
void smmu_dma_alloc_sme(void)
{
	duowen_dma_iommus[0] = iommu_register_master(SMMU_SME_DMA_TBU0);
	duowen_dma_iommus[1] = iommu_register_master(SMMU_SME_DMA_TBU1);
	duowen_dma_iommus[2] = iommu_register_master(SMMU_SME_DMA_TBU2);
	duowen_dma_iommus[3] = iommu_register_master(SMMU_SME_DMA_TBU3);
	duowen_dma_iommus[4] = iommu_register_master(SMMU_SME_DMA_TBU4);
	duowen_dma_iommus[5] = iommu_register_master(SMMU_SME_DMA_TBU5);
	duowen_dma_iommus[6] = iommu_register_master(SMMU_SME_DMA_TBU6);
	duowen_dma_iommus[7] = iommu_register_master(SMMU_SME_DMA_TBU7);
}
#endif

dma_addr_t dma_hw_map_single(dma_t dma, void *ptr,
			     size_t size, dma_dir_t dir)
{
	return (dma_addr_t)ptr;
}

void dma_hw_unmap_single(dma_t dma, dma_addr_t addr,
			 size_t size, dma_dir_t dir)
{
}

void dmac_hw_ctrl_init(void)
{
	/* TODO: clock enabling */
        dw_dma_init();
}
