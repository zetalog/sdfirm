/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
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
 * @(#)page.h: RISCV page allocator interfaces
 * $Id: page.h,v 1.1 2019-08-14 13:53:00 zhenglv Exp $
 */

#ifndef __PAGE_RISCV_H_INCLUDE__
#define __PAGE_RISCV_H_INCLUDE__

#include <target/barrier.h>
#include <target/console.h>
#include <target/mem.h>

/* This file is intended to be used by page allocator and paging. Page
 * allocator requires basic address space range definitions. And paging
 * requires pre-requisite prior than those defined in <target/page.h> while
 * asm/paging includes those that can appear after those defined in
 * <target/page.h>..
 */
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
#ifdef CONFIG_RISCV_SV32
#define VA_BITS			32
#endif
#ifdef CONFIG_RISCV_SV39
#define VA_BITS			39
#endif
#ifdef CONFIG_RISCV_SV48
#define VA_BITS			48
#endif

#if __riscv_xlen == 32
#define PHYS_MASK_SHIFT		34
#define PAGE_PTR_BITS		2
#ifndef VA_BITS
#define VA_BITS			32
#endif
#elif __riscv_xlen == 64
#define PHYS_MASK_SHIFT		56
#define PAGE_PTR_BITS		3
#ifndef VA_BITS
#define VA_BITS			39
#endif
#else
#error "Unsupported"
#endif

#define PAGE_PTE_BITS		PAGE_SHIFT
#define PAGE_PXD_BITS		(PAGE_SHIFT - PAGE_PTR_BITS)

/* Every address range is linear */
#define FIXADDR_END		(ULL(0x1) << VA_BITS)
/*#define FIXADDR_END		PAGE_OFFSET*/
#define PAGE_OFFSET		ULL(0x0)

/* Highest possible physical address supported */
#define PHYS_MASK		((PTR_VAL_ONE << PHYS_MASK_SHIFT) - 1)

#ifndef __ASSEMBLY__
typedef uint64_t pfn_t;
typedef uint64_t pteval_t;
typedef uint64_t pmdval_t;
typedef uint64_t pudval_t;
typedef uint64_t pgdval_t;

extern unsigned long empty_zero_page[PAGE_SIZE / sizeof (unsigned long)];

static inline void page_wmb(void)
{
}

#define ARCH_HAVE_SET_PTE 1
static inline void set_pte(pteval_t *ptep, pteval_t pte)
{
	mmu_dbg_tbl("PTE: %016llx: %016llx\n", ptep, pte);
	*ptep = pte;
	page_wmb();
}
/* #define set_pte(ptep, pte)	((*(ptep) = (pte)), page_wmb()) */
#define ARCH_HAVE_SET_PMD 1
static inline void set_pmd(pmdval_t *pmdp, pmdval_t pmd)
{
	mmu_dbg_tbl("PMD: %016llx: %016llx\n", pmdp, pmd);
	*pmdp = pmd;
       	page_wmb();
}
/* #define set_pmd(pmdp, pmd)	((*(pmdp) = (pmd)), page_wmb()) */
#if PGTABLE_LEVELS > 2
#define ARCH_HAVE_SET_PUD 1
static inline void set_pud(pudval_t *pudp, pudval_t pud)
{
	mmu_dbg_tbl("PUD: %016llx: %016llx\n", pudp, pud);
	*pudp = pud;
       	page_wmb();
}
/* #define set_pud(pudp, pud)	((*(pudp) = (pud)), page_wmb()) */
#endif /* PGTABLE_LEVELS > 2 */
#if PGTABLE_LEVELS > 3
#define ARCH_HAVE_SET_PGD 1
static inline void set_pgd(pgdval_t *pgdp, pgdval_t pgd)
{
	mmu_dbg_tbl("PGD: %016llx: %016llx\n", pgdp, pgd);
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
#define BPGT_PGTABLE_LEVELS	(PGTABLE_LEVELS)
#define IDMAP_PGTABLE_LEVELS	(__PGTABLE_LEVELS(PHYS_MASK_SHIFT))
#define BPGT_DIR_SIZE		(BPGT_PGTABLE_LEVELS * PAGE_SIZE)
#define IDMAP_DIR_SIZE		(IDMAP_PGTABLE_LEVELS * PAGE_SIZE)

#if 0
/* Initial memory map size */
#define BPGT_BLOCK_SHIFT	PAGE_SHIFT
#define BPGT_BLOCK_SIZE		PAGE_SIZE
#define BPGT_TABLE_SHIFT	PMD_SHIFT

/* The size of the initial kernel direct mapping */
#define BPGT_INIT_MAP_SIZE	(_AC(1, UL) << BPGT_TABLE_SHIFT)

/* Initial memory map attributes. */
#define BPGT_PTE_FLAGS		(PTE_TYPE_PAGE | PTE_AF | PTE_SHARED)
#define BPGT_PMD_FLAGS		(PMD_TYPE_SECT | PMD_SECT_AF | PMD_SECT_S)

#define BPGT_MM_MMUFLAGS	(PTE_ATTRINDX(MT_NORMAL) | BPGT_PTE_FLAGS)
#define BPGT_MM_DEVFLAGS	(PMD_ATTRINDX(MT_DEVICE_nGnRnE) | BPGT_PTE_FLAGS)
#endif

#endif /* __PAGE_RISCV_H_INCLUDE__ */
