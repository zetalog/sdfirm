#include <target/iommu.h>
#include <target/paging.h>
#include <target/dma.h>
#include <target/console.h>
#include <target/panic.h>
#include <target/atomic.h>
#include <target/heap.h>

#define ARM_LPAE_MAX_ADDR_BITS		52
#define ARM_LPAE_S2_MAX_CONCAT_PAGES	16
#define ARM_LPAE_MAX_LEVELS		4

/* Calculate the right shift amount to get to the portion describing level l
 * in a virtual address mapped by the pagetable in d.
 */
#define ARM_LPAE_LVL_SHIFT(l,d)						\
	(((ARM_LPAE_MAX_LEVELS - (l)) * (d)->bits_per_level) +		\
	ilog2_const(sizeof(arm_lpae_iopte)))

#define ARM_LPAE_GRANULE(d)						\
	(sizeof(arm_lpae_iopte) << (d)->bits_per_level)
#define ARM_LPAE_PGD_SIZE(d)						\
	(sizeof(arm_lpae_iopte) << (d)->pgd_bits)

/* Calculate the index at level l used to map virtual address a using the
 * pagetable in d.
 */
#define ARM_LPAE_PGD_IDX(l,d)						\
	((l) == (d)->start_level ? (d)->pgd_bits - (d)->bits_per_level : 0)

#define ARM_LPAE_LVL_IDX(a,l,d)						\
	(((uint64_t)(a) >> ARM_LPAE_LVL_SHIFT(l,d)) &			\
	 ((1 << ((d)->bits_per_level + ARM_LPAE_PGD_IDX(l,d))) - 1))

/* Calculate the block/page mapping size at level l for pagetable in d. */
#define ARM_LPAE_BLOCK_SIZE(l,d)	(ULL(1) << ARM_LPAE_LVL_SHIFT(l,d))

/* Page table bits */
#define ARM_LPAE_PTE_TYPE_SHIFT		0
#define ARM_LPAE_PTE_TYPE_MASK		0x3

#define ARM_LPAE_PTE_TYPE_BLOCK		1
#define ARM_LPAE_PTE_TYPE_TABLE		3
#define ARM_LPAE_PTE_TYPE_PAGE		3

#define ARM_LPAE_PTE_ADDR_MASK		GENMASK_ULL(47,12)

#define ARM_LPAE_PTE_NSTABLE		(((arm_lpae_iopte)1) << 63)
#define ARM_LPAE_PTE_XN			(((arm_lpae_iopte)3) << 53)
#define ARM_LPAE_PTE_AF			(((arm_lpae_iopte)1) << 10)
#define ARM_LPAE_PTE_SH_NS		(((arm_lpae_iopte)0) << 8)
#define ARM_LPAE_PTE_SH_OS		(((arm_lpae_iopte)2) << 8)
#define ARM_LPAE_PTE_SH_IS		(((arm_lpae_iopte)3) << 8)
#define ARM_LPAE_PTE_NS			(((arm_lpae_iopte)1) << 5)
#define ARM_LPAE_PTE_VALID		(((arm_lpae_iopte)1) << 0)

#define ARM_LPAE_PTE_ATTR_LO_MASK	(((arm_lpae_iopte)0x3ff) << 2)
/* Ignore the contiguous bit for block splitting */
#define ARM_LPAE_PTE_ATTR_HI_MASK	(((arm_lpae_iopte)6) << 52)
#define ARM_LPAE_PTE_ATTR_MASK		(ARM_LPAE_PTE_ATTR_LO_MASK |	\
					 ARM_LPAE_PTE_ATTR_HI_MASK)
/* Software bit for solving coherency races */
#define ARM_LPAE_PTE_SW_SYNC		(((arm_lpae_iopte)1) << 55)

/* Stage-1 PTE */
#define ARM_LPAE_PTE_AP_UNPRIV		(((arm_lpae_iopte)1) << 6)
#define ARM_LPAE_PTE_AP_RDONLY		(((arm_lpae_iopte)2) << 6)
#define ARM_LPAE_PTE_ATTRINDX_SHIFT	2
#define ARM_LPAE_PTE_nG			(((arm_lpae_iopte)1) << 11)

/* Stage-2 PTE */
#define ARM_LPAE_PTE_HAP_FAULT		(((arm_lpae_iopte)0) << 6)
#define ARM_LPAE_PTE_HAP_READ		(((arm_lpae_iopte)1) << 6)
#define ARM_LPAE_PTE_HAP_WRITE		(((arm_lpae_iopte)2) << 6)
#define ARM_LPAE_PTE_MEMATTR_OIWB	(((arm_lpae_iopte)0xf) << 2)
#define ARM_LPAE_PTE_MEMATTR_NC		(((arm_lpae_iopte)0x5) << 2)
#define ARM_LPAE_PTE_MEMATTR_DEV	(((arm_lpae_iopte)0x1) << 2)

