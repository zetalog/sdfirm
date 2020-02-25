#ifndef __DMA_DRIVER_H_INCLUDE__
#define __DMA_DRIVER_H_INCLUDE__

#ifdef CONFIG_ARCH_HAS_DMA
#include <asm/mach/dma.h>
#endif

#ifndef ARCH_HAVE_DMA
#define dma_hw_ctrl_init()		do { } while (0)
#endif

#endif /* __DMA_DRIVER_H_INCLUDE__ */
