#include <target/mem.h>
#include <target/paging.h>
#include <target/arch.h>
#include <target/console.h>
#include <target/cmdline.h>
#include <target/init.h>

#ifdef CONFIG_MMU_IDMAP
pgd_t mmu_pg_dir[PTRS_PER_PGD] __page_aligned_bss;
#endif

#ifdef CONFIG_ARCH_HAS_MMU_CONT
#define NO_CONT_MAPPINGS	_BV(1)

#define CONT_PTES		(1 << CONT_PTE_SHIFT)
#define CONT_PTE_SIZE		(CONT_PTES * PAGE_SIZE)
#define CONT_PTE_MASK		(~(CONT_PTE_SIZE - 1))
#define CONT_PMDS		(1 << CONT_PMD_SHIFT)
#define CONT_PMD_SIZE		(CONT_PMDS * PMD_SIZE)
#define CONT_PMD_MASK		(~(CONT_PMD_SIZE - 1))

static inline pgprot_t pud_cont_prot(caddr_t addr, caddr_t next,
				     phys_addr_t phys, pgprot_t prot,
				     int flags)
{
	pgprot_t __prot = prot;
	if ((((addr | next | phys) & ~CONT_PMD_MASK) == 0) &&
	    (flags & NO_CONT_MAPPINGS) == 0) {
		con_dbg("PMD cont: addr=%016llx\n", addr);
		__prot = __pgprot(pgprot_val(prot) | PTE_CONT);
	}
	return __prot;
}

static inline pgprot_t pmd_cont_prot(caddr_t addr, caddr_t next,
				     phys_addr_t phys, pgprot_t prot,
				     int flags)
{
	pgprot_t __prot = prot;
	if ((((addr | next | phys) & ~CONT_PTE_MASK) == 0) &&
	    (flags & NO_CONT_MAPPINGS) == 0) {
		con_dbg("PTE cont: addr=%016llx\n", addr);
		__prot = __pgprot(pgprot_val(prot) | PTE_CONT);
	}
	return __prot;
}

static inline caddr_t pte_cont_addr_end(caddr_t addr, caddr_t end)
{
	caddr_t __boundary = ((addr) + CONT_PTE_SIZE) & CONT_PTE_MASK;
	return (__boundary - 1 < end - 1) ? __boundary : end;
}

static inline caddr_t pmd_cont_addr_end(caddr_t addr, caddr_t end)
{
	caddr_t __boundary = ((addr) + CONT_PMD_SIZE) & CONT_PMD_MASK;
	return (__boundary - 1 < end - 1) ? __boundary : end;
}
#else
#define NO_CONT_MAPPINGS				0
#define pud_cont_prot(addr, next, phys, prot, flags)	(prot)
#define pmd_cont_prot(addr, next, phys, prot, flags)	(prot)
#define pte_cont_addr_end(addr, end)	pte_addr_end(addr, end)
#define pmd_cont_addr_end(addr, end)	pmd_addr_end(addr, end)
#endif

#ifdef CONFIG_ARCH_HAS_MMU_HUGE
#define NO_BLOCK_MAPPINGS	_BV(0)

static inline bool pud_use_huge(caddr_t addr, caddr_t next, phys_addr_t phys)
{
	/* Only allows huge PUD for 4K granule */
	if (PAGE_SHIFT != 12)
		return false;
	if (((addr | next | phys) & ~PUD_MASK) != 0)
		return false;
	return true;
}

int pud_set_huge(pud_t *pudp, phys_addr_t phys, pgprot_t prot)
{
	pud_t new_pud = pfn_pud(phys_to_pfn(phys), mk_pud_sect_prot(prot));

	/* Only allow permission changes for now */
	if (!mmu_hw_pgattr_safe(READ_ONCE(pud_val(*pudp)),
				pud_val(new_pud)))
		return 0;

	BUG_ON(phys & ~PUD_MASK);
	con_dbg("PUD huge: %016llx=%016llx\n", pudp, new_pud);
	set_pud(pudp, new_pud);
	return 1;
}

