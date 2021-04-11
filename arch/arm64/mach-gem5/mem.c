#include <target/mem.h>
#include <target/arch.h>

#ifdef CONFIG_GEM5_STATIC_PAGES
extern uint64_t *simpoint_pages_start;
extern uint64_t *simpoint_pages_end;
#endif

void mem_hw_range_init(void)
{
#ifdef CONFIG_GEM5_ROM_AS_RAM
#if (ROMEND <= RAMEND)
	mem_add(ROM_BASE, ROMEND - ROM_BASE);
#else
	mem_add(ROM_BASE, RAMEND - ROM_BASE);
#endif
#endif /* CONFIG_GEM5_ROM_AS_RAM */

#if (RAM_BASE >= ROMEND)
	mem_add(RAM_BASE, RAMEND - RAM_BASE);
#elif (RAMEND > ROMEND)
	mem_add(ROMEND, RAMEND - ROMEND);
#endif

#ifdef CONFIG_GEM5_STATIC_PAGES
	con_log("Reserved Simpoint pages: start = %p, end= %p\n",
		simpoint_pages_start,
		(simpoint_pages_end - simpoint_pages_start));
	mem_reserve((phys_addr_t)simpoint_pages_start,
				(phys_addr_t)(simpoint_pages_end - simpoint_pages_start));
#endif
}
