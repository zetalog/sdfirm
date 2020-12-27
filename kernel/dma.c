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
 * @(#)dma.c: direct memory access (DMA) implementation
 * $Id: dma.c,v 1.0 2020-12-6 10:20:00 zhenglv Exp $
 */

#include <target/dma.h>
#include <target/panic.h>

#define sdma2dma(dma)			dma_nr_table[sdma]

irq_handler dma_handlers[MAX_CHANNELS];
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

	if (!chan || !chan->has_range)
		return __dma_to_phys(addr);
	return addr - chan->dma_base + chan->phys_base;
}

dma_addr_t phys_to_dma(dma_t dma, phys_addr_t phys)
{
	struct dma_channel *chan = dma2chan(dma);

	if (!chan || !chan->has_range)
		return __phys_to_dma(phys);
	return phys - chan->phys_base + chan->dma_base;
}

bool dma_is_coherent(dma_t dma)
{
	struct dma_channel *chan = dma2chan(dma);

	if (!chan)
		return false;
	return chan->coherent;
}

bool dma_is_direct(dma_t dma)
{
	struct dma_channel *chan = dma2chan(dma);

	if (!chan)
		return true;
	return !chan->indirect;
}

/* 0 ~ NR_DMAS-1 is allowed. */
void dma_register_channel(dma_t dma, irq_handler h)
{
	uint8_t curr = dma_nr_regs;

	BUG_ON(dma <= 0 || dma >= NR_DMAS);
	BUG_ON(curr == MAX_CHANNELS);
	dma_nr_table[curr] = dma;
	dma_nr_regs++;
	dma_handlers[curr] = h;
}

void dma_direct_sync_cpu(dma_t dma, dma_addr_t addr, size_t size,
			 dma_dir_t dir)
{
	__unused phys_addr_t phys = dma_to_phys(dma, addr);

	if (!dma_is_coherent(dma)) {
		dma_hw_sync_cpu(dma, phys, size, dir);
		dma_hw_sync_cpu_all();
	}
}

void dma_direct_sync_dev(dma_t dma, dma_addr_t addr, size_t size,
			 dma_dir_t dir)
{
	__unused phys_addr_t phys = dma_to_phys(dma, addr);

	if (!dma_is_coherent(dma))
		dma_hw_sync_dev(dma, phys, size, dir);
}

void dma_sync_cpu(dma_t dma, dma_addr_t addr, size_t size, dma_dir_t dir)
{
	__unused phys_addr_t phys = dma_to_phys(dma, addr);

	if (dma_is_direct(dma))
		dma_direct_sync_cpu(dma, addr, size, dir);
	else
		dma_hw_sync_cpu(dma, phys, size, dir);
}

void dma_sync_dev(dma_t dma, dma_addr_t addr, size_t size, dma_dir_t dir)
{
	__unused phys_addr_t phys = dma_to_phys(dma, addr);

	if (dma_is_direct(dma))
		dma_direct_sync_dev(dma, addr, size, dir);
	else
		dma_hw_sync_dev(dma, phys, size, dir);
}

dma_addr_t dma_direct_map(dma_t dma, void *phys, size_t size, dma_dir_t dir)
{
	dma_hw_sync_dev(dma, phys, size, dir);
	return phys_to_dma(dma, (phys_addr_t)phys);
}

void dma_direct_unmap(dma_t dma, dma_addr_t addr, size_t size, dma_dir_t dir)
{
	__unused phys_addr_t phys = dma_to_phys(dma, addr);
	dma_hw_sync_cpu(dma, phys, size, dir);
}

dma_addr_t dma_map_single(dma_t dma, void *ptr, size_t size, dma_dir_t dir)
{
	if (dma_is_direct(dma))
		return dma_direct_map(dma, ptr, size, dir);
	else
		return dma_hw_map_single(dma, ptr, size, dir);
}

void dma_unmap_single(dma_t dma, dma_addr_t addr, size_t size, dma_dir_t dir)
{
	if (dma_is_direct(dma))
		dma_direct_unmap(dma, addr, size, dir);
	else
		dma_hw_unmap_single(dma, addr, size, dir);
}

void dma_channels_init(void)
{
	dmac_hw_ctrl_init();
}

void dma_init(void)
{
	dma_channels_init();
}
