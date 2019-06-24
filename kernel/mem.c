#include <string.h>
#include <limits.h>
#include <target/mem.h>
#include <target/arch.h>
#include <target/heap.h>
#include <target/paging.h>

#ifdef CONFIG_MEM
#ifdef CONFIG_MEM_DEBUG
#define mem_dbg			printf
#else
#define mem_dbg(fmt, ...)
#endif

#define MEM_ALLOC_ANYWHERE		(~(phys_addr_t)0)
#define MEM_ALLOC_ACCESSIBLE		0

static struct mem_region mem_init_free[NR_MEM_REGIONS];
static struct mem_region mem_init_alloc[NR_MEM_REGIONS];

static struct mem_type mem_free_regions = {
	.regions = mem_init_free,
	.cnt = 1,	/* empty dummy entry */
	.max = NR_MEM_REGIONS,
};
static struct mem_type mem_alloc_regions = {
	.regions = mem_init_alloc,
	.cnt = 1,	/* empty dummy entry */
	.max = NR_MEM_REGIONS,
};
static bool mem_bottom_up = false;
static phys_addr_t mem_current_limit = MEM_ALLOC_ANYWHERE;

#define for_each_mem_range(i, type_a, type_b, p_start, p_end)	\
	for (i = 0, __next_mem_range(&i, type_a, type_b,	\
				     p_start, p_end);		\
	     i != (uint64_t)ULLONG_MAX;				\
	     __next_mem_range(&i, type_a, type_b,		\
			      p_start, p_end))
#define for_each_mem_range_rev(i, type_a, type_b,		\
			       p_start, p_end)			\
	for (i = (uint64_t)ULLONG_MAX,				\
		     __next_mem_range_rev(&i, type_a, type_b,	\
					 p_start, p_end);	\
	     i != (uint64_t)ULLONG_MAX;				\
	     __next_mem_range_rev(&i, type_a, type_b,		\
				  p_start, p_end))
#define for_each_free_mem_range(i, p_start, p_end)		\
	for_each_mem_range(i, &mem_free_regions,		\
			   &mem_alloc_regions, p_start, p_end)
#define for_each_free_mem_range_reverse(i, p_start, p_end)	\
	for_each_mem_range_rev(i, &mem_free_regions,		\
			       &mem_alloc_regions,		\
			       p_start, p_end)
#define for_each_reserved_mem_region(i, p_start, p_end)		\
	for (i = 0UL,						\
	     __next_reserved_mem_region(&i, p_start, p_end);	\
	     i != (uint64_t)ULLONG_MAX;				\
	     __next_reserved_mem_region(&i, p_start, p_end))
#define for_each_memblock_type(type, rgn)	\
	idx = 0;				\
	rgn = &type->regions[idx];		\
	for (idx = 0; idx < type->cnt;		\
	     idx++,rgn = &type->regions[idx])

static const char *mem_type_name(struct mem_type *type)
{
	if (type == &mem_free_regions)
		return "memory";
	else if (type == &mem_alloc_regions)
		return "reserved";
	else
		return "unknown";
}

static void __next_mem_range(uint64_t *idx,
			     struct mem_type *type_a,
			     struct mem_type *type_b,
			     phys_addr_t *out_start,
			     phys_addr_t *out_end)
{
	int idx_a = *idx & 0xffffffff;
	int idx_b = *idx >> 32;

	for (; idx_a < type_a->cnt; idx_a++) {
		struct mem_region *m = &type_a->regions[idx_a];

		phys_addr_t m_start = m->base;
		phys_addr_t m_end = m->base + m->size;

		if (!type_b) {
			if (out_start)
				*out_start = m_start;
			if (out_end)
				*out_end = m_end;
			idx_a++;
			*idx = (uint32_t)idx_a | (uint64_t)idx_b << 32;
			return;
		}

		/* scan areas before each reservation */
		for (; idx_b < type_b->cnt + 1; idx_b++) {
			struct mem_region *r;
			phys_addr_t r_start;
			phys_addr_t r_end;

			r = &type_b->regions[idx_b];
			r_start = idx_b ? r[-1].base + r[-1].size : 0;
			r_end = idx_b < type_b->cnt ?
				r->base : ULLONG_MAX;

			/*
			 * if idx_b advanced past idx_a,
			 * break out to advance idx_a
			 */
			if (r_start >= m_end)
				break;
			/* if the two regions intersect, we're done */
			if (m_start < r_end) {
				if (out_start)
					*out_start =
						max(m_start, r_start);
				if (out_end)
					*out_end = min(m_end, r_end);
				/*
				 * The region which ends first is
				 * advanced for the next iteration.
				 */
				if (m_end <= r_end)
					idx_a++;
				else
					idx_b++;
				*idx = (uint32_t)idx_a | (uint64_t)idx_b << 32;
				return;
			}
		}
	}

	/* signal end of iteration */
	*idx = ULLONG_MAX;
}

