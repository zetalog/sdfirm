// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright © 2022-2023 Rivos Inc.
 *
 * RISC-V IOMMU page table allocator.
 *
 * Authors:
 *	Tomasz Jeznach <tjeznach@rivosinc.com>
 *	Sebastien Boeuf <seb@rivosinc.com>
 */

#include <target/iommu.h>
#include <target/iommu-pgtable.h>
#include <target/panic.h>
#include <target/iommu.h>
#include <target/page.h>
#include <target/paging.h>

#include "iommu.h"

static inline int pmd_leaf_xx(pmd_t pmd);

extern int riscv_iommu_save_pgtbl(uint64_t pgtbl);
static struct io_pgtable riscv_io_pgtables[NR_IOMMU_DOMAINS];

static inline size_t get_page_size(size_t size)
{
	if (size >= IOMMU_PAGE_SIZE_512G)
		return IOMMU_PAGE_SIZE_512G;

	if (size >= IOMMU_PAGE_SIZE_1G)
		return IOMMU_PAGE_SIZE_1G;

	if (size >= IOMMU_PAGE_SIZE_2M)
		return IOMMU_PAGE_SIZE_2M;

	return IOMMU_PAGE_SIZE_4K;
}
#define _PAGE_PFN_MASK  GENMASK(53, 10)
#define __page_val_to_pfn(_val)  (((_val) & _PAGE_PFN_MASK) >> _PAGE_PFN_SHIFT)

static void riscv_iommu_pt_walk_free(pmd_t * ptp, unsigned shift, bool root)
{
	pmd_t *pte, *pt_base;
	int i;

	if (shift == PAGE_SHIFT)
		return;

	if (root)
		pt_base = ptp;
	else
		pt_base =
		    (pmd_t *) pfn_to_virt(__page_val_to_pfn(pmd_val(*ptp)));

	/* Recursively free all sub page table pages */
	for (i = 0; i < PTRS_PER_PMD; i++) {
		pte = pt_base + i;
		if (pmd_present(*pte) && !pmd_leaf_xx(*pte))
			riscv_iommu_pt_walk_free(pte, shift - 9, false);
	}

	/* Now free the current page table page */
	if (!root && pmd_present(*pt_base))
		page_free_pages((struct page *)pt_base, shift);
}

void riscv_iommu_free_pgtable(struct io_pgtable *iop)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;
	riscv_iommu_pt_walk_free((pmd_t *) domain->pgd_root, PGDIR_SHIFT, true);
}

static pte_t *riscv_iommu_pt_walk_alloc(pmd_t * ptp, unsigned long iova,
					unsigned shift, bool root,
					size_t pgsize,
					unsigned long (*pd_alloc)(void))
{
	pmd_t *pte;
	unsigned long pfn;

	if (root)
		pte = ptp + ((iova >> shift) & (PTRS_PER_PMD - 1));
	else
		pte = (pmd_t *) pfn_to_virt(__page_val_to_pfn(pmd_val(*ptp))) +
		    ((iova >> shift) & (PTRS_PER_PMD - 1));

	if ((1ULL << shift) <= pgsize) {
		if (pmd_present(*pte) && !pmd_leaf_xx(*pte))
			riscv_iommu_pt_walk_free(pte, shift - 9, false);
		return (pte_t *) pte;
	}

	if (pmd_none(*pte)) {
		pfn = pd_alloc ? virt_to_pfn(pd_alloc()) : 0;
		if (!pfn)
			return NULL;
		riscv_iommu_save_pgtbl((uint64_t)(pfn << PAGE_SHIFT));
		set_pmd(pte, __pmd((pfn << _PAGE_PFN_SHIFT) | _PAGE_TABLE));
	}

	return riscv_iommu_pt_walk_alloc(pte, iova, shift - 9, false,
					 pgsize, pd_alloc);
}

static inline int pmd_leaf_xx(pmd_t pmd)
{
	return pmd_present(pmd) && (pmd_val(pmd) & _PAGE_LEAF);
}

static pte_t *riscv_iommu_pt_walk_fetch(pmd_t * ptp,
					unsigned long iova, unsigned shift,
					bool root)
{
	pmd_t *pte;

	if (root)
		pte = ptp + ((iova >> shift) & (PTRS_PER_PMD - 1));
	else
		pte = (pmd_t *) pfn_to_virt(__page_val_to_pfn(pmd_val(*ptp))) +
		    ((iova >> shift) & (PTRS_PER_PMD - 1));
	con_log("io pgtbl: pte(%p) = 0x%lx  \n", pte, pmd_val(*pte));
	if (pmd_leaf_xx(*pte))
		return (pte_t *) pte;
	else if (pmd_none(*pte))
		return NULL;
	else if (shift == PAGE_SHIFT)
		return NULL;

	return riscv_iommu_pt_walk_fetch(pte, iova, shift - 9, false);
}
	
