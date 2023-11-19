#include <target/dma.h>
#include <target/iommu.h>

iommu_grp_t k1matrix_dmac_iommus[NR_DMAC_DMAS];

#ifdef CONFIG_K1MATRIX_SMMU
void smmu_dma_alloc_sme(void)
{
	k1matrix_dmac_iommus[0] = iommu_register_master(SMMU_SME_DMA_TBU0);
	k1matrix_dmac_iommus[1] = iommu_register_master(SMMU_SME_DMA_TBU1);
	k1matrix_dmac_iommus[2] = iommu_register_master(SMMU_SME_DMA_TBU2);
	k1matrix_dmac_iommus[3] = iommu_register_master(SMMU_SME_DMA_TBU3);
	k1matrix_dmac_iommus[4] = iommu_register_master(SMMU_SME_DMA_TBU4);
	k1matrix_dmac_iommus[5] = iommu_register_master(SMMU_SME_DMA_TBU5);
	k1matrix_dmac_iommus[6] = iommu_register_master(SMMU_SME_DMA_TBU6);
	k1matrix_dmac_iommus[7] = iommu_register_master(SMMU_SME_DMA_TBU7);
}
#endif

dma_addr_t dma_hw_map_single(dma_t dma, phys_addr_t ptr,
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
	smmu_dma_alloc_sme();
}
