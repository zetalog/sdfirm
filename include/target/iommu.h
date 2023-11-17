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
 * @(#)iommu.h: I/O memory management unit definitions
 * $Id: iommu.h,v 1.1 2020-11-19 15:13:00 zhenglv Exp $
 */

#ifndef __IOMMU_H_INCLUDE__
#define __IOMMU_H_INCLUDE__

#include <target/generic.h>

#ifdef CONFIG_IOMMU_MAX_DEVICES
#define NR_IOMMU_DEVICES	CONFIG_IOMMU_MAX_DEVICES
#else
#define NR_IOMMU_DEVICES	1
#endif
#define INVALID_IOMMU_DEV	NR_IOMMU_DEVICES
#ifdef CONFIG_IOMMU_MAX_GROUPS
#define NR_IOMMU_GROUPS		CONFIG_IOMMU_MAX_GROUPS
#else
#define NR_IOMMU_GROUPS		1
#endif
#if NR_IOMMU_GROUPS > NR_IOMMUS
#undef NR_IOMMU_GROUPS
#define NR_IOMMU_GROUPS		NR_IOMMUS
#endif
#define INVALID_IOMMU_GRP	NR_IOMMU_GROUPS
#ifdef CONFIG_IOMMU_MAX_DOMAINS
#define NR_IOMMU_DOMAINS	CONFIG_IOMMU_MAX_DOMAINS
#else
#define NR_IOMMU_DOMAINS	1
#endif
#define INVALID_IOMMU_DOM	NR_IOMMU_DOMAINS

typedef uint8_t iommu_dev_t;
typedef uint8_t iommu_dom_t;
typedef uint8_t iommu_grp_t;

#define IOMMU_READ	(1 << 0)
#define IOMMU_WRITE	(1 << 1)
#define IOMMU_CACHE	(1 << 2) /* DMA cache coherency */
#define IOMMU_NOEXEC	(1 << 3)
#define IOMMU_MMIO	(1 << 4) /* e.g. things like MSI doorbells */
/* The bus includes a privilege level as part of its access type, and
 * certain devices are capable of issuing transactions marked as either
 * 'supervisor' or 'user', the IOMMU_PRIV flag requests that the other
 * given permission flags only apply to accesses at the higher privilege
 * level, and that unprivileged transactions should have as little access
 * as possible. This would usually imply the same permissions as kernel
 * mappings on the CPU, if the IOMMU page table format is equivalent.
 */
#define IOMMU_PRIV	(1 << 5)
/* Non-coherent masters can use this page protection flag to set cacheable
 * memory attributes for only a transparent outer level of cache, also
 * known as the last-level or system cache.
 */
#define IOMMU_SYS_CACHE_ONLY	(1 << 6)

struct iommu_iotlb_gather {
	unsigned long start;
	unsigned long end;
	size_t pgsize;
};

#include <target/iommu-pgtable.h>

/* SoC specific descriptor, used to indicate an IOMMU master controlling a
 * translation group.
 */
typedef uint64_t iommu_t;

#include <driver/iommu.h>

struct scatterlist {
	unsigned long page_link;
	unsigned int offset;
	unsigned int length;
	phys_addr_t dma_address;
#ifdef CONFIG_NEED_SG_DMA_LENGTH
	unsigned int dma_length;
#endif
};

struct iommu_device {
	iommu_dev_t id;
	unsigned long pgsize_bitmap;
};

struct iommu_group {
	iommu_grp_t id;
	iommu_dev_t dev;
	iommu_dom_t default_dom;
	iommu_dom_t dom;
};

struct iommu_domain {
	iommu_dom_t id;
	iommu_grp_t grp;
	uint8_t type;
#define IOMMU_DOMAIN_BLOCKED		0
#define IOMMU_DOMAIN_IDENTITY		1
#define IOMMU_DOMAIN_UNMANAGED		2
#define IOMMU_DOMAIN_DMA		6
#ifdef CONFIG_IOMMU_DEF_DOMAIN_TRANSLATED
#define IOMMU_DOMAIN_DEFAULT		IOMMU_DOMAIN_DMA
#endif
#ifdef CONFIG_IOMMU_DEF_DOMAIN_PASSTHROUGH
#define IOMMU_DOMAIN_DEFAULT		IOMMU_DOMAIN_IDENTITY
#endif
	unsigned long pgsize_bitmap;
	iommu_fmt_t fmt;
};

iommu_grp_t iommu_alloc_group(void);
void iommu_free_group(iommu_grp_t grp);
iommu_dom_t iommu_alloc_domain(uint8_t type);
void iommu_free_domain(iommu_dom_t dom);

iommu_grp_t iommu_register_master(iommu_t iommu);

int iommu_map(unsigned long iova, size_t size, phys_addr_t paddr, int prot);
int iommu_unmap(unsigned long iova, size_t size);

#if NR_IOMMU_DEVICES > 1
extern iommu_dev_t iommu_dev;
extern struct iommu_device iommu_devices[NR_IOMMU_DEVICES];
iommu_dev_t iommu_device_save(iommu_dev_t dev);
void iommu_device_restore(iommu_dev_t dev);
#define iommu_device_ctrl		iommu_devices[iommu_dev]
#else
#define iommu_dev			0
#define iommu_device_save(dev)		0
#define iommu_device_restore(dev)
extern struct iommu_device iommu_device_ctrl;
#endif
#define iommu_device_select(dev)	iommu_device_restore(dev)

#if NR_IOMMU_GROUPS > 1
extern iommu_grp_t iommu_grp;
extern struct iommu_group iommu_groups[NR_IOMMU_GROUPS];
iommu_grp_t iommu_group_save(iommu_grp_t grp);
void iommu_group_restore(iommu_grp_t grp);
#define iommu_group_ctrl		iommu_groups[iommu_grp]
#else
#define iommu_grp			0
#define iommu_group_save(grp)		0
#define iommu_group_restore(grp)
extern struct iommu_group iommu_group_ctrl;
#endif
#define iommu_group_select(dev)		iommu_group_restore(dev)

#if NR_IOMMU_DOMAINS > 1
extern iommu_dom_t iommu_dom;
extern struct iommu_domain iommu_domains[NR_IOMMU_DOMAINS];
iommu_dom_t iommu_domain_save(iommu_dom_t dom);
void iommu_domain_restore(iommu_dom_t dom);
#define iommu_domain_ctrl		iommu_domains[iommu_dom]
#else
#define iommu_dom			0
#define iommu_domain_save(dom)		0
#define iommu_domain_restore(dom)
extern struct iommu_domain iommu_domain_ctrl;
#endif
#define iommu_domain_select(dev)	iommu_domain_restore(dev)

iommu_dom_t iommu_get_domain(iommu_grp_t grp);
iommu_dom_t iommu_get_dma_domain(iommu_grp_t grp);

int dma_info_to_prot(uint8_t dir, bool coherent, unsigned long attrs);
unsigned long iommu_iova_alloc(iommu_dom_t dom, size_t size);

#endif /* __IOMMU_H_INCLUDE__ */
