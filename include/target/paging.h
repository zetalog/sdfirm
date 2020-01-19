#ifndef __PAGING_H_INCLUDE__
#define __PAGING_H_INCLUDE__

#include <target/page.h>
#include <target/tlb.h>

#ifndef PHYS_OFFSET
#define PHYS_OFFSET		0
#endif

/* to align the pointer to the (next) page boundary */
#define PAGE_ALIGN(addr)	ALIGN((addr), PAGE_SIZE)
#define PAGE_ALIGNED(addr)	IS_ALIGNED((addr), PAGE_SIZE)

#ifdef CONFIG_ARCH_HAS_MMU
#define __PA(x)			((x) - PAGE_OFFSET + PHYS_OFFSET)
#define __VA(x)			((x) - PHYS_OFFSET + PAGE_OFFSET)
#define virt_to_phys(x)		(((phys_addr_t)(x) - PAGE_OFFSET + PHYS_OFFSET))
#define phys_to_virt(x)		(((caddr_t)(x) - PHYS_OFFSET + PAGE_OFFSET))
#define __pa(x)			virt_to_phys((caddr_t)(x))
#define __va(x)			phys_to_virt((phys_addr_t)(x))

/* Convert a physical address to a Page Frame Number and back */
#define phys_to_pfn(paddr)	((pfn_t)((paddr) >> PAGE_SHIFT))
#define pfn_to_phys(pfn)	((phys_addr_t)(pfn) << PAGE_SHIFT)
/* Convert a virtual address to a Page Frame Number and back */
#define virt_to_pfn(vaddr)	phys_to_pfn(__pa(vaddr))
#define pfn_to_virt(pfn)	__va(pfn_to_phys(pfn))

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

#define pte_none(pte)		(!pte_val(pte))
#define pte_index(addr)		(((addr) >> PAGE_SHIFT) & (PTRS_PER_PTE - 1))
#ifndef ARCH_HAVE_SET_PTE
#define set_pte(ptep, pte)	(*(ptep) = (pte))
#endif
#define pte_clear(addr, ptep)	set_pte(ptep, __pte(0))
#define pte_offset(dir, addr)	(pmd_page_vaddr(*(dir)) + pte_index(addr))

#define pte_offset_phys(dir, addr)	\
	(pmd_page_paddr(*(dir)) + pte_index(addr) * sizeof(pte_t))

#ifndef __ASSEMBLY__
static inline caddr_t pte_addr_end(caddr_t addr, caddr_t end)
{
	caddr_t __boundary = (addr + PAGE_SIZE) & PAGE_MASK;
	return (__boundary - 1 < end - 1) ? __boundary : end;
}
#endif /* !__ASSEMBLY__ */

#define pmd_none(pmd)		(!pmd_val(pmd))
#define pmd_clear(pmdp)		set_pmd(pmdp, __pmd(0))

#define __pmd_populate(pmdp, pte, prot)	set_pmd(pmdp, __pmd(pte | prot))
#define pmd_populate(pmdp, ptep)	\
	__pmd_populate(pmdp, __pa(ptep), PMD_TYPE_TABLE)

/* PGDIR_SHIFT determines the size a top-level page table entry can map
 * (depending on the configuration, this level can be 0, 1 or 2).
 */
#define PGDIR_SHIFT	\
	(PAGE_PXD_BITS * (PGTABLE_LEVELS - 1) + PAGE_SHIFT)
#define PGDIR_SIZE	(PTR_VAL_ONE << PGDIR_SHIFT)
#define PGDIR_MASK	(~(PGDIR_SIZE-1))
#define PTRS_PER_PGD	(PTR_VAL_ONE << (VA_BITS - PGDIR_SHIFT))

#define pgd_index(addr)			\
	(((addr) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))
#define pgd_offset(pgd, addr)		((pgd) + pgd_index(addr))

#define pgd_set_fixmap(addr)			\
	((pgd_t *)set_fixmap_offset(FIX_PGD, addr))
#define pgd_clear_fixmap()			clear_fixmap(FIX_PGD)
#define pte_set_fixmap(addr)			\
	((pte_t *)set_fixmap_offset(FIX_PTE, addr))
#define pte_set_fixmap_offset(pmd, addr)	\
	pte_set_fixmap(pte_offset_phys(pmd, addr))
#define pte_clear_fixmap()			clear_fixmap(FIX_PTE)

#if PGTABLE_LEVELS == 2
#include <target/paging-nop2d.h>
#else /* PGTABLE_LEVELS > 2 */
#define pud_page_paddr(pud)	(pud_val(pud) & PHYS_MASK & PAGE_MASK)

/* PMD_SHIFT determines the size a level 2 page table entry can map. */
#define PMD_SHIFT	(PAGE_PXD_BITS + PAGE_SHIFT)
#define PMD_SIZE	(PTR_VAL_ONE << PMD_SHIFT)
#define PMD_MASK	(~(PMD_SIZE-1))
#define PTRS_PER_PMD	PAGE_MAX_TABLE_ENTRIES

