#include <target/generic.h>
#include <target/cpus.h>
#include <target/arch.h>
#include <target/irq.h>
#include <target/clk.h>
#include <target/heap.h>

extern caddr_t __bss_stop[];

#define cpu_init()	clk_hw_set_config(CLK_CONFIG)

void board_init(void)
{
	qdf2400_gblct_init();
	heap_range_init((caddr_t)__bss_stop);

	DEVICE_ARCH(DEVICE_ARCH_ARM);
}
