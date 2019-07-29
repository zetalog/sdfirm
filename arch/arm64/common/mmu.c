#include <target/paging.h>

void mmu_hw_ctrl_init(void)
{
#if 0
	cpu_replace_ttbr1(__va(pgd_phys));
	memcpy(mmu_pg_dir, pgdp, PAGE_SIZE);
	cpu_replace_ttbr1(mmu_pg_dir);
#endif
}