void __next_mem_range_rev(uint64_t *idx,
			  struct mem_type *type_a,
			  struct mem_type *type_b,
			  phys_addr_t *out_start,
			  phys_addr_t *out_end)
{
	int idx_a = *idx & 0xffffffff;
	int idx_b = *idx >> 32;

	if (*idx == (uint64_t)ULLONG_MAX) {
		idx_a = type_a->cnt - 1;
		idx_b = type_b->cnt;
	}

	for (; idx_a >= 0; idx_a--) {
		struct mem_region *m = &type_a->regions[idx_a];

		phys_addr_t m_start = m->base;
		phys_addr_t m_end = m->base + m->size;

		if (!type_b) {
			if (out_start)
				*out_start = m_start;
			if (out_end)
				*out_end = m_end;
			idx_a++;
			*idx = (uint32_t)idx_a | (uint64_t)idx_b << 32;
			return;
		}

		/* scan areas before each reservation */
		for (; idx_b >= 0; idx_b--) {
			struct mem_region *r;
			phys_addr_t r_start;
			phys_addr_t r_end;

			r = &type_b->regions[idx_b];
			r_start = idx_b ? r[-1].base + r[-1].size : 0;
			r_end = idx_b < type_b->cnt ?
				r->base : ULLONG_MAX;
			/* if idx_b advanced past idx_a,
			 * break out to advance idx_a
			 */
			if (r_end <= m_start)
				break;
			/* if the two regions intersect, we're done */
			if (m_end > r_start) {
				if (out_start)
					*out_start = max(m_start, r_start);
				if (out_end)
					*out_end = min(m_end, r_end);
				if (m_start >= r_start)
					idx_a--;
				else
					idx_b--;
				*idx = (uint32_t)idx_a | (uint64_t)idx_b << 32;
				return;
			}
		}
	}
	/* signal end of iteration */
	*idx = ULLONG_MAX;
}

void __next_reserved_mem_region(uint64_t *idx,
				phys_addr_t *out_start,
				phys_addr_t *out_end)
{
	struct mem_type *type = &mem_alloc_regions;

	if (*idx >= 0 && *idx < type->cnt) {
		struct mem_region *r = &type->regions[*idx];
		phys_addr_t base = r->base;
		phys_addr_t size = r->size;

		if (out_start)
			*out_start = base;
		if (out_end)
			*out_end = base + size - 1;
		*idx += 1;
		return;
	}
	/* signal end of iteration */
	*idx = ULLONG_MAX;
}

static void mem_merge_regions(struct mem_type *type)
{
	int i = 0;

	/* cnt never goes below 1 */
	while (i < type->cnt - 1) {
		struct mem_region *this = &type->regions[i];
		struct mem_region *next = &type->regions[i + 1];

		if (this->base + this->size != next->base) {
			BUG_ON(this->base + this->size > next->base);
			i++;
			continue;
		}

		this->size += next->size;
		/* move forward from next + 1, index of which is i + 2 */
		memmove(next, next + 1, (type->cnt - (i + 2)) * sizeof(*next));
		type->cnt--;
	}
}

