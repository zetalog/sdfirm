#ifndef __PAGING_H_INCLUDE__
#define __PAGING_H_INCLUDE__

#include <target/page.h>

#ifdef CONFIG_ARCH_HAS_MMU
#include <driver/mmu.h>
extern pgd_t mmu_pg_dir[PTRS_PER_PGD];
#endif

#endif /* __PAGING_H_INCLUDE__ */