/* Register bits */
#define ARM_LPAE_TCR_TG0_4K		0
#define ARM_LPAE_TCR_TG0_64K		1
#define ARM_LPAE_TCR_TG0_16K		2

#define ARM_LPAE_TCR_TG1_16K		1
#define ARM_LPAE_TCR_TG1_4K		2
#define ARM_LPAE_TCR_TG1_64K		3

#define ARM_LPAE_TCR_SH_NS		0
#define ARM_LPAE_TCR_SH_OS		2
#define ARM_LPAE_TCR_SH_IS		3

#define ARM_LPAE_TCR_RGN_NC		0
#define ARM_LPAE_TCR_RGN_WBWA		1
#define ARM_LPAE_TCR_RGN_WT		2
#define ARM_LPAE_TCR_RGN_WB		3

#define ARM_LPAE_VTCR_SL0_MASK		0x3

#define ARM_LPAE_TCR_T0SZ_SHIFT		0

#define ARM_LPAE_VTCR_PS_SHIFT		16
#define ARM_LPAE_VTCR_PS_MASK		0x7

#define ARM_LPAE_TCR_PS_32_BIT		0x0ULL
#define ARM_LPAE_TCR_PS_36_BIT		0x1ULL
#define ARM_LPAE_TCR_PS_40_BIT		0x2ULL
#define ARM_LPAE_TCR_PS_42_BIT		0x3ULL
#define ARM_LPAE_TCR_PS_44_BIT		0x4ULL
#define ARM_LPAE_TCR_PS_48_BIT		0x5ULL
#define ARM_LPAE_TCR_PS_52_BIT		0x6ULL

#define ARM_LPAE_MAIR_ATTR_SHIFT(n)	((n) << 3)
#define ARM_LPAE_MAIR_ATTR_MASK		0xff
#define ARM_LPAE_MAIR_ATTR_DEVICE	0x04
#define ARM_LPAE_MAIR_ATTR_NC		0x44
#define ARM_LPAE_MAIR_ATTR_INC_OWBRWA	0xf4
#define ARM_LPAE_MAIR_ATTR_WBRWA	0xff
#define ARM_LPAE_MAIR_ATTR_IDX_NC	0
#define ARM_LPAE_MAIR_ATTR_IDX_CACHE	1
#define ARM_LPAE_MAIR_ATTR_IDX_DEV	2
#define ARM_LPAE_MAIR_ATTR_IDX_INC_OCACHE	3

/* IOPTE accessors */
#define iopte_deref(pte,d)		((void *)__va(iopte_to_paddr(pte, d)))

#define iopte_type(pte,l)					\
	(((pte) >> ARM_LPAE_PTE_TYPE_SHIFT) & ARM_LPAE_PTE_TYPE_MASK)

#define iopte_prot(pte)			((pte) & ARM_LPAE_PTE_ATTR_MASK)

struct arm_lpae_io_pgtable {
	struct io_pgtable_cfg cfg;
	int pgd_bits;
	int start_level;
	int bits_per_level;
	void *pgd;
};

typedef uint64_t arm_lpae_iopte;

struct arm_lpae_io_pgtable arm_io_pgtables[NR_IOMMU_DOMAINS];

static inline bool iopte_leaf(arm_lpae_iopte pte, int lvl,
			      enum io_pgtable_fmt fmt)
{
	if (lvl == (ARM_LPAE_MAX_LEVELS - 1))
		return iopte_type(pte, lvl) == ARM_LPAE_PTE_TYPE_PAGE;

	return iopte_type(pte, lvl) == ARM_LPAE_PTE_TYPE_BLOCK;
}

static arm_lpae_iopte paddr_to_iopte(phys_addr_t paddr,
				     struct arm_lpae_io_pgtable *data)
{
	arm_lpae_iopte pte = paddr;

	/* Of the bits which overlap, either 51:48 or 15:12 are always RES0 */
	return (pte | (pte >> (48 - 12))) & ARM_LPAE_PTE_ADDR_MASK;
}

static phys_addr_t iopte_to_paddr(arm_lpae_iopte pte,
				  struct arm_lpae_io_pgtable *data)
{
	uint64_t paddr = pte & ARM_LPAE_PTE_ADDR_MASK;

