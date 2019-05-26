#include <target/paging.h>

static void alloc_init_pte(pmd_t *pmd, caddr_t addr,
			   caddr_t end, caddr_t pfn,
			   pgprot_t prot)
{
	pte_t *pte;

	if (pmd_none(*pmd)) {
		pte = (pte_t *)early_page_alloc_zeroed();
		__pmd_populate(pmd, __pa(pte), PMD_TYPE_TABLE);
	}
	BUG_ON(pmd_bad(*pmd));

	pte = pte_offset_kernel(pmd, addr);
	do {
		set_pte(pte, pfn_pte(pfn, prot));
		pfn++;
	} while (pte++, addr += PAGE_SIZE, addr != end);
}

static void alloc_init_pmd(pud_t *pud, caddr_t addr,
			   caddr_t end, phys_addr_t phys,
			   int map_io)
{
	pmd_t *pmd;
	caddr_t next;
	pmdval_t prot_sect;
	pgprot_t prot_pte;

	if (map_io) {
		prot_sect = PROT_SECT_DEVICE_nGnRE;
		prot_pte = __pgprot(PROT_DEVICE_nGnRE);
	} else {
		prot_sect = PROT_SECT_NORMAL_EXEC;
		prot_pte = PAGE_KERNEL_EXEC;
	}

	/*
	 * Check for initial section mappings in the pgd/pud and remove them.
	 */
	if (pud_none(*pud) || pud_bad(*pud)) {
		pmd = (pmd_t *)early_page_alloc_zeroed();
		pud_populate(pud, pmd);
	}

	pmd = pmd_offset(pud, addr);
	do {
		next = pmd_addr_end(addr, end);
		/* try section mapping first */
		if (((addr | next | phys) & ~SECTION_MASK) == 0) {
			pmd_t old_pmd =*pmd;
			set_pmd(pmd, __pmd(phys | prot_sect));
			/*
			 * Check for previous table entries created during
			 * boot (__create_page_tables) and flush them.
			 */
			if (!pmd_none(old_pmd))
				flush_tlb_all();
		} else {
			alloc_init_pte(pmd, addr, next, phys_to_pfn(phys),
				       prot_pte);
		}
		phys += next - addr;
	} while (pmd++, addr = next, addr != end);
}

static void alloc_init_pud(pgd_t *pgd, caddr_t addr,
			   caddr_t end, phys_addr_t phys,
			   int map_io)
{
	pud_t *pud;
	caddr_t next;

	if (pgd_none(*pgd)) {
		pud = (pud_t *)early_page_alloc_zeroed();
		pgd_populate(&init_mm, pgd, pud);
	}
	BUG_ON(pgd_bad(*pgd));

	pud = pud_offset(pgd, addr);
	do {
		next = pud_addr_end(addr, end);

		/*
		 * For 4K granule only, attempt to put down a 1GB block
		 */
		if (!map_io && (PAGE_SHIFT == 12) &&
		    ((addr | next | phys) & ~PUD_MASK) == 0) {
			pud_t old_pud = *pud;
			set_pud(pud, __pud(phys | PROT_SECT_NORMAL_EXEC));

			/*
			 * If we have an old value for a pud, it will
			 * be pointing to a pmd table that we no longer
			 * need (from swapper_pg_dir).
			 *
			 * Look up the old pmd table and free it.
			 */
			if (!pud_none(old_pud)) {
				phys_addr_t table = __pa(pmd_offset(&old_pud, 0));
				early_page_free(table);
				flush_tlb_all();
			}
		} else {
			alloc_init_pmd(pud, addr, next, phys, map_io);
		}
		phys += next - addr;
	} while (pud++, addr = next, addr != end);
}

/*
 * Create the page directory entries and any necessary page tables for the
 * mapping specified by 'md'.
 */
static void __create_mapping(pgd_t *pgd, phys_addr_t phys,
			     caddr_t virt, phys_addr_t size,
			     int map_io)
{
	caddr_t addr, length, end, next;

	addr = virt & PAGE_MASK;
	length = PAGE_ALIGN(size + (virt & ~PAGE_MASK));

	end = addr + length;
	do {
		next = pgd_addr_end(addr, end);
		alloc_init_pud(pgd, addr, next, phys, map_io);
		phys += next - addr;
	} while (pgd++, addr = next, addr != end);
}

void mmu_hw_create_mapping(phys_addr_t phys, caddr_t virt,
			   phys_addr_t size)
{
#if 0
	if (virt < VMALLOC_START) {
		printf("BUG: not creating mapping for %pa at 0x%016lx - outside kernel range\n",
			&phys, virt);
		return;
	}
#endif
	__create_mapping(pgd_offset_k(virt & PAGE_MASK), phys, virt, size, 0);
}

#if 0
static void __init map_mem(void)
{
	struct memblock_region *reg;
	phys_addr_t limit;

	/*
	 * Temporarily limit the memblock range. We need to do this as
	 * create_mapping requires puds, pmds and ptes to be allocated from
	 * memory addressable from the initial direct kernel mapping.
	 *
	 * The initial direct kernel mapping, located at swapper_pg_dir, gives
	 * us PUD_SIZE (4K pages) or PMD_SIZE (64K pages) memory starting from
	 * PHYS_OFFSET (which must be aligned to 2MB as per
	 * Documentation/arm64/booting.txt).
	 */
	if (IS_ENABLED(CONFIG_ARM64_64K_PAGES))
		limit = PHYS_OFFSET + PMD_SIZE;
	else
		limit = PHYS_OFFSET + PUD_SIZE;
	memblock_set_current_limit(limit);

	/* map all the memory banks */
	for_each_memblock(memory, reg) {
		phys_addr_t start = reg->base;
		phys_addr_t end = start + reg->size;

		if (start >= end)
			break;

#ifndef CONFIG_ARM64_64K_PAGES
		/*
		 * For the first memory bank align the start address and
		 * current memblock limit to prevent create_mapping() from
		 * allocating pte page tables from unmapped memory.
		 * When 64K pages are enabled, the pte page table for the
		 * first PGDIR_SIZE is already present in swapper_pg_dir.
		 */
		if (start < limit)
			start = ALIGN(start, PMD_SIZE);
		if (end < limit) {
			limit = end & PMD_MASK;
			memblock_set_current_limit(limit);
		}
#endif

		create_mapping(start, __phys_to_virt(start), end - start);
	}

	/* Limit no longer required. */
	memblock_set_current_limit(MEMBLOCK_ALLOC_ANYWHERE);
}
#endif