static inline bool pud_huge_map(pud_t *pudp, caddr_t addr, caddr_t next,
				phys_addr_t phys, pgprot_t prot, int flags)
{
	pud_t old_pud = READ_ONCE(*pudp);

	if ((flags & NO_BLOCK_MAPPINGS) != 0)
		return false;
	if (!pud_use_huge(addr, next, phys))
		return false;

	pud_set_huge(pudp, phys, prot);
	/* After the PUD entry has been populated once, we
	 * only allow updates to the permission attributes.
	 */
	if (!mmu_hw_pgattr_safe(pud_val(old_pud),
				READ_ONCE(pud_val(*pudp))))
		BUG();
	return true;
}

static inline bool pmd_use_huge(caddr_t addr, caddr_t next,
				phys_addr_t phys)
{
	if (((addr | next | phys) & ~SECTION_MASK) != 0)
		return false;
	return true;
}

int pmd_set_huge(pmd_t *pmdp, phys_addr_t phys, pgprot_t prot)
{
	pmd_t new_pmd = pfn_pmd(phys_to_pfn(phys), mk_pmd_sect_prot(prot));

	/* Only allow permission changes for now */
	if (!mmu_hw_pgattr_safe(READ_ONCE(pmd_val(*pmdp)),
				pmd_val(new_pmd)))
		return 0;

	BUG_ON(phys & ~PMD_MASK);
	con_dbg("PMD huge: %016llx=%016llx\n", pmdp, new_pmd);
	set_pmd(pmdp, new_pmd);
	return 1;
}

static inline bool pmd_huge_map(pmd_t *pmdp, caddr_t addr, caddr_t next,
				phys_addr_t phys, pgprot_t prot, int flags)
{
	pmd_t old_pmd = READ_ONCE(*pmdp);

	if ((flags & NO_BLOCK_MAPPINGS) != 0)
		return false;
	if (!pmd_use_huge(addr, next, phys))
		return false;

	pmd_set_huge(pmdp, phys, prot);
	/* After the PMD entry has been populated once, we only allow
	 * updates to the permission attributes.
	 */
	if (!mmu_hw_pgattr_safe(pmd_val(old_pmd),
				READ_ONCE(pmd_val(*pmdp))))
		BUG();
	return true;
}
#else
#define NO_BLOCK_MAPPINGS					0
#define pud_huge_map(pmdp, addr, next, phys, prot, flags)	false
#define pmd_huge_map(pmdp, addr, next, phys, prot, flags)	false
#define pud_sect(pud)						false
#define pmd_sect(pmd)						false
#endif

#ifdef CONFIG_ARCH_HAS_MMU_ORDER
__init static pte_t *get_pte_virt(phys_addr_t pa)
{
	pte_clear_fixmap();
	return pte_set_fixmap(pa);
}

__init static pte_t *get_pte_virt_offset(pmd_t *pmdp, phys_addr_t pa)
{
	pte_clear_fixmap();
	return pte_set_fixmap_offset(pmdp, pa);
}

__init static pmd_t *get_pmd_virt(phys_addr_t pa)
{
	pmd_clear_fixmap();
	return pmd_set_fixmap(pa);
}

__init static pmd_t *get_pmd_virt_offset(pud_t *pudp, phys_addr_t pa)
{
	pmd_clear_fixmap();
	return pmd_set_fixmap_offset(pudp, pa);
}

__init static pud_t *get_pud_virt(phys_addr_t pa)
{
	pud_clear_fixmap();
	return pud_set_fixmap(pa);
}

__init static pud_t *get_pud_virt_offset(pgd_t *pgdp, phys_addr_t pa)
{
	pud_clear_fixmap();
	return pud_set_fixmap_offset(pgdp, pa);
}

__init static pgd_t *get_pgd_virt(phys_addr_t pa)
{
	pgd_clear_fixmap();
	return pgd_set_fixmap(pa);
}

