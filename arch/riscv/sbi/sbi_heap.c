#include <target/sbi.h>
#include <target/mem.h>
#include <target/heap.h>
#include <target/paging.h>

extern uintptr_t __sbi_heap_start[];
extern uintptr_t __sbi_heap_end[];

int sbi_heap_init(struct sbi_scratch *scratch)
{
        phys_addr_t pa_start = round_down(__pa(SDFIRM_START), PAGE_SIZE);
        phys_addr_t pa_end = round_up(__pa(SDFIRM_END), PAGE_SIZE);

        mem_hw_range_init();
        mem_reserve(pa_start, pa_end - pa_start);

	heap_range_init((caddr_t)__sbi_heap_start);
	heap_init();
	return SBI_OK;
}
