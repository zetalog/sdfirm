#ifndef __DMA_ARM64_H_INCLUDE__
#define __DMA_ARM64_H_INCLUDE__

#define dma_hw_sync_dev(paddr, size, dir)	\
	__dma_map_area((void *)phys_to_virt(paddr), size, dir)
#define dma_hw_sync_cpu(paddr, size, dir)	\
	__dma_unmap_area((void *)phys_to_virt(paddr), size, dir)
#define dma_hw_sync_cpu_all()			do { } while (0)

#endif /* __DMA_ARM64_H_INCLUDE__ */