#define put_pte_virt()			do { } while (0)
#define put_pmd_virt()			do { } while (0)
#define put_pud_virt()			do { } while (0)
#define put_pgd_virt()			do { } while (0)
#else
#define get_pte_virt(pa)		pte_set_fixmap(pa)
#define get_pmd_virt(pa)		pmd_set_fixmap(pa)
#define get_pud_virt(pa)		pud_set_fixmap(pa)
#define get_pgd_virt(pa)		pgd_set_fixmap(pa)
#define get_pte_virt_offset(pmdp, pa)	pte_set_fixmap_offset(pmdp, pa)
#define get_pmd_virt_offset(pudp, pa)	pmd_set_fixmap_offset(pudp, pa)
#define get_pud_virt_offset(pgdp, pa)	pud_set_fixmap_offset(pgdp, pa)
#define put_pte_virt()			pte_clear_fixmap()
#define put_pmd_virt()			pmd_clear_fixmap()
#define put_pud_virt()			pud_clear_fixmap()
#define put_pgd_virt()			pgd_clear_fixmap()
#endif

static phys_addr_t early_pgtable_alloc(void)
{
	phys_addr_t phys;
	void *ptr;

	phys = mem_alloc(PAGE_SIZE, PAGE_SIZE);
	if (!phys) {
		con_dbg("Failed to allocate page table page\n");
		BUG();
	}

	/* The FIX_{PGD,PUD,PMD} slots may be in active use, but the
	 * FIX_PTE slot will be free, so we can (ab)use the FIX_PTE slot to
	 * initialise any level of table.
	 */
	ptr = get_pte_virt(phys);
	mmu_dbg_tbl("ALLOC: P=%016llx, V=%016llx\n", phys, ptr);
#ifndef CONFIG_GEM5_SKIP_SET_PGT
	memory_set((caddr_t)ptr, 0, PAGE_SIZE);
#endif

	/* Implicit barriers also ensure the zeroed page is visible to the
	 * page table walker.
	 */
	put_pte_virt();
	return phys;
}

#if 0
static void early_pgtable_free(phys_addr_t phys)
{
	mmu_dbg_tbl("FREE: %016llx\n", phys);
	mem_free(phys, PAGE_SIZE);
}
#endif

static void init_pte(pmd_t *pmdp, caddr_t addr, caddr_t end,
		     phys_addr_t phys, pgprot_t prot)
{
	pte_t *ptep;

	ptep = get_pte_virt_offset(pmdp, addr);
	do {
		__unused pte_t old_pte = READ_ONCE(*ptep);
		pteval_t new_pte;

		new_pte = pfn_pte(phys_to_pfn(phys), prot);
		con_dbg("PTE: %016llx=%016llx, addr=%016llx\n",
			ptep, new_pte, addr);
		set_pte(ptep, new_pte);
		/* After the PTE entry has been populated once, we
		 * only allow updates to the permission attributes.
		 */
		if (!mmu_hw_pgattr_safe(pte_val(old_pte),
					READ_ONCE(pte_val(*ptep))))
			BUG();
		phys += PAGE_SIZE;
	} while (ptep++, addr += PAGE_SIZE, addr != end);
	put_pte_virt();
}

static void alloc_init_cont_pte(pmd_t *pmdp, caddr_t addr,
				caddr_t end, phys_addr_t phys,
				pgprot_t prot,
				phys_addr_t (*pgtable_alloc)(void),
				int flags)
{
	caddr_t next;
	pmd_t pmd = READ_ONCE(*pmdp);

	BUG_ON(pmd_sect(pmd));
	if (pmd_none(pmd)) {
		phys_addr_t pte_phys;
		BUG_ON(!pgtable_alloc);
		pte_phys = pgtable_alloc();
		__pmd_populate(pmdp, pte_phys, PMD_TYPE_TABLE);
		pmd = READ_ONCE(*pmdp);
	}
	BUG_ON(pmd_bad(pmd));

	do {
		pgprot_t __prot;
		next = pte_cont_addr_end(addr, end);

		/* Use a contiguous mapping if the range is suitably aligned */
		__prot = pmd_cont_prot(addr, next, phys, prot, flags);
		init_pte(pmdp, addr, next, phys, __prot);
		phys += next - addr;
	} while (addr = next, addr != end);
}

