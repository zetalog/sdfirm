#include <target/arch.h>
#include <target/cmdline.h>
#include <target/spinlock.h>

LIST_HEAD(page_free_list);
DEFINE_SPINLOCK(page_lock);

void page_insert(struct page *page)
{
	irq_flags_t flags;
	struct page *curr, *next, *tmp;

	INIT_LIST_HEAD(&page->link);
	spin_lock_irqsave(&page_lock, flags);
	list_for_each_entry_safe(struct page, curr, next,
				 &page_free_list, link) {
again:
		if ((phys_addr_t)page == curr->end) {
			curr->end += page->end - (phys_addr_t)page;
			page = list_entry(&next->link, struct page, link);
			if (curr->end == (phys_addr_t)page) {
				list_del_init(&page->link);
				goto again;
			}
			goto exit_lock;
		}
		if (curr > page) {
			list_add_tail(&page->link, &curr->link);
			if ((phys_addr_t)curr == page->end) {
				tmp = curr;
				curr = page;
				page = tmp;
				list_del_init(&page->link);
				goto again;
			}
			goto exit_lock;
		}
	}
	list_add_tail(&page->link, &page_free_list);
exit_lock:
	spin_unlock_irqrestore(&page_lock, flags);
}

int page_nr(struct page *page)
{
	return (int)((page->end - (phys_addr_t)page) >> PAGE_SHIFT);
}

struct page *page_offset(struct page *page, int nr_pages)
{
	phys_addr_t address;

	address = (phys_addr_t)page +
		  ((phys_addr_t)nr_pages << PAGE_SHIFT);
	return (struct page *)address;
}

struct page *page_alloc_pages(int nr_pages)
{
	struct page *pos;
	struct page *page;
	int nr;
	irq_flags_t flags;

	spin_lock_irqsave(&page_lock, flags);
	list_for_each_entry(struct page, pos, &page_free_list, link) {
		nr = page_nr(pos);
		if (nr == nr_pages) {
			list_del(&pos->link);
			spin_unlock_irqrestore(&page_lock, flags);
			return pos;
		}
		if (nr > nr_pages) {
			page = page_offset(pos, nr_pages);
			page->end = pos->end;
			list_del(&pos->link);
			spin_unlock_irqrestore(&page_lock, flags);
			page_insert(page);
			return pos;
		}
	}
	spin_unlock_irqrestore(&page_lock, flags);
	return NULL;
}

void page_free_pages(struct page *page, int nr_pages)
{
	page->end = (phys_addr_t)page_offset(page, nr_pages);
	page_insert(page);
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
	struct page *page;

	page = (struct page *)ALIGN_UP(base, PAGE_SIZE);
	page->end = ALIGN_DOWN(base + size, PAGE_SIZE);
	page_insert(page);
}

void page_init(void)
{
	mem_free_all();
}

struct page *page_test_ptr;
int page_test_num;

static int do_page_free(int argc, char **argv)
{
	int nr_pages = page_test_num;

	if (!page_test_ptr)
		return -EINVAL;

	if (argc > 2)
		nr_pages = strtoul(argv[2], 0, 0);
	if (nr_pages > page_test_num)
		nr_pages = page_test_num;

	printf("Freeing page...\n");
	page_free_pages(page_test_ptr, nr_pages);
	if (nr_pages < page_test_num) {
		page_test_ptr = page_offset(page_test_ptr, nr_pages);
		printf("alloc = %016llx\n", page_test_ptr);
	} else
		page_test_ptr = NULL;
	page_test_num -= nr_pages;
	return 0;
}

static int do_page_alloc(int argc, char **argv)
{
	int nr_pages = 1;

	if (argc > 2)
		nr_pages = strtoul(argv[2], 0, 0);

	if (page_test_ptr) {
		printf("Freeing page...\n");
		page_free_pages(page_test_ptr, page_test_num);
	}
	printf("Allocating page...\n");
	page_test_ptr = page_alloc_pages(nr_pages);
	page_test_num = nr_pages;
	printf("alloc = %016llx\n", page_test_ptr);
	return 0;
}

static int do_page_dump(int argc, char **argv)
{
	struct page *page;

	list_for_each_entry(struct page, page,
			    &page_free_list, link) {
		printf("%016llx - %016llx\n",
		       (phys_addr_t)page, page->end);
	}
	return 0;
}

static int do_page(int argc, char **argv)
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0)
		return do_page_dump(argc, argv);
	if (strcmp(argv[1], "alloc") == 0)
		return do_page_alloc(argc, argv);
	if (strcmp(argv[1], "free") == 0)
		return do_page_free(argc, argv);
	return -EINVAL;
}
DEFINE_COMMAND(page, do_page, "Display free pages",
	"page dump\n"
	"    -dump free page information\n"
	"page alloc [N]\n"
	"    -test page alloc\n"
	"page free [N]\n"
	"    -test page free\n"
);
