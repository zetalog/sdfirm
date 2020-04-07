#include <target/bench.h>
#include <target/cfft.h>
#include <target/cpus.h>
#include <target/percpu.h>

#include "fft.h"

/*
 * Test entry.
 */
#ifndef CONFIG_CFFT_VALUE_CNT
#define CONFIG_CFFT_VALUE_CNT 30
#endif

#define VALUE_CNT CONFIG_CFFT_VALUE_CNT

int cfft_run(struct cfft_context *cntx)
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
struct cfft_percpu {
	struct cfft_context *ptr;
} __cache_aligned;
static struct cfft_percpu cfft_ctx[MAX_CPU_NUM];

#ifdef HOSTED
void main (int argc, char *argv[])
#else
int cfft(caddr_t percpu_area)
#endif
{
	cfft_ctx[smp_processor_id()].ptr = (struct cfft_context *)percpu_area;
	printf("Start FFT test...\n");
	cfft_run(cfft_ctx[smp_processor_id()].ptr);
	printf("End of FFT test... Success\n");
	return cfft_ctx[smp_processor_id()].ptr->result;
}

__define_testfn(cfft, sizeof(struct cfft_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