/* Find an entry in the second-level page table. */
#define pmd_index(addr)			(((addr) >> PMD_SHIFT) & (PTRS_PER_PMD - 1))
#define pmd_offset_phys(dir, addr)	\
	(pud_page_paddr(*(dir)) + pmd_index(addr) * sizeof(pmd_t))
#define pmd_offset(pud, addr)		\
	((pmd_t *)pud_page_vaddr(*(pud)) + pmd_index(addr))

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

#define pud_none(pud)		(!pud_val(pud))
#define pud_clear(pudp)		set_pud(pudp, __pud(0))

#define __pud_populate(pudp, pmd, prot)	set_pud(pudp, __pud((pmd) | prot))
#define pud_populate(pudp, pmd)		__pud_populate(pudp, __pa(pmd), PMD_TYPE_TABLE)
#define pud_page(pud)		pmd_page(pud_pmd(pud))

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
#define pud_index(addr)			(((addr) >> PUD_SHIFT) & (PTRS_PER_PUD - 1))
#define pud_offset_phys(dir, addr)	(pgd_page_paddr(*(dir)) + pud_index(addr) * sizeof(pud_t))
#define pud_offset(pgd, addr)		((pud_t *)pgd_page_vaddr(*(pgd)) + pud_index(addr))

#define pud_set_fixmap(addr)			((pud_t *)set_fixmap_offset(FIX_PUD, addr))
#define pud_set_fixmap_offset(pgd, addr)	pud_set_fixmap(pud_offset_phys(pgd, addr))
#define pud_clear_fixmap()			clear_fixmap(FIX_PUD)

#define pgd_bad(pgd)		(!(pgd_val(pgd) & 2))
#define pgd_present(pgd)	pgd_val(pgd)
#define pgd_none(pgd)		(!pgd_val(pgd))
#define pgd_clear(pgdp)		set_pgd(pgdp, __pgd(0))

#define __pgd_populate(pgdp, pud, prot)	set_pgd(pgdp, __pgd((pud) | prot))
#define pgd_populate(pgdp, pud)		__pgd_populate(pgdp, __pa(pud), PUD_TYPE_TABLE)
#define pgd_page_paddr(pgd)	(pgd_val(pgd) & PHYS_MASK & PAGE_MASK)
#define pgd_page_vaddr(pgd)	__va(pgd_page_paddr(pgd))

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

#ifdef CONFIG_MMU_IDMAP
extern pgd_t mmu_pg_dir[PTRS_PER_PGD];
#else
#define mmu_pg_dir		mmu_id_map
#endif
#endif /* !__ASSEMBLY__ */
#else
#define __pa(x)			((phys_addr_t)(caddr_t)(x))
#define __va(x)			((caddr_t)(x))
#endif /* CONFIG_ARCH_HAS_MMU */

#include <driver/mmu.h>

#define fix_to_virt(x)		(FIXADDR_END - ((x) << PAGE_SHIFT))
#define virt_to_fix(x)		((FIXADDR_END - ((x)&PAGE_MASK)) >> PAGE_SHIFT)

/* Provide some reasonable defaults for page flags.
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

#ifndef __ASSEMBLY__
#ifdef CONFIG_MMU
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
/* Some hardware wants to get fixmapped without caching. */
#define set_fixmap_nocache(idx, phys) \
	__set_fixmap(idx, phys, FIXMAP_PAGE_NOCACHE)
#define set_fixmap_offset_nocache(idx, phys) \
	__set_fixmap_offset(idx, phys, FIXMAP_PAGE_NOCACHE)
/* Some fixmaps are for IO */
#define set_fixmap_io(idx, phys) \
	__set_fixmap(idx, phys, FIXMAP_PAGE_IO)
#define set_fixmap_offset_io(idx, phys) \
	__set_fixmap_offset(idx, phys, FIXMAP_PAGE_IO)

void __set_fixmap(enum fixed_addresses idx,
		  phys_addr_t phys, pgprot_t prot);

void early_fixmap_init(void);
void create_pgd_mapping(pgd_t *pgdir, phys_addr_t phys,
			caddr_t virt, phys_addr_t size,
			pgprot_t prot, bool page_mapping_only);
void paging_init(void);
#else /* CONFIG_MMU */
#define __set_fixmap(idx, phys, prot)		do { } while (0)
#define early_fixmap_init()			do { } while (0)
#define create_pgd_mapping(pgdir, phys, virt, size, prot, mapping_only)	\
	do { } while (0)
#define paging_init()				do { } while (0)
#endif /* CONFIG_MMU */
#endif /* !__ASSEMBLY__ */

#endif /* __PAGING_H_INCLUDE__ */
