#ifndef __PAGE_ARM64_H_INCLUDE__
#define __PAGE_ARM64_H_INCLUDE__

#include <stdio.h>
#include <target/const.h>
#include <target/barrier.h>
#include <target/mem.h>
#include <asm/vmsa.h>

/*===========================================================================
 * sdfirm specific definitions
 *===========================================================================*/
 /* Acronym translations between sdfirm and linux:
  *
  * PGT: page table
  * PTR: page table pointer
  *
  * P1D: Linux PTE
  * P2D: Linux PMD
  * P3D: Linux PUD
  * P4D: Linux P4D
  * PGD: Linux PGDIR
  *
  *                   ->|PTR|<-
  * +---------+---------+---+
  * |<- PxD ->|<--- PTE --->|
  * +---------+---------+---+
  */
 /* PTE is 8(2^3)-bytes sized,
  * number of PTE in PT is 2^(PAGE_SHIFT-PTE_SIZE_SHIFT)
  * All MMU is implemented in this way, while we still keep the code
  * architecture specific.
  */
#define PAGE_PTR_BITS		3
#define PAGE_PTE_BITS		PAGE_SHIFT
#define PAGE_PXD_BITS		(PAGE_SHIFT - PAGE_PTR_BITS)

/*===========================================================================
 * linux style definitions
 *===========================================================================*/
/* Using linux style definitions makes the world easier for the linux kernel
 * developers.
 */
/* PAGE_OFFSET - the virtual address of the start of the kernel image (top
 *		 (VA_BITS - 1))
 * VA_BITS - the maximum number of bits for virtual addresses.
 */
#define VA_BITS			VMSA_VA_SIZE_SHIFT
#ifdef CONFIG_VMSA_VA_2_RANGES
#define PAGE_OFFSET		VMSA_VA_BASE_HI
#else
#define PAGE_OFFSET		0
#endif

/* Highest possible physical address supported */
#define PHYS_MASK_SHIFT		VMSA_PA_SIZE_SHIFT
#define PHYS_MASK		((PTR_VAL_ONE << PHYS_MASK_SHIFT) - 1)

#ifndef __ASSEMBLY__
typedef uint64_t pfn_t;
typedef uint64_t pteval_t;
typedef uint64_t pmdval_t;
typedef uint64_t pudval_t;
typedef uint64_t pgdval_t;

static inline void page_wmb(void)
{
	dsb(ishst);
	isb();
}

#define ARCH_HAVE_SET_PTE 1
static inline void set_pte(pteval_t *ptep, pteval_t pte)
{
	printf("PTE: %016llx: %016llx\n", ptep, pte);
	*ptep = pte;
	page_wmb();
}
/* #define set_pte(ptep, pte)	((*(ptep) = (pte)), page_wmb()) */
#define ARCH_HAVE_SET_PMD 1
static inline void set_pmd(pmdval_t *pmdp, pmdval_t pmd)
{
	printf("PMD: %016llx: %016llx\n", pmdp, pmd);
	*pmdp = pmd;
       	page_wmb();
}
/* #define set_pmd(pmdp, pmd)	((*(pmdp) = (pmd)), page_wmb()) */
#if PGTABLE_LEVELS > 2
#define ARCH_HAVE_SET_PUD 1
static inline void set_pud(pudval_t *pudp, pudval_t pud)
{
	printf("PUD: %016llx: %016llx\n", pudp, pud);
	*pudp = pud;
       	page_wmb();
}
/* #define set_pud(pudp, pud)	((*(pudp) = (pud)), page_wmb()) */
#endif /* PGTABLE_LEVELS > 2 */
#if PGTABLE_LEVELS > 3
#define ARCH_HAVE_SET_PGD 1
static inline void set_pgd(pgdval_t *pgdp, pgdval_t pgd)
{
	printf("PGD: %016llx: %016llx\n", pgdp, pgd);
	*pgdp = pgd;
	page_wmb();
}
/* #define set_pgd(pgdp, pgd)	((*(pgdp) = (pgd)), page_wmb()) */
#endif /* PGTABLE_LEVELS > 3 */
#endif

/*===========================================================================
 * boot page table (bpgt)
 *===========================================================================*/
/* The idmap and boot page tables need some space reserved in the kernel
 * image. Both require pgd, pud (4 levels only) and pmd tables to (section)
 * map the kernel. With the 64K page configuration, swapper and idmap need to
 * map to pte level. Note that the number of ID map translation levels could
 * be increased on the fly if system RAM is out of reach for the default VA
 * range, so pages required to map highest possible PA are reserved in all
 * cases.
 */
#define BPGT_DIR_SIZE		(PGTABLE_LEVELS * PAGE_SIZE)
#define IDMAP_PGTABLE_LEVELS	(__PGTABLE_LEVELS(PHYS_MASK_SHIFT))
#define IDMAP_DIR_SIZE		(IDMAP_PGTABLE_LEVELS * PAGE_SIZE)

/* Initial memory map size */
#define BPGT_BLOCK_SHIFT	PAGE_SHIFT
#define BPGT_BLOCK_SIZE	PAGE_SIZE
#define BPGT_TABLE_SHIFT	PMD_SHIFT

/* The size of the initial kernel direct mapping */
#define BPGT_INIT_MAP_SIZE	(_AC(1, UL) << BPGT_TABLE_SHIFT)

/* Initial memory map attributes. */
#define BPGT_PTE_FLAGS		(PTE_TYPE_PAGE | PTE_AF | PTE_SHARED)
#define BPGT_PMD_FLAGS		(PMD_TYPE_SECT | PMD_SECT_AF | PMD_SECT_S)

#define BPGT_MM_MMUFLAGS	(PMD_ATTRINDX(MT_NORMAL) | BPGT_PMD_FLAGS)
#define BPGT_MM_DEVFLAGS	(PMD_ATTRINDX(MT_DEVICE_nGnRnE) | BPGT_PMD_FLAGS)

#endif /* __PAGE_ARM64_H_INCLUDE__ */
