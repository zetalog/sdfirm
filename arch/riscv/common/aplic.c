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
#include <target/sbi.h>

static void aplic_writel_msicfg(struct aplic_msicfg_data *msicfg,
				void *msicfgaddr, void *msicfgaddrH)
{
	uint32_t val;
	unsigned long base_ppn;

	/* Check if MSI config is already locked */
	if (__raw_readl((caddr_t)msicfgaddrH) & APLIC_xMSICFGADDRH_L)
		return;

	/* Compute the MSI base PPN */
	base_ppn = msicfg->base_addr >> APLIC_xMSICFGADDR_PPN_SHIFT;
	base_ppn &= ~APLIC_xMSICFGADDR_PPN_HART(msicfg->lhxs);
	base_ppn &= ~APLIC_xMSICFGADDR_PPN_LHX(msicfg->lhxw, msicfg->lhxs);
	base_ppn &= ~APLIC_xMSICFGADDR_PPN_HHX(msicfg->hhxw, msicfg->hhxs);

	/* Write the lower MSI config register */
	__raw_writel((uint32_t)base_ppn, (caddr_t)msicfgaddr);

	/* Write the upper MSI config register */
	val = (((uint64_t)base_ppn) >> 32) &
		APLIC_xMSICFGADDRH_BAPPN_MASK;
	val |= (msicfg->lhxw & APLIC_xMSICFGADDRH_LHXW_MASK)
		<< APLIC_xMSICFGADDRH_LHXW_SHIFT;
	val |= (msicfg->hhxw & APLIC_xMSICFGADDRH_HHXW_MASK)
		<< APLIC_xMSICFGADDRH_HHXW_SHIFT;
	val |= (msicfg->lhxs & APLIC_xMSICFGADDRH_LHXS_MASK)
		<< APLIC_xMSICFGADDRH_LHXS_SHIFT;
	val |= (msicfg->hhxs & APLIC_xMSICFGADDRH_HHXS_MASK)
		<< APLIC_xMSICFGADDRH_HHXS_SHIFT;
	__raw_writel(val, (caddr_t)msicfgaddrH);
}

static int aplic_check_msicfg(struct aplic_msicfg_data *msicfg)
{
	if (APLIC_xMSICFGADDRH_LHXS_MASK < msicfg->lhxs)
		return SBI_EINVAL;

	if (APLIC_xMSICFGADDRH_LHXW_MASK < msicfg->lhxw)
		return SBI_EINVAL;

	if (APLIC_xMSICFGADDRH_HHXS_MASK < msicfg->hhxs)
		return SBI_EINVAL;

	if (APLIC_xMSICFGADDRH_HHXW_MASK < msicfg->hhxw)
		return SBI_EINVAL;

	return 0;
}

