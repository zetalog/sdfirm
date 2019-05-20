#include <target/page.h>

struct page **free_page_list;

void page_free(caddr_t address)
{
	struct page *page = \
		(struct page *)ALIGN((uintptr_t)address, PAGE_SIZE-1);

	page->next = *free_page_list;
	*free_page_list = page;
}

caddr_t page_alloc(void)
{
	struct page *page;

	page = *free_page_list;
	*free_page_list = page->next;
	return (caddr_t)page;
}

void page_init(caddr_t base, int nr_pages)
{
	int pfn, pfn_start = page_to_pfn(base);
	struct page **last_page, *page;

	last_page = free_page_list;
	for (pfn = 0; pfn < nr_pages; pfn++) {
		page = pfn_to_page(pfn + pfn_start);
		page->next = NULL;
		*last_page = page;
		last_page = &page->next;
	}
}
