/*
 * C program for Tower of Hanoi using Recursion
 */

#include <target/bench.h>
#include <target/tlbtest.h>
#include <target/cpus.h>
#include <target/percpu.h>

#include <asm/tlb.h>

struct tlbtest_percpu {
	struct tlbtest_context *ptr;
} __cache_aligned;
static struct tlbtest_percpu tlbtest_ctx[MAX_CPU_NUM];

#define BUF_SIZE 1024

#ifdef HOSTED
void main (int argc, char *argv[])
#else
int tlbtest(caddr_t percpu_area)
#endif
{
	unsigned char test_buf_1[BUF_SIZE];
	__unused unsigned char test_buf_2[BUF_SIZE];
	int i;

	tlbtest_ctx[smp_processor_id()].ptr =
		(struct tlbtest_context *)percpu_area;

	for (i = 0; i < BUF_SIZE; i+= 17)
		test_buf_1[i] = i;
	flush_tlb_range_kern((caddr_t)test_buf_1,
		(caddr_t)(test_buf_1+BUF_SIZE));

	for (i = 0; i < BUF_SIZE; i+= 13)
		test_buf_2[i] = i;
	flush_tlb_all();

	tlbtest_ctx[smp_processor_id()].ptr->result = 1;
	return 1;
}
__define_testfn(tlbtest, sizeof(struct tlbtest_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
