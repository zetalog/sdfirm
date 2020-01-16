/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2018
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)mmu.h: memory management unit definitions
 * $Id: mmu.h,v 1.0 2019-12-26 15:15:00 zhenglv Exp $
 */

#ifndef __MMU_RISCV_H_INCLUDE__
#define __MMU_RISCV_H_INCLUDE__

#include <target/barrier.h>

/*---------------------------------------------------------------------------
 * Hardware MMU definitions
 *---------------------------------------------------------------------------*/
/* megapage definitions */
#define SECTION_SHIFT			PMD_SHIFT
#define SECTION_SIZE			(_AC(1, ULL) << SECTION_SHIFT)
#define SECTION_MASK			(~(SECTION_SIZE-1))

#define PUD_TYPE_TABLE			PAGE_TABLE
#define PMD_TYPE_TABLE			PAGE_TABLE

/* Hardware page table definitions */

/* PTE format:
 * | XLEN-1  10 | 9             8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0
 *       PFN      reserved for SW   D   A   G   U   X   W   R   V
 */

#define _PAGE_ACCESSED_OFFSET	6

#define _PAGE_PRESENT	(1 << 0)
#define _PAGE_VALID	(1 << 0) /* Valid */
#define _PAGE_READ	(1 << 1) /* Readable */
#define _PAGE_WRITE	(1 << 2) /* Writable */
#define _PAGE_EXEC	(1 << 3) /* Executable */
#define _PAGE_USER	(1 << 4) /* User */
#define _PAGE_GLOBAL	(1 << 5) /* Global */
#define _PAGE_ACCESSED	(1 << 6) /* Set by hardware on any access */
#define _PAGE_DIRTY	(1 << 7) /* Set by hardware on any write */
#define _PAGE_SOFT1	(1 << 8) /* Reserved for supervisor software */
#define _PAGE_SOFT2	(1 << 9) /* Reserved for supervisor software */

#define _PAGE_SPECIAL	_PAGE_SOFT1
#define _PAGE_TABLE	_PAGE_PRESENT

/* _PAGE_PROT_NONE is set on not-present pages (and ignored by the
 * hardware) to distinguish them from swapped out pages
 */
#define _PAGE_PROT_NONE	_PAGE_READ
#define _PAGE_PFN_SHIFT	10

/* Set of bits to preserve across pte_modify() */
#define _PAGE_CHG_MASK	(~(unsigned long)(_PAGE_PRESENT | _PAGE_READ |	\
					  _PAGE_WRITE | _PAGE_EXEC |	\
					  _PAGE_USER | _PAGE_GLOBAL))

/* Page protection bits */
#define _PAGE_BASE	(_PAGE_PRESENT | _PAGE_ACCESSED | _PAGE_USER)

#define PAGE_NONE		__pgprot(_PAGE_PROT_NONE)
#define PAGE_READ		__pgprot(_PAGE_BASE | _PAGE_READ)
#define PAGE_WRITE		__pgprot(_PAGE_BASE | _PAGE_READ | _PAGE_WRITE)
#define PAGE_EXEC		__pgprot(_PAGE_BASE | _PAGE_EXEC)
#define PAGE_READ_EXEC		__pgprot(_PAGE_BASE | _PAGE_READ | _PAGE_EXEC)
#define PAGE_WRITE_EXEC		__pgprot(_PAGE_BASE | _PAGE_READ |	\
					 _PAGE_EXEC | _PAGE_WRITE)

#define PAGE_COPY		PAGE_READ
#define PAGE_COPY_EXEC		PAGE_EXEC
#define PAGE_COPY_READ_EXEC	PAGE_READ_EXEC
#define PAGE_SHARED		PAGE_WRITE
#define PAGE_SHARED_EXEC	PAGE_WRITE_EXEC

#define _PAGE_KERNEL		(_PAGE_READ \
				| _PAGE_PRESENT \
				| _PAGE_ACCESSED \
				| _PAGE_DIRTY)

#define PAGE_KERNEL		__pgprot(_PAGE_KERNEL | _PAGE_WRITE)
#define PAGE_KERNEL_RO		__pgprot(_PAGE_KERNEL)
#define PAGE_KERNEL_ROX		__pgprot(_PAGE_KERNEL | _PAGE_EXEC)
#define PAGE_KERNEL_EXEC	__pgprot(_PAGE_KERNEL \
					| _PAGE_WRITE | _PAGE_EXEC)

#define PAGE_TABLE		__pgprot(_PAGE_TABLE)

/* MAP_PRIVATE permissions: xwr (copy-on-write) */
#define __P000	PAGE_NONE
#define __P001	PAGE_READ
#define __P010	PAGE_COPY
#define __P011	PAGE_COPY
#define __P100	PAGE_EXEC
#define __P101	PAGE_READ_EXEC
#define __P110	PAGE_COPY_EXEC
#define __P111	PAGE_COPY_READ_EXEC

/* MAP_SHARED permissions: xwr */
#define __S000	PAGE_NONE
#define __S001	PAGE_READ
#define __S010	PAGE_SHARED
#define __S011	PAGE_SHARED
#define __S100	PAGE_EXEC
#define __S101	PAGE_READ_EXEC
#define __S110	PAGE_SHARED_EXEC
#define __S111	PAGE_SHARED_EXEC

