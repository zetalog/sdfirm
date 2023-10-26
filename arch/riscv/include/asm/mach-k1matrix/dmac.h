/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)dmac.h: DUOWEN specific DMA controller implementation
 * $Id: dmac.h,v 1.1 2021-09-02 21:52:00 zhenglv Exp $
 */

#ifndef __DMAC_DUOWEN_H_INCLUDE__
#define __DMAC_DUOWEN_H_INCLUDE__

#define DW_DMA_BASE(n)		DMA_BASE
#define DW_DMA_MAX_CHIPS	1
#define DW_DMA_MAX_CHANS	8
#define DW_DMAC_CAPS		(DMA_CAP_MEM_TO_DEV | DMA_CAP_DEV_TO_MEM | \
				 DMA_CAP_MEM_TO_MEM | DMA_CAP_DEV_TO_DEV | \
				 DMA_CAP_COHERENT)

#if defined(CONFIG_DW_DMA)
#include <driver/dw_dma.h>
#ifndef ARCH_HAVE_DMAC
#define ARCH_HAVE_DMAC		1
#else
#error "Multiple DMA controller defined"
#endif
#endif

#define DW_DMA_CHIP_IRQ		IRQ_DMAC
#define DW_DMA_CHAN_IRQ(n)	INVALID_IRQ

#define dmac_hw_ctrl_init()		do { } while (0)
#define dma_hw_map_single(dma, ptr, size, dir) 0
#define dma_hw_unmap_single(dma, addr, size, dir)	do {} while (0)
#define dmac_hw_irq_poll()		do { } while (0)
#define dmac_hw_irq_init()		do { } while (0)
#define dmac_hw_irq_handle()		do { } while (0)

#endif /* __DMAC_DUOWEN_H_INCLUDE__ */