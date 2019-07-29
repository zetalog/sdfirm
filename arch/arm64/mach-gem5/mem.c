#include <target/mem.h>
#include <target/arch.h>

void mem_hw_range_init(void)
{
#ifdef CONFIG_GEM5_ROM_AS_RAM
	mem_add(ROM_BASE, ROMEND - ROM_BASE);
#endif
#if (RAM_BASE >= ROMEND)
	mem_add(RAM_BASE, RAMEND - RAM_BASE);
#else
	mem_add(ROMEND, RAMEND - ROMEND);
#endif
}
