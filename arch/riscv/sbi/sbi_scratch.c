 /*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>
#include <asm/asm-offsets.h>

extern struct sbi_scratch *sbi_scratches[];

DEFINE_SPINLOCK(extra_lock);
static unsigned long extra_offset = SBI_SCRATCH_EXTRA_SPACE;

unsigned long sbi_scratch_alloc_offset(unsigned long size, const char *owner)
{
	int i;
	void *ptr;
	unsigned long ret = 0;
	struct sbi_scratch *rscratch;

	/*
	 * We have a simple brain-dead allocator which never expects
	 * anything to be free-ed hence it keeps incrementing the
	 * next allocation offset until it runs-out of space.
	 *
	 * In future, we will have more sophisticated allocator which
	 * will allow us to re-claim free-ed space.
	 */

	if (!size)
		return 0;

	while (size & (__SIZEOF_POINTER__ - 1))
		size++;

	spin_lock(&extra_lock);

	if (SBI_SCRATCH_SIZE < (extra_offset + size))
		goto done;

	ret = extra_offset;
	extra_offset += size;

done:
	spin_unlock(&extra_lock);

	if (ret) {
		for (i = 0; i < MAX_HARTS; i++) {
			rscratch = sbi_scratches[i];
			if (!rscratch)
				continue;
			ptr = sbi_scratch_offset_ptr(rscratch, ret);
			memset(ptr, 0, size);
		}
	}

	return ret;
}

void sbi_scratch_free_offset(unsigned long offset)
{
	if ((offset < SBI_SCRATCH_EXTRA_SPACE) ||
	    (SBI_SCRATCH_SIZE <= offset))
		return;

	/*
	 * We don't actually free-up because it's a simple
	 * brain-dead allocator.
	 */
}
