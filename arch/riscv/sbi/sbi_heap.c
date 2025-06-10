#include <target/sbi.h>
#include <target/mem.h>
#include <target/heap.h>
#include <target/paging.h>

#define RESERVED_HEAP		8192

int sbi_heap_init(struct sbi_scratch *scratch)
{
        phys_addr_t pa_start = round_down(__pa(SDFIRM_START), PAGE_SIZE);
        phys_addr_t pa_end = round_up(__pa(SDFIRM_END), PAGE_SIZE) + RESERVED_HEAP;

        mem_hw_range_init();
        mem_reserve(pa_start, pa_end - pa_start);
        mem_reserve(pa_end, round_up(CONFIG_HEAP_SIZE, PAGE_SIZE));

	heap_range_init(pa_end);
	heap_init();
	return SBI_OK;
}
