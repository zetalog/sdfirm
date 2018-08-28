#include <target/irq.h>

uint32_t gicv3_hw_init_spis(void)
{
	return 0;
}

void irqc_hw_ctrl_init(void)
{
	uint8_t cpu = 0;

	gicv3_init_gicd();
	gicv3_init_gits();
	gicv3_init_gicr(cpu);
	gicv3_init_gicc();
}
