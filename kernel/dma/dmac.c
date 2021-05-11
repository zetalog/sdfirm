/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)dmac.c: direct memory access (DMA) channel implementation
 * $Id: dmac.c,v 1.0 2020-12-6 10:20:00 zhenglv Exp $
 */

#include <target/dma.h>
#include <target/paging.h>
#include <target/panic.h>
#include <target/bh.h>
#include <target/irq.h>

bh_t dma_bh = INVALID_BH;

#define sdma2dma(sdma)			dma_nr_table[sdma]
dma_t dma_nr_table[MAX_CHANNELS];
uint8_t dma_nr_regs = 0;

#if NR_DMAS > 1
dma_t dma_sdma = INVALID_DMA;
struct dma_channel dma_channels[NR_DMAS];

void dma_channel_restore(dma_t dma)
{
	dma_sdma = dma;
}

dma_t dma_channel_save(dma_t dma)
{
	dma_t rdma = dma_sdma;

	dma_channel_restore(dma);
	return rdma;
}
#else
struct dma_channel dma_channel_ctrl;
#endif

dma_t dma_current(void)
{
	return sdma2dma(dma_sdma);
}

/* Given a DMA channel ID, find an index to our dma_channels */
dma_t dma2sdma(dma_t dma)
{
	uint8_t sdma;

	for (sdma = 0; sdma < dma_nr_regs; sdma++) {
		if (dma_nr_table[sdma] == dma)
			return sdma;
	}
	return INVALID_DMA;
}

struct dma_channel *dma2chan(dma_t dma)
{
	dma_t sdma = dma2sdma(dma);

	if (sdma == INVALID_DMA)
		return NULL;
	return &dma_channels[sdma];
}

phys_addr_t dma_to_phys(dma_t dma, dma_addr_t addr)
{
	struct dma_channel *chan = dma2chan(dma);

	if (!chan || !(chan->caps & DMA_CAP_HAS_RANGE))
		return __dma_to_phys(addr);
	return addr - chan->dma_base + chan->phys_base;
}

dma_addr_t phys_to_dma(dma_t dma, phys_addr_t phys)
{
	struct dma_channel *chan = dma2chan(dma);

	if (!chan || !(chan->caps & DMA_CAP_HAS_RANGE))
		return __phys_to_dma(phys);
	return phys - chan->phys_base + chan->dma_base;
}

bool dma_is_coherent(dma_t dma)
{
	struct dma_channel *chan = dma2chan(dma);

	if (!chan)
		return false;
	return !!(chan->caps & DMA_CAP_COHERENT);
}

bool dma_is_direct(dma_t dma)
{
	struct dma_channel *chan = dma2chan(dma);

	if (!chan)
		return true;
	return !chan->indirect;
}

uint8_t dma_direction(dma_t dma)
{
	struct dma_channel *chan = dma2chan(dma);

	if (!chan)
		return false;
	return chan->direction;
}

bool do_DMA(dma_t dma, bool success)
{
	uint8_t curr;
	struct dma_channel *chan;

	BUG_ON(dma >= NR_DMAS || dma <= 0);
	for (curr = 0; curr < dma_nr_regs; curr++) {
		if (dma == dma_nr_table[curr]) {
			chan = dma2chan(dma);
			if (chan->handler)
				chan->handler(dma, success);
			return true;
		}
	}
	return false;
}

/* 0 ~ NR_DMAS-1 is allowed. */
void dma_register_channel(dma_t dma, dma_caps_t caps)
{
	dma_t curr = dma_nr_regs;
	struct dma_channel *chan = dma2chan(dma);

	BUG_ON(dma <= 0 || dma >= NR_DMAS);
	BUG_ON(curr == MAX_CHANNELS);
	dma_nr_table[curr] = dma;
	dma_nr_regs++;
	chan = dma2chan(dma);
	chan->caps = caps;
}

void dma_config_range(dma_t dma, phys_addr_t phys_base, dma_addr_t dma_base)
{
	struct dma_channel *chan = dma2chan(dma);

	if (!chan)
		return;
	chan->caps |= DMA_CAP_HAS_RANGE;
	chan->phys_base = phys_base;
	chan->dma_base = dma_base;
}

dma_t dma_request_channel(uint8_t direction, dma_handler h)
{
	dma_t dma;
	struct dma_channel *chan;

	for (dma = 0; dma < MAX_CHANNELS; dma++) {
		chan = dma2chan(dma);
		if (chan->caps & _BV(direction) &&
		    chan->direction == DMA_NONE) {
			chan->direction = direction;
			chan->handler = h;
			return dma;
		}
	}
	return INVALID_DMA;
}

#ifdef SYS_REALTIME
#define dmac_irq_init_noirq()		irq_register_poller(dma_bh)
#define dmac_irq_init_irq()		do { } while (0)
#define dmac_irq_poll(event)		dmac_hw_irq_poll()
#else
#define dmac_irq_init_noirq()		do { } while (0)
#define dmac_irq_init_irq()		dmac_hw_irq_init()
#define dmac_irq_poll(event)		do { } while (0)
#endif

static void dma_async_handler(void)
{
	dma_t dma;
	__unused dma_t sdma;

	for (dma = 0; dma < NR_DMAS; dma++) {
		sdma = dma_channel_save(dma);
		dmac_hw_irq_handle();
		dma_channel_restore(sdma);
	}
}

static void dma_bh_handler(uint8_t event)
{
	if (event == BH_POLLIRQ)
		dmac_irq_poll(event);
	else {
		switch (event) {
		case BH_WAKEUP:
			dma_async_handler();
			break;
		default:
			BUG();
		}
	}
}

static void dmac_reset_channel(void)
{
}

void dma_init(void)
{
	dma_t dma;
	__unused dma_t sdma;

	DEVICE_INTF(DEVICE_INTF_DMA);
	dma_bh = bh_register_handler(dma_bh_handler);
	dmac_hw_ctrl_init();
	for (dma = 0; dma < NR_DMAS; dma++) {
		sdma = dma_channel_save(dma);
		dmac_reset_channel();
		dmac_irq_init_irq();
		dma_channel_restore(sdma);
	}
	dmac_irq_init_noirq();
}
