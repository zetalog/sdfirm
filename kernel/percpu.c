/*
 * Author: Lv Zheng <lv.zheng@hxt-semitech.com>
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <target/heap.h>
#include <target/percpu.h>
#include <target/heap.h>
#include <target/spinlock.h>
#include <target/linpack.h>
#include <target/dhrystone.h>

#if defined(CONFIG_SMP) && !(CONFIG_PERCPU_INTERLEAVE)
uint64_t __percpu_offset[NR_CPUS + NR_EXTRA_CPU] __always_cache;

#ifdef CONFIG_PERCPU_PATTERN
/* Deprecated feature, currently, patterns will dynamically allocate
 * required memory from heap when being dispatched.
 */
union percpu_area {
	struct linpack_context linpack;
	struct dhrystone_context dhrystone;
} __aligned(SMP_CACHE_BYTES);

uint64_t percpu_free_area;

caddr_t percpu_get_free_area(uint8_t cpu)
{
	return (caddr_t)(percpu_free_area +
			 cpu * sizeof (union percpu_area));
}

int percpu_init_free_area(void)
{
	void *ptr;

	ptr = heap_aligned_alloc(SMP_CACHE_BYTES,
				 sizeof (union percpu_area) * NR_CPUS);
	if (!ptr) {
		printf("Failed to allocate pattern area\n");
		return -1;
	}
	percpu_free_area = (uint64_t)ptr;
	return 0;
}
#else
caddr_t percpu_get_free_area(uint8_t cpu)
{
	return 0;
}

static int inline percpu_init_free_area(void)
{
	return 0;
}
#endif

int percpu_init(void)
{
	int ret;
	size_t size, i;
	void *ptr;

	if (smp_processor_id() != cpus_boot_cpu)
		return 0;

	size = PERCPU_END - PERCPU_START;
	ptr = heap_aligned_alloc(SMP_CACHE_BYTES,
				 size * NR_CPUS);
	printf("alloc percpu: %016llx(%d)\n",
	       (uint64_t)ptr, (int)(size * NR_CPUS));
	for (i = 0; i < NR_CPUS; i++, ptr += size) {
		__percpu_offset[i] = ((uint64_t)ptr) - PERCPU_START;
		memcpy(ptr, (void *)PERCPU_START, size);
	}
	ret = percpu_init_free_area();
	return ret;
}
#else
caddr_t percpu_get_free_area(uint8_t cpu)
{
	return 0;
}

int percpu_init(void)
{
	return 0;
}
#endif
