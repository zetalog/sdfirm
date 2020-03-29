#ifndef __PAGE_H_INCLUDE__
#define __PAGE_H_INCLUDE__

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

#ifdef CONFIG_MMU_DEBUG_TABLE
#define mmu_dbg_tbl		printf
#else
#define mmu_dbg_tbl(fmt, ...)	do { } while (0)
#endif

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

#if PAGE_PTR_BITS == 3
#define PTR_VAL_ONE		_AC(1, ULL)
#define PTR_VAL_ZERO		_AC(0, ULL)
#elif PAGE_PTR_BITS == 2
#define PTR_VAL_ONE		_AC(1, UL)
#define PTR_VAL_ZERO		_AC(0, UL)
#endif

#define PGDIR_BYTES		(PGTABLE_LEVELS * PAGE_SIZE)

#define PAGE_PTR_BYTES		(1 << PAGE_PTR_BITS)

#ifndef __ASSEMBLY__
#if PAGE_SIZE < 0x100
typedef uint8_t page_size_t;
#elif PAGE_SIZE < 0x10000
typedef uint16_t page_size_t;
#elif PAGE_SIZE < 0x100000000
typedef uint32_t page_size_t;
#else
typedef uint64_t page_size_t;
#endif

#define pfn_to_page(pfn)	\
	((struct page *)((caddr_t)(((pfn_t)(pfn)) << PAGE_SHIFT)))
#define page_to_pfn(page)	\
	(((pfn_t)((caddr_t)(page)) >> PAGE_SHIFT))

#define PFN_ALIGN(x)	(((pfn_t)(x) + (PAGE_SIZE - 1)) & PAGE_MASK)
#define PFN_UP(x)	(((x) + PAGE_SIZE-1) >> PAGE_SHIFT)
#define PFN_DOWN(x)	((x) >> PAGE_SHIFT)
#define PFN_PHYS(x)	((phys_addr_t)(x) << PAGE_SHIFT)
#define PHYS_PFN(x)	((pfn_t)((x) >> PAGE_SHIFT))

struct page {
	struct list_head link;
	phys_addr_t end;
};

#define ptr_to_phys(x)		((phys_addr_t)((uintptr_t)(x)))
#define ptr_to_virt(x)		((caddr_t)((uintptr_t)(x)))
#define phys_to_ptr(x)		((void *)((uintptr_t)(x)))
#define virt_to_ptr(x)		((void *)((uintptr_t)(x)))

#ifdef CONFIG_PAGE
caddr_t page_alloc_zeroed(void);
struct page *page_alloc_pages(int nr_pages);
void page_free_pages(struct page *page, int nr_pages);
void page_alloc_init(caddr_t base, caddr_t size);
void page_init(void);
#else
#define page_init()				do { } while (0)
#define page_alloc_init(base, nr)		do { } while (0)
#define page_alloc_pages(nr_pages)		NULL
#define page_free_pages(address, nr_pages)	do { } while (0)
#endif
#define page_alloc()				(caddr_t)page_alloc_pages(1)
#define page_free(address)						\
	page_free_pages((struct page *)ALIGN_DOWN((uintptr_t)address,	\
						  PAGE_SIZE), 1)
#endif /* __ASSEMBLY__ */

#endif /* __PAGE_H_INCLUDE__ */
