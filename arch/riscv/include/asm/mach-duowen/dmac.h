#ifndef __DMAC_DUOWEN_H_INCLUDE__
#define __DMAC_DUOWEN_H_INCLUDE__

#if defined(CONFIG_DW_DMA)
#include <driver/dw_dma.h>
#ifndef ARCH_HAVE_DMAC
#define ARCH_HAVE_DMAC		1
#else
#error "Multiple DMA controller defined"
#endif
#endif

#define INVALID_DMA		-1

#ifdef CONFIG_DW_DMA
void smmu_dma_alloc_sme(void);
void dmac_hw_ctrl_init(void);
#else
#define smmu_dma_alloc_sme()		do { } while (0)
#endif

#endif /* __DMAC_DUOWEN_H_INCLUDE__ */