int aplic_cold_irqchip_init(struct aplic_data *aplic)
{
	int rc;
	uint32_t i, j, tmp;
//	struct sbi_domain_memregion reg;
	struct aplic_delegate_data *deleg;
	uint32_t first_deleg_irq, last_deleg_irq;

	/* Sanity checks */
	if (!aplic ||
	    !aplic->num_source || APLIC_MAX_SOURCE <= aplic->num_source ||
	    APLIC_MAX_IDC <= aplic->num_idc)
		return SBI_EINVAL;
	if (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		rc = aplic_check_msicfg(&aplic->msicfg_mmode);
		if (rc)
			return rc;
	}
	if (aplic->targets_mmode && aplic->has_msicfg_smode) {
		rc = aplic_check_msicfg(&aplic->msicfg_smode);
		if (rc)
			return rc;
	}

	/* Set domain configuration to 0 */
	__raw_writel(0, (caddr_t)(aplic->addr + APLIC_DOMAINCFG));

	/* Disable all interrupts */
	for (i = 0; i <= aplic->num_source; i += 32)
		__raw_writel(-1U, (caddr_t)(aplic->addr + APLIC_CLRIE_BASE +
				     (i / 32) * sizeof(uint32_t)));

	/* Set interrupt type and priority for all interrupts */
	for (i = 1; i <= aplic->num_source; i++) {
		/* Set IRQ source configuration to 0 */
		__raw_writel(0, (caddr_t)(aplic->addr + APLIC_SOURCECFG_BASE +
			  (i - 1) * sizeof(uint32_t)));
		/* Set IRQ target hart index and priority to 1 */
		__raw_writel(APLIC_DEFAULT_PRIORITY, (caddr_t)(aplic->addr +
						APLIC_TARGET_BASE +
						(i - 1) * sizeof(uint32_t)));
	}

	/* Configure IRQ delegation */
	first_deleg_irq = -1U;
	last_deleg_irq = 0;
	for (i = 0; i < APLIC_MAX_DELEGATE; i++) {
		deleg = &aplic->delegate[i];
		if (!deleg->first_irq || !deleg->last_irq)
			continue;
		if (aplic->num_source < deleg->first_irq ||
		    aplic->num_source < deleg->last_irq)
			continue;
		if (APLIC_SOURCECFG_CHILDIDX_MASK < deleg->child_index)
			continue;
		if (deleg->first_irq > deleg->last_irq) {
			tmp = deleg->first_irq;
			deleg->first_irq = deleg->last_irq;
			deleg->last_irq = tmp;
		}
		if (deleg->first_irq < first_deleg_irq)
			first_deleg_irq = deleg->first_irq;
		if (last_deleg_irq < deleg->last_irq)
			last_deleg_irq = deleg->last_irq;
		for (j = deleg->first_irq; j <= deleg->last_irq; j++)
			__raw_writel(APLIC_SOURCECFG_D | deleg->child_index,
				(caddr_t)(aplic->addr + APLIC_SOURCECFG_BASE +
				(j - 1) * sizeof(uint32_t)));
	}

	/* Default initialization of IDC structures */
	for (i = 0; i < aplic->num_idc; i++) {
		__raw_writel(0, (caddr_t)(aplic->addr + APLIC_IDC_BASE +
			  i * APLIC_IDC_SIZE + APLIC_IDC_IDELIVERY));
		__raw_writel(0, (caddr_t)(aplic->addr + APLIC_IDC_BASE +
			  i * APLIC_IDC_SIZE + APLIC_IDC_IFORCE));
		__raw_writel(APLIC_DISABLE_ITHRESHOLD, (caddr_t)(aplic->addr +
						  APLIC_IDC_BASE +
						  (i * APLIC_IDC_SIZE) +
						  APLIC_IDC_ITHRESHOLD));
	}

	/* MSI configuration */
	if (aplic->targets_mmode && aplic->has_msicfg_mmode) {
		aplic_writel_msicfg(&aplic->msicfg_mmode,
				(void *)(aplic->addr + APLIC_MMSICFGADDR),
				(void *)(aplic->addr + APLIC_MMSICFGADDRH));
	}
	if (aplic->targets_mmode && aplic->has_msicfg_smode) {
		aplic_writel_msicfg(&aplic->msicfg_smode,
				(void *)(aplic->addr + APLIC_SMSICFGADDR),
				(void *)(aplic->addr + APLIC_SMSICFGADDRH));
	}

//	/*
//	 * Add APLIC region to the root domain if:
//	 * 1) It targets M-mode of any HART directly or via MSIs
//	 * 2) All interrupts are delegated to some child APLIC
//	 */
//	if (aplic->targets_mmode ||
//	    ((first_deleg_irq < last_deleg_irq) &&
//	    (last_deleg_irq == aplic->num_source) &&
//	    (first_deleg_irq == 1))) {
//		sbi_domain_memregion_init(aplic->addr, aplic->size,
//					  (SBI_DOMAIN_MEMREGION_MMIO |
//					   SBI_DOMAIN_MEMREGION_M_READABLE |
//					   SBI_DOMAIN_MEMREGION_M_WRITABLE),
//					  &reg);
//		rc = sbi_domain_root_add_memregion(&reg);
//		if (rc)
//			return rc;
//	}

	return 0;
}

