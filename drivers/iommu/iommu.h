// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright © 2022-2023 Rivos Inc.
 * Copyright © 2023 FORTH-ICS/CARV
 *
 * RISC-V Ziommu - IOMMU Interface Specification.
 *
 * Authors
 *	Tomasz Jeznach <tjeznach@rivosinc.com>
 *	Nick Kossifidis <mick@ics.forth.gr>
 */

#ifndef _RISCV_IOMMU_H_
#define _RISCV_IOMMU_H_

#include <target/iommu.h>

#include "iommu-bits.h"

#define IOMMU_PAGE_SIZE_4K	BIT_ULL(12)
#define IOMMU_PAGE_SIZE_2M	BIT_ULL(21)
#define IOMMU_PAGE_SIZE_1G	BIT_ULL(30)
#define IOMMU_PAGE_SIZE_512G	BIT_ULL(39)

struct riscv_iommu_domain {
	struct iommu_device_t *iommu;	/* iommu core interface */
	struct iommu_domain domain;
	struct io_pgtable pgtbl;

	struct list_head endpoints;
	struct list_head notifiers;
	// struct mmu_notifier mn;

	unsigned mode;		/* RIO_ATP_MODE_* enum */
	unsigned pscid;		/* RISC-V IOMMU PSCID / GSCID */
	uint32_t pasid;		/* IOMMU_DOMAIN_SVA: Cached PASID */
	bool g_stage;		/* 2nd stage translation domain */

	pgd_t *pgd_root;	/* page table root pointer */
};

extern struct riscv_iommu_domain riscv_iommu_domains[NR_IOMMU_DOMAINS];
#define riscv_iommu_domain_ctrl			riscv_iommu_domains[iommu_dom]

int riscv_iommu_init(void);


#endif
