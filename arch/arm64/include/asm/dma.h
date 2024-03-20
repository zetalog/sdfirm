#ifndef __DMA_ARM64_H_INCLUDE__
#define __DMA_ARM64_H_INCLUDE__

#ifndef __ASSEMBLY__
typedef phys_addr_t dma_addr_t;
#endif

#if 1
#define dma_hw_sync_dev(dma, phys, size, dir)	dma_mb()
#define dma_hw_sync_cpu(dma, phys, size, dir)	dma_mb()
#define dma_hw_sync_cpu_all()			dma_mb()
#else
#define dma_hw_sync_dev(dma, paddr, size, dir)	\
	__dma_map_area((void *)phys_to_virt(paddr), size, dir)
#define dma_hw_sync_cpu(dma, paddr, size, dir)	\
	__dma_unmap_area((void *)phys_to_virt(paddr), size, dir)
#define dma_hw_sync_cpu_all()			do { } while (0)
#endif

#ifdef CONFIG_ARCH_HAS_DMAC
#include <asm/mach/dma.h>
#endif

#endif /* __DMA_ARM64_H_INCLUDE__ */
