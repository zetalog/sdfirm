#include <stdio.h>
#include <string.h>
#include <target/arch.h>
#include <target/cmdline.h>

struct page *page_empty = NULL;
struct page **page_free_list = &page_empty;
bool page_early = true;

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

void page_late_init(void)
{
	/* TODO: convert page allocator to VA based */
	page_early = false;
}

void page_alloc_init(caddr_t base, pfn_t nr_pages)
{
	pfn_t pfn, pfn_start = page_to_pfn(base);
	struct page **last_page, *page;

	/* The lower pages are used for early page table allocation. */
	last_page = page_free_list;
	for (pfn = 0; pfn < nr_pages; pfn++) {
		page = pfn_to_page(pfn + pfn_start);
		page->next = NULL;
		*last_page = page;
		last_page = &page->next;
	}
}

static int do_page(int argc, char **argv)
{
	struct page *page;
	caddr_t addr;
	int nr_pages;

	printf("Allocating page...\n");
	addr = page_alloc();
	printf("alloc = %016llx", addr);
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
	"    -test page allocator and display first N free pages\n"
	"help command ...\n"
	"     page [N]"
	"\n"
);

void page_early_init(void)
{
	page_alloc_init(PAGEABLE_START,
			(PAGEABLE_END - PAGEABLE_START) / PAGE_SIZE);
}