	if (ARM_LPAE_GRANULE(data) < SZ_64K)
		return paddr;

	/* Rotate the packed high-order bits back to the top */
	return (paddr | (paddr << (48 - 12))) & (ARM_LPAE_PTE_ADDR_MASK << 4);
}

static dma_addr_t __arm_lpae_dma_addr(void *pages)
{
	return (dma_addr_t)virt_to_phys(pages);
}

static void *__arm_lpae_alloc_pages(size_t size, struct io_pgtable_cfg *cfg)
{
	struct page *p;
	dma_addr_t dma;
	int nr_pages = size >> PAGE_SHIFT;
	void *pages;

	p = page_alloc_pages(nr_pages);
	if (!p)
		return NULL;

	pages = page_address(p);
	if (!cfg->coherent_walk) {
		dma = dma_map_single(iommu_group_ctrl.id, (phys_addr_t)pages,
				     size, DMA_TO_DEVICE);
		/* We depend on the IOMMU being able to work with any physical
		 * address directly, so if the DMA layer suggests otherwise by
		 * translating or truncating them, that bodes very badly...
		 */
		if (dma != virt_to_phys(pages))
			goto out_unmap;
	}
	return pages;

out_unmap:
	con_err("iommu_armv8: Cannot accommodate DMA translation for IOMMU page tables\n");
	dma_unmap_single(iommu_group_ctrl.id, dma, size, DMA_TO_DEVICE);
	page_free_pages(p, nr_pages);
	return NULL;
}

static void __arm_lpae_free_pages(void *pages, size_t size,
				  struct io_pgtable_cfg *cfg)
{
	if (!cfg->coherent_walk)
		dma_unmap_single(iommu_group_ctrl.id,
				 __arm_lpae_dma_addr(pages), size,
				 DMA_TO_DEVICE);
	page_free_pages(pages, size >> PAGE_SHIFT);
}

static void __arm_lpae_sync_pte(arm_lpae_iopte *ptep,
				struct io_pgtable_cfg *cfg)
{
	dma_sync_dev(iommu_group_ctrl.id, __arm_lpae_dma_addr(ptep),
		     sizeof(*ptep), DMA_TO_DEVICE);
}

static void __arm_lpae_set_pte(arm_lpae_iopte *ptep, arm_lpae_iopte pte,
			       struct io_pgtable_cfg *cfg)
{
	*ptep = pte;

	if (!cfg->coherent_walk)
		__arm_lpae_sync_pte(ptep, cfg);
}

static size_t __arm_lpae_unmap(struct iommu_iotlb_gather *gather,
			       unsigned long iova, size_t size, int lvl,
			       arm_lpae_iopte *ptep);

static void __arm_lpae_init_pte(phys_addr_t paddr, arm_lpae_iopte prot,
				int lvl, arm_lpae_iopte *ptep)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	arm_lpae_iopte pte = prot;

	if (lvl == ARM_LPAE_MAX_LEVELS - 1)
		pte |= ARM_LPAE_PTE_TYPE_PAGE;
	else
		pte |= ARM_LPAE_PTE_TYPE_BLOCK;

	pte |= paddr_to_iopte(paddr, data);

	__arm_lpae_set_pte(ptep, pte, &data->cfg);
}

static int arm_lpae_init_pte(unsigned long iova, phys_addr_t paddr,
			     arm_lpae_iopte prot, int lvl,
			     arm_lpae_iopte *ptep)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	arm_lpae_iopte pte = *ptep;

	if (iopte_leaf(pte, lvl, data->cfg.fmt)) {
		/* We require an unmap first */
		return -EEXIST;
	} else if (iopte_type(pte, lvl) == ARM_LPAE_PTE_TYPE_TABLE) {
		/* We need to unmap and free the old table before
		 * overwriting it with a block entry.
		 */
		arm_lpae_iopte *tblp;
		size_t sz = ARM_LPAE_BLOCK_SIZE(lvl, data);

		tblp = ptep - ARM_LPAE_LVL_IDX(iova, lvl, data);
		if (__arm_lpae_unmap(NULL, iova, sz, lvl, tblp) != sz) {
			return -EINVAL;
		}
	}

	__arm_lpae_init_pte(paddr, prot, lvl, ptep);
	return 0;
}

static arm_lpae_iopte arm_lpae_install_table(arm_lpae_iopte *table,
					     arm_lpae_iopte *ptep,
					     arm_lpae_iopte curr,
					     struct io_pgtable_cfg *cfg)
{
	arm_lpae_iopte old, new;

	new = __pa(table) | ARM_LPAE_PTE_TYPE_TABLE;

