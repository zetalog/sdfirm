#ifndef __PAGE_H_INCLUDE__
#define __PAGE_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/page.h>

#ifdef CONFIG_MMU_4K_PAGE
#define PAGE_SHIFT		12
#endif
#ifdef CONFIG_MMU_16K_PAGE
#define PAGE_SHIFT		14
#endif
#ifdef CONFIG_MMU_64K_PAGE
#define PAGE_SHIFT		16
#endif
#define PAGE_SIZE		(1 << PAGE_SHIFT)

#ifdef CONFIG_MMU_2L_TABLE
#define PGTABLE_LEVELS		2
#endif
#ifdef CONFIG_MMU_3L_TABLE
#define PGTABLE_LEVELS		3
#endif
#ifdef CONFIG_MMU_4L_TABLE
#define PGTABLE_LEVELS		4
#endif

#define PGDIR_BYTES		(PGTABLE_LEVELS * PAGE_SIZE)

#ifndef __ASSEMBLY__
#define pfn_to_page(pfn)	\
	((struct page *)(((uintptr_t)(pfn)) << PAGE_SHIFT))
#define page_to_pfn(page)	\
	((int)((uintptr_t)(page) >> PAGE_SHIFT))

struct page {
	struct page *next;
};

caddr_t page_alloc(void);
void page_free(caddr_t address);
void page_init(caddr_t base, int nr_pages);
#endif /* __ASSEMBLY__ */

#endif /* __PAGE_H_INCLUDE__ */
