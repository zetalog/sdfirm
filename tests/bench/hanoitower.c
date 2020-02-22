/*
 * C program for Tower of Hanoi using Recursion
 */

#include <target/bench.h>
#include <target/hanoitower.h>
#include <target/cpus.h>
#include <target/percpu.h>

#define DISK_NUM_MAX 16
static int hanoitower_results[DISK_NUM_MAX] = {
/* num =  1, move_cnt = */ 1,
/* num =  2, move_cnt = */ 3,
/* num =  3, move_cnt = */ 7,
/* num =  4, move_cnt = */ 15,
/* num =  5, move_cnt = */ 31,
/* num =  6, move_cnt = */ 63,
/* num =  7, move_cnt = */ 127,
/* num =  8, move_cnt = */ 255,
/* num =  9, move_cnt = */ 511,
/* num = 10, move_cnt = */ 1023,
/* num = 11, move_cnt = */ 2047,
/* num = 12, move_cnt = */ 4095,
/* num = 13, move_cnt = */ 8191,
/* num = 14, move_cnt = */ 16383,
/* num = 15, move_cnt = */ 32767,
/* num = 16, move_cnt = */ 65535
};

struct hanoitower_percpu {
	struct hanoitower_context *ptr;
} __cache_aligned;
static struct hanoitower_percpu hanoitower_ctx[MAX_CPU_NUM];

static void hanoitower_run(int num, char frompeg, char topeg, char auxpeg)
{
    if (num == 1)
    {
		hanoitower_ctx[smp_processor_id()].ptr->move_cnt++;

        return;
    }
    hanoitower_run(num - 1, frompeg, auxpeg, topeg);
	hanoitower_ctx[smp_processor_id()].ptr->move_cnt++;
    hanoitower_run(num - 1, auxpeg, topeg, frompeg);
	return;
}

#ifndef CONFIG_HANOITOWER_DEPTH
#define CONFIG_HANOITOWER_DEPTH 4
#endif

#ifdef HOSTED
void main (int argc, char *argv[])
#else
int hanoitower(caddr_t percpu_area)
#endif
{
	unsigned int num = CONFIG_HANOITOWER_DEPTH;
	hanoitower_ctx[smp_processor_id()].ptr = (struct hanoitower_context *)percpu_area;

	/* Check paramter */
	if (num < 0 || num > DISK_NUM_MAX) {
		return -1;
	}

	hanoitower_ctx[smp_processor_id()].ptr->disk_num = num;
	hanoitower_ctx[smp_processor_id()].ptr->move_cnt = 0;

	/* Do bench */
    hanoitower_run(num, 'A', 'C', 'B');

	/* Checkout result and return */
	if (hanoitower_results[num-1] == 
			hanoitower_ctx[smp_processor_id()].ptr->move_cnt) {
		printf("Bench %s(disk_num = %d): Success.\n", __func__, num);
		return 1;
	} else {
		printf("Bench %s(disk_num = %d): Failed. move_cnt = %d\n", __func__,
				num, hanoitower_ctx[smp_processor_id()].ptr->move_cnt);
		return 0;
	}
}
__define_testfn(hanoitower, sizeof(struct hanoitower_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