	/* Ensure the table itself is visible before its PTE can be.
	 * Whilst we could get away with cmpxchg_release below, this
	 * doesn't have any ordering semantics when !CONFIG_SMP.
	 */
	dma_wmb();

	old = cmpxchg_relaxed(ptep, curr, new);

	if (cfg->coherent_walk || (old & ARM_LPAE_PTE_SW_SYNC))
		return old;

	/* Even if it's not ours, there's no point waiting; just kick it */
	__arm_lpae_sync_pte(ptep, cfg);
	if (old == curr)
		WRITE_ONCE(*ptep, new | ARM_LPAE_PTE_SW_SYNC);

	return old;
}

static int __arm_lpae_map(unsigned long iova, phys_addr_t paddr,
			  size_t size, arm_lpae_iopte prot, int lvl,
			  arm_lpae_iopte *ptep)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	arm_lpae_iopte *cptep = NULL, pte;
	size_t block_size = ARM_LPAE_BLOCK_SIZE(lvl, data);
	size_t tblsz = ARM_LPAE_GRANULE(data);
	struct io_pgtable_cfg *cfg = &data->cfg;

	/* Find our entry at the current level */
	ptep += ARM_LPAE_LVL_IDX(iova, lvl, data);

	/* If we can install a leaf entry at this level, then do so */
	if (size == block_size)
		return arm_lpae_init_pte(iova, paddr, prot, lvl, ptep);

	/* We can't allocate tables at the final level */
	if (lvl >= ARM_LPAE_MAX_LEVELS - 1)
		return -EINVAL;

	/* Grab a pointer to the next level */
	pte = READ_ONCE(*ptep);
	if (!pte) {
		cptep = __arm_lpae_alloc_pages(tblsz, cfg);
		if (!cptep)
			return -ENOMEM;

		pte = arm_lpae_install_table(cptep, ptep, 0, cfg);
		if (pte)
			__arm_lpae_free_pages(cptep, tblsz, cfg);
	} else if (!cfg->coherent_walk && !(pte & ARM_LPAE_PTE_SW_SYNC)) {
		__arm_lpae_sync_pte(ptep, cfg);
	}

	if (pte && !iopte_leaf(pte, lvl, data->cfg.fmt)) {
		cptep = iopte_deref(pte, data);
	} else if (pte) {
		/* We require an unmap first */
		return -EEXIST;
	}

	/* Rinse, repeat */
	return __arm_lpae_map(iova, paddr, size, prot, lvl + 1, cptep);
}

static arm_lpae_iopte arm_lpae_prot_to_pte(iommu_fmt_t fmt, int prot)
{
	arm_lpae_iopte pte;

	if (fmt == ARM_64_LPAE_S1 || fmt == ARM_32_LPAE_S1) {
		pte = ARM_LPAE_PTE_nG;
		if (!(prot & IOMMU_WRITE) && (prot & IOMMU_READ))
			pte |= ARM_LPAE_PTE_AP_RDONLY;
		if (!(prot & IOMMU_PRIV))
			pte |= ARM_LPAE_PTE_AP_UNPRIV;
	} else {
		pte = ARM_LPAE_PTE_HAP_FAULT;
		if (prot & IOMMU_READ)
			pte |= ARM_LPAE_PTE_HAP_READ;
		if (prot & IOMMU_WRITE)
			pte |= ARM_LPAE_PTE_HAP_WRITE;
	}

	/* Note that this logic is structured to accommodate Mali LPAE
	 * having stage-1-like attributes but stage-2-like permissions.
	 */
	if (fmt == ARM_64_LPAE_S2 || fmt == ARM_32_LPAE_S2) {
		if (prot & IOMMU_MMIO)
			pte |= ARM_LPAE_PTE_MEMATTR_DEV;
		else if (prot & IOMMU_CACHE)
			pte |= ARM_LPAE_PTE_MEMATTR_OIWB;
		else
			pte |= ARM_LPAE_PTE_MEMATTR_NC;
	} else {
		if (prot & IOMMU_MMIO)
			pte |= (ARM_LPAE_MAIR_ATTR_IDX_DEV
				<< ARM_LPAE_PTE_ATTRINDX_SHIFT);
		else if (prot & IOMMU_CACHE)
			pte |= (ARM_LPAE_MAIR_ATTR_IDX_CACHE
				<< ARM_LPAE_PTE_ATTRINDX_SHIFT);
		else if (prot & IOMMU_SYS_CACHE_ONLY)
			pte |= (ARM_LPAE_MAIR_ATTR_IDX_INC_OCACHE
				<< ARM_LPAE_PTE_ATTRINDX_SHIFT);
	}

	if (prot & IOMMU_CACHE)
		pte |= ARM_LPAE_PTE_SH_IS;
	else
		pte |= ARM_LPAE_PTE_SH_OS;

	if (prot & IOMMU_NOEXEC)
		pte |= ARM_LPAE_PTE_XN;

	return pte;
}

