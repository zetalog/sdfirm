#include <target/irq.h>

static void gicd_wait_rwp(void)
{
	while (__raw_readl(GICD_CTLR) & GICD_RWP);
}

static void gicd_clear_ctlr(uint32_t value)
{
	__raw_clearl(value, GICD_CTLR);
	gicd_wait_rwp();
}

static void gicd_set_ctlr(uint32_t value)
{
	__raw_setl(value, GICD_CTLR);
	gicd_wait_rwp();
}

void gicd_set_spi(irq_t irq, boolean secure)
{
	if (secure)
		__raw_writel(GICD_INTID(irq), GICD_SETSPI_SR);
	else
		__raw_writel(GICD_INTID(irq), GICD_SETSPI_NSR);
}

void gicd_clear_spi(irq_t irq, boolean secure)
{
	if (secure)
		__raw_writel(GICD_INTID(irq), GICD_CLRSPI_SR);
	else
		__raw_writel(GICD_INTID(irq), GICD_CLRSPI_NSR);
}

void gicv3_init_gicd(void)
{
	irq_t irq;
	uint8_t ns_prio = GIC_PRIORITY_NS_HIGH(GIC_PRIORITY_MAX);
	uint32_t cfg = GICD_ENABLE_GRP0 | GICD_ENABLE_GRP1_S |
		       GICD_ENABLE_GRP1_NS;

	gicd_clear_ctlr(cfg);
	gicd_set_ctlr(GICD_ARE_S | GICD_ARE_NS);
	for (irq = IRQ_SPI_BASE; irq <= GIC_PRIORITY_MAX; irq++) {
		__raw_setl(GIC_PRIORITY(irq, ns_prio),
			   GICD_IPRIORITYR(irq));
		/* SPIs default to group0, level triggerred */
		__raw_clearl(GIC_GROUP1(irq), GICD_IGROUPR(irq));
		__raw_setl(GIC_TRIGGER(GIC_TRIGGER_LEVEL),
			   GICD_ICFGR(irq));
	}
	cfg = gicv3_hw_init_spis();
	if (cfg)
		gicd_set_ctlr(cfg);
}

static void gicr_wait_rwp(uint8_t cpu)
{
	while (__raw_readl(GICR_CTLR(cpu)) & GICR_RWP);
}

void gicr_clear_ctlr(uint8_t cpu, uint32_t value)
{
	__raw_clearl(value, GICR_CTLR(cpu));
	gicr_wait_rwp(cpu);
}

void gicr_set_ctlr(uint8_t cpu, uint32_t value)
{
	__raw_setl(value, GICR_CTLR(cpu));
	gicr_wait_rwp(cpu);
}

static boolean gicr_is_asleep(uint8_t cpu)
{
	return !!(__raw_readl(GICR_WAKER(cpu)) & GICR_CHILDREN_ASLEEP);
}

void gicr_enable_sgi(uint8_t cpu, uint8_t irq)
{
	__raw_setl(GIC_INTERRUPT_ID(irq), GICR_ISENABLER0(cpu));
	gicr_wait_rwp(cpu);
}

void gicr_disable_sgi(uint8_t cpu, uint8_t irq)
{
	__raw_setl(GIC_INTERRUPT_ID(irq), GICR_ICENABLER0(cpu));
	gicr_wait_rwp(cpu);
}

void gicr_enable_all_sgis(uint8_t cpu)
{
	__raw_setl(~0, GICR_ISENABLER0(cpu));
	gicr_wait_rwp(cpu);
}

void gicr_disable_all_sgis(uint8_t cpu)
{
	__raw_setl(~0, GICR_ICENABLER0(cpu));
	gicr_wait_rwp(cpu);
}

static void gicr_mark_awake(uint8_t cpu)
{
	if (gicr_is_asleep(cpu)) {
		__raw_clearl(GICR_PROCESSOR_SLEEP, GICR_WAKER(cpu));
		while (gicr_is_asleep(cpu));
	}
}

#if 0
static void gicr_mark_asleep(uint8_t cpu)
{
	__raw_setl(GICR_PROCESSOR_SLEEP, GICR_WAKER(cpu));
	while (!gicr_is_asleep(cpu));
}
#endif

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

	gicr_disable_all_sgis(cpu);
	/* SPIs default to group1, NS */
	/* PPIs default to group1, NS, level triggerred */
	__raw_writel(~0, GICR_IGROUPR0(cpu));
	for (irq = 0; irq < IRQ_SPI_BASE; irq++) {
		__raw_setl(GIC_PRIORITY(irq, ns_prio),
			   GICR_IPRIORITYR(cpu, irq));
		if (irq < IRQ_PPI_BASE)
			continue;
		__raw_setl(GIC_TRIGGER(GIC_TRIGGER_LEVEL),
			   GICR_ICFGR(cpu, irq));
	}
	gicr_mark_awake(cpu);
}

void gicv3_init_gits(void)
{
}

void gicv3_init_gicc(uint8_t cpu)
{
}
