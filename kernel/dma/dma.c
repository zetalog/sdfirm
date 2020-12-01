#include <target/dma.h>

dma_addr_t dma_map_single(void *pages, size_t size, dma_dir_t dir)
{
	return (dma_addr_t)pages;
}

void dma_unmap_single(dma_addr_t addr, size_t size, dma_dir_t dir)
{
}

void dma_sync_single_for_dev(dma_addr_t addr, size_t size, dma_dir_t dir)
{
}

void dma_init(void)
{
	dma_hw_ctrl_init();
}