static void init_pmd(pud_t *pudp, caddr_t addr, caddr_t end,
		     phys_addr_t phys, pgprot_t prot,
		     phys_addr_t (*pgtable_alloc)(void), int flags)
{
	caddr_t next;
	pmd_t *pmdp;

	pmdp = get_pmd_virt_offset(pudp, addr);
	do {
		next = pmd_addr_end(addr, end);
		con_dbg("PMD: %016llx=%016llx, addr=%016llx\n",
			pmdp, *pmdp, addr);

		/* try section mapping first */
		if (!pmd_huge_map(pmdp, addr, next, phys, prot, flags)) {
			pmd_t old_pmd = READ_ONCE(*pmdp);

			alloc_init_cont_pte(pmdp, addr, next, phys, prot,
					    pgtable_alloc, flags);
			BUG_ON(pmd_val(old_pmd) != 0 &&
			       pmd_val(old_pmd) != READ_ONCE(pmd_val(*pmdp)));
		}
		phys += next - addr;
	} while (pmdp++, addr = next, addr != end);
	put_pmd_virt();
}

static void alloc_init_cont_pmd(pud_t *pudp, caddr_t addr,
				caddr_t end, phys_addr_t phys,
				pgprot_t prot,
				phys_addr_t (*pgtable_alloc)(void),
				int flags)
{
	caddr_t next;
	pud_t pud = READ_ONCE(*pudp);

	/* Check for initial section mappings in the pgd/pud. */
	BUG_ON(pud_sect(pud));
	if (pud_none(pud)) {
		phys_addr_t pmd_phys;
		BUG_ON(!pgtable_alloc);
		pmd_phys = pgtable_alloc();
		__pud_populate(pudp, pmd_phys, PUD_TYPE_TABLE);
		pud = READ_ONCE(*pudp);
	}
	BUG_ON(pud_bad(pud));

	do {
		pgprot_t __prot;
		next = pmd_cont_addr_end(addr, end);

		/* Use a contiguous mapping if the range is suitably aligned */
		__prot = pud_cont_prot(addr, next, phys, prot, flags);
		init_pmd(pudp, addr, next, phys, __prot, pgtable_alloc,
			 flags);
		phys += next - addr;
	} while (addr = next, addr != end);
}

static void alloc_init_pud(pgd_t *pgdp, caddr_t addr, caddr_t end,
			   phys_addr_t phys, pgprot_t prot,
			   phys_addr_t (*pgtable_alloc)(void),
			   int flags)
{
	caddr_t next;
	pud_t *pudp;
	__unused pgd_t pgd = READ_ONCE(*pgdp);

	if (pgd_none(pgd)) {
		__unused phys_addr_t pud_phys;
		BUG_ON(!pgtable_alloc);
		pud_phys = pgtable_alloc();
		__pgd_populate(pgdp, pud_phys, PUD_TYPE_TABLE);
		pgd = READ_ONCE(*pgdp);
	}
	BUG_ON(pgd_bad(pgd));

	pudp = get_pud_virt_offset(pgdp, addr);
	do {
		next = pud_addr_end(addr, end);
		con_dbg("PUD: %016llx=%016llx, addr=%016llx\n",
			pudp, *pudp, addr);

		if (!pud_huge_map(pudp, addr, next, phys, prot, flags)) {
			pud_t old_pud = READ_ONCE(*pudp);

			alloc_init_cont_pmd(pudp, addr, next, phys, prot,
					    pgtable_alloc, flags);
			BUG_ON(pud_val(old_pud) != 0 &&
			       pud_val(old_pud) != READ_ONCE(pud_val(*pudp)));
		}
		phys += next - addr;
	} while (pudp++, addr = next, addr != end);
	put_pud_virt();
}

static void __create_pgd_mapping(pgd_t *pgdir, phys_addr_t phys,
				 caddr_t virt, phys_addr_t size,
				 pgprot_t prot,
				 phys_addr_t (*pgtable_alloc)(void),
				 int flags)
{
	caddr_t addr, length, end, next;
	pgd_t *pgdp = pgd_offset(pgdir, virt);

	con_dbg("LOWMAP: %016llx -> %016llx: %016llx\n", phys, virt, size);

	/* If the virtual and physical address don't have the same offset
	 * within a page, we cannot map the region as the caller expects.
	 */
	BUG_ON((phys ^ virt) & ~PAGE_MASK);

	phys &= PAGE_MASK;
	addr = virt & PAGE_MASK;
	length = PAGE_ALIGN(size + (virt & ~PAGE_MASK));
	end = addr + length;
	do {
		con_dbg("PGD: %016llx=%016llx, addr=%016llx\n",
			pgdp, *pgdp, addr);
		next = pgd_addr_end(addr, end);
		alloc_init_pud(pgdp, addr, next, phys, prot, pgtable_alloc,
			       flags);
		phys += next - addr;
	} while (pgdp++, addr = next, addr != end);
}

