#include <target/dma.h>
#include <target/iommu.h>

iommu_t k1matrix_dmac_iommus[NR_DMAC_DMAS] = {
	IOMMU_DMA_TBU0,
	IOMMU_DMA_TBU1,
	IOMMU_DMA_TBU2,
	IOMMU_DMA_TBU3,
};

iommu_grp_t k1matrix_dmac_iommu;

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
	iommu_register_dma(ARRAY_SIZE(k1matrix_dmac_iommus),
				      k1matrix_dmac_iommus);
}
