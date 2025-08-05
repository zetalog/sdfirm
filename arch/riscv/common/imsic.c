/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2021 Western Digital Corporation or its affiliates.
 * Copyright (c) 2022 Ventana Micro Systems Inc.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/irq.h>
#include <target/smp.h>
#include <target/percpu.h>
#include <target/irq.h>
#include <target/sbi.h>

#ifdef CONFIG_SBI
static unsigned long imsic_ptr_offset;

#define imsic_get_hart_data_ptr(__scratch)				\
	sbi_scratch_read_type((__scratch), void *, imsic_ptr_offset)

#define imsic_set_hart_data_ptr(__scratch, __imsic)			\
	sbi_scratch_write_type((__scratch), void *, imsic_ptr_offset, (__imsic))

static unsigned long imsic_file_offset;

#define imsic_get_hart_file(__scratch)					\
	sbi_scratch_read_type((__scratch), long, imsic_file_offset)

#define imsic_set_hart_file(__scratch, __file)				\
	sbi_scratch_write_type((__scratch), long, imsic_file_offset, (__file))

int imsic_map_hartid_to_data(uint32_t hartid, struct imsic_data *imsic, int file)
{
	struct sbi_scratch *scratch;

	if (!imsic || !imsic->targets_mmode)
		return SBI_EINVAL;

	/*
	 * We don't need to fail if scratch pointer is not available
	 * because we might be dealing with hartid of a HART disabled
	 * in device tree. For HARTs disabled in device tree, the
	 * imsic_get_data() and imsic_get_target_file() will anyway
	 * fail.
	 */
	scratch = sbi_hartid_to_scratch(hartid);
	if (!scratch)
		return 0;

	imsic_set_hart_data_ptr(scratch, imsic);
	imsic_set_hart_file(scratch, file);
	return 0;
}

struct imsic_data *imsic_get_data(uint32_t hartid)
{
	struct sbi_scratch *scratch;

	if (!imsic_ptr_offset)
		return NULL;

	scratch = sbi_hartid_to_scratch(hartid);
	if (!scratch)
		return NULL;

	return imsic_get_hart_data_ptr(scratch);
}

int imsic_get_target_file(uint32_t hartid)
{
	struct sbi_scratch *scratch;

	if (!imsic_file_offset)
		return SBI_ENOENT;

	scratch = sbi_hartid_to_scratch(hartid);
	if (!scratch)
		return SBI_ENOENT;

	return imsic_get_hart_file(scratch);
}

static void imsic_local_eix_update(unsigned long base_id,
				   unsigned long num_id, bool pend, bool val)
{
	unsigned long i, isel, ireg;
	unsigned long id = base_id, last_id = base_id + num_id;

	while (id < last_id) {
		isel = id / __riscv_xlen;
		isel *= __riscv_xlen / IMSIC_EIPx_BITS;
		isel += (pend) ? IMSIC_EIP0 : IMSIC_EIE0;

		ireg = 0;
		for (i = id & (__riscv_xlen - 1);
		     (id < last_id) && (i < __riscv_xlen); i++) {
			ireg |= BIT(i);
			id++;
		}

		if (val)
			imsic_csr_set(isel, ireg);
		else
			imsic_csr_clear(isel, ireg);
	}
}

void imsic_local_irqchip_init(void)
{
	struct csr_trap_info trap = { 0 };

	/*
	 * This function is expected to be called from:
	 * 1) nascent_init() platform callback which is called
	 *    very early on each HART in boot-up path and and
	 *    HSM resume path.
	 * 2) irqchip_init() platform callback which is called
	 *    in boot-up path.
	 */

	/* If Smaia not available then do nothing */
	csr_read_allowed(CSR_MTOPI, (ulong)&trap);
	if (trap.cause)
		return;

	/* Setup threshold to allow all enabled interrupts */
	imsic_csr_write(IMSIC_EITHRESHOLD, IMSIC_ENABLE_EITHRESHOLD);

	/* Enable interrupt delivery */
	imsic_csr_write(IMSIC_EIDELIVERY, IMSIC_ENABLE_EIDELIVERY);

	/* Enable IPI */
	imsic_local_eix_update(IMSIC_IPI_ID, 1, false, true);
}

