#include <target/irq.h>
#include <target/smp.h>
#include <target/sbi.h>

#ifndef CONFIG_ARCH_HAS_APLIC_DELEG
#define aplic_hw_deleg_num		1
static struct aplic_delegate_data aplic_hw_deleg_data[1] = {
	{0, APLIC_MAX_IRQS},
};
#endif

#ifdef CONFIG_APLIC_MSI
static void aplic_writel_msicfg(uint8_t soc, unsigned long base_addr)
{
	unsigned long base_ppn;

	if (aplic_msiaddr_locked(soc))
		return;

	base_ppn = APLIC_ADDR2PFN(base_addr);
	base_ppn &= ~APLIC_PPN_HART(APLIC_HW_MSI_LHXS);
	base_ppn &= ~APLIC_PPN_LHX(APLIC_HW_MSI_LHXW, APLIC_HW_MSI_LHXS);
	base_ppn &= ~APLIC_PPN_HHX(APLIC_HW_MSI_HHXW, APLIC_HW_MSI_HHXS);
	__raw_writel(LODWORD(base_ppn), APLIC_MSICFGADDR(soc));
	__raw_writel(APLIC_BASE_PPN_H(HIDWORD(base_ppn)) |
		     APLIC_LHXW(APLIC_HW_MSI_LHXW) |
		     APLIC_HHXW(APLIC_HW_MSI_HHXW) |
		     APLIC_LHXS(APLIC_HW_MSI_LHXS) |
		     APLIC_HHXS(APLIC_HW_MSI_HHXS),
		     APLIC_MSICFGADDRH(soc));
}

static bool aplic_check_msicfg(struct aplic_msicfg_data *msicfg)
{
	if (APLIC_LHXS_MASK < APLIC_HW_MSI_LHXS)
		return false;
	if (APLIC_LHXW_MASK < APLIC_HW_MSI_LHXW)
		return false;
	if (APLIC_HHXS_MASK < APLIC_HW_MSI_HHXS)
		return false;
	if (APLIC_HHXW_MASK < APLIC_HW_MSI_HHXW)
		return true;
	return true;
}
#else
#define aplic_writel_msicfg(cfg, addr)	do { } while (0)
#define aplic_check_msicfg(cfg)		true

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
#endif

void aplic_sbi_init_cold(uint8_t soc)
{
	uint32_t i, tmp;
//	struct sbi_domain_memregion reg;
	struct aplic_delegate_data *deleg;
	uint32_t first_deleg_irq, last_deleg_irq;
	irq_t irq;

	aplic_ctrl_disable(soc);
	aplic_disable_all_irqs(soc);

	for (irq = 1; irq <= APLIC_MAX_IRQS; irq++) {
		__raw_writel(APLIC_SM(APLIC_SM_INACTIVE),
			     APLIC_SOURCECFG(soc, irq));
		aplic_configure_wsi(irq, BOOT_HART, APLIC_IPRIO_DEF);
	}

	/* Configure IRQ delegation */
	first_deleg_irq = -1U;
	last_deleg_irq = 0;
	for (i = 0; i < aplic_hw_deleg_num; i++) {
		deleg = &aplic_hw_deleg_data[i];
		if (!deleg->first_irq || !deleg->last_irq)
			continue;
		if (APLIC_MAX_IRQS < deleg->first_irq ||
		    APLIC_MAX_IRQS < deleg->last_irq)
			continue;
		if (APLIC_CHILD_INDEX_MASK < deleg->child_index)
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
		for (irq = deleg->first_irq; irq <= deleg->last_irq; irq++)
			aplic_delegate_irq(irq, deleg->child_index);
	}

#ifdef CONFIG_APLIC_WSI
	/* Default initialization of IDC structures */
	for (i = 0; i < MAX_HARTS; i++) {
		__raw_writel(APLIC_ITHRES_ALL, APLIC_IDELIVERY(soc, i));
		__raw_writel(0, APLIC_IFORCE(soc, i));
		__raw_writel(APLIC_ITHRES_NONE, APLIC_IDELIVERY(soc, i));
	}
#endif

#if 0
	/* MSI configuration */
	aplic_writel_msicfg(soc, aplic->msicfg_mmode.base_addr);
	aplic_writel_msicfg(soc, aplic->msicfg_smode.base_addr);
#endif

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
		aplic_trigger_irq(irq_ext(irq));
	else if (irq >= NR_INT_IRQS)
		imsic_trigger_irq(irq_msi(irq));
	else
		aplic_hw_trigger_int(irq);
}

void irqc_hw_configure_irq(irq_t irq, uint8_t prio, uint8_t trigger)
{
	if (irq >= IRQ_PLATFORM)
		aplic_configure_pri(irq_ext(irq),
				    prio + APLIC_IPRIO_MIN);
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