#ifndef __ASSEMBLY__
#define pfn_pgd(pfn, prot)	\
	__pgd((pfn << _PAGE_PFN_SHIFT) | pgprot_val(prot))
#define _pgd_pfn(pgd)		(pgd_val(pgd) >> _PAGE_PFN_SHIFT)
#define pgd_index(addr)		(((addr) >> PGDIR_SHIFT) & (PTRS_PER_PGD - 1))

/* Yields the page frame number (PFN) of a page table entry */
#define pte_pfn(pte)			((pte_val(pte) >> _PAGE_PFN_SHIFT))
#define pte_page(x)     		pfn_to_page(pte_pfn(x))
#define pfn_pte(pfn, prot)		\
	(__pte((pfn << _PAGE_PFN_SHIFT) | pgprot_val(prot)))

#define mk_pud_sect_prot(prot)		(prot)
#define mk_pmd_sect_prot(prot)		(prot)
#define pgattr_change_is_safe(old, new)	true

#define pte_present(pte)	\
	(pte_val(pte) & (_PAGE_PRESENT | _PAGE_PROT_NONE))
#define pte_write(pte)		(pte_val(pte) & _PAGE_WRITE)
#define pte_exec(pte)		(pte_val(pte) & _PAGE_EXEC)
#define pte_dirty(pte)		(pte_val(pte) & _PAGE_DIRTY)
#define pte_young(pte)		(pte_val(pte) & _PAGE_ACCESSED)
#define pte_special(pte)	(pte_val(pte) & _PAGE_SPECIAL)
#define pte_huge(pte)		\
	(pte_present(pte) &&	\
	 (pte_val(pte) & (_PAGE_READ | _PAGE_WRITE | _PAGE_EXEC)))

/* static inline pte_t pte_rdprotect(pte_t pte) */
#define pte_wrprotect(pte)	(__pte(pte_val(pte) & ~(_PAGE_WRITE)))

/* static inline pte_t pte_mkread(pte_t pte) */
/* static inline pte_t pte_mkexec(pte_t pte) */
#define pte_mkwrite(pte)	(__pte(pte_val(pte) | _PAGE_WRITE))
#define pte_mkdirty(pte)	(__pte(pte_val(pte) | _PAGE_DIRTY))
#define pte_mkclean(pte)	(__pte(pte_val(pte) & ~(_PAGE_DIRTY)))
#define pte_mkyoung(pte)	(__pte(pte_val(pte) | _PAGE_ACCESSED))
#define pte_mkold(pte)		(__pte(pte_val(pte) & ~(_PAGE_ACCESSED)))
#define pte_mkspecial(pte)	(__pte(pte_val(pte) | _PAGE_SPECIAL))
#define pte_mkhuge(pte)		(pte)
/* Modify page protection bits */
#define pte_modify(pte, newprot)	\
	(__pte((pte_val(pte) & _PAGE_CHG_MASK) | pgprot_val(newprot)))

#define pmd_page(pmd)		(pfn_to_page(pmd_val(pmd) >> _PAGE_PFN_SHIFT))
#define pmd_page_vaddr(pmd)	\
	((unsigned long)pfn_to_virt(pmd_val(pmd) >> _PAGE_PFN_SHIFT))
#define pmd_page_paddr(pmd)	(pmd_val(pmd) & PHYS_MASK & PAGE_MASK)
#define pmd_present(pmd)	\
	(pmd_val(pmd) & (_PAGE_PRESENT | _PAGE_PROT_NONE))
#define pmd_bad(pmd)		(!pmd_present(pmd))

#if PGTABLE_LEVELS > 3
#endif
#if PGTABLE_LEVELS > 2
#define pud_present(pud)	(pud_val(pud) & _PAGE_PRESENT)
#define pud_bad(pud)		(!pud_present(pud))
#define pud_page_vaddr(pud)	\
	((unsigned long)pfn_to_virt(pud_val(pud) >> _PAGE_PFN_SHIFT))

#define pfn_pmd(pfn, prot)	\
	__pmd((pfn << _PAGE_PFN_SHIFT) | pgprot_val(prot))
#define pmd_pfn(pmd)		(pmd_val(pmd) >> _PAGE_PFN_SHIFT)
#endif /* PGTABLE_LEVELS > 2 */

/* To include device specific fixmaps */
#include <asm/mach/mmu.h>

void mmu_hw_create_mapping(phys_addr_t phys, caddr_t virt,
			   phys_addr_t size);
void mmu_hw_ctrl_init(void);
#endif /* !__ASSEMBLY__ */

#ifndef LINKER_SCRIPT
#ifdef __ASSEMBLY__
.extern mmu_id_map
.extern empty_zero_page
#else
extern pgd_t mmu_id_map[IDMAP_DIR_SIZE / sizeof (pgd_t)];
extern unsigned long empty_zero_page[PAGE_SIZE / sizeof (unsigned long)];
#endif
#endif

#endif /* __MMU_RISCV_H_INCLUDE__ */
