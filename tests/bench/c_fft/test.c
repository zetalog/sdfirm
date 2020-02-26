#include <target/bench.h>
#include <target/c_fft.h>
#include <target/cpus.h>
#include <target/percpu.h>

#include "fft.h"

/*
 * Test entry.
 */
#ifndef CONFIG_C_FFT_VALUE_CNT
#define CONFIG_C_FFT_VALUE_CNT 30
#endif

#define VALUE_CNT CONFIG_C_FFT_VALUE_CNT

int c_fft_run(struct c_fft_context *cntx)
{
	complex input[VALUE_CNT] = {0};

	if (cntx == NULL) return -1;

    for (int i=0; i < VALUE_CNT; i++) {
        input[i].re = (double)i;
        input[i].im = 0.0;
    }

    DFT_naive(input, VALUE_CNT);

    /* TODO Compare results */
	cntx->result = 1;

    return 1;
}

/*
 * Define one instance in bench command.
 */
struct c_fft_percpu {
	struct c_fft_context *ptr;
} __cache_aligned;
static struct c_fft_percpu c_fft_ctx[MAX_CPU_NUM];

#ifdef HOSTED
void main (int argc, char *argv[])
#else
int c_fft(caddr_t percpu_area)
#endif
{
	c_fft_ctx[smp_processor_id()].ptr = (struct c_fft_context *)percpu_area;
	printf("Start FFT test...\n");
	c_fft_run(c_fft_ctx[smp_processor_id()].ptr);
	printf("End of FFT test... Success\n");
	return c_fft_ctx[smp_processor_id()].ptr->result;
}

__define_testfn(c_fft, sizeof(struct c_fft_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
