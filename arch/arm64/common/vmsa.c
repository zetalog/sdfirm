#include <target/paging.h>

#if 0
static void alloc_init_pte(pmd_t *pmd, caddr_t addr,
			   caddr_t end, caddr_t pfn,
			   pgprot_t prot)
{
	pte_t *pte;

	if (pmd_none(*pmd)) {
		pte = (pte_t *)page_alloc_zeroed();
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
		pmd = (pmd_t *)page_alloc_zeroed();
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
		pud = (pud_t *)page_alloc_zeroed();
		pgd_populate(pgd, pud);
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
				page_free(table);
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
	__create_mapping(pgd_offset(virt & PAGE_MASK), phys, virt, size, 0);
}

#if 0
void __init mmap_init(void)
{
	struct memory_region *reg;

	/* map all the memory regions */
	for_each_memblock(memory, reg) {
		phys_addr_t start = reg->base;
		phys_addr_t end = start + reg->size;

		if (start >= end)
			break;
		create_mapping(start, __phys_to_virt(start), end - start);
	}
}
#endif
#endif
