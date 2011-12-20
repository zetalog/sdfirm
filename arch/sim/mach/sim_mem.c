#include "mach.h"
#include <target/heap.h>
#include <target/task.h>

caddr_t sim_mem;

void sim_mem_init(void)
{
	size_t mem_size = CONFIG_HEAP_SIZE+HEAP_ALIGN;
	sim_mem = (caddr_t)calloc(1, mem_size);
	heap_range_init(sim_mem);
}
