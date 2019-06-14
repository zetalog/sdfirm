#include <target/irq.h>

void irqc_hw_ctrl_init(void)
{
	gicv2_init_gicd();

	/* Enable CPU interface */
	gicv2_init_gicc();
}
