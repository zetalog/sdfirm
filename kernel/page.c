#include <stdio.h>
#include <string.h>
#include <target/arch.h>
#include <target/cmdline.h>

struct page *page_empty = NULL;
struct page **page_free_list = &page_empty;

void page_free(caddr_t address)
{
	struct page *page = \
		(struct page *)ALIGN((uintptr_t)address, PAGE_SIZE-1);

	page->next = *page_free_list;
	*page_free_list = page;
}

caddr_t page_alloc(void)
{
	struct page *page;

	page = *page_free_list;
	*page_free_list = page->next;
	return (caddr_t)page;
}

caddr_t page_alloc_zeroed(void)
{
	caddr_t page = page_alloc();

	if (page)
		memory_set(page, 0, PAGE_SIZE);
	return page;
}

void page_alloc_init(caddr_t base, caddr_t size)
{
	pfn_t pfn, pfn_start = PFN_UP(base);
	pfn_t nr_pages = PFN_DOWN(base + size) - pfn_start;
	struct page **last_page, *page;

	/* The lower pages are used for early page table allocation. */
	last_page = page_free_list;
	for (pfn = 0; pfn < nr_pages; pfn++) {
		page = pfn_to_page(pfn + pfn_start);
		page->next = NULL;
		*last_page = page;
		last_page = &page->next;
		printf("Free Page: %016llx\n", (uint64_t)page);
	}
}

void page_init(void)
{
#if !defined(CONFIG_MMU) || defined(CONFIG_MMU_MAP_MEM)
	mem_free_all();
#endif
}

static int do_page(int argc, char **argv)
{
	struct page *page;
	caddr_t addr;
	int nr_pages;

	printf("Allocating page...\n");
	addr = page_alloc();
	printf("alloc = %016llx\n", addr);
	printf("Freeing page...\n");
	page_free(addr);

	if (argc > 1) {
		nr_pages = strtoul(argv[1], 0, 0);
		for (page = *page_free_list;
		     nr_pages-- && page != NULL; page = page->next)
			printf("%d - %016llx\n",
			       page_to_pfn(page), (uintptr_t)page);
	}
	return 0;
}

DEFINE_COMMAND(page, do_page, "Display free pages",
	"page [N]"
	"    -test page allocator and display first N free pages\n"
	"\n"
);
