#include <target/paging.h>

void paging_init(void)
{
	mmu_hw_ctrl_init();
}
