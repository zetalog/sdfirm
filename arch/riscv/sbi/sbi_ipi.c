/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 *   Nick Kossifidis <mick@ics.forth.gr>
 */

#include <target/sbi.h>

struct sbi_ipi_data {
	atomic_t ipi_type;
};

static unsigned long ipi_data_off;

static int sbi_ipi_send(struct sbi_scratch *scratch, cpu_t cpu,
			u32 event, void *data)
{
	int ret;
	u32 hartid = smp_hw_cpu_hart(cpu);
	struct sbi_scratch *remote_scratch = NULL;
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	struct sbi_ipi_data *ipi_data;

	if (sbi_platform_hart_disabled(plat, hartid))
		return -1;

	/*
	 * Set IPI type on remote hart's scratch area and
	 * trigger the interrupt
	 */
	remote_scratch = sbi_hart_id_to_scratch(scratch, hartid);
	ipi_data = sbi_scratch_offset_ptr(remote_scratch, ipi_data_off);
	if (event == SBI_IPI_EVENT_SFENCE_VMA ||
	    event == SBI_IPI_EVENT_SFENCE_VMA_ASID ||
	    event == SBI_IPI_EVENT_FENCE_I) {
		ret = sbi_tlb_fifo_update(remote_scratch, hartid, data);
		if (ret < 0)
			return ret;
	}
	atomic_or(_BV(event), &ipi_data->ipi_type);
	smp_mb();
	sbi_platform_ipi_send(plat, cpu);

	if (event == SBI_IPI_EVENT_SFENCE_VMA ||
	    event == SBI_IPI_EVENT_SFENCE_VMA_ASID ||
	    event == SBI_IPI_EVENT_FENCE_I) {
		sbi_tlb_fifo_sync(scratch);
	}

	return 0;
}

int sbi_ipi_send_many(struct sbi_scratch *scratch, struct unpriv_trap *uptrap,
		      ulong *pmask, u32 event, void *data)
{
	ulong i, m;
	ulong mask = sbi_hart_available_mask();
	u32 hartid = sbi_current_hartid();

	if (pmask) {
		mask &= load_ulong(pmask, scratch, uptrap);
		if (uptrap->cause)
			return ETRAP;
	}

	/* Send IPIs to every other hart on the set */
	for (i = 0, m = mask; m; i++, m >>= 1)
		if ((m & 1UL) && (i != hartid))
			sbi_ipi_send(scratch, smp_hw_hart_cpu(i),
				     event, data);

	/*
	 * If the current hart is on the set, send an IPI
	 * to it as well
	 */
	if (mask & (1UL << hartid))
		sbi_ipi_send(scratch, smp_hw_hart_cpu(hartid),
			     event, data);
	return 0;
}

void sbi_ipi_clear_smode(struct sbi_scratch *scratch)
{
	csr_clear(CSR_MIP, IR_SSI);
}

void sbi_ipi_process(struct sbi_scratch *scratch)
{
	volatile atomic_count_t ipi_type;
	unsigned int ipi_event;
	const struct sbi_platform *plat = sbi_platform_ptr(scratch);
	struct sbi_ipi_data *ipi_data =
			sbi_scratch_offset_ptr(scratch, ipi_data_off);

	u32 hartid = sbi_current_hartid();
	sbi_platform_ipi_clear(plat, smp_hw_hart_cpu(hartid));

	ipi_type = atomic_xchg(&ipi_data->ipi_type, 0);
	ipi_event = 0;
	while (ipi_type) {
		if (!(ipi_type & 1UL))
			goto skip;

		switch (ipi_event) {
		case SBI_IPI_EVENT_SOFT:
			sbi_trap_log("%d: IPI_EVENT_SOFT\n", hartid);
			csr_set(CSR_MIP, IR_SSI);
			break;
		case SBI_IPI_EVENT_FENCE_I:
			sbi_trap_log("%d: IPI_EVENT_FENCE_I\n", hartid);
			sbi_tlb_fifo_process(scratch);
			break;
		case SBI_IPI_EVENT_SFENCE_VMA:
			sbi_trap_log("%d: IPI_EVENT_SFENCE_VMA\n", hartid);
			sbi_tlb_fifo_process(scratch);
			break;
		case SBI_IPI_EVENT_SFENCE_VMA_ASID:
			sbi_trap_log("%d: IPI_EVENT_SFENCE_VMA_ASID\n", hartid);
			sbi_tlb_fifo_process(scratch);
			break;
		case SBI_IPI_EVENT_HALT:
			sbi_trap_log("%d: IPI_EVENT_HALT\n", hartid);
			bh_panic();
			break;
		default:
			break;
		};

skip:
		ipi_type = ipi_type >> 1;
		ipi_event++;
	};
}

int sbi_ipi_init(struct sbi_scratch *scratch, bool cold_boot)
{
	int ret;
	struct sbi_ipi_data *ipi_data;

	if (cold_boot) {
		ipi_data_off = sbi_scratch_alloc_offset(sizeof(*ipi_data),
							"IPI_DATA");
		if (!ipi_data_off)
			return -ENOMEM;
	} else {
		if (!ipi_data_off)
			return -ENOMEM;
	}

	ipi_data = sbi_scratch_offset_ptr(scratch, ipi_data_off);
	INIT_ATOMIC(&ipi_data->ipi_type, 0);

	ret = sbi_tlb_fifo_init(scratch, cold_boot);
	if (ret)
		return ret;

	/* Enable software interrupts */
	csr_set(CSR_MIE, IR_MSI);
	return sbi_platform_ipi_init(sbi_platform_ptr(scratch), cold_boot);
}
