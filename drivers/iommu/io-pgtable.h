/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __IO_PGTABLE_H
#define __IO_PGTABLE_H

#include <target/bitops.h>
#include <target/iommu-pgtable.h>

/**
 * struct iommu_flush_ops - IOMMU callbacks for TLB and page table management.
 *
 * @tlb_flush_all:  Synchronously invalidate the entire TLB context.
 * @tlb_flush_walk: Synchronously invalidate all intermediate TLB state
 *                  (sometimes referred to as the "walk cache") for a virtual
 *                  address range.
 * @tlb_add_page:   Optional callback to queue up leaf TLB invalidation for a
 *                  single page.  IOMMUs that cannot batch TLB invalidation
 *                  operations efficiently will typically issue them here, but
 *                  others may decide to update the iommu_iotlb_gather structure
 *                  and defer the invalidation until iommu_iotlb_sync() instead.
 *
 * Note that these can all be called in atomic context and must therefore
 * not block.
 */
struct iommu_flush_ops {
	void (*tlb_flush_all)(void *cookie);
	void (*tlb_flush_walk)(unsigned long iova, size_t size, size_t granule,
			       void *cookie);
	void (*tlb_add_page)(struct iommu_iotlb_gather *gather,
			     unsigned long iova, size_t granule, void *cookie);
};

/**
 * struct io_pgtable_ops - Page table manipulation API for IOMMU drivers.
 *
 * @map_pages:    Map a physically contiguous range of pages of the same size.
 * @unmap_pages:  Unmap a range of virtually contiguous pages of the same size.
 * @iova_to_phys: Translate iova to physical address.
 *
 * These functions map directly onto the iommu_ops member functions with
 * the same names.
 */
// struct io_pgtable_ops {
// 	int (*map_pages)(struct io_pgtable_ops *ops, unsigned long iova,
// 			 phys_addr_t paddr, size_t pgsize, size_t pgcount,
// 			 int prot, gfp_t gfp, size_t *mapped);
// 	size_t (*unmap_pages)(struct io_pgtable_ops *ops, unsigned long iova,
// 			      size_t pgsize, size_t pgcount,
// 			      struct iommu_iotlb_gather *gather);
// 	phys_addr_t (*iova_to_phys)(struct io_pgtable_ops *ops,
// 				    unsigned long iova);
// };

/**
 * alloc_io_pgtable_ops() - Allocate a page table allocator for use by an IOMMU.
 *
 * @fmt:    The page table format.
 * @cfg:    The page table configuration. This will be modified to represent
 *          the configuration actually provided by the allocator (e.g. the
 *          pgsize_bitmap may be restricted).
 * @cookie: An opaque token provided by the IOMMU driver and passed back to
 *          the callback routines in cfg->tlb.
 */
// struct io_pgtable_ops *alloc_io_pgtable_ops(enum io_pgtable_fmt fmt,
// 					    struct io_pgtable_cfg *cfg,
// 					    void *cookie);

/**
 * free_io_pgtable_ops() - Free an io_pgtable_ops structure. The caller
 *                         *must* ensure that the page table is no longer
 *                         live, but the TLB can be dirty.
 *
 * @ops: The ops returned from alloc_io_pgtable_ops.
 */
// void free_io_pgtable_ops(struct io_pgtable_ops *ops);


/*
 * Internal structures for page table allocator implementations.
 */

/**
 * struct io_pgtable - Internal structure describing a set of page tables.
 *
 * @fmt:    The page table format.
 * @cookie: An opaque token provided by the IOMMU driver and passed back to
 *          any callback routines.
 * @cfg:    A copy of the page table configuration.
 * @ops:    The page table operations in use for this set of page tables.
 */
// struct io_pgtable {
// 	enum io_pgtable_fmt	fmt;
// 	void			*cookie;
// 	struct io_pgtable_cfg	cfg;
// 	// struct io_pgtable_ops	ops;
// };

#define io_pgtable_ops_to_pgtable(x) container_of((x), struct io_pgtable, ops)

static inline void io_pgtable_tlb_flush_all(struct io_pgtable *iop)
{
	// if (iop->cfg.tlb && iop->cfg.tlb->tlb_flush_all)
	// 	iop->cfg.tlb->tlb_flush_all(iop->cookie);
}

static inline void
io_pgtable_tlb_flush_walk(struct io_pgtable *iop, unsigned long iova,
			  size_t size, size_t granule)
{
	// if (iop->cfg.tlb && iop->cfg.tlb->tlb_flush_walk)
	// 	iop->cfg.tlb->tlb_flush_walk(iova, size, granule, iop->cookie);
}

static inline void
io_pgtable_tlb_add_page(struct io_pgtable *iop,
			struct iommu_iotlb_gather * gather, unsigned long iova,
			size_t granule)
{
	// if (iop->cfg.tlb && iop->cfg.tlb->tlb_add_page)
	// 	iop->cfg.tlb->tlb_add_page(gather, iova, granule, iop->cookie);
}

/**
 * struct io_pgtable_init_fns - Alloc/free a set of page tables for a
 *                              particular format.
 *
 * @alloc: Allocate a set of page tables described by cfg.
 * @free:  Free the page tables associated with iop.
 */
struct io_pgtable_init_fns {
	struct io_pgtable *(*alloc)(struct io_pgtable_cfg *cfg, void *cookie);
	void (*free)(struct io_pgtable *iop);
};

extern struct io_pgtable_init_fns io_pgtable_arm_32_lpae_s1_init_fns;
extern struct io_pgtable_init_fns io_pgtable_arm_32_lpae_s2_init_fns;
extern struct io_pgtable_init_fns io_pgtable_arm_64_lpae_s1_init_fns;
extern struct io_pgtable_init_fns io_pgtable_arm_64_lpae_s2_init_fns;
extern struct io_pgtable_init_fns io_pgtable_arm_v7s_init_fns;
extern struct io_pgtable_init_fns io_pgtable_arm_mali_lpae_init_fns;
extern struct io_pgtable_init_fns io_pgtable_amd_iommu_v1_init_fns;
extern struct io_pgtable_init_fns io_pgtable_amd_iommu_v2_init_fns;
extern struct io_pgtable_init_fns io_pgtable_apple_dart_init_fns;
extern struct io_pgtable_init_fns io_pgtable_riscv_init_fns;

#endif /* __IO_PGTABLE_H */
