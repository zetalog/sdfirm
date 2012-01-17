#include <target/heap.h>
#include <target/string.h>

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

void heap_init(void)
{
	__heap_brk = __heap_start;
	heap_alloc_init();
}
