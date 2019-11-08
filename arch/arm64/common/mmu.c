#include <target/linkage.h>
#include <target/paging.h>

unsigned long empty_zero_page[PAGE_SIZE / sizeof (unsigned long)] __page_aligned_bss;
pgd_t mmu_id_map[IDMAP_DIR_SIZE / sizeof (pgd_t)] __page_aligned_bss;

void mmu_hw_ctrl_init(void)
{
	cpu_replace_ttbr1((caddr_t)mmu_pg_dir);
}
