#ifndef __PAGING_H_INCLUDE__
#define __PAGING_H_INCLUDE__

#include <target/page.h>

#ifdef CONFIG_MMU_2L_TABLE
#define PGTABLE_LEVEL	2
#endif
#ifdef CONFIG_MMU_3L_TABLE
#define PGTABLE_LEVEL	3
#endif
#ifdef CONFIG_MMU_4L_TABLE
#define PGTABLE_LEVEL	4
#endif

#ifdef CONFIG_ARCH_HAS_MMU
#include <driver/mmu.h>
#endif

#endif /* __PAGING_H_INCLUDE__ */
