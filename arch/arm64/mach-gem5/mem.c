#include <target/mem.h>
#include <target/arch.h>

void mem_hw_range_init(void)
{
#ifdef CONFIG_GEM5_ROM_AS_RAM
#if (ROMEND <= RAMEND)
	mem_add(ROM_BASE, ROMEND - ROM_BASE);
#else
	mem_add(ROM_BASE, RAMEND - ROM_BASE);
#endif
	mem_reserve(__pa_symbol(SDFIRM_XIP_START),
		    __pa_symbol(SDFIRM_XIP_END) -
		    __pa_symbol(SDFIRM_XIP_START));
#endif
#if (RAM_BASE >= ROMEND)
	mem_add(RAM_BASE, RAMEND - RAM_BASE);
#elif (RAMEND > ROMEND)
	mem_add(ROMEND, RAMEND - ROMEND);
#endif
}
