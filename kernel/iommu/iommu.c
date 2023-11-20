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
 * @(#)iommu.c: I/O memory management unit implementation
 * $Id: iommu.h,v 1.1 2020-11-19 16:07:00 zhenglv Exp $
 */

#include <target/iommu.h>
#include <target/panic.h>

/* ======================================================================
 * IOMMU Devices
 * ====================================================================== */
#if NR_IOMMU_DEVICES > 1
struct iommu_device iommu_devices[NR_IOMMU_DEVICES];
iommu_dev_t iommu_dev;

void iommu_device_restore(iommu_dev_t dev)
{
	iommu_dev = dev;
}

iommu_dev_t iommu_device_save(iommu_dev_t dev)
{
	iommu_dev_t rdev = iommu_dev;

	iommu_device_restore(dev);
	return rdev;
}
#else
struct iommu_device iommu_device_ctrl;
#endif

/* ======================================================================
 * IOMMU Groups
 * ====================================================================== */
#if NR_IOMMU_GROUPS > 1
struct iommu_group iommu_groups[NR_IOMMU_GROUPS];
iommu_grp_t iommu_grp;

void iommu_group_restore(iommu_grp_t grp)
{
	iommu_grp = grp;
}

iommu_grp_t iommu_group_save(iommu_grp_t grp)
{
	iommu_grp_t rgrp = iommu_grp;

	iommu_group_restore(grp);
	return rgrp;
}
#else
struct iommu_group iommu_group_ctrl;
#endif

iommu_grp_t iommu_alloc_group(int nr_iommus, iommu_t *iommus)
{
	__unused iommu_grp_t grp = INVALID_IOMMU_GRP, sgrp;

	BUG_ON(iommu_dev == INVALID_IOMMU_DEV);

	sgrp = iommu_group_save(grp);
	for (grp = 0; grp < NR_IOMMU_GROUPS; grp++) {
		iommu_group_restore(sgrp);
		sgrp = iommu_group_save(grp);
		if (iommu_group_ctrl.dev == INVALID_IOMMU_DEV) {
			iommu_group_ctrl.dev = iommu_dev;
			iommu_group_ctrl.nr_iommus = nr_iommus;
			iommu_group_ctrl.iommus = iommus;
			break;
		}
	}
	iommu_group_restore(sgrp);
	return grp;
}

void iommu_free_group(iommu_grp_t grp)
{
	__unused iommu_grp_t sgrp;

	if (grp < NR_IOMMU_GROUPS) {
		sgrp = iommu_group_save(grp);
		iommu_group_ctrl.dev = INVALID_IOMMU_DEV;
		iommu_group_restore(sgrp);
	}
}

unsigned long iommu_iova_alloc(iommu_dom_t dom, size_t size)
{
	return 0;
}

int dma_info_to_prot(uint8_t dir, bool coherent, unsigned long attrs)
{
	int prot = coherent ? IOMMU_CACHE : 0;

	switch (dir) {
		case DMA_BIDIRECTIONAL:
			return prot | IOMMU_READ | IOMMU_WRITE;
		case DMA_TO_DEVICE:
			return prot | IOMMU_READ;
		case DMA_FROM_DEVICE:
			return prot | IOMMU_WRITE;
		default:
			return 0;
	}
}

/* ======================================================================
 * IOMMU Domains
 * ====================================================================== */
#if NR_IOMMU_DOMAINS > 1
struct iommu_domain iommu_domains[NR_IOMMU_DOMAINS];
iommu_dom_t iommu_dom;

void iommu_domain_restore(iommu_dom_t dom)
{
	iommu_dom = dom;
	iommu_hw_domain_select();
}

iommu_dom_t iommu_domain_save(iommu_dom_t dom)
{
	iommu_dom_t rdom = iommu_dom;

	iommu_domain_restore(dom);
	return rdom;
}
#else
struct iommu_domain iommu_domain_ctrl;
#endif

iommu_dom_t iommu_alloc_domain(uint8_t type)
{
	__unused iommu_dom_t dom = INVALID_IOMMU_DOM, sdom;
	__unused iommu_dev_t sdev;

	BUG_ON(iommu_grp == INVALID_IOMMU_GRP);

	sdom = iommu_domain_save(dom);
	for (dom = 0; dom < NR_IOMMU_DOMAINS; dom++) {
		iommu_domain_restore(sdom);
		sdom = iommu_domain_save(dom);
		if (iommu_domain_ctrl.grp == INVALID_IOMMU_GRP) {
			iommu_domain_ctrl.grp = iommu_grp;
			iommu_domain_ctrl.type = type;
			sdev = iommu_device_save(iommu_group_ctrl.dev);
			iommu_domain_ctrl.pgsize_bitmap =
				iommu_device_ctrl.pgsize_bitmap;
			iommu_device_restore(sdev);
			break;
		}
	}
	iommu_domain_restore(sdom);
	return dom;
}

void iommu_free_domain(iommu_dom_t dom)
{
	__unused iommu_dom_t sdom;

	if (dom < NR_IOMMU_DOMAINS) {
		sdom = iommu_domain_save(dom);
		iommu_domain_ctrl.grp = INVALID_IOMMU_GRP;
		iommu_domain_restore(sdom);
	}
}

iommu_dom_t iommu_get_domain(iommu_grp_t grp)
{
	__unused iommu_grp_t rgrp;
	iommu_dom_t dom;

	rgrp = iommu_group_save(grp);
	dom = iommu_group_ctrl.dom;
	iommu_group_restore(rgrp);

	return dom;
}

iommu_dom_t iommu_get_dma_domain(iommu_grp_t grp)
{
	__unused iommu_grp_t rgrp;
	iommu_dom_t dom;

	rgrp = iommu_group_save(grp);
	dom = iommu_group_ctrl.default_dom;
	iommu_group_restore(rgrp);

	return dom;
}

/* ======================================================================
 * IOMMU Mappings
 * ====================================================================== */
static size_t iommu_pgsize(unsigned long addr_merge, size_t size)
{
	unsigned int pgsize_idx;
	size_t pgsize;

	pgsize_idx = __fls64(size);

	if (addr_merge) {
		unsigned int aligned_pgsize_idx = __ffs64(addr_merge);
		pgsize_idx = min(pgsize_idx, aligned_pgsize_idx);
	}

	pgsize = (UL(1) << (pgsize_idx + 1)) - 1;
	pgsize &= iommu_domain_ctrl.pgsize_bitmap;

	BUG_ON(!pgsize);

	pgsize_idx = __fls64(pgsize);
	pgsize = UL(1) << pgsize_idx;

	return pgsize;
}

int iommu_map(unsigned long iova, size_t size, phys_addr_t paddr, int prot)
{
	unsigned long orig_iova = iova;
	unsigned long orig_size = size;
	unsigned int min_pagesz;
	int ret;

	min_pagesz = 1 << __ffs64(iommu_domain_ctrl.pgsize_bitmap);
	BUG_ON(!IS_ALIGNED(iova | paddr | size, min_pagesz));

	while (size) {
		size_t pgsize = iommu_pgsize(iova | paddr, size);

		ret = iommu_hw_map(iova, pgsize, paddr, prot);
		if (ret)
			break;

		iova += pgsize;
		paddr += pgsize;
		size -= pgsize;
	}

	if (ret)
		iommu_unmap(orig_iova, orig_size - size);
	return ret;
}

static inline void iommu_iotlb_gather_init(struct iommu_iotlb_gather *gather)
{
	*gather = (struct iommu_iotlb_gather) {
		.start = ULONG_MAX,
	};
}

bool iommu_iotlb_gather_is_disjoint(struct iommu_iotlb_gather *gather,
				    unsigned long iova, size_t size)
{
	unsigned long start = iova, end = start + size - 1;

	return gather->end != 0 &&
		(end + 1 < gather->start || start > gather->end + 1);
}

void iommu_iotlb_gather_add_range(struct iommu_iotlb_gather *gather,
				  unsigned long iova, size_t size)
{
	unsigned long end = iova + size - 1;

	if (gather->start > iova)
		gather->start = iova;
	if (gather->end < end)
		gather->end = end;
}

void iommu_iotlb_gather_add_page(struct iommu_iotlb_gather *gather,
				 unsigned long iova, size_t size)
{
	/*
	 * If the new page is disjoint from the current range or is mapped at
	 * a different granularity, then sync the TLB so that the gather
	 * structure can be rewritten.
	 */
	if ((gather->pgsize && gather->pgsize != size) ||
	    iommu_iotlb_gather_is_disjoint(gather, iova, size))
		iommu_iotlb_sync(gather);

	gather->pgsize = size;
	iommu_iotlb_gather_add_range(gather, iova, size);
}

void iommu_iotlb_sync(struct iommu_iotlb_gather *gather)
{
	iommu_hw_iotlb_sync(gather);
	iommu_iotlb_gather_init(gather);
}

int iommu_unmap(unsigned long iova, size_t size)
{
	unsigned int min_pagesz;
	size_t unmapped_page, unmapped = 0;
	__unused unsigned long orig_iova = iova;
	struct iommu_iotlb_gather iotlb_gather;

	iommu_iotlb_gather_init(&iotlb_gather);

	min_pagesz = 1 << __ffs64(iommu_domain_ctrl.pgsize_bitmap);
	BUG_ON(!IS_ALIGNED(iova | size, min_pagesz));

	while (unmapped < size) {
		__unused size_t pgsize = iommu_pgsize(iova, size - unmapped);

		unmapped_page = iommu_hw_unmap(iova, pgsize, &iotlb_gather);
		if (!unmapped_page)
			break;

		iova += unmapped_page;
		unmapped += unmapped_page;
	}

	iommu_iotlb_sync(&iotlb_gather);

	iommu_iotlb_gather_init(&iotlb_gather);
	return unmapped;
}

bool iommu_pgtable_alloc(iommu_cfg_t *cfg)
{
	BUG_ON(!cfg || cfg->fmt >= NR_IOMMU_PGTABLE_FMTS);

	return iommu_hw_alloc_table(cfg);
}

/* ======================================================================
 * IOMMU Initializers
 * ====================================================================== */
bool iommu_dma_mapped(iommu_map_t iommu)
{
	for (i = 0; i < iommu_device_ctrl.count; i++) {
		if (iommu_device_ctrl.iommus[i] == iommu)
			return true;
	}
	return false;
}

void iommu_register_dma(int nr_iommus, iommu_t *iommus)
{
	__unused iommu_dev_t dev, sdev;
	__unused iommu_map_t map;
	int i;

	for (i = 0; i < nr_iommus; i++) {
		iommu_t iommu = iommus[i];

		dev = IOMMU_DEV(iommu);
		sdev = iommu_device_save(dev);
		if (!iommu_device_ctrl.valid) {
			iommu_device_ctrl.valid = true;
			iommu_device_ctrl.id = dev;
			iommu_device_ctrl.count = 0;
			iommu_device_ctrl.dma = DMA_DEFAULT;
		}
		map = IOMMU_MAP(iommu);
		if (!iommu_dma_mapped(map)) {
			BUG_ON(iommu_device_ctrl.count >= MAX_IOMMU_RIDS);
			iommu_device_ctrl.iommus[iommu_device_ctrl.count] = map;
			iommu_device_ctrl.count++
			iommu_device_restore(sdev);
		}
	}
}

#if 0
iommu_grp_t iommu_probe_device(int nr_iommus, iommu_t *iommus)
{
	iommu_grp_t grp;
	iommu_dom_t dom;

	grp = iommu_hw_alloc_master(nr_iommus, iommus);
	if (grp != INVALID_IOMMU_GRP) {
		iommu_group_select(grp);
		dom = iommu_alloc_domain(IOMMU_DOMAIN_DEFAULT);
		con_log("iommu: No domain space.\n");
		BUG_ON(dom == INVALID_IOMMU_DOM);
		iommu_group_ctrl.dom = dom;
		iommu_group_ctrl.default_dom = dom;
		/* iommu_map_direct(); */
	}

	return grp;
}
#endif

void iommu_init(void)
{
	__unused iommu_dev_t dev, sdev;
	__unused iommu_grp_t grp, sgrp;
	__unused iommu_dom_t dom, sdom;

	/* Allocatable groups are initialized */
	for (grp = 0; grp < NR_IOMMU_GROUPS; grp++) {
		sgrp = iommu_group_save(grp);
		iommu_group_ctrl.id = grp;
		iommu_group_ctrl.dev = INVALID_IOMMU_DEV;
		iommu_group_restore(sgrp);
	}
	/* Allocatable domains are initialized */
	for (dom = 0; dom < NR_IOMMU_DOMAINS; dom++) {
		sdom = iommu_domain_save(dom);
		iommu_domain_ctrl.id = dom;
		iommu_domain_ctrl.grp = INVALID_IOMMU_GRP;
		iommu_domain_ctrl.fmt = INVALID_IOMMU_FMT;
		iommu_domain_restore(sdom);
	}

	/* Non-allocatable devices are probed */
	for (dev = 0; dev < NR_IOMMU_DEVICES; dev++) {
		sdev = iommu_device_save(dev);
		if (iommu_device_ctrl.valid)
			iommu_hw_ctrl_init();
		iommu_device_restore(sdev);
	}
}
