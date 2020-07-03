#include <target/irq.h>

#ifdef CONFIG_MMU
void irqc_hw_mmu_init(void)
{
	set_fixmap_io(FIX_GICD, __GICD_BASE & PAGE_MASK);
	set_fixmap_io(FIX_GICC, __GICC_BASE & PAGE_MASK);
}
#endif

void irqc_hw_ctrl_init(void)
{
	gicv2_init_gicd();

	/* Enable CPU interface */
	gicv2_init_gicc();
}

#ifdef CONFIG_SMP
void irqc_hw_smp_init(void)
{
	/* Enable CPU interface */
	gicv2_init_gicc();
}
#endif