int arm_lpae_map(unsigned long iova, phys_addr_t paddr,
		 size_t size, int iommu_prot)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	struct io_pgtable_cfg *cfg = &data->cfg;
	arm_lpae_iopte *ptep = data->pgd;
	int ret, lvl = data->start_level;
	arm_lpae_iopte prot;
	long iaext = (int64_t)iova >> cfg->ias;

	/* If no access, then nothing to do */
	if (!(iommu_prot & (IOMMU_READ | IOMMU_WRITE)))
		return 0;
	if (!size || (size & cfg->pgsize_bitmap) != size)
		return -EINVAL;
	if (iaext || paddr >> cfg->oas)
		return -ERANGE;

	prot = arm_lpae_prot_to_pte(data->cfg.fmt, iommu_prot);
	ret = __arm_lpae_map(iova, paddr, size, prot, lvl, ptep);
	/* Synchronise all PTE updates for the new mapping before there's
	 * a chance for anything to kick off a table walk for the new iova.
	 */
	wmb();

	return ret;
}

static void __arm_lpae_free_pgtable(int lvl, arm_lpae_iopte *ptep)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	arm_lpae_iopte *start, *end;
	unsigned long table_size;

	if (lvl == data->start_level)
		table_size = ARM_LPAE_PGD_SIZE(data);
	else
		table_size = ARM_LPAE_GRANULE(data);

	start = ptep;

	/* Only leaf entries at the last level */
	if (lvl == ARM_LPAE_MAX_LEVELS - 1)
		end = ptep;
	else
		end = (arm_lpae_iopte *)((uint8_t *)ptep + table_size);

	while (ptep != end) {
		arm_lpae_iopte pte = *ptep++;

		if (!pte || iopte_leaf(pte, lvl, data->cfg.fmt))
			continue;

		__arm_lpae_free_pgtable(lvl + 1, iopte_deref(pte, data));
	}

	__arm_lpae_free_pages(start, table_size, &data->cfg);
}

void arm_lpae_free_pgtable(void)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];

	__arm_lpae_free_pgtable(data->start_level, data->pgd);
}

static size_t arm_lpae_split_blk_unmap(struct iommu_iotlb_gather *gather,
				       unsigned long iova, size_t size,
				       arm_lpae_iopte blk_pte, int lvl,
				       arm_lpae_iopte *ptep)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	struct io_pgtable_cfg *cfg = &data->cfg;
	arm_lpae_iopte pte, *tablep;
	phys_addr_t blk_paddr;
	size_t tablesz = ARM_LPAE_GRANULE(data);
	size_t split_sz = ARM_LPAE_BLOCK_SIZE(lvl, data);
	int i, unmap_idx = -1;

	if (lvl == ARM_LPAE_MAX_LEVELS)
		return 0;

	tablep = __arm_lpae_alloc_pages(tablesz, cfg);
	if (!tablep)
		return 0; /* Bytes unmapped */

	if (size == split_sz)
		unmap_idx = ARM_LPAE_LVL_IDX(iova, lvl, data);

	blk_paddr = iopte_to_paddr(blk_pte, data);
	pte = iopte_prot(blk_pte);

	for (i = 0; i < tablesz / sizeof(pte); i++, blk_paddr += split_sz) {
		/* Unmap! */
		if (i == unmap_idx)
			continue;

		__arm_lpae_init_pte(blk_paddr, pte, lvl, &tablep[i]);
	}

	pte = arm_lpae_install_table(tablep, ptep, blk_pte, cfg);
	if (pte != blk_pte) {
		__arm_lpae_free_pages(tablep, tablesz, cfg);
		/*
		 * We may race against someone unmapping another part of this
		 * block, but anything else is invalid. We can't misinterpret
		 * a page entry here since we're never at the last level.
		 */
		if (iopte_type(pte, lvl - 1) != ARM_LPAE_PTE_TYPE_TABLE)
			return 0;

		tablep = iopte_deref(pte, data);
	} else if (unmap_idx >= 0) {
		iommu_hw_tlb_add_page(gather, iova, size);
		return size;
	}

	return __arm_lpae_unmap(gather, iova, size, lvl, tablep);
}