int imsic_warm_irqchip_init(void)
{
	struct imsic_data *imsic = imsic_get_data(sbi_current_hartid());

	/* Sanity checks */
	if (!imsic || !imsic->targets_mmode)
		return SBI_EINVAL;

	/* Disable all interrupts */
	imsic_local_eix_update(1, imsic->num_ids, false, false);

	/* Clear IPI pending */
	imsic_local_eix_update(IMSIC_IPI_ID, 1, true, false);

	/* Local IMSIC initialization */
	imsic_local_irqchip_init();

	return 0;
}

int imsic_data_check(struct imsic_data *imsic)
{
	uint32_t i, tmp;
	unsigned long base_addr, addr, mask;

	/* Sanity checks */
	if (!imsic ||
	    (imsic->num_ids < IMSIC_MIN_ID) ||
	    (IMSIC_MAX_ID < imsic->num_ids))
		return SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT;
	if (tmp < imsic->guest_index_bits)
		return SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits;
	if (tmp < imsic->hart_index_bits)
		return SBI_EINVAL;

	tmp = BITS_PER_LONG - IMSIC_MMIO_PAGE_SHIFT -
	      imsic->guest_index_bits - imsic->hart_index_bits;
	if (tmp < imsic->group_index_bits)
		return SBI_EINVAL;

	tmp = IMSIC_MMIO_PAGE_SHIFT + imsic->guest_index_bits +
	      imsic->hart_index_bits;
	if (imsic->group_index_shift < tmp)
		return SBI_EINVAL;
	tmp = imsic->group_index_bits + imsic->group_index_shift - 1;
	if (tmp >= BITS_PER_LONG)
		return SBI_EINVAL;

	/*
	 * Number of interrupt identities should be 1 less than
	 * multiple of 63
	 */
	if ((imsic->num_ids & IMSIC_MIN_ID) != IMSIC_MIN_ID)
		return SBI_EINVAL;

	/* We should have at least one regset */
	if (!imsic->regs[0].size)
		return SBI_EINVAL;

	/* Match patter of each regset */
	base_addr = imsic->regs[0].addr;
	base_addr &= ~((1UL << (imsic->guest_index_bits +
				 imsic->hart_index_bits +
				 IMSIC_MMIO_PAGE_SHIFT)) - 1);
	base_addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
			imsic->group_index_shift);
	for (i = 0; i < IMSIC_MAX_REGS && imsic->regs[i].size; i++) {
		mask = (1UL << imsic->guest_index_bits) * IMSIC_MMIO_PAGE_SZ;
		mask -= 1UL;
		if (imsic->regs[i].size & mask)
			return SBI_EINVAL;

		addr = imsic->regs[i].addr;
		addr &= ~((1UL << (imsic->guest_index_bits +
					 imsic->hart_index_bits +
					 IMSIC_MMIO_PAGE_SHIFT)) - 1);
		addr &= ~(((1UL << imsic->group_index_bits) - 1) <<
				imsic->group_index_shift);
		if (base_addr != addr)
			return SBI_EINVAL;
	}

	return 0;
}

int imsic_cold_irqchip_init(struct imsic_data *imsic)
{
	int rc;

	/* Sanity checks */
	rc = imsic_data_check(imsic);
	if (rc)
		return rc;

	/* We only initialize M-mode IMSIC */
	if (!imsic->targets_mmode)
		return SBI_EINVAL;

	/* Allocate scratch space pointer */
	if (!imsic_ptr_offset) {
		imsic_ptr_offset = sbi_scratch_alloc_type_offset(void *);
		if (!imsic_ptr_offset)
			return SBI_ENOMEM;
	}

	/* Allocate scratch space file */
	if (!imsic_file_offset) {
		imsic_file_offset = sbi_scratch_alloc_type_offset(long);
		if (!imsic_file_offset)
			return SBI_ENOMEM;
	}

	return 0;
}
#endif /* CONFIG_SBI */
