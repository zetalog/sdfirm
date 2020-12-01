/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)iommu-pgtable.h: IOMMU page table translation formats
 * $Id: iommu-pgtable.h,v 1.1 2020-11-30 13:45:00 zhenglv Exp $
 */

#ifndef __IOMMU_PGTABLE_H_INCLUDE__
#define __IOMMU_PGTABLE_H_INCLUDE__

typedef enum io_pgtable_fmt {
	ARM_32_LPAE_S1,
	ARM_32_LPAE_S2,
	ARM_64_LPAE_S1,
	ARM_64_LPAE_S2,
	ARM_V7S,
	ARM_MALI_LPAE,
	RISCV_32_SV32_S1,
	RISCV_64_SV39_S1,
	RISCV_64_SV48_S1,
	NR_IOMMU_PGTABLE_FMTS,
} iommu_fmt_t;
#define INVALID_IOMMU_FMT	NR_IOMMU_PGTABLE_FMTS

/* struct io_pgtable_cfg - Configuration data for a set of page tables.
 *
 * pgsize_bitmap: A bitmap of page sizes supported by this set of page
 *                tables.
 * ias:           Input address (iova) size, in bits.
 * oas:           Output address (paddr) size, in bits.
 * coherent_walk  A flag to indicate whether or not page table walks made
 *                by the IOMMU are coherent with the CPU caches.
 */
typedef struct io_pgtable_cfg {
	iommu_fmt_t fmt;
	unsigned long pgsize_bitmap;
	unsigned int ias;
	unsigned int oas;
	bool coherent_walk;

	/* Low-level data specific to the table format */
	union {
		struct {
			uint64_t ttbr;
			struct {
				uint32_t ips:3;
				uint32_t tg:2;
				uint32_t sh:2;
				uint32_t orgn:2;
				uint32_t irgn:2;
				uint32_t tsz:6;
			} tcr;
			uint64_t mair;
		} arm_lpae_s1_cfg;

		struct {
			uint64_t vttbr;
			struct {
				uint32_t ps:3;
				uint32_t tg:2;
				uint32_t sh:2;
				uint32_t orgn:2;
				uint32_t irgn:2;
				uint32_t sl:2;
				uint32_t tsz:6;
			} vtcr;
		} arm_lpae_s2_cfg;

		struct {
			uint32_t ttbr;
			uint32_t tcr;
			uint32_t nmrr;
			uint32_t prrr;
		} arm_v7s_cfg;

		struct {
			uint64_t transtab;
			uint64_t memattr;
		} arm_mali_lpae_cfg;
	};
} iommu_cfg_t;

bool iommu_pgtable_alloc(iommu_cfg_t *cfg);
void iommu_pgtable_free(void);

#endif /* __IO_PGTABLE_H_INCLUDE__ */
