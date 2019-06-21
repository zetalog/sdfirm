#ifndef __PAGING_H_INCLUDE__
#define __PAGING_H_INCLUDE__

#include <target/generic.h>
#include <target/page.h>
#include <target/tlb.h>

#ifndef PHYS_OFFSET
#define PHYS_OFFSET		0
#endif

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)	ALIGN(addr, PAGE_SIZE)

#ifdef CONFIG_ARCH_HAS_MMU
#define __PA(x)			((x) - PAGE_OFFSET + PHYS_OFFSET)
#define __VA(x)			((x) - PHYS_OFFSET + PAGE_OFFSET)
#define virt_to_phys(x)		(((phys_addr_t)(x) - PAGE_OFFSET + PHYS_OFFSET))
#define phys_to_virt(x)		((caddr_t)((x) - PHYS_OFFSET + PAGE_OFFSET))
#define __pa(x)			virt_to_phys((caddr_t)(x))
#define __va(x)			phys_to_virt((phys_addr_t)(x))
#define __pa_symbol(x)		__pa(RELOC_HIDE((caddr_t)(x), 0))

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

#define __pte_to_phys(pte)	(pte_val(pte) & PTE_ADDR_MASK)
#define __phys_to_pte_val(phys)	(phys)
#define pte_pfn(pte)		((pte_val(pte) & PHYS_MASK) >> PAGE_SHIFT)
#define pfn_pte(pfn, prot)	__pte(((phys_addr_t)(pfn) << PAGE_SHIFT) | pgprot_val(prot))

#define pte_none(pte)		(!pte_val(pte))
#define pte_clear(addr, ptep)	set_pte(ptep, __pte(0))
#define pte_page(pte)		pfn_to_page(pte_pfn(pte))
#define pte_alloc_one_kernel(addr)	((pte_t *)page_alloc_zeroed())
#define pte_free_kernel(pte)		page_free((caddr_t)pte)
#define pte_alloc_one(addr)		((pte_t *)page_alloc_zeroed())
#define pte_free(pte)			page_free((caddr_t)pte)

#define pmd_page_paddr(pmd)		(pmd_val(pmd) & PHYS_MASK & PAGE_MASK)
/* Find an entry in the third-level page table. */
#define pte_index(addr)			(((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#define pte_offset_phys(dir,addr)	(pmd_page_paddr(*(dir)) + pte_index(addr) * sizeof(pte_t))

#ifndef ARCH_HAVE_SET_PTE
#define set_pte(ptep, pte)	(*(ptep) = (pte))
#endif

#define __pmd_to_phys(pmd)	__pte_to_phys(pmd_pte(pmd))
#define __phys_to_pmd_val(phys)	__phys_to_pte_val(phys)
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

#define __pud_to_phys(pud)	__pte_to_phys(pud_pte(pud))
#define __phys_to_pud_val(phys)	__phys_to_pte_val(phys)
#define pud_page_paddr(pud)	__pud_to_phys(pud)
#define pud_pfn(pud)		(((pud_val(pud) & PUD_MASK) & PHYS_MASK) >> PAGE_SHIFT)

#define __pgd_to_phys(pgd)	__pte_to_phys(pgd_pte(pgd))
#define __phys_to_pgd_val(phys)	__phys_to_pte_val(phys)

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

#define pgd_set_fixmap(addr)			((pgd_t *)set_fixmap_offset(FIX_PGD, addr))
#define pgd_clear_fixmap()			clear_fixmap(FIX_PGD)

#define pte_set_fixmap(addr)			((pte_t *)set_fixmap_offset(FIX_PTE, addr))
#define pte_set_fixmap_offset(pmd, addr)	pte_set_fixmap(pte_offset_phys(pmd, addr))
#define pte_clear_fixmap()			clear_fixmap(FIX_PTE)

#ifndef __ASSEMBLY__
static inline caddr_t pte_addr_end(caddr_t addr, caddr_t end)
{
	caddr_t __boundary = (addr + PAGE_SIZE) & PAGE_MASK;
	return (__boundary - 1 < end - 1) ? __boundary : end;
}
#endif /* !__ASSEMBLY__ */

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
#define pmd_index(addr)			(((addr) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))

#define pmd_offset_phys(pud, addr)	(pud_page_paddr(READ_ONCE(*(pud))) + pmd_index(addr) * sizeof(pmd_t))
#define pmd_offset(pud, addr)		\
	((pmd_t *)pud_page_vaddr(*(pud)) + __pmd_index(addr))

#define pmd_set_fixmap(addr)			((pmd_t *)set_fixmap_offset(FIX_PMD, addr))
#define pmd_set_fixmap_offset(pud, addr)	pmd_set_fixmap(pmd_offset_phys(pud, addr))
#define pmd_clear_fixmap()			clear_fixmap(FIX_PMD)

#ifndef __ASSEMBLY__
static inline caddr_t pmd_addr_end(caddr_t addr, caddr_t end)
{
	caddr_t __boundary = (addr + PMD_SIZE) & PMD_MASK;
	return (__boundary - 1 < end - 1) ? __boundary : end;
}
#endif /* !__ASSEMBLY__ */
#define pmd_alloc_one(addr)	((pmd_t *)page_alloc_zeroed())
#define pmd_free(pmd)		page_free((caddr_t)pmd)

#define pud_none(pud)		(!pud_val(pud))
#define pud_bad(pud)		(!(pud_val(pud) & 2))
#define pud_present(pud)	(pud_val(pud))
#define pud_clear(pudp)		set_pud(pudp, __pud(0))
#define __pud_populate(pud, pmd, prot)	\
	set_pud(pud, __pud(__phys_to_pud_val(pmd) | prot))
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
#elif PGTABLE_LEVELS > 3
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

