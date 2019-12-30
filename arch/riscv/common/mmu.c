#include <target/paging.h>

pgd_t mmu_id_map[IDMAP_DIR_SIZE / sizeof (pgd_t)] __page_aligned_bss;

void mmu_hw_ctrl_init(void)
{
}