int riscv_iommu_map_pages(unsigned long iova, phys_addr_t phys,
				 size_t pgsize, size_t pgcount, int prot)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;
	size_t size = 0;
	size_t page_size = get_page_size(pgsize);
	pte_t *pte;
	pte_t pte_val;
	pgprot_t pte_prot;
	int pgdir_shift = 0;

	if (domain->domain.type == IOMMU_DOMAIN_BLOCKED) {
		return -ENODEV;
	}

	if (domain->domain.type == IOMMU_DOMAIN_IDENTITY) {
		return 0;
	}

	if (domain->mode == 0x8) {
		pgdir_shift = 30;
	} else 	if (domain->mode == 0x9) {
		pgdir_shift = 39;
	} else {
		pgdir_shift = 30;
	}

	pte_prot = (prot & IOMMU_WRITE) ?
	    __pgprot(_PAGE_BASE | _PAGE_READ | _PAGE_WRITE | _PAGE_DIRTY) :
	    __pgprot(_PAGE_BASE | _PAGE_READ);

	while (pgcount--) {
		pte =
		    riscv_iommu_pt_walk_alloc((pmd_t *) domain->pgd_root, iova,
					      pgdir_shift, true, page_size,
					      page_alloc_zeroed);
		if (!pte) {
			return -ENOMEM;
		}

		pte_val = pfn_pte(phys_to_pfn(phys), pte_prot);

		con_log("PTE: %p = %016lx\n", pte, pte_val);
		set_pte(pte, pte_val);

		size += page_size;
		iova += page_size;
		phys += page_size;
	}

	return 0;
}

phys_addr_t riscv_iommu_pgtbl_ptw(uint64_t pgd_root, uint64_t iova, int mode)
{
	pte_t *pte;
	int pgdir_shift = 0;

	if (mode == 0x8) {
		pgdir_shift = 30;
	} else 	if (mode == 0x9) {
		pgdir_shift = 39;
	} else {
		pgdir_shift = 30;
	}

	pte = riscv_iommu_pt_walk_fetch((pmd_t *)pgd_root, iova, pgdir_shift, true);

	if (!pte || !pte_present(*pte)) {
		con_log("PTE: %p = 0x%lx addr:%lx\n", pte, pte_pfn(*pte), pfn_to_phys(pte_pfn(*pte)));
		return (pfn_to_phys(pte_pfn(*pte)) | (iova & PAGE_MASK));
	}
	
	return 0;
}

size_t riscv_iommu_unmap_pages(unsigned long iova, size_t pgsize,
				      size_t pgcount,
				      void *gather)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;
	size_t size = 0;

	size_t page_size = get_page_size(pgsize);
	pte_t *pte;

	if (domain->domain.type == IOMMU_DOMAIN_IDENTITY)
		return pgsize * pgcount;

	while (pgcount--) {
		pte = riscv_iommu_pt_walk_fetch((pmd_t *) domain->pgd_root,
						iova, PGDIR_SHIFT, true);
		if (!pte)
			return size;

		set_pte(pte, __pte(0));

		iommu_iotlb_gather_add_page(gather, iova, pgsize);

		size += page_size;
		iova += page_size;
	}

	return size;
}

static phys_addr_t riscv_iommu_iova_to_phys(unsigned long iova)
{
	struct riscv_iommu_domain *domain = &riscv_iommu_domain_ctrl;
	pte_t *pte;

	if (domain->domain.type == IOMMU_DOMAIN_IDENTITY)
		return (phys_addr_t) iova;

	pte = riscv_iommu_pt_walk_fetch((pmd_t *) domain->pgd_root,
					iova, PGDIR_SHIFT, true);
	if (!pte || !pte_present(*pte))
		return 0;

	return (pfn_to_phys(pte_pfn(*pte)) | (iova & PAGE_MASK));
}
#if 0
static void riscv_iommu_tlb_inv_all(void *cookie)
{
}

static void riscv_iommu_tlb_inv_walk(unsigned long iova, size_t size,
				     size_t granule, void *cookie)
{
}

static void riscv_iommu_tlb_add_page(struct iommu_iotlb_gather *gather,
				     unsigned long iova, size_t granule,
				     void *cookie)
{
}
#endif

/* NOTE: cfg should point to riscv_iommu_domain structure member pgtbl.cfg */

struct io_pgtable *riscv_iommu_alloc_pgtable(iommu_cfg_t *cfg)
{
	struct io_pgtable *iop = &riscv_io_pgtables[iommu_dom];

	cfg->pgsize_bitmap = SZ_4K | SZ_2M | SZ_1G;
	cfg->ias = 57;		// va mode, SvXX -> ias
	cfg->oas = 57;		// pa mode, or SvXX+4 -> oas
	// cfg->tlb = &riscv_iommu_flush_ops;

	// iop->ops.map_pages = riscv_iommu_map_pages;
	// iop->ops.unmap_pages = riscv_iommu_unmap_pages;
	// iop->ops.iova_to_phys = riscv_iommu_iova_to_phys;

	return iop;
}
