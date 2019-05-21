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

#define PTRS_BITS		(PAGE_SHIFT - 3)
#define PTRS_PER_PTE		(1 << PTRS_BITS)

/*
 * PMD_SHIFT determines the size a level 2 page table entry can map.
 */
#if PGTABLE_LEVEL > 2
#define PMD_SHIFT		(PTRS_BITS * 2 + 3)
#define PMD_SIZE		(_AC(1, UL) << PMD_SHIFT)
#define PMD_MASK		(~(PMD_SIZE-1))
#define PTRS_PER_PMD		PTRS_PER_PTE
#endif

/*
 * PUD_SHIFT determines the size a level 1 page table entry can map.
 */
#if PGTABLE_LEVEL > 3
#define PUD_SHIFT		(PTRS_BITS * 3 + 3)
#define PUD_SIZE		(_AC(1, UL) << PUD_SHIFT)
#define PUD_MASK		(~(PUD_SIZE-1))
#define PTRS_PER_PUD		PTRS_PER_PTE
#endif

/*
 * PGDIR_SHIFT determines the size a top-level page table entry can map
 * (depending on the configuration, this level can be 0, 1 or 2).
 */
#define PGDIR_SHIFT		((PAGE_SHIFT - 3) * PGTABLE_LEVELS + 3)
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
#define PHYS_MASK		((UL(1) << PHYS_MASK_SHIFT) - 1)

#define mmu_hw_ctrl_init()

#endif /* __VMSA_ARM64_H_INCLUDE__ */
