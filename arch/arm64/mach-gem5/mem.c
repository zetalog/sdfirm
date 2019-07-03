#include <target/mem.h>
#include <target/arch.h>

void mem_hw_range_init(void)
{
#ifdef CONFIG_GEM5_ROM_AS_RAM
	mem_add(ROM_BASE, ROMEND);
#endif
#if RAM_BASE != ROM_BASE
	mem_add(RAM_BASE, RAMEND);
#endif
}
