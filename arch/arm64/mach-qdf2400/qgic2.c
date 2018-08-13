#include <target/irq.h>

void irqc_hw_ctrl_init(void)
{
	irq_t irq;

	gicv2_init_gicd();
	gicv2_init_gicc();
	/* Enable CPU interface */
	__raw_setl(GICC_ENABLE_GRP1_NS, GICC_CTLR);
	for (irq = 0; irq < NR_IRQS; irq += 32)
		__raw_writel(0xFFFFFFFF, GICD_ISR(irq));
}