#define __pgd_populate(pgd, pud, prot)	\
	set_pgd(pgd, __pgd(__phys_to_pgd_val(pud) | prot))
#define pgd_populate(pgd, pud)	set_pgd(pgd, __pgd(__pa(pud) | PUD_TYPE_TABLE))
#define pgd_page_vaddr(pgd)	\
	((pud_t *)__va(pgd_val(pgd) & PHYS_MASK & (int32_t)PAGE_MASK))

#define pud_set_fixmap(addr)			((pud_t *)set_fixmap_offset(FIX_PUD, addr))
#define pud_set_fixmap_offset(pgd, addr)	pud_set_fixmap(pud_offset_phys(pgd, addr))
#define pud_clear_fixmap()			clear_fixmap(FIX_PUD)

#ifndef __ASSEMBLY__
static inline caddr_t pud_addr_end(caddr_t addr, caddr_t end)
{
	caddr_t __boundary = (addr + PUD_SIZE) & PUD_MASK;
	return (__boundary - 1 < end - 1) ? __boundary : end;
}
#endif /* !__ASSEMBLY__ */

#ifndef ARCH_HAVE_SET_PGD
#define set_pgd(pgdp, pgd)	(*(pgdp) = (pgd))
#endif
#endif

/* When walking page tables, get the address of the next boundary,
 * or the end address of the range if that comes earlier.  Although no
 * vma end wraps to 0, rounded up __boundary may wrap to 0 throughout.
 */
#ifndef __ASSEMBLY__
static inline caddr_t pgd_addr_end(caddr_t addr, caddr_t end)
{
	caddr_t __boundary = (addr + PGDIR_SIZE) & PGDIR_MASK;
	return (__boundary - 1 < end - 1) ? __boundary : (end);
}

extern pgd_t mmu_pg_dir[PTRS_PER_PGD];
#endif /* !__ASSEMBLY__ */
#endif

#include <driver/mmu.h>

#ifndef __ASSEMBLY__
#ifdef CONFIG_MMU
#define __fix_to_virt(x)	(FIXADDR_END - ((x) << PAGE_SHIFT))
#define __virt_to_fix(x)	((FIXADDR_END - ((x)&PAGE_MASK)) >> PAGE_SHIFT)

/*
 * 'index to address' translation. If anyone tries to use the idx
 * directly without translation, we catch the bug with a NULL-deference
 * kernel oops. Illegal ranges of incoming indices are caught too.
 */
static inline unsigned long fix_to_virt(const unsigned int idx)
{
	BUG_ON(idx >= __end_of_fixed_addresses);
	return __fix_to_virt(idx);
}

static inline unsigned long virt_to_fix(const unsigned long vaddr)
{
	BUG_ON(vaddr >= FIXADDR_TOP || vaddr < FIXADDR_START);
	return __virt_to_fix(vaddr);
}

/*
 * Provide some reasonable defaults for page flags.
 * Not all architectures use all of these different types and some
 * architectures use different names.
 */
#ifndef FIXMAP_PAGE_NORMAL
#define FIXMAP_PAGE_NORMAL PAGE_KERNEL
#endif
#if !defined(FIXMAP_PAGE_RO) && defined(PAGE_KERNEL_RO)
#define FIXMAP_PAGE_RO PAGE_KERNEL_RO
#endif
#ifndef FIXMAP_PAGE_NOCACHE
#define FIXMAP_PAGE_NOCACHE PAGE_KERNEL_NOCACHE
#endif
#ifndef FIXMAP_PAGE_IO
#define FIXMAP_PAGE_IO PAGE_KERNEL_IO
#endif
#ifndef FIXMAP_PAGE_CLEAR
#define FIXMAP_PAGE_CLEAR __pgprot(0)
#endif

#ifndef set_fixmap
#define set_fixmap(idx, phys)				\
	__set_fixmap(idx, phys, FIXMAP_PAGE_NORMAL)
#endif

#ifndef clear_fixmap
#define clear_fixmap(idx)			\
	__set_fixmap(idx, 0, FIXMAP_PAGE_CLEAR)
#endif

/* Return a pointer with offset calculated */
#define __set_fixmap_offset(idx, phys, flags)				\
({									\
	caddr_t ________addr;						\
	__set_fixmap(idx, phys, flags);					\
	________addr = fix_to_virt(idx) + ((phys) & (PAGE_SIZE - 1));	\
	________addr;							\
})

#define set_fixmap_offset(idx, phys) \
	__set_fixmap_offset(idx, phys, FIXMAP_PAGE_NORMAL)

/*
 * Some hardware wants to get fixmapped without caching.
 */
#define set_fixmap_nocache(idx, phys) \
	__set_fixmap(idx, phys, FIXMAP_PAGE_NOCACHE)

#define set_fixmap_offset_nocache(idx, phys) \
	__set_fixmap_offset(idx, phys, FIXMAP_PAGE_NOCACHE)

/*
 * Some fixmaps are for IO
 */
#define set_fixmap_io(idx, phys) \
	__set_fixmap(idx, phys, FIXMAP_PAGE_IO)

#define set_fixmap_offset_io(idx, phys) \
	__set_fixmap_offset(idx, phys, FIXMAP_PAGE_IO)

void early_fixmap_init(void);
void paging_init(void);
#else
#define early_fixmap_init()			do { } while (0)
#define paging_init()				do { } while (0)
#endif /* CONFIG_MMU */
#endif /* !__ASSEMBLY__ */

#endif /* __PAGING_H_INCLUDE__ */