static void mem_insert_region(struct mem_type *type, int idx,
			      phys_addr_t base, phys_addr_t size)
{
	struct mem_region *rgn = &type->regions[idx];

	memmove(rgn + 1, rgn, (type->cnt - idx) * sizeof(*rgn));
	rgn->base = base;
	rgn->size = size;
	type->cnt++;
	type->total_size += size;
}

static phys_addr_t __mem_find_range_bottom_up(phys_addr_t start,
					      phys_addr_t end,
					      phys_addr_t size,
					      phys_addr_t align)
{
	phys_addr_t this_start, this_end, cand;
	uint64_t i;

	for_each_free_mem_range(i, &this_start, &this_end) {
		this_start = clamp(this_start, start, end);
		this_end = clamp(this_end, start, end);
		cand = round_up(this_start, align);
		if (cand < this_end && this_end - cand >= size)
			return cand;
	}
	return 0;
}

static phys_addr_t __mem_find_range_top_down(phys_addr_t start,
					     phys_addr_t end,
					     phys_addr_t size,
					     phys_addr_t align)
{
	phys_addr_t this_start, this_end, cand;
	uint64_t i;

	for_each_free_mem_range_reverse(i, &this_start, &this_end) {
		this_start = clamp(this_start, start, end);
		this_end = clamp(this_end, start, end);
		if (this_end < size)
			continue;
		cand = round_down(this_end - size, align);
		if (cand >= this_start)
			return cand;
	}
	return 0;
}

static phys_addr_t mem_find_in_range(phys_addr_t size, phys_addr_t align,
				     phys_addr_t start, phys_addr_t end)
{
	phys_addr_t kernel_end, ret;

	/* pump up @end */
	if (end == MEM_ALLOC_ACCESSIBLE)
		end = mem_current_limit;

	/* avoid allocating the first page */
	start = max(start, PAGE_SIZE);
	end = max(start, end);
	kernel_end = __pa_symbol(__end);

	/* try bottom-up allocation only when bottom-up mode
	 * is set and @end is above the kernel image.
	 */
	if (mem_bottom_up && end > kernel_end) {
		phys_addr_t bottom_up_start;

		/* make sure we will allocate above the kernel */
		bottom_up_start = max(start, kernel_end);

		/* ok, try bottom-up allocation first */
		ret = __mem_find_range_bottom_up(bottom_up_start, end,
						 size, align);
		if (ret)
			return ret;

		/* we always limit bottom-up allocation above the kernel,
		 * but top-down allocation doesn't have the limit, so
		 * retrying top-down allocation may succeed when bottom-up
		 * allocation failed.
		 *
		 * bottom-up allocation is expected to be fail very rarely,
		 * so we use WARN_ONCE() here to see the stack trace if
		 * fail happens.
		 */
		printf("memblock: bottom-up allocation failed, "
		       "memory hotunplug may be affected\n");
	}
	return __mem_find_range_top_down(start, end, size, align);
}

