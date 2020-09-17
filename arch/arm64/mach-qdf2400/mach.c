#include <target/generic.h>
#include <target/cpus.h>
#include <target/arch.h>
#include <target/irq.h>
#include <target/clk.h>
#include <target/heap.h>

extern caddr_t __bss_stop[];

#define cpu_init()	clk_hw_set_config(CLK_CONFIG)

void board_early_init(void)
{
	qdf2400_gblct_init();
	DEVICE_ARCH(DEVICE_ARCH_ARM);
}

void board_late_init(void)
{
}

void board_smp_init(void)
{
}
