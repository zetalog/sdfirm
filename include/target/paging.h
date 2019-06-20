#ifndef __PAGING_H_INCLUDE__
#define __PAGING_H_INCLUDE__

#include <target/generic.h>
#include <target/page.h>
#include <target/tlb.h>

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)	ALIGN(addr, PAGE_SIZE)

#ifdef CONFIG_ARCH_HAS_MMU
#define virt_to_phys(x)		(((phys_addr_t)(x) - PAGE_OFFSET + PHYS_OFFSET))
#define phys_to_virt(x)		((caddr_t)((x) - PHYS_OFFSET + PAGE_OFFSET))
#define __pa(x)			virt_to_phys((caddr_t)(x))
#define __va(x)			phys_to_virt((phys_addr_t)(x))

/* Convert a physical address to a Page Frame Number and back */
#define phys_to_pfn(paddr)	((pfn_t)((paddr) >> PAGE_SHIFT))
#define pfn_to_phys(pfn)	((phys_addr_t)(pfn) << PAGE_SHIFT)

/* Convert a page to/from a physical address */
#define page_to_phys(page)	pfn_to_phys(page_to_pfn(page))
#define phys_to_page(phys)	pfn_to_page(phys_to_pfn(phys))

/* PFNs are used to describe any physical page; this means
 * PFN 0 == physical address 0.
 *
 * This is the PFN of the first RAM page in the kernel
 * direct-mapped view.  We assume this is the first page
 * of RAM in the mem_map as well.
 */
#define PHYS_PFN_OFFSET		(PHYS_OFFSET >> PAGE_SHIFT)
#define ARCH_PFN_OFFSET		((caddr_t)PHYS_PFN_OFFSET)

#define pfn_to_kaddr(pfn)	__va((pfn) << PAGE_SHIFT)
#define virt_to_pfn(x)		phys_to_pfn(virt_to_phys(x))

/* virt_to_page(k)	convert a _valid_ virtual address to struct page *
 * virt_addr_valid(k)	indicates whether a virtual address is valid
 */
#define virt_to_page(kaddr)	pfn_to_page(__pa(kaddr) >> PAGE_SHIFT)
#define	virt_addr_valid(kaddr)	pfn_valid(__pa(kaddr) >> PAGE_SHIFT)

#if PAGE_PTR_BITS == 3
#define PTR_VAL_ONE		_AC(1, ULL)
#define PTR_VAL_ZERO		_AC(0, ULL)
#elif PAGE_PTR_BITS == 2
#define PTR_VAL_ONE		_AC(1, UL)
#define PTR_VAL_ZERO		_AC(0, UL)
#endif

#ifndef __ASSEMBLY__
typedef struct page *pgtable_t;

typedef pteval_t pte_t;
#define pte_val(x)	(x)
#define __pte(x)	(x)
#if PGTABLE_LEVELS > 2
typedef pmdval_t pmd_t;
#define pmd_val(x)	(x)
#define __pmd(x)	(x)
#endif
#if PGTABLE_LEVELS > 3
typedef pudval_t pud_t;
#define pud_val(x)	(x)
#define __pud(x)	(x)
#endif
typedef pgdval_t pgd_t;
#define pgd_val(x)	(x)
#define __pgd(x)	(x)
typedef pteval_t pgprot_t;
#define pgprot_val(x)	(x)
#define __pgprot(x)	(x)
#endif /* !__ASSEMBLY__ */

#define PTRS_PER_PTE		PAGE_MAX_TABLE_ENTRIES

#define pte_pfn(pte)		((pte_val(pte) & PHYS_MASK) >> PAGE_SHIFT)
#define pfn_pte(pfn, prot)	__pte(((phys_addr_t)(pfn) << PAGE_SHIFT) | pgprot_val(prot))

#define pte_none(pte)		(!pte_val(pte))
#define pte_clear(addr, ptep)	set_pte(ptep, __pte(0))
#define pte_page(pte)		pfn_to_page(pte_pfn(pte))
#define pte_alloc_one_kernel(addr)	((pte_t *)page_alloc_zeroed())
#define pte_free_kernel(pte)		page_free((caddr_t)pte)
#define pte_alloc_one(addr)		((pte_t *)page_alloc_zeroed())
#define pte_free(pte)			page_free((caddr_t)pte)

#ifndef ARCH_HAVE_SET_PTE
#define set_pte(ptep, pte)	(*(ptep) = (pte))
#endif

#define pmd_pfn(pmd)		(((pmd_val(pmd) & PMD_MASK) & PHYS_MASK) >> PAGE_SHIFT)
#define pfn_pmd(pfn,prot)	__pmd(((phys_addr_t)(pfn) << PAGE_SHIFT) | pgprot_val(prot))

#define pmd_none(pmd)		(!pmd_val(pmd))
#define pmd_bad(pmd)		(!(pmd_val(pmd) & 2))
#define pmd_present(pmd)	pmd_val(pmd)
#define pmd_clear(pmdp)		set_pmd(pmdp, __pmd(0))

#define pmd_page_vaddr(pmd)	\
	((pte_t *)__va(pmd_val(pmd) & PHYS_MASK & (int32_t)PAGE_MASK))
#define pmd_page(pmd)		pfn_to_page(__phys_to_pfn(pmd_val(pmd) & PHYS_MASK))

#define __pmd_populate(pmdp, pte, prot)	set_pmd(pmdp, __pmd(pte | prot))
/*
 * Populate the pmdp entry with a pointer to the pte.  This pmd is part
 * of the mm address space.
 *
 * The pmd must be loaded with the physical address of the PTE table.
 */
#define pmd_populate_kernel(pmdp, ptep)		\
	__pmd_populate(pmdp, __pa(ptep), PMD_TYPE_TABLE)