#ifdef CONFIG_MEM_RESIZE
static int mem_double_array(struct mem_type *type,
			    phys_addr_t new_area_start,
			    phys_addr_t new_area_size)
{
	struct mem_region *new_array, *old_array;
	phys_addr_t old_alloc_size, new_alloc_size;
	phys_addr_t old_size, new_size, addr;

	/* Calculate new doubled size */
	old_size = type->max * sizeof(struct mem_region);
	new_size = old_size << 1;
	/* We need to allocated new one align to PAGE_SIZE,
	 *   so we can free them completely later.
	 */
	old_alloc_size = PAGE_ALIGN(old_size);
	new_alloc_size = PAGE_ALIGN(new_size);

	/* Try to find some space for it.
	 *
	 * WARNING: We assume that either slab_is_available() and we use it or
	 * we use MEMBLOCK for allocations. That means that this is unsafe to
	 * use when bootmem is currently active (unless bootmem itself is
	 * implemented on top of MEMBLOCK which isn't the case yet)
	 *
	 * This should however not be an issue for now, as we currently only
	 * call into MEMBLOCK while it's still active, or much later when slab
	 * is active for memory hotplug operations
	 */
	/* only exclude range when trying to double reserved.regions */
	if (type != &mem_alloc_regions)
		new_area_start = new_area_size = 0;

	addr = mem_find_in_range(new_area_start + new_area_size,
				 mem_current_limit,
				 new_alloc_size, PAGE_SIZE);
	if (!addr && new_area_size)
		addr = mem_find_in_range(0,
			min(new_area_start, mem_current_limit),
			new_alloc_size, PAGE_SIZE);

	new_array = (struct mem_region *)((addr ? (void *)(__va(addr)) : NULL));
	if (!addr) {
		printf("memblock: Failed to double %s array from %ld to %ld entries !\n",
		       mem_type_name(type), type->max, type->max * 2);
		return -1;
	}

	mem_dbg("mem: %s is doubled to %ld at [%#010llx-%#010llx]",
		mem_type_name(type), type->max * 2, (uint64_t)addr,
		(uint64_t)addr + new_size - 1);

	/* Found space, we now need to move the array over before we add the
	 * reserved region since it may be our reserved array itself that is
	 * full.
	 */
	memcpy(new_array, type->regions, old_size);
	memset(new_array + type->max, 0, old_size);
	old_array = type->regions;
	type->regions = new_array;
	type->max <<= 1;

	/* Free old array. We needn't free it if the array is the static one */
	if (old_array != mem_init_free && old_array != mem_init_alloc)
		mem_free(__pa(old_array), old_alloc_size);

	/* Reserve the new array if that comes from the memblock. Otherwise, we
	 * needn't do it.
	 */
	mem_reserve(addr, new_alloc_size);
	return 0;
}
#else
static int mem_double_array(struct mem_type *type,
			    phys_addr_t new_area_start,
			    phys_addr_t new_area_size)
{
	return -1;
}
#endif

static void mem_isolate_range(struct mem_type *type,
			      phys_addr_t base, phys_addr_t size,
			      int *start_rgn, int *end_rgn)
{
	phys_addr_t end = base + size;
	int idx;
	struct mem_region *rgn;

	*start_rgn = *end_rgn = 0;

	if (!size)
		return;

	/* we'll create at most two more regions */
	while (type->cnt + 2 > type->max) {
		if (mem_double_array(type, base, size) < 0) {
			BUG();
			return;
		}
	}

	for_each_memblock_type(type, rgn) {
		phys_addr_t rbase = rgn->base;
		phys_addr_t rend = rbase + rgn->size;

		if (rbase >= end)
			break;
		if (rend <= base)
			continue;

		if (rbase < base) {
			/* Region intersects from below.  Split and continue
			 * to process the next region - the new top half.
			 */
			rgn->base = base;
			rgn->size -= base - rbase;
			type->total_size -= base - rbase;
			mem_insert_region(type, idx, rbase, base - rbase);
		} else if (rend > end) {
			/* Region intersects from above.  Split and redo the
			 * current region - the new bottom half.
			 */
			rgn->base = end;
			rgn->size -= end - rbase;
			type->total_size -= end - rbase;
			mem_insert_region(type, idx--, rbase, end - rbase);
		} else {
			/* @rgn is fully contained, record it */
			if (!*end_rgn)
				*start_rgn = idx;
			*end_rgn = idx + 1;
		}
	}
}

static void mem_remove_region(struct mem_type *type, unsigned long r)
{
	type->total_size -= type->regions[r].size;
	memmove(&type->regions[r], &type->regions[r + 1],
		(type->cnt - (r + 1)) * sizeof(type->regions[r]));
	type->cnt--;

	/* Special case for empty arrays */
	if (type->cnt == 0) {
		BUG_ON(type->total_size != 0);
		type->cnt = 1;
		type->regions[0].base = 0;
		type->regions[0].size = 0;
	}
}

static void mem_remove_range(struct mem_type *type,
			     phys_addr_t base, phys_addr_t size)
{
	int start_rgn, end_rgn;
	int i;

	mem_isolate_range(type, base, size, &start_rgn, &end_rgn);

	for (i = end_rgn - 1; i >= start_rgn; i--)
		mem_remove_region(type, i);
}

