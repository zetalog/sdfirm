#include <target/irq.h>

void irqc_hw_ctrl_init(void)
{
	gicv2_init_gicd();

	/* Enable CPU interface */
	__raw_clearl(GICC_ENABLE_NS, GICC_CTLR);
	gicv2_init_gicc();
	__raw_setl(GICC_ENABLE_NS, GICC_CTLR);

	/* Allow NS access to GICD_CGCR via GICD_ANSACR */
	__raw_writel(GICD_GICD_CGCR, GICD_ANSACR);
}
