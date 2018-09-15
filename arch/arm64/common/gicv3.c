#include <target/irq.h>

void gicv3_init_gicd(void)
{
	irq_t irq;
	uint8_t ns_prio = GIC_PRIORITY_NS_HIGH(GIC_PRIORITY_MAX);

	/* Change ARE_NS=0->1 only when ENABLE_GRP1_NS=0 */
	/* Change ARE_S=0->1 only when ENABLE_GRP0/1_S/NS=0 */
	gicd_clear_ctlr(GICD_ENABLE_GRP0 |
			GICD_ENABLE_GRP1_S |
			GICD_ENABLE_GRP1_NS);
	/* Enable affinity routing */
	gicd_set_ctlr(GICD_ARE_S | GICD_ARE_NS);
	/* SPIs default to group1, NS, level triggerred */
	for (irq = IRQ_SPI_BASE; irq < NR_IRQS; irq++) {
		__raw_setl(GIC_GROUP1(irq), GICD_IGROUPR(irq));
		gicd_configure_irq(irq, ns_prio,
				   IRQ_LEVEL_TRIGGERED);
	}
	/* Always enable GRP0 and GRP1_S SPIs */
	gicd_set_ctlr(GICD_ENABLE_GRP0 | GICD_ENABLE_GRP1_S |
		      GICD_ENABLE_GRP1_NS);
}

void gicv3_init_gicd_ns(irq_t max_irq, uint8_t max_pri)
{
	/* GRP0 & GRP1 are all GRP1_NS aliases */
	if (__raw_readl(GICD_CTLR & GICD_ARE_NS))	
		gicd_set_ctlr(GICD_ENABLE_GRP1);
	else
		gicd_set_ctlr(GICD_ENABLE_GRP0);
}

void irqc_hw_begin_irq(irq_t *irq, uint8_t *cpu)
{
}

void irqc_hw_end_irq(irq_t irq, uint8_t cpu)
{
}

void gicv3_init_gicr(uint8_t cpu)
{
	irq_t irq;
	uint8_t ns_prio = GIC_PRIORITY_NS_HIGH(GIC_PRIORITY_MAX);

	gicr_disable_all_irqs(cpu);
	/* SGIs default to group1, NS, edge triggerred */
	/* PPIs default to group1, NS, level triggerred */
	__raw_writel(~0, GICR_IGROUPR0(cpu));
	for (irq = 0; irq < IRQ_SPI_BASE; irq++) {
		gicr_configure_group(cpu, irq, GIC_GROUP1NS);
		gicr_configure_irq(cpu, irq, ns_prio,
				   IRQ_LEVEL_TRIGGERED);
	}
	gicr_mark_awake(cpu);
}

void gicv3_init_gits(void)
{
}

void gicc_enable_priority_mask(uint8_t priority)
{
	write_sysreg(ICC_PMHE, ICC_CTLR_EL3);
	write_sysreg(priority, ICC_PMR_EL1);
}

void gicc_disable_priority_mask(void)
{
	write_sysreg(GIC_PRIORITY_IDLE, ICC_PMR_EL1);
	write_sysreg(read_sysreg(ICC_CTLR_EL3) & ~ICC_PMHE,
		     ICC_CTLR_EL3);
}

void gicv3_init_gicc(void)
{
	unsigned int scr;
	unsigned int icc_sre;
	uint8_t max_prio = GIC_PRIORITY_NS_HIGH(GIC_PRIORITY_MAX);

	/* Disable the legacy interrupt bypass and enable system register
	 * access for EL3 and allow lower exception levels.
	 */
	icc_sre = ICC_DIB | ICC_DFB;
	icc_sre |= (ICC_SRE_ENABLE | ICC_SRE);
	write_sysreg(read_sysreg(ICC_SRE_EL3) | icc_sre, ICC_SRE_EL3);

	/* 1. Switch to non-secure state;
	 * 2. Write non-secure ICC_SRE_EL1 and ICC_SRE_EL2 registers;
	 * 3. Switch to secure state.
	 */
	scr = read_sysreg(SCR_EL3);
	write_sysreg(scr | SCR_NS, SCR_EL3);
	isb();
	write_sysreg(read_sysreg(ICC_SRE_EL2) | icc_sre, ICC_SRE_EL2);
	write_sysreg(ICC_SRE, ICC_SRE_EL1);
	isb();
	scr |= SCR_FIQ;
	write_sysreg(scr & (~SCR_NS), SCR_EL3);
	isb();

	gicc_enable_priority_mask(max_prio);

	write_sysreg(ICC_ENABLE_GRP, ICC_IGRPEN0_EL1);
	write_sysreg(read_sysreg(ICC_IGRPEN1_EL3) |
		     ICC_ENABLE_GRP1_NS | ICC_ENABLE_GRP1_S,
		     ICC_IGRPEN1_EL3);

	/* Write the secure ICC_SRE_EL1 register */
	write_sysreg(ICC_SRE, ICC_SRE_EL1);
	isb();
}