static size_t __arm_lpae_unmap(struct iommu_iotlb_gather *gather,
			       unsigned long iova, size_t size, int lvl,
			       arm_lpae_iopte *ptep)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	arm_lpae_iopte pte;

	/* Something went horribly wrong and we ran out of page table */
	if (lvl == ARM_LPAE_MAX_LEVELS)
		return 0;

	ptep += ARM_LPAE_LVL_IDX(iova, lvl, data);
	pte = READ_ONCE(*ptep);
	if (!pte)
		return 0;

	/* If the size matches this level, we're in the right place */
	if (size == ARM_LPAE_BLOCK_SIZE(lvl, data)) {
		__arm_lpae_set_pte(ptep, 0, &data->cfg);

		if (!iopte_leaf(pte, lvl, data->cfg.fmt)) {
			/* Also flush any partial walks */
			iommu_hw_tlb_flush_walk(iova, size,
						ARM_LPAE_GRANULE(data));
			ptep = iopte_deref(pte, data);
			__arm_lpae_free_pgtable(lvl + 1, ptep);
		} else {
			iommu_hw_tlb_add_page(gather, iova, size);
		}

		return size;
	} else if (iopte_leaf(pte, lvl, data->cfg.fmt)) {
		/*
		 * Insert a table at the next level to map the old region,
		 * minus the part we want to unmap
		 */
		return arm_lpae_split_blk_unmap(gather, iova, size,
						pte, lvl + 1, ptep);
	}

	/* Keep on walkin' */
	ptep = iopte_deref(pte, data);
	return __arm_lpae_unmap(gather, iova, size, lvl + 1, ptep);
}

size_t arm_lpae_unmap(unsigned long iova, size_t size,
		      struct iommu_iotlb_gather *gather)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	struct io_pgtable_cfg *cfg = &data->cfg;
	arm_lpae_iopte *ptep = data->pgd;
	long iaext = (int64_t)iova >> cfg->ias;

	if (!size || (size & cfg->pgsize_bitmap) != size)
		return 0;
	if (iaext)
		return 0;

	return __arm_lpae_unmap(gather, iova, size, data->start_level, ptep);
}

phys_addr_t arm_lpae_iova_to_phys(unsigned long iova)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	arm_lpae_iopte pte, *ptep = data->pgd;
	int lvl = data->start_level;

	do {
		/* Valid IOPTE pointer? */
		if (!ptep)
			return 0;

		/* Grab the IOPTE we're interested in */
		ptep += ARM_LPAE_LVL_IDX(iova, lvl, data);
		pte = READ_ONCE(*ptep);

		/* Valid entry? */
		if (!pte)
			return 0;

		/* Leaf entry? */
		if (iopte_leaf(pte, lvl, data->cfg.fmt))
			goto found_translation;

		/* Take it to the next level */
		ptep = iopte_deref(pte, data);
	} while (++lvl < ARM_LPAE_MAX_LEVELS);

	/* Ran out of page tables to walk */
	return 0;

found_translation:
	iova &= (ARM_LPAE_BLOCK_SIZE(lvl, data) - 1);
	return iopte_to_paddr(pte, data) | iova;
}

static void arm_lpae_restrict_pgsizes(struct io_pgtable_cfg *cfg)
{
	unsigned long granule, page_sizes;
	unsigned int max_addr_bits = 48;

	/* We need to restrict the supported page sizes to match the
	 * translation regime for a particular granule. Aim to match the
	 * CPU page size if possible, otherwise prefer smaller sizes.
	 * While we're at it, restrict the block sizes to match the
	 * chosen granule.
	 */
	if (cfg->pgsize_bitmap & PAGE_SIZE)
		granule = PAGE_SIZE;
	else if (cfg->pgsize_bitmap & ~PAGE_MASK)
		granule = 1UL << __fls64(cfg->pgsize_bitmap & ~PAGE_MASK);
	else if (cfg->pgsize_bitmap & PAGE_MASK)
		granule = 1UL << __ffs64(cfg->pgsize_bitmap & PAGE_MASK);
	else
		granule = 0;

	switch (granule) {
	case SZ_4K:
		page_sizes = (SZ_4K | SZ_2M | SZ_1G);
		break;
	case SZ_16K:
		page_sizes = (SZ_16K | SZ_32M);
		break;
	case SZ_64K:
		max_addr_bits = 52;
		page_sizes = (SZ_64K | SZ_512M);
		if (cfg->oas > 48)
			page_sizes |= 1ULL << 42; /* 4TB */
		break;
	default:
		page_sizes = 0;
	}

	cfg->pgsize_bitmap &= page_sizes;
	cfg->ias = min(cfg->ias, max_addr_bits);
	cfg->oas = min(cfg->oas, max_addr_bits);
}