void mem_remove(phys_addr_t base, phys_addr_t size)
{
	mem_remove_range(&mem_alloc_regions, base, size);
}

void mem_free(phys_addr_t base, phys_addr_t size)
{
	mem_dbg("mem_free: [%#016llx-%#016llx]\n",
		(unsigned long long)base,
		(unsigned long long)base + size - 1);
	mem_remove_range(&mem_alloc_regions, base, size);
}

void mem_add_range(struct mem_type *type, phys_addr_t base, phys_addr_t size)
{
	bool insert = false;
	phys_addr_t obase = base;
	phys_addr_t end = base + size;
	int idx, nr_new;
	struct mem_region *rgn;
	phys_addr_t rbase;
	phys_addr_t rend;

	if (!size)
		return;

	/* special case for empty array */
	if (type->regions[0].size == 0) {
		type->regions[0].base = base;
		type->regions[0].size = size;
		type->total_size = size;
		return;
	}

repeat:
	/* The following is executed twice. Once with insert=false and then
	 * with insert=true. The first counts the number of regions needed
	 * to accomodate the new area. The second actually inserts them.
	 */
	base = obase;
	nr_new = 0;

	for_each_memblock_type(type, rgn) {
		rbase = rgn->base;
		rend = rbase + rgn->size;

		if (rbase >= end)
			break;
		if (rend <= base)
			continue;
		/* Region overlaps. If it separates the lower part of new
		 * area, insert that portion.
		 */
		if (rbase > base) {
			nr_new++;
			if (insert)
				mem_insert_region(type, idx++,
						  base, rbase - base);
		}
		/* area below rend is dealt with, forget about it */
		base = min(rend, end);
	}

	/* insert the remaining portion */
	if (base < end) {
		nr_new++;
		if (insert)
			mem_insert_region(type, idx,
					  base, end - base);
	}

	/* If this was the first round, resize array and repeat for actual
	 * insertions; otherwise, merge and return.
	 */
	if (!insert) {
		while (type->cnt + nr_new > type->max) {
			if (mem_double_array(type, obase, size) < 0) {
				BUG();
				return;
			}
		}
		insert = true;
		goto repeat;
	} else {
		mem_merge_regions(type);
	}
}

void mem_add(phys_addr_t base, phys_addr_t size)
{
	struct mem_type *type = &mem_free_regions;

	mem_dbg("mem_add: [%#016llx-%#016llx]\n",
		(unsigned long long)base,
		(unsigned long long)base + size - 1);
	mem_add_range(type, base, size);
}

void mem_reserve(phys_addr_t base, phys_addr_t size)
{
	struct mem_type *type = &mem_alloc_regions;

	mem_dbg("mem_reserve: [%#016llx-%#016llx]\n",
		(unsigned long long)base,
		(unsigned long long)base + size - 1);
	mem_add_range(type, base, size);
}

static phys_addr_t mem_alloc_range(phys_addr_t size,
				   phys_addr_t align, phys_addr_t start,
				   phys_addr_t end)
{
	phys_addr_t found;

	if (!align)
		align = SMP_CACHE_BYTES;

	found = mem_find_in_range(size, align, start, end);
	if (found) {
		mem_reserve(found, size);
		return found;
	}
	return 0;
}

phys_addr_t mem_alloc(phys_addr_t size, phys_addr_t align)
{
	return mem_alloc_range(size, align, 0, MEM_ALLOC_ACCESSIBLE);
}

void mem_init(void)
{
	mem_hw_range_init();
	mem_reserve(PHYS_OFFSET + TEXT_OFFSET, __pa_symbol(__end));
}
#else
extern caddr_t __end[];

void mem_free(phys_addr_t base, phys_addr_t size)
{
	heap_free(base);
}

phys_addr_t mem_alloc(phys_addr_t size, phys_addr_t align)
{
	caddr_t addr = heap_alloc(size + (align - 1));

	if (!addr)
		return (phys_addr_t)addr;
	return (phys_addr_t)ALIGN(addr, align);
}

void mem_init(void)
{
	heap_range_init((caddr_t)__end);
}
#endif
