#include <target/paging.h>

unsigned long empty_zero_page[PAGE_SIZE / sizeof (unsigned long)] __align(PAGE_SIZE);

void mmu_hw_ctrl_init(void)
{
	cpu_replace_ttbr1((caddr_t)mmu_pg_dir);
}
