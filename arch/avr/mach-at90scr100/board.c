#include <target/generic.h>
#include <target/arch.h>
#include <target/wdt.h>
#include <target/irq.h>

void clk_hw_set_cpu(void)
{
	irq_flags_t flags;

	irq_local_save(flags);
	__clk_hw_set_prescale(CLK_CLKPR);
	irq_local_restore(flags);
}

static void clk_init(void)
{
	clk_hw_set_cpu();
}

void board_early_init(void)
{
	clk_init();
	wdt_ctrl_stop();

	DEVICE_ARCH(DEVICE_ARCH_AVR);
}

void board_late_init(void)
{
}
