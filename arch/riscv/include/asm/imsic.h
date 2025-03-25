/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __IMSIC_RISCV_H_INCLUDE__
#define __IMSIC_RISCV_H_INCLUDE__

#include <target/arch.h>

#define IMSIC_MMIO_PAGE_SHIFT		12
#define IMSIC_MMIO_PAGE_SZ		(1UL << IMSIC_MMIO_PAGE_SHIFT)
#define IMSIC_MAX_REGS			16

#define IMSIC_MMIO_PAGE_LE		0x00
#define IMSIC_MMIO_PAGE_BE		0x04

#define IMSIC_MIN_ID			63
#define IMSIC_MAX_ID			2047

#define IMSIC_EIDELIVERY		0x70

#define IMSIC_EITHRESHOLD		0x72

#define IMSIC_TOPEI			0x76
#define IMSIC_TOPEI_ID_SHIFT		16
#define IMSIC_TOPEI_ID_MASK		0x7ff
#define IMSIC_TOPEI_PRIO_MASK		0x7ff

#define IMSIC_EIP0			0x80

#define IMSIC_EIP63			0xbf

#define IMSIC_EIPx_BITS			32

#define IMSIC_EIE0			0xc0

#define IMSIC_EIE63			0xff

#define IMSIC_EIEx_BITS			32

#define IMSIC_DISABLE_EIDELIVERY	0
#define IMSIC_ENABLE_EIDELIVERY		1
#define IMSIC_DISABLE_EITHRESHOLD	1
#define IMSIC_ENABLE_EITHRESHOLD	0

#define IMSIC_IPI_ID			1

#define imsic_csr_write(__c, __v)	\
do { \
	csr_write(CSR_MISELECT, __c); \
	csr_write(CSR_MIREG, __v); \
} while (0)

#define imsic_csr_read(__c)	\
({ \
	unsigned long __v; \
	csr_write(CSR_MISELECT, __c); \
	__v = csr_read(CSR_MIREG); \
	__v; \
})

#define imsic_csr_set(__c, __v)		\
do { \
	csr_write(CSR_MISELECT, __c); \
	csr_set(CSR_MIREG, __v); \
} while (0)

#define imsic_csr_clear(__c, __v)	\
do { \
	csr_write(CSR_MISELECT, __c); \
	csr_clear(CSR_MIREG, __v); \
} while (0)

struct imsic_regs {
	unsigned long addr;
	unsigned long size;
};

struct imsic_data {
	bool targets_mmode;
	uint32_t guest_index_bits;
	uint32_t hart_index_bits;
	uint32_t group_index_bits;
	uint32_t group_index_shift;
	unsigned long num_ids;
	struct imsic_regs regs[IMSIC_MAX_REGS];
};

#define imsic_clear_irq(irq)
#define imsic_trigger_irq(irq)
#define imsic_enable_irq(irq)
#define imsic_disable_irq(irq)

int imsic_map_hartid_to_data(uint32_t hartid, struct imsic_data *imsic, int file);
struct imsic_data *imsic_get_data(uint32_t hartid);
int imsic_get_target_file(uint32_t hartid);
void imsic_local_irqchip_init(void);
int imsic_warm_irqchip_init(void);
int imsic_data_check(struct imsic_data *imsic);
int imsic_cold_irqchip_init(struct imsic_data *imsic);
int imsic_sbi_init_cold(void);

#endif /* __IMSIC_RISCV_H_INCLUDE__ */
