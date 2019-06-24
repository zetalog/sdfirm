#include <target/generic.h>
#include <target/cpus.h>
#include <target/arch.h>
#include <target/irq.h>
#include <target/clk.h>
#include <target/heap.h>

void board_init(void)
{
	DEVICE_ARCH(DEVICE_ARCH_ARM);
}