void irqc_hw_enable_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		aplic_enable_irq(irq_ext(irq));
	else if (irq >= NR_INT_IRQS)
		imsic_enable_irq(irq_msi(irq));
	else
		aplic_hw_enable_int(irq);
}

void irqc_hw_disable_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		aplic_disable_irq(irq_ext(irq));
	else if (irq >= NR_INT_IRQS)
		imsic_enable_irq(irq_msi(irq));
	else
		aplic_hw_disable_int(irq);
}

void irqc_hw_mask_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		aplic_mask_irq(irq_ext(irq));
	else if (irq >= NR_INT_IRQS)
		imsic_mask_irq(irq_msi(irq));
	else
		aplic_hw_disable_int(irq);
}

void irqc_hw_unmask_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		aplic_unmask_irq(irq_ext(irq));
	else if (irq >= NR_INT_IRQS)
		imsic_unmask_irq(irq_msi(irq));
	else
		aplic_hw_enable_int(irq);
}

void irqc_hw_clear_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		aplic_clear_irq(irq_ext(irq));
	else if (irq >= NR_INT_IRQS)
		imsic_clear_irq(irq_msi(irq));
	else
		aplic_hw_clear_int(irq);
}

void irqc_hw_trigger_irq(irq_t irq)
{
	if (irq >= IRQ_PLATFORM)
		aplic_set_irq(irq_ext(irq));
	else if (irq >= NR_INT_IRQS)
		imsic_trigger_irq(irq_msi(irq));
	else
		aplic_hw_trigger_int(irq);
}

void irqc_hw_configure_irq(irq_t irq, uint8_t prio, uint8_t trigger)
{
#if 0
	if (irq >= IRQ_PLATFORM)
		aplic_configure_priority(irq_ext(irq),
					 prio + PLIC_PRI_MIN);
#endif
}

#ifdef CONFIG_RISCV_IRQ_VERBOSE
#define aplic_irq_completion_verbose(cpu, irq, is_ack)		\
	do {							\
		aplic_irq_completion((cpu), (irq));		\
		printf("APLIC %d %s completion.\n", (irq),	\
		       (is_ack) ? "external" : "internal");	\
	} while (0)
#else
#define aplic_irq_completion_verbose(cpu, irq, is_ack)		\
	aplic_irq_completion(cpu, irq)
#endif

#ifdef CONFIG_APLIC_COMPLETION
#ifdef CONFIG_APLIC_COMPLETION_ENTRY
#define aplic_completion_entry(cpu, irq)	\
	aplic_irq_completion_verbose(cpu, irq, false)
#define aplic_completion_exit(cpu, irq)		do { } while (0)
#endif
#ifdef CONFIG_APLIC_COMPLETION_EXIT
#define aplic_completion_entry(cpu, irq)	do { } while (0)
#define aplic_completion_exit(cpu, irq)		\
	aplic_irq_completion_verbose(cpu, irq, false)
#endif
#else
#define aplic_completion_entry(cpu, irq)	do { } while (0)
#define aplic_completion_exit(cpu, irq)		do { } while (0)

void irqc_hw_ack_irq(irq_t irq)
{
	__unused uint8_t cpu = smp_processor_id();

	aplic_irq_completion_verbose(cpu, irq_ext(irq), true);
}
#endif

void irqc_hw_handle_irq(void)
{
	irq_t irq;
	__unused uint8_t cpu = smp_processor_id();

	aplic_hw_disable_int(IRQ_EXT);
	irq = aplic_claim_irq(cpu);
	if (irq >= NR_EXT_IRQS) {
		/* Invalid IRQ */
		aplic_disable_irq(irq);
		aplic_irq_completion(cpu, irq);
	} else {
#ifdef CONFIG_RISCV_IRQ_VERBOSE
		printf("External IRQ %d\n", irq);
#endif
		aplic_completion_entry(cpu, irq);
		if (!do_IRQ(EXT_IRQ(irq))) {
			/* No IRQ handler registered, disabling... */
			aplic_disable_irq(irq);
			aplic_irq_completion(cpu, irq);
		} else
			aplic_completion_exit(cpu, irq);
	}
	aplic_hw_enable_int(IRQ_EXT);
}
