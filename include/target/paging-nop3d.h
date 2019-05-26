#ifndef __PAGING_NOP3D_H_INCLUDE__
#define __PAGING_NOP3D_H_INCLUDE__

#ifndef __ASSEMBLY__
#define __PAGETABLE_PUD_FOLDED
#define __PAGETABLE_P3D_FOLDED

/* Having the pud type consist of a pgd gets the size right, and allows
 * us to conceptually access the pgd entry that this pud is folded into
 * without casting.
 */
typedef pgd_t pud_t;
#define pud_val(x)	pgd_val(x)
#define __pud(x)	((pud_t)__pgd(x))

#define PUD_SHIFT	PGDIR_SHIFT
#define PTRS_PER_PUD	1
#define PUD_SIZE  	(PTR_VAL_ONE << PUD_SHIFT)
#define PUD_MASK  	(~(PUD_SIZE-1))

#define pud_offset(pgd, address)		((pud_t *)pgd)
#define pud_addr_end(addr, end)			(end)
/* allocating and freeing a pud is trivial: the 1-entry pud is
 * inside the pgd, so has no extra memory associated with it.
 */
#define pud_alloc_one(address)			NULL
#define pud_free(x)				do { } while (0)
#define __pud_free_tlb(tlb, x, a)		do { } while (0)

/* The "pgd_xxx()" functions here are trivial for a folded two-level
 * setup: the pud is never bad, and a pud always exists (as it's folded
 * into the pgd entry)
 */
#define pgd_none(pgd)				0
#define pgd_bad(pgd)				0
#define pgd_present(pgd)			1
#define pgd_clear(pgdptr)			do { } while (0)
#define pgd_populate(mm, pgd, pud)		do { } while (0)
#define pgd_page(pgd)				pud_page((pud_t)pgd)
#define pgd_page_vaddr(pgd)			pud_page_vaddr((pud_t)pgd)

/*
 * (puds are folded into pgds so this doesn't get actually called,
 * but the define is needed for a generic inline function.)
 */
#define set_pgd(pgdptr, pgdval)			set_pud((pud_t *)(pgdptr), (pud_t)pgdval)
#endif /* __ASSEMBLY__ */

#endif /* __PAGING_NOP3D_H_INCLUDE__ */
