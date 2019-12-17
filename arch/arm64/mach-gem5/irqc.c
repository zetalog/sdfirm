#include <target/irq.h>

void irqc_hw_ctrl_init(void)
{
	set_fixmap_io(FIX_GICD, __GICD_BASE & PAGE_MASK);
	set_fixmap_io(FIX_GICC, __GICC_BASE & PAGE_MASK);

	gicv2_init_gicd();

	/* Enable CPU interface */
	gicv2_init_gicc();
}

void irqc_hw_smp_init(void)
{
	/* Enable CPU interface */
	gicv2_init_gicc();
}
