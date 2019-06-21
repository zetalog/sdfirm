#ifndef __PAGING_NOP2D_H_INCLUDE__
#define __PAGING_NOP2D_H_INCLUDE__

#include <target/paging-nop3d.h>

#ifndef __ASSEMBLY__
#define __PAGETABLE_PMD_FOLDED
#define __PAGETABLE_P2D_FOLDED

/*
 * Having the pmd type consist of a pud gets the size right, and allows
 * us to conceptually access the pud entry that this pmd is folded into
 * without casting.
 */
typedef pud_t pmd_t;
#define pmd_val(x)	pud_val((x))
#define __pmd(x)	((pmd_t)__pud(x))

#define PMD_SHIFT	PUD_SHIFT
#define PTRS_PER_PMD	1
#define PMD_SIZE  	(PTR_VAL_ONE << PMD_SHIFT)
#define PMD_MASK  	(~(PMD_SIZE-1))

#define pmd_offset(pud, address)		((pmd_t *)pud)
#define pmd_addr_end(addr, end)			(end)
/*
 * allocating and freeing a pmd is trivial: the 1-entry pmd is
 * inside the pud, so has no extra memory associated with it.
 */
#define pmd_alloc_one(address)			NULL
#define pmd_free(pmd)				do { } while (0)
#define __pmd_free_tlb(tlb, x, a)		do { } while (0)

/* The "pud_xxx()" functions here are trivial for a folded two-level
 * setup: the pmd is never bad, and a pmd always exists (as it's folded
 * into the pud entry)
 */
#define pud_none(pud)				0
#define pud_bad(pud)				0
#define pud_present(pud)			1
#define pud_clear(pudptr)			do { } while (0)
#define pud_populate(pmd, pte)			do { } while (0)
#define pud_page(pud)				pmd_page((pmd_t)pud)
#define pud_page_vaddr(pud)			pmd_page_vaddr((pmd_t)pud)

/* (pmds are folded into puds so this doesn't get actually called,
 * but the define is needed for a generic inline function.)
 */
#define set_pud(pudptr, pudval)			set_pmd((pmd_t *)(pudptr), (pmd_t)pudval)

#define pmd_set_fixmap(addr)			NULL
#define pmd_set_fixmap_offset(pudp, addr)	((pmd_t *)pudp)
#define pmd_clear_fixmap()
#endif /* __ASSEMBLY__ */

#endif /* __PAGING_NOP2D_H_INCLUDE__ */
