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
 * @(#)dmar.c: direct memory access (DMA) remap implementation
 * $Id: dmar.c,v 1.0 2020-12-6 10:20:00 zhenglv Exp $
 */

#include <target/dma.h>
#include <target/paging.h>
#include <target/panic.h>

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

dma_addr_t dma_direct_map(dma_t dma, phys_addr_t phys, size_t size,
			  dma_dir_t dir)
{
	dma_hw_sync_dev(dma, phys, size, dir);
	return phys_to_dma(dma, (phys_addr_t)phys);
}

void dma_direct_unmap(dma_t dma, dma_addr_t addr, size_t size, dma_dir_t dir)
{
	__unused phys_addr_t phys = dma_to_phys(dma, addr);
	dma_hw_sync_cpu(dma, phys, size, dir);
}

dma_addr_t dma_map_single(dma_t dma, phys_addr_t ptr, size_t size,
			  dma_dir_t dir)
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
