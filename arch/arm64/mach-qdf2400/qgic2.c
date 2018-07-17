#include <target/irq.h>

void gic_hw_ctrl_init(void)
{
	irq_t irq;

	/* Enable distributor */
	__raw_writel(GICD_CTLR_ENABLE_NS | GICD_CTLR_ENABLE, GICD_CTLR);
	/* Enable CPU interface */
	__raw_writel(GICC_CTLR_ENABLE_NS | GICC_CTLR_ENABLE, GICC_CTLR);
	/* Set priority mask to the lowest to allow all interrupts. */
	__raw_writel(GICC_LEVEL(0x1F), GICC_PMR);
	/* Set the binary point register to indicate that every priority
	 * level is it's own priority group. See table 3-2 in the ARM GIC
	 * specification.
	 */
	__raw_writel(GICC_BINARY_POINT(2), GICC_BPR);

	/* Disable all IRQs */
	for(irq = 0; irq < NR_IRQS; irq += 32) {
		__raw_writel(0xFFFFFFFF, GICD_ISR(irq));
		__raw_writel(0xFFFFFFFF, GICD_ICENABLER(irq));
	}
}