#define pmd_populate(pmdp, ptep)		\
	__pmd_populate(pmdp, page_to_phys(ptep), PMD_TYPE_TABLE);
#define pmd_pgtable(pmd) pmd_page(pmd)

#define pud_pfn(pud)		(((pud_val(pud) & PUD_MASK) & PHYS_MASK) >> PAGE_SHIFT)

/* PGDIR_SHIFT determines the size a top-level page table entry can map
 * (depending on the configuration, this level can be 0, 1 or 2).
 */
#define PGDIR_SHIFT	\
	(PAGE_PXD_BITS * (PGTABLE_LEVELS - 1) + PAGE_SHIFT)
#define PGDIR_SIZE	(PTR_VAL_ONE << PGDIR_SHIFT)
#define PGDIR_MASK	(~(PGDIR_SIZE-1))
#define PTRS_PER_PGD	(PTR_VAL_ONE << (VA_BITS - PGDIR_SHIFT))

#define pgd_alloc()	(BUG_ON(0), (pgd_t *)NULL)
#define pgd_free(pgd)	BUG_ON(0)

#if PGTABLE_LEVELS == 2
#include <target/paging-nop2d.h>
#else /* PGTABLE_LEVELS > 2 */
/* PMD_SHIFT determines the size a level 2 page table entry can map. */
#define PMD_SHIFT	(PAGE_PXD_BITS + PAGE_SHIFT)
#define PMD_SIZE	(PTR_VAL_ONE << PMD_SHIFT)
#define PMD_MASK	(~(PMD_SIZE-1))
#define PTRS_PER_PMD	PAGE_MAX_TABLE_ENTRIES

/* Find an entry in the second-level page table. */
#define __pmd_index(addr)		(((addr) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define pmd_offset(pud, addr)		\
	((pmd_t *)pud_page_vaddr(*(pud)) + __pmd_index(addr))
static inline caddr_t pmd_addr_end(caddr_t addr, caddr_t end)
{
	caddr_t __boundary = (addr + PMD_SIZE) & PMD_MASK;
	return (__boundary - 1 < end - 1) ? __boundary : end;
}
#define pmd_alloc_one(addr)	((pmd_t *)page_alloc_zeroed())
#define pmd_free(pmd)		page_free((caddr_t)pmd)

#define pud_none(pud)		(!pud_val(pud))
#define pud_bad(pud)		(!(pud_val(pud) & 2))
#define pud_present(pud)	(pud_val(pud))
#define pud_clear(pudp)		set_pud(pudp, __pud(0))
#define pud_populate(pud, pmd)	set_pud(pud, __pud(__pa(pmd) | PMD_TYPE_TABLE))
#define pud_page(pud)		pmd_page(pud_pmd(pud))
#define pud_page_vaddr(pud)	\
	((pmd_t *)__va(pud_val(pud) & PHYS_MASK & (int32_t)PAGE_MASK))

#ifndef ARCH_HAVE_SET_PUD
#define set_pud(pudp, pud)	(*(pudp) = (pud))
#endif
#endif
#if PGTABLE_LEVELS == 3
#include <target/paging-nop3d.h>
#else /* PGTABLE_LEVELS > 3 */
/* PUD_SHIFT determines the size a level 1 page table entry can map. */
#define PUD_SHIFT	(PAGE_PXD_BITS * 2 + PAGE_SHIFT)
#define PUD_SIZE	(PTR_VAL_ONE << PUD_SHIFT)
#define PUD_MASK	(~(PUD_SIZE-1))
#define PTRS_PER_PUD	PAGE_MAX_TABLE_ENTRIES

/* Find an entry in the frst-level page table. */
#define __pud_index(addr)	(((addr) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define pud_offset(pgd, addr)	\
	((pud_t *)pgd_page_vaddr(*(pgd)) + __pud_index(addr))
#define pud_alloc_one(addr)	((pud_t *)page_alloc_zeroed())
#define pud_free(pud)		page_free((caddr_t)pud)

#define pgd_none(pgd)		(!pgd_val(pgd))
#define pgd_bad(pgd)		(!(pgd_val(pgd) & 2))
#define pgd_present(pgd)	pgd_val(pgd)
#define pgd_clear(pgdp)		set_pgd(pgdp, __pgd(0))
#define pgd_populate(pgd, pud)	set_pgd(pgd, __pgd(__pa(pud) | PUD_TYPE_TABLE))
#define pgd_page_vaddr(pgd)	\
	((pud_t *)__va(pgd_val(pgd) & PHYS_MASK & (int32_t)PAGE_MASK))
static inline caddr_t pud_addr_end(caddr_t addr, caddr_t end)
{
	caddr_t __boundary = (addr + PUD_SIZE) & PUD_MASK;
	return (__boundary - 1 < end - 1) ? __boundary : end;
}

#ifndef ARCH_HAVE_SET_PGD
#define set_pgd(pgdp, pgd)	(*(pgdp) = (pgd))
#endif
#endif

#include <driver/mmu.h>

/* When walking page tables, get the address of the next boundary,
 * or the end address of the range if that comes earlier.  Although no
 * vma end wraps to 0, rounded up __boundary may wrap to 0 throughout.
 */
static inline caddr_t pgd_addr_end(caddr_t addr, caddr_t end)
{
	caddr_t __boundary = (addr + PGDIR_SIZE) & PGDIR_MASK;
	return (__boundary - 1 < end - 1) ? __boundary : (end);
}

extern pgd_t mmu_pg_dir[PTRS_PER_PGD];
#endif

#ifdef CONFIG_MMU
void paging_init(void);
#else
#define paging_init()		do { } while (0)
#endif /* CONFIG_MMU */

#endif /* __PAGING_H_INCLUDE__ */