void create_pgd_mapping(pgd_t *pgdir, phys_addr_t phys,
			caddr_t virt, phys_addr_t size,
			pgprot_t prot, bool page_mapping_only)
{
	int flags;

	if (page_mapping_only)
		flags = NO_BLOCK_MAPPINGS | NO_CONT_MAPPINGS;
	else
		flags = 0;
	__create_pgd_mapping(pgdir, phys, virt, size, prot,
			     early_pgtable_alloc, flags);
}

static void map_kernel_segment(pgd_t *pgdp, void *va_start, void *va_end,
			       pgprot_t prot, int flags)
{
	phys_addr_t pa_start = __pa(va_start);
	phys_addr_t size = va_end - va_start;

	BUG_ON(!PAGE_ALIGNED(pa_start));
	BUG_ON(!PAGE_ALIGNED(size));

	__create_pgd_mapping(pgdp, pa_start, (caddr_t)va_start, size, prot,
			     early_pgtable_alloc, flags);
}

#ifdef CONFIG_MMU_MAP_MEM
#ifdef CONFIG_GEM5
phys_addr_t *pages_list = NULL;
#ifdef CONFIG_GEM5_STATIC_PAGES
extern phys_addr_t *simpoint_pages_dump(void);
#else
extern phys_addr_t *simpoint_pages_alloc(void);
#endif
extern void simpoint_pages_map(pgd_t *pgdp, phys_addr_t *pages_list);
extern void simpoint_mem_restore(void);

static void map_mem(pgd_t *pgdp)
{
#ifdef CONFIG_GEM5_STATIC_PAGES
	con_printf("Simpoint: Start simpoint_pages_dump\n");
	pages_list = simpoint_pages_dump();
#else
	con_printf("Simpoint: Start simpoint_pages_alloc\n");
	pages_list = simpoint_pages_alloc();
#endif
	con_printf("Simpoint: Start simpoint_pages_map\n");
	simpoint_pages_map(pgdp, pages_list);
#if defined(CONFIG_GEM5_NOT_RESTORE_MEM) || !defined(CONFIG_GEM5_STATIC_PAGES)
	con_printf("Simpoint: Start simpoint_mem_restore\n");
	simpoint_mem_restore();
#endif
}
#else /* CONFIG_GEM5 */
static void __map_mem_region(pgd_t *pgdp, phys_addr_t start, phys_addr_t end,
			     pgprot_t prot, int flags)
{
	phys_addr_t kernel_start = __pa(SDFIRM_START);
	phys_addr_t kernel_end = __pa(SDFIRM_END);

	/* The kernel itself is mapped at page granularity. Map all other
	 * memory, making sure we don't overwrite the existing kernel mappings.
	 */

	/* No overlap with the kernel. */
	if (end < kernel_start || start >= kernel_end) {
		__create_pgd_mapping(pgdp, start, phys_to_virt(start),
				     end - start, prot,
				     early_pgtable_alloc, flags);
		return;
	}

	/* This block overlaps the kernel mapping. Map the portion(s) which
	 * don't overlap.
	 */
	if (start < kernel_start) {
		__create_pgd_mapping(pgdp, start,
				     phys_to_virt(start),
				     kernel_start - start, prot,
				     early_pgtable_alloc, flags);
	}
	if (kernel_end < end) {
		__create_pgd_mapping(pgdp, kernel_end,
				     phys_to_virt(kernel_end),
				     end - kernel_end, prot,
				     early_pgtable_alloc, flags);
	}
}

