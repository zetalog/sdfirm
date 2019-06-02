#ifndef __PAGE_ARM64_H_INCLUDE__
#define __PAGE_ARM64_H_INCLUDE__

#include <target/barrier.h>
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
  * +---------+---------+---+
  * |<- PxD ->|<--- PTE --->|
  */
 /* PTE is 8(2^3)-bytes sized,
  * number of PTE in PT is 2^(PAGE_SHIFT-PTE_SIZE_SHIFT)
  * All MMU is implemented in this way, while we still keep the code
  * architecture specific.
  */
#define PAGE_PTR_BITS		3
#define PAGE_PTE_BITS		PAGE_SHIFT
#define PAGE_PXD_BITS		(PAGE_SHIFT - PAGE_PTR_BITS)
typedef uint64_t pfn_t;

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
#define PAGE_OFFSET		VMSA_VA_BASE_HI

/* Highest possible physical address supported */
#define PHYS_MASK_SHIFT		VMSA_PA_SIZE_SHIFT
#define PHYS_MASK		((PTR_VAL_ONE << PHYS_MASK_SHIFT) - 1)

#ifndef __ASSEMBLY__
typedef uint64_t		phys_addr_t;

static inline void page_wmb(void)
{
	dsb(ishst);
	isb();
}
#endif

#define ARCH_HAVE_SET_PTE 1
#define set_pte(ptep, pte)	((*(ptep) = (pte)), page_wmb())
#define ARCH_HAVE_SET_PMD 1
#define set_pmd(pmdp, pmd)	((*(pmdp) = (pmd)), page_wmb())
#if PGTABLE_LEVELS > 2
#define ARCH_HAVE_SET_PUD 1
#define set_pud(pudp, pud)	((*(pudp) = (pud)), page_wmb())
#endif /* PGTABLE_LEVELS > 2 */
#if PGTABLE_LEVELS > 3
#define ARCH_HAVE_SET_PGD 1
#define set_pgd(pgdp, pgd)	((*(pgdp) = (pgd)), dsb(ishst))
#endif /* PGTABLE_LEVELS > 3 */

#endif /* __PAGE_ARM64_H_INCLUDE__ */
