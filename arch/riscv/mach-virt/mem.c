#include <target/mem.h>

void mem_hw_range_init(void)
{
	mem_add(RAM_BASE, RAMEND - RAM_BASE);
}