static bool __map_mem(struct mem_region *rgn, void *data)
{
	pgd_t *pgdp = (pgd_t *)data;
	phys_addr_t start = rgn->base;
	phys_addr_t end = start + rgn->size;
	int flags = 0;

	if (start >= end)
		return false;
#if 0
	if (mem_is_nomap(rgn))
		return true;
#endif
	__map_mem_region(pgdp, start, end, PAGE_KERNEL, flags);
	return true;
}

static void map_mem(pgd_t *pgdp)
{
	mem_walk_memory(__map_mem, pgdp);
}
#endif /* CONFIG_GEM5 */
#else /* CONFIG_MMU_MAP_MEM */
#define map_mem(pgdp)		do { } while (0)
#endif /* CONFIG_MMU_MAP_MEM */

static void map_kernel(pgd_t *pgdp)
{
	/* External debuggers may need to write directly to the text
	 * mapping to install SW breakpoints. Allow this (only) when
	 * explicitly requested with rodata=off.
	 */
#ifdef CONFIG_MMU_TEXT_MODIFY
#define PAGE_TEXT_PROT		PAGE_KERNEL_EXEC
#else
#define PAGE_TEXT_PROT		PAGE_KERNEL_ROX
#endif

	/* Only rodata will be remapped with different permissions later on,
	 * all other segments are allowed to use contiguous mappings.
	 */
	map_kernel_segment(pgdp, __stext, __etext, PAGE_TEXT_PROT, 0);
	map_kernel_segment(pgdp, __srodata, __erodata,
			   PAGE_KERNEL_RO, NO_CONT_MAPPINGS);
	map_kernel_segment(pgdp, _sdata, _edata, PAGE_KERNEL, 0);
	/* Map stacks */
	map_kernel_segment(pgdp, (void *)PERCPU_STACKS_START,
			   (void *)PERCPU_STACKS_END, PAGE_KERNEL, 0);

	/* The fixmap falls in a separate pgd to the kernel, and doesn't live
	 * in the carveout for the mmu_pg_dir. We can simply re-use the
	 * existing dir for the fixmap.
	 */
	set_pgd(pgd_offset(pgdp, FIXADDR_START),
		READ_ONCE(*pgd_offset(mmu_id_map, FIXADDR_START)));
#ifndef CONFIG_MMU_IDMAP
	/* Expand BPGT to 4K page tables */
	set_pgd(pgd_offset(mmu_id_map, (caddr_t)__stext),
		READ_ONCE(*pgd_offset(pgdp, (caddr_t)__stext)));
	set_pgd(pgd_offset(mmu_id_map, (caddr_t)__srodata),
		READ_ONCE(*pgd_offset(pgdp, (caddr_t)__srodata)));
	set_pgd(pgd_offset(mmu_id_map, (caddr_t)_sdata),
		READ_ONCE(*pgd_offset(pgdp, (caddr_t)_sdata)));
	set_pgd(pgd_offset(mmu_id_map, (caddr_t)PERCPU_STACKS_START),
		READ_ONCE(*pgd_offset(pgdp,
					  (caddr_t)PERCPU_STACKS_START)));
#endif
}

static pte_t bm_pte[PTRS_PER_PTE] __page_aligned_bss __unused;
#if PGTABLE_LEVELS > 2
static pmd_t bm_pmd[PTRS_PER_PMD] __page_aligned_bss __unused;
#endif
#if PGTABLE_LEVELS > 3
static pud_t bm_pud[PTRS_PER_PUD] __page_aligned_bss __unused;
#endif

static inline pud_t *fixmap_pud(caddr_t addr)
{
	pgd_t *pgdp = pgd_offset(mmu_id_map, addr);
	__unused pgd_t pgd = READ_ONCE(*pgdp);

	if (pgd_none(pgd) || pgd_bad(pgd))
		BUG();
	return pud_offset(pgdp, addr);
}

static inline pmd_t *fixmap_pmd(caddr_t addr)
{
	pud_t *pudp = fixmap_pud(addr);
	__unused pud_t pud = READ_ONCE(*pudp);

	if (pud_none(pud) || pud_bad(pud))
		BUG();
	return pmd_offset(pudp, addr);
}

