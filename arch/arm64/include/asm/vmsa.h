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
 * @(#)vmsa.h: virtual memory system architecture (VMSA) definitions
 * $Id: vmsa.h,v 1.279 2018-7-19 10:19:18 zhenglv Exp $
 */

#ifndef __VMSA_ARM64_H_INCLUDE__
#define __VMSA_ARM64_H_INCLUDE__

#include <target/const.h>
#include <asm/reg.h>
#include <asm/sysreg.h>

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
 */

#ifdef VMSA_VA_4PB
#define VMSA_VA_SIZE_SHIFT	52
#endif
#ifdef VMSA_VA_256TB
#define VMSA_VA_SIZE_SHIFT	48
#endif
#ifdef VMSA_VA_16TB
#define VMSA_VA_SIZE_SHIFT	44
#endif
#ifdef VMSA_VA_1TB
#define VMSA_VA_SIZE_SHIFT	40
#endif
#ifdef VMSA_VA_64GB
#define VMSA_VA_SIZE_SHIFT	36
#endif
#ifdef VMSA_VA_4GB
#define VMSA_VA_SIZE_SHIFT	32
#endif

#ifndef VMSA_VA_SHIFT_SIZE
#ifdef CONFIG_CPU_64v8_2_LVA
#define VMSA_VA_SIZE_SHIFT	52
#else
#define VMSA_VA_SIZE_SHIFT	48
#endif
#endif

#ifdef CONFIG_VMSA_VA_2_RANGES
#define VMSA_VA_BASE_HI		(ULL(0) - (ULL(1) << VMSA_VA_SIZE_SHIFT))
#define VMSA_VA_BASE_LO		ULL(0x0000000000000000)
#else
#define VMSA_VA_BASE_HI		ULL(0x0000000000000000)
#define VMSA_VA_BASE_LO		ULL(0x0000000000000000)
#endif

#ifdef CONFIG_VMSA_ADDRESS_TAGGING
#if (EL_RUNTIME == ARM_EL3)
#define is_tbi(address)		(!!(read_sysreg(TCR_EL3) & TCR_TBI))
#elif (EL_RUNTIME == ARM_EL2)
#ifdef CONFIG_CPU_64v8_1_VHE
#define is_tbi(address)				\
	(((uintptrt_t)address) & _BV_ULL(55) ?	\
	 !!(read_sysreg(TCR_EL2) & TCR_TBI1) :	\
	 !!(read_sysreg(TCR_EL2) & TCR_TBI0))
#else
#define is_tbi(address)		(!!(read_sysreg(TCR_EL2) & TCR_TBI))
#endif
#elif (EL_RUNTIME == ARM_EL1)
#define is_tbi(address)				\
	(((uintptrt_t)address) & _BV_ULL(55) ?	\
	 !!(read_sysreg(TCR_EL1) & TCR_TBI1) :	\
	 !!(read_sysreg(TCR_EL1) & TCR_TBI0))
#else
#define is_tbi(address)		true
#endif
#else
#define is_tbi(address)		true
#endif

#define write_ttbr_el1(tbl, top)	 \
	((top) ? write_sysreg(tbl, TTBR1_EL1) : write_sysreg(tbl, TTBR0_EL1))
#ifdef CONFIG_SYS_HOST
#define write_ttbr_el2(tbl, top)	 \
	((top) ? write_sysreg(tbl, TTBR1_EL2) : write_sysreg(tbl, TTBR0_EL2))
#else
#define write_ttbr_el2(tbl, top)	 write_sysreg(tbl, TTBR0_EL2)
#endif
#define write_ttbr_el3(tbl, top)	 write_sysreg(tbl, TTBR0_EL3)

static inline void write_ttbr(caddr_t tbl, uint8_t el, bool top)
{
	switch (el) {
	case ARM_EL1:
		write_ttbr_el1(tbl, top);
		break;
	case ARM_EL2:
		write_ttbr_el2(tbl, top);
		break;
	case ARM_EL3:
		write_ttbr_el3(tbl, top);
		break;
	default:
		break;
	}
}

/* PTE is 8(2^3)-bytes sized,
 * number of PTE in PT is 2^(PAGE_SHIFT-PTE_SIZE_SHIFT)
 * All MMU is implemented in this way, while we still keep the code
 * architecture specific.
 */
#define PTR_SIZE_BITS		3
#define PTRS_PER_PGT_BITS	(PAGE_SHIFT - PTR_SIZE_BITS)
#define PTRS_PER_PGT		(1 << PTRS_PER_PGT_BITS)

/*===========================================================================
 * linux style definitions
 *===========================================================================*/
/* Using linux style definitions makes the world easier for the linux kernel
 * developers.
 */
typedef uint64_t pteval_t;
typedef uint64_t pmdval_t;
typedef uint64_t pudval_t;
typedef uint64_t pgdval_t;

#undef STRICT_MM_TYPECHECKS

#ifdef STRICT_MM_TYPECHECKS

/*
 * These are used to make use of C type-checking..
 */
typedef struct { pteval_t pte; } pte_t;
#define pte_val(x)	((x).pte)
#define __pte(x)	((pte_t) { (x) } )

#if PGTABLE_LEVELS > 2
typedef struct { pmdval_t pmd; } pmd_t;
#define pmd_val(x)	((x).pmd)
#define __pmd(x)	((pmd_t) { (x) } )
#endif

#if PGTABLE_LEVELS > 3
typedef struct { pudval_t pud; } pud_t;
#define pud_val(x)	((x).pud)
#define __pud(x)	((pud_t) { (x) } )
#endif

typedef struct { pgdval_t pgd; } pgd_t;
#define pgd_val(x)	((x).pgd)
#define __pgd(x)	((pgd_t) { (x) } )

typedef struct { pteval_t pgprot; } pgprot_t;
#define pgprot_val(x)	((x).pgprot)
#define __pgprot(x)	((pgprot_t) { (x) } )

#else	/* !STRICT_MM_TYPECHECKS */

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

#endif /* STRICT_MM_TYPECHECKS */

#if PGTABLE_LEVELS == 2
#include <target/paging-nop2d.h>
#elif PGTABLE_LEVELS == 3
#include <target/paging-nop3d.h>
#endif

#define VA_BITS			VMSA_VA_SIZE_SHIFT

#define PTRS_PER_PTE		PTRS_PER_PGT

/*
 * PMD_SHIFT determines the size a level 2 page table entry can map.
 */
#if PGTABLE_LEVEL > 2
#define PMD_SHIFT		(PTRS_PER_PGT_BITS + PAGE_SHIFT)
#define PMD_SIZE		(_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK		(~(PMD_SIZE-1))
#define PTRS_PER_PMD		PTRS_PER_PGT
#endif

/*
 * PUD_SHIFT determines the size a level 1 page table entry can map.
 */
#if PGTABLE_LEVEL > 3
#define PUD_SHIFT		(PTRS_PER_PGT_BITS * 2 + PAGE_SHIFT)
#define PUD_SIZE		(_AC(1, UL) << PUD_SHIFT)
#define PUD_MASK		(~(PUD_SIZE-1))
#define PTRS_PER_PUD		PTRS_PER_PGT
#endif

/*
 * PGDIR_SHIFT determines the size a top-level page table entry can map
 * (depending on the configuration, this level can be 0, 1 or 2).
 */
#define PGDIR_SHIFT		\
	(PTRS_PER_PGT_BITS * (PGTABLE_LEVELS - 1) + PAGE_SHIFT)
#define PGDIR_SIZE		(_AC(1, UL) << PGDIR_SHIFT)
#define PGDIR_MASK		(~(PGDIR_SIZE-1))
#define PTRS_PER_PGD		(1 << (VA_BITS - PGDIR_SHIFT))

/*
 * Highest possible physical address supported.
 */
#ifdef CONFIG_CPU_64v8_2_LPA
#define PHYS_MASK_SHIFT		52
#else
#define PHYS_MASK_SHIFT		48
#endif
#define PHYS_MASK		((_AC(1, UL) << PHYS_MASK_SHIFT) - 1)

#define mmu_hw_ctrl_init()

#endif /* __VMSA_ARM64_H_INCLUDE__ */
