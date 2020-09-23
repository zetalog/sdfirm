#include <target/generic.h>
#include <target/arch.h>
#include <target/irq.h>
#include <target/heap.h>

extern caddr_t __bss_stop[];

void mem_init(void)
{
	heap_range_init((caddr_t)__bss_stop);
}

void board_early_init(void)
{
	mem_init();

	DEVICE_ARCH(DEVICE_ARCH_ARM);
}

void board_late_init(void)
{
}

void board_smp_init(void)
{
}