static inline pte_t *fixmap_pte(caddr_t addr)
{
#if 1
	return &bm_pte[pte_index(addr)];
#else
	pmd_t *pmdp = fixmap_pmd(addr);
	__unused pmd_t pmd = READ_ONCE(*pmdp);

	if (pmd_none(pmd) || pmd_bad(pmd))
		BUG();
	return pte_offset(pmdp, addr);
#endif
}

/* Unusually, this is also called in IRQ context (ghes_iounmap_irq) so if we
 * ever need to use IPIs for TLB broadcasting, then we're in trouble here.
 */
void __set_fixmap(fixmap_t idx, phys_addr_t phys, pgprot_t flags)
{
	caddr_t addr = fix_to_virt(idx);
	pte_t *ptep;

	BUG_ON(idx <= FIX_HOLE || idx >= FIX_BTMAP_BEGIN);

	ptep = fixmap_pte(addr);

	if (pgprot_val(flags)) {
		set_pte(ptep, pfn_pte(phys >> PAGE_SHIFT, flags));
	} else {
		pte_clear(addr, ptep);
		flush_tlb_range_kern(addr, addr+PAGE_SIZE);
	}
}

void early_fixmap_init(void)
{
	pgd_t *pgd;
	pud_t *pud;
	pmd_t *pmd;
	caddr_t addr;

	con_dbg("FIXMAP(t): %016llx - %016llx\n",
		__FIXADDR_START, FIXADDR_END);
	con_dbg("FIXMAP(p): %016llx - %016llx\n",
		FIXADDR_START, FIXADDR_END);
	mmu_dbg_tbl("IDMAP: %016llx, PGDIR: %016llx\n",
		    mmu_id_map, mmu_pg_dir);
	addr = FIXADDR_START;
	pgd = pgd_offset(mmu_id_map, addr);
	pgd_populate(pgd, bm_pud);
	pud = pud_offset(pgd, addr);
	pud_populate(pud, bm_pmd);
	pmd = pmd_offset(pud, addr);
	pmd_populate(pmd, bm_pte);

	/* The boot-ioremap range spans multiple pmds, for which
	 * we are not preparted:
	 */
	BUG_ON((fix_to_virt(FIX_BTMAP_BEGIN) >> PMD_SHIFT) !=
	       (fix_to_virt(FIX_BTMAP_END) >> PMD_SHIFT));

	if ((pmd != fixmap_pmd(fix_to_virt(FIX_BTMAP_BEGIN))) ||
	    (pmd != fixmap_pmd(fix_to_virt(FIX_BTMAP_END)))) {
		con_dbg("pmd %p != %p, %p\n",
		       pmd, fixmap_pmd(fix_to_virt(FIX_BTMAP_BEGIN)),
		       fixmap_pmd(fix_to_virt(FIX_BTMAP_END)));
		con_dbg("fix_to_virt(FIX_BTMAP_BEGIN): %016llx\n",
		       fix_to_virt(FIX_BTMAP_BEGIN));
		con_dbg("fix_to_virt(FIX_BTMAP_END):   %016llx\n",
		       fix_to_virt(FIX_BTMAP_END));

		con_dbg("FIX_BTMAP_END:       %d\n", FIX_BTMAP_END);
		con_dbg("FIX_BTMAP_BEGIN:     %d\n", FIX_BTMAP_BEGIN);
		BUG();
	}
}

void paging_init(void)
{
	pgd_t *pgdp;

	printf("==============================================================\n");
	printf("BPGT_PGTABLE_LEVELS=%d\n", BPGT_PGTABLE_LEVELS);
	printf("PGTABLE_LEVELS=%d\n", PGTABLE_LEVELS);
	/* expand boot kernel mappings */
	pgdp = get_pgd_virt(__pa(mmu_pg_dir));
	map_kernel(pgdp);
	put_pgd_virt();

	/* map memory */
	pgdp = get_pgd_virt(__pa(mmu_pg_dir));
	map_mem(pgdp);
	put_pgd_virt();

	/* switch to mmu_pg_dir */
#ifdef CONFIG_MMU_IDMAP
	mmu_hw_ctrl_init();
#endif
}
