#include <target/dma.h>
#include <target/iommu.h>

dma_addr_t dma_hw_map_single(dma_t dma, phys_addr_t ptr,
			     size_t size, dma_dir_t dir)
{
	return (dma_addr_t)ptr;
}

void dma_hw_unmap_single(dma_t dma, dma_addr_t addr,
			 size_t size, dma_dir_t dir)
{
}
// void dmac_hw_ctrl_init(void)
// {
// 	dma_t dma;

// 	for (dma = 0; dma < NR_DMAS; dma++) {
// 		dma_register_channel(dma, AXI_DW_DMAC_CAPS);
// 		dma_clr_direct(dma);
// 	}
// 	// iommu_register_dma(ARRAY_SIZE(k1mxlite_dmac_iommus), \
// 				      k1mxlite_dmac_iommus);
// }
