#ifndef __PAGE_H_INCLUDE__
#define __PAGE_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#if defined(CONFIG_MMU_64K_PAGE)
#define PAGE_SHIFT		16
#elif defined(CONFIG_MMU_16K_PAGE)
#define PAGE_SHIFT		14
#elif defined(CONFIG_MMU_4K_PAGE)
#define PAGE_SHIFT		12
#else
#define PAGE_SHIFT		12
#endif
#define PAGE_SIZE		(1 << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))

#if defined(CONFIG_MMU_4L_TABLE)
#define PGTABLE_LEVELS		4
#elif defined(CONFIG_MMU_3L_TABLE)
#define PGTABLE_LEVELS		3
#elif defined(CONFIG_MMU_2L_TABLE)
#define PGTABLE_LEVELS		2
#else
#define PGTABLE_LEVELS		2
#endif
#define __PGTABLE_LEVELS(va_bits)	\
	(((va_bits) - PAGE_PTR_BITS - 1) / (PAGE_SHIFT - PAGE_PTR_BITS))

#include <asm/page.h>

#ifndef PAGE_PTR_BITS
#define PAGE_PTR_BITS		BITS_PER_LONG
#endif
#ifndef PAGE_PTE_BITS
#define PAGE_PTE_BITS		PAGE_SHIFT
#endif
#ifndef PAGE_PXD_BITS
#define PAGE_PXD_BITS		(PAGE_SHIFT - PAGE_PTR_BITS)
#endif
#define PAGE_MAX_TABLE_ENTRIES	(1 << PAGE_PXD_BITS)

#define PGDIR_BYTES		(PGTABLE_LEVELS * PAGE_SIZE)

#ifndef __ASSEMBLY__
#define pfn_to_page(pfn)	\
	((struct page *)(((pfn_t)(pfn)) << PAGE_SHIFT))
#define page_to_pfn(page)	\
	(((pfn_t)(page) >> PAGE_SHIFT))

struct page {
	struct page *next;
};

#ifdef CONFIG_PAGE
caddr_t page_alloc(void);
caddr_t page_alloc_zeroed(void);
void page_free(caddr_t address);
void page_early_init(void);
void page_late_init(void);
#else
#define page_early_init()	do { } while (0)
#define page_late_init()	do { } while (0)
#endif
#endif /* __ASSEMBLY__ */

#endif /* __PAGE_H_INCLUDE__ */