static bool arm_lpae_alloc_pgtable(struct io_pgtable_cfg *cfg)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	int levels, va_bits, pg_shift;

	arm_lpae_restrict_pgsizes(cfg);

	if (!(cfg->pgsize_bitmap & (SZ_4K | SZ_16K | SZ_64K)))
		return false;
	if (cfg->ias > ARM_LPAE_MAX_ADDR_BITS)
		return false;
	if (cfg->oas > ARM_LPAE_MAX_ADDR_BITS)
		return false;
#if 0
	if (cfg->dma_pfn_offset) {
		con_err("iommu_armv8: Cannot accommodate DMA offset for IOMMU tables\n");
		return false;
	}
#endif

	pg_shift = __ffs64(cfg->pgsize_bitmap);
	data->bits_per_level = pg_shift - ilog2_const(sizeof(arm_lpae_iopte));

	va_bits = cfg->ias - pg_shift;
	levels = DIV_ROUND_UP(va_bits, data->bits_per_level);
	data->start_level = ARM_LPAE_MAX_LEVELS - levels;

	/* Calculate the actual size of our pgd (without concatenation) */
	data->pgd_bits = va_bits - (data->bits_per_level * (levels - 1));

	return true;
}

bool arm_64_lpae_alloc_pgtable_s1(struct io_pgtable_cfg *cfg)
{
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	uint64_t reg;
	struct arm_lpae_s1_tcr *tcr = &cfg->arm_lpae_s1_cfg.tcr;
	bool tg1;

	if (!arm_lpae_alloc_pgtable(cfg))
		return false;

	/* TCR */
	if (cfg->coherent_walk) {
		tcr->sh = ARM_LPAE_TCR_SH_IS;
		tcr->irgn = ARM_LPAE_TCR_RGN_WBWA;
		tcr->orgn = ARM_LPAE_TCR_RGN_WBWA;
	} else {
		tcr->sh = ARM_LPAE_TCR_SH_OS;
		tcr->irgn = ARM_LPAE_TCR_RGN_NC;
		tcr->orgn = ARM_LPAE_TCR_RGN_NC;
	}

	tg1 = 0;
	switch (ARM_LPAE_GRANULE(data)) {
	case SZ_4K:
		tcr->tg = tg1 ? ARM_LPAE_TCR_TG1_4K : ARM_LPAE_TCR_TG0_4K;
		break;
	case SZ_16K:
		tcr->tg = tg1 ? ARM_LPAE_TCR_TG1_16K : ARM_LPAE_TCR_TG0_16K;
		break;
	case SZ_64K:
		tcr->tg = tg1 ? ARM_LPAE_TCR_TG1_64K : ARM_LPAE_TCR_TG0_64K;
		break;
	}

	switch (cfg->oas) {
	case 32:
		tcr->ips = ARM_LPAE_TCR_PS_32_BIT;
		break;
	case 36:
		tcr->ips = ARM_LPAE_TCR_PS_36_BIT;
		break;
	case 40:
		tcr->ips = ARM_LPAE_TCR_PS_40_BIT;
		break;
	case 42:
		tcr->ips = ARM_LPAE_TCR_PS_42_BIT;
		break;
	case 44:
		tcr->ips = ARM_LPAE_TCR_PS_44_BIT;
		break;
	case 48:
		tcr->ips = ARM_LPAE_TCR_PS_48_BIT;
		break;
	case 52:
		tcr->ips = ARM_LPAE_TCR_PS_52_BIT;
		break;
	default:
		return false;
	}

	tcr->tsz = 64ULL - cfg->ias;

	/* MAIRs */
	reg = (ARM_LPAE_MAIR_ATTR_NC
	       << ARM_LPAE_MAIR_ATTR_SHIFT(ARM_LPAE_MAIR_ATTR_IDX_NC)) |
	      (ARM_LPAE_MAIR_ATTR_WBRWA
	       << ARM_LPAE_MAIR_ATTR_SHIFT(ARM_LPAE_MAIR_ATTR_IDX_CACHE)) |
	      (ARM_LPAE_MAIR_ATTR_DEVICE
	       << ARM_LPAE_MAIR_ATTR_SHIFT(ARM_LPAE_MAIR_ATTR_IDX_DEV)) |
	      (ARM_LPAE_MAIR_ATTR_INC_OWBRWA
	       << ARM_LPAE_MAIR_ATTR_SHIFT(ARM_LPAE_MAIR_ATTR_IDX_INC_OCACHE));

	cfg->arm_lpae_s1_cfg.mair = reg;

	/* Looking good; allocate a pgd */
	data->pgd = __arm_lpae_alloc_pages(ARM_LPAE_PGD_SIZE(data), cfg);
	if (!data->pgd)
		return false;

	/* Ensure the empty pgd is visible before any actual TTBR write */
	wmb();

	/* TTBR */
	cfg->arm_lpae_s1_cfg.ttbr = virt_to_phys(data->pgd);
	return true;
}

