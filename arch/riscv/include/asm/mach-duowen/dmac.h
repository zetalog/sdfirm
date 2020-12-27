#ifndef __DMAC_DUOWEN_H_INCLUDE__
#define __DMAC_DUOWEN_H_INCLUDE__

#define DW_DMA_REG(n, offset)		(DMA_BASE + (offset))
#define NR_DW_DMAS			1
#define DW_DMA_MAX_MASTERS(n)		2 /* snps,dma-masters */
#define DW_DMA_M_DATA_WIDTH(n)		8 /* snps,data-width */

#if defined(CONFIG_DW_DMA)
#include <driver/dw_dma.h>
#ifndef ARCH_HAVE_DMAC
#define ARCH_HAVE_DMAC		1
#else
#error "Multiple DMA controller defined"
#endif
#endif

#define INVALID_DMA			-1

#define DUOWEN_DMAC_CHANNEL_BASE	0
#define DUOWEN_DMAC_MAX_CHANNELS	8

#ifdef CONFIG_DW_DMA
#ifdef CONFIG_DUOWEN_SMMU
void smmu_dma_alloc_sme(void);
#else
#define smmu_dma_alloc_sme()		do { } while (0)
#endif
void dmac_hw_ctrl_init(void);

dma_addr_t dma_hw_map_single(dma_t dma, void *ptr,
			     size_t size, dma_dir_t dir);
void dma_hw_unmap_single(dma_t dma, dma_addr_t addr,
			 size_t size, dma_dir_t dir);
#else
#define smmu_dma_alloc_sme()		do { } while (0)
#endif

#endif /* __DMAC_DUOWEN_H_INCLUDE__ */
