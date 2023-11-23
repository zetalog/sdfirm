/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)heap.c: heap allocator implementation
 * $Id: heap.c,v 1.1 2019-12-18 09:40:00 zhenglv Exp $
 */

#include <target/panic.h>
#include <target/heap.h>

caddr_t __heap_brk;
caddr_t __heap_start;
#define __heap_stop	(__heap_start+CONFIG_HEAP_SIZE)

extern void heap_alloc_init(void);

caddr_t heap_sbrk(heap_offset_t increment)
{
	caddr_t _old = __heap_brk;
	caddr_t _new = _old + increment;

	if ((_new < __heap_start) || (_new > __heap_stop))
		return INVALID_HEAP_ADDR;
	
	__heap_brk = _new;
	return _old;
}

void heap_range_init(caddr_t start_addr)
{
	__heap_start = ALIGN(start_addr, HEAP_ALIGN);
}

#ifdef CONFIG_HEAP_PAGE
static int heap_page_nr(void)
{
	return ALIGN_UP(CONFIG_HEAP_SIZE, PAGE_SIZE) / PAGE_SIZE;
}

void heap_page_init(void)
{
	struct page *page;
	int nr_pages = heap_page_nr();

	page = page_alloc_pages(nr_pages);
	BUG_ON(!page);
	heap_range_init((caddr_t)page);
}
#else
#define heap_page_init()	do { } while (0)
#endif

#ifdef CONFIG_HEAP_TEST
caddr_t heap_test_addrs[13];
int heap_test_last = 0;
struct heap_chunk* heap_test_chunks[13];

void alloc_chunk(heap_size_t size, const char* name)
{
	heap_test_addrs[heap_test_last] = heap_alloc(size);
	printf("M(%s:%d): %016llx - %08lx\r\n", name, heap_test_last,
	       (unsigned long long)heap_test_addrs[heap_test_last],
	       (unsigned long)size);
	heap_test_chunks[heap_test_last] =
		(void *)heap_chunk(heap_test_addrs[heap_test_last]);
	heap_test_last++;
}

void free_chunk(int order, const char* name)
{
	int heap_test_last = order;

	printf("F(%s:%d): %016llx\r\n", name, heap_test_last,
	       (unsigned long long)heap_test_addrs[heap_test_last]);
	heap_free(heap_test_addrs[heap_test_last]);
	heap_test_addrs[heap_test_last] = 0;
}

static void heap_test1(void)
{
	alloc_chunk(0x40, "old");
	alloc_chunk(0x20, "next");
	free_chunk(0, "old");
	alloc_chunk(0x20, "new");
	free_chunk(1, "next");
	free_chunk(2, "new");

	heap_test_last = 0;
}

static void heap_test2(void)
{
	alloc_chunk(0x4020, "B");
	alloc_chunk(0x20, "S");
	alloc_chunk(0x4080, "B");
	alloc_chunk(0x20, "S");
	alloc_chunk(0x4000, "B");
	alloc_chunk(0x20, "S");
	alloc_chunk(0x4060, "B");
	alloc_chunk(0x20, "S");
	alloc_chunk(0x4040, "B");
	alloc_chunk(0x20, "S");

	free_chunk(0, "B");
	free_chunk(2, "B");
	free_chunk(4, "B");
	free_chunk(6, "B");
	free_chunk(8, "B");
	free_chunk(1, "S");
	free_chunk(3, "S");
	free_chunk(5, "S");
	free_chunk(7, "S");
	free_chunk(9, "S");

	heap_test_last = 0;
}

void heap_test(void)
{
	heap_test2();
	heap_test1();
}
#else
#define heap_test()		do { } while (0)
#endif

void heap_init(void)
{
	heap_page_init();
	__heap_brk = __heap_start;
	heap_alloc_init();

	heap_test();
}