bool arm_64_lpae_alloc_pgtable_s2(struct io_pgtable_cfg *cfg)
{
	uint64_t sl;
	struct arm_lpae_io_pgtable *data = &arm_io_pgtables[iommu_dom];
	typeof(&cfg->arm_lpae_s2_cfg.vtcr) vtcr = &cfg->arm_lpae_s2_cfg.vtcr;

#if 0
	/* The NS quirk doesn't apply at stage 2 */
	if (cfg->quirks & ~(IO_PGTABLE_QUIRK_NON_STRICT))
		return NULL;
#endif

	/*
	 * Concatenate PGDs at level 1 if possible in order to reduce
	 * the depth of the stage-2 walk.
	 */
	if (data->start_level == 0) {
		unsigned long pgd_pages;

		pgd_pages = ARM_LPAE_PGD_SIZE(data) / sizeof(arm_lpae_iopte);
		if (pgd_pages <= ARM_LPAE_S2_MAX_CONCAT_PAGES) {
			data->pgd_bits += data->bits_per_level;
			data->start_level++;
		}
	}

	/* VTCR */
	if (cfg->coherent_walk) {
		vtcr->sh = ARM_LPAE_TCR_SH_IS;
		vtcr->irgn = ARM_LPAE_TCR_RGN_WBWA;
		vtcr->orgn = ARM_LPAE_TCR_RGN_WBWA;
	} else {
		vtcr->sh = ARM_LPAE_TCR_SH_OS;
		vtcr->irgn = ARM_LPAE_TCR_RGN_NC;
		vtcr->orgn = ARM_LPAE_TCR_RGN_NC;
	}

	sl = data->start_level;

	switch (ARM_LPAE_GRANULE(data)) {
	case SZ_4K:
		vtcr->tg = ARM_LPAE_TCR_TG0_4K;
		sl++; /* SL0 format is different for 4K granule size */
		break;
	case SZ_16K:
		vtcr->tg = ARM_LPAE_TCR_TG0_16K;
		break;
	case SZ_64K:
		vtcr->tg = ARM_LPAE_TCR_TG0_64K;
		break;
	}

	switch (cfg->oas) {
	case 32:
		vtcr->ps = ARM_LPAE_TCR_PS_32_BIT;
		break;
	case 36:
		vtcr->ps = ARM_LPAE_TCR_PS_36_BIT;
		break;
	case 40:
		vtcr->ps = ARM_LPAE_TCR_PS_40_BIT;
		break;
	case 42:
		vtcr->ps = ARM_LPAE_TCR_PS_42_BIT;
		break;
	case 44:
		vtcr->ps = ARM_LPAE_TCR_PS_44_BIT;
		break;
	case 48:
		vtcr->ps = ARM_LPAE_TCR_PS_48_BIT;
		break;
	case 52:
		vtcr->ps = ARM_LPAE_TCR_PS_52_BIT;
		break;
	default:
		goto out_free_data;
	}

	vtcr->tsz = 64ULL - cfg->ias;
	vtcr->sl = ~sl & ARM_LPAE_VTCR_SL0_MASK;

	/* Allocate pgd pages */
	data->pgd = __arm_lpae_alloc_pages(ARM_LPAE_PGD_SIZE(data), cfg);
	if (!data->pgd)
		goto out_free_data;

	/* Ensure the empty pgd is visible before any actual TTBR write */
	wmb();

	/* VTTBR */
	cfg->arm_lpae_s2_cfg.vttbr = virt_to_phys(data->pgd);
	return true;

out_free_data:
	return false;
}

bool arm_lpae_pgtable_alloc(struct io_pgtable_cfg *cfg)
{
	if (iommu_domain_ctrl.fmt == ARM_64_LPAE_S1)
		return arm_64_lpae_alloc_pgtable_s1(cfg);
	else if (iommu_domain_ctrl.fmt == ARM_64_LPAE_S2)
		return arm_64_lpae_alloc_pgtable_s2(cfg);
	return false;
}
