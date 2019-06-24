#include <target/mem.h>
#include <target/arch.h>

void mem_hw_range_init(void)
{
	mem_add(RAM_BASE, RAMEND);
}
