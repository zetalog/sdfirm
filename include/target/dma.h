#ifndef __DMA_H_INCLUDE__
#define __DMA_H_INCLUDE__

#include <target/generic.h>

typedef enum dma_dir {
	DMA_BIDIRECTIONAL = 0,
	DMA_TO_DEVICE = 1,
	DMA_FROM_DEVICE = 2,
	DMA_NONE = 3,
} dma_dir_t;

typedef phys_addr_t dma_addr_t;

#include <driver/dma.h>

dma_addr_t dma_map_single(void *pages, size_t size, dma_dir_t dir);
void dma_unmap_single(dma_addr_t dma, size_t size, dma_dir_t dir);
void dma_sync_single_for_dev(dma_addr_t addr, size_t size, dma_dir_t dir);

#endif /* __DMA_H_INCLUDE__ */
