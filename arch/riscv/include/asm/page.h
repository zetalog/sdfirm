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
#define SATP_MODE		ATP_MODE_32
#endif
#ifdef CONFIG_RISCV_SV39
#define VA_BITS			39
#define SATP_MODE		ATP_MODE_39
#endif
#ifdef CONFIG_RISCV_SV48
#define VA_BITS			48
#define SATP_MODE		ATP_MODE_48
#endif

#if __riscv_xlen == 32
#define PHYS_MASK_SHIFT		34
#define PAGE_PTR_BITS		2
#ifndef __ASSEMBLY__
typedef uint32_t pfn_t;
typedef uint32_t pteval_t;
typedef uint32_t pmdval_t;
typedef uint32_t pudval_t;
typedef uint32_t pgdval_t;
#endif
#elif __riscv_xlen == 64
#define PHYS_MASK_SHIFT		56
#define PAGE_PTR_BITS		3
#ifndef __ASSEMBLY__
typedef uint64_t pfn_t;
typedef uint64_t pteval_t;
typedef uint64_t pmdval_t;
typedef uint64_t pudval_t;
typedef uint64_t pgdval_t;
#endif
#elif defined(CONFIG_MMU)
#ifndef VA_BITS
#error "MMU scheme is not defined."
#endif
#endif

#define PAGE_PTE_BITS		PAGE_SHIFT
#define PAGE_PXD_BITS		(PAGE_SHIFT - PAGE_PTR_BITS)

/* Every address range is linear */
#define FIXADDR_END		PAGE_OFFSET
#ifdef CONFIG_MMU_PAGE_OFFSET
#if __riscv_xlen == 32
#define PAGE_OFFSET		(UL(1) << (VA_BITS - 1))
#endif
#if __riscv_xlen == 64
#define PAGE_OFFSET		(ULL(1) << (VA_BITS - 1))
#endif
#else
#define PAGE_OFFSET		ULL(0x0)
#endif

/* Highest possible physical address supported */
#define PHYS_MASK		((PTR_VAL_ONE << PHYS_MASK_SHIFT) - 1)

#ifndef __ASSEMBLY__
extern unsigned long empty_zero_page[PAGE_SIZE / sizeof (unsigned long)];

#ifdef CONFIG_ARCH_HAS_MMU_ORDER
#define page_barrier()			do  {} while (0)
#else
#define page_barrier()			mb()
#endif

#define ARCH_HAVE_SET_PTE 1
static inline void set_pte(pteval_t *ptep, pteval_t pte)
{
	mmu_dbg_tbl("PTE: %016llx: %016llx\n", ptep, pte);
	*ptep = pte;
	page_barrier();
}
/* #define set_pte(ptep, pte)	((*(ptep) = (pte)), page_wmb()) */
#define ARCH_HAVE_SET_PMD 1
static inline void set_pmd(pmdval_t *pmdp, pmdval_t pmd)
{
	mmu_dbg_tbl("PMD: %016llx: %016llx\n", pmdp, pmd);
	*pmdp = pmd;
	page_barrier();
}
/* #define set_pmd(pmdp, pmd)	((*(pmdp) = (pmd)), page_wmb()) */
#if PGTABLE_LEVELS > 2
#define ARCH_HAVE_SET_PUD 1
static inline void set_pud(pudval_t *pudp, pudval_t pud)
{
	mmu_dbg_tbl("PUD: %016llx: %016llx\n", pudp, pud);
	*pudp = pud;
	page_barrier();
}
/* #define set_pud(pudp, pud)	((*(pudp) = (pud)), page_wmb()) */
#endif /* PGTABLE_LEVELS > 2 */
#if PGTABLE_LEVELS > 3
#define ARCH_HAVE_SET_PGD 1
static inline void set_pgd(pgdval_t *pgdp, pgdval_t pgd)
{
	mmu_dbg_tbl("PGD: %016llx: %016llx\n", pgdp, pgd);
	*pgdp = pgd;
	page_barrier();
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
#define BPGT_PGTABLE_LEVELS	(PGTABLE_LEVELS - 1)
#define IDMAP_PGTABLE_LEVELS	(__PGTABLE_LEVELS(PHYS_MASK_SHIFT) - 1)

#define BPGT_DIR_SIZE		(BPGT_PGTABLE_LEVELS * PAGE_SIZE)
#define IDMAP_DIR_SIZE		(IDMAP_PGTABLE_LEVELS * PAGE_SIZE)

/* Initial memory map uses megapage */
#define BPGT_BLOCK_SHIFT	SECTION_SHIFT
#define BPGT_BLOCK_SIZE		SECTION_SIZE
#define BPGT_TABLE_SHIFT	PUD_SHIFT

/* Initial memory map attributes. */
#define BPGT_PMD_FLAGS		(PAGE_TABLE)

#define BPGT_MM_MMUFLAGS	(PAGE_KERNEL_EXEC)
#define BPGT_MM_DEVFLAGS	(PAGE_KERNEL)

#endif /* __PAGE_RISCV_H_INCLUDE__ */
