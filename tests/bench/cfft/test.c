#include <target/bench.h>
#include <target/cfft.h>
#include <target/cpus.h>
#include <target/percpu.h>

#include "fft.h"

#define CFFT_FLOAT_TOLERANCE 0.001

float float_abs(float v)
{
	return (v < 0) ? (-v) : v;
}

/*
 * Test entry.
 */
#define VALUE_CNT 30

static unsigned long input_30[] = {
0x0, 0x403e000000000000,
0x3ff0000000000000, 0xc03d000000000000,
0x4000000000000000, 0x403c000000000000,
0x4008000000000000, 0xc03b000000000000,
0x4010000000000000, 0x403a000000000000,
0x4014000000000000, 0xc039000000000000,
0x4018000000000000, 0x4038000000000000,
0x401c000000000000, 0xc037000000000000,
0x4020000000000000, 0x4036000000000000,
0x4022000000000000, 0xc035000000000000,
0x4024000000000000, 0x4034000000000000,
0x4026000000000000, 0xc033000000000000,
0x4028000000000000, 0x4032000000000000,
0x402a000000000000, 0xc031000000000000,
0x402c000000000000, 0x4030000000000000,
0x402e000000000000, 0xc02e000000000000,
0x4030000000000000, 0x402c000000000000,
0x4031000000000000, 0xc02a000000000000,
0x4032000000000000, 0x4028000000000000,
0x4033000000000000, 0xc026000000000000,
0x4034000000000000, 0x4024000000000000,
0x4035000000000000, 0xc022000000000000,
0x4036000000000000, 0x4020000000000000,
0x4037000000000000, 0xc01c000000000000,
0x4038000000000000, 0x4018000000000000,
0x4039000000000000, 0xc014000000000000,
0x403a000000000000, 0x4010000000000000,
0x403b000000000000, 0xc008000000000000,
0x403c000000000000, 0x4000000000000000,
0x403d000000000000, 0xbff0000000000000,
};

static unsigned long result_30[] = {
0x407b300000000000, 0x402e000000000000,
0xc030939973d7bb3b, 0x4063b6e517aef91c,
0xc03230377312e9bf, 0x40556471e55c2e22,
0xc033dfb0b5efb4bf, 0x404e9526c9763252,
0xc035adad74860224, 0x40485863fe159dba,
0xc037a905da62bc4c, 0x40447d8939d3616a,
0xc039e5ec564ada58, 0x4041d2a73dda6b1a,
0xc03c818cf73965bf, 0x403fa8c0712f45aa,
0xc03fa8c05ffedcc8, 0x403c818d2ecef0e0,
0xc041d2a7116c4d1a, 0x4039e5ebea6ed777,
0xc0447d899cae3db7, 0x4037a906689c09ad,
0xc0485863e1fbbbcb, 0x4035adad85183293,
0xc04e9526ff7d6cc6, 0x4033dfb10efe62c2,
0xc0556471d76b71e8, 0x403230378415ed03,
0xc063b6e515ceb77a, 0x40309399ad9ba1a3,
0xc02dfffcac1459f9, 0x407d0fffdacc7311,
0x405fedca40a11abf, 0x402ad8cca4c8bac8,
0x404bc8e3e5bb1042, 0x40279f90973de308,
0x403f2a4e0dd544a6, 0x4024409de2033b64,
0x4032b0c831bfd103, 0x4020a4a495395c4a,
0x4025f6267497704b, 0x40195be65d8fe010,
0x4016953b52cbc53c, 0x4010684cbb736304,
0x3ffa8c0b40d8cc60, 0x3ff7e72d1310f3f0,
0xbff7e729afe42c96, 0xbffa8c0a17a67df0,
0xc010684e6b6ae8c0, 0xc0169539eed35674,
0xc0195be424b1b412, 0xc025f627ffff5cec,
0xc020a4a46ed69a98, 0xc032b0c7fc2b3a0a,
0xc024409d28e619d6, 0xc03f2a4e66d224ef,
0xc0279f9071bcd136, 0xc04bc8e3cab85bd0,
0xc02ad8cc3cbfd81a, 0xc05fedca3b70baa1,
};


int cfft_run(struct cfft_context *cntx)
{

	complex input[VALUE_CNT];
	complex *result = (complex *)result_30;
	complex *output;
	int error_cnt = 0;
	int i;

	if (cntx == NULL) return -1;

	/* Make a copy of input, because it will be modified. */
	memcpy(input, input_30, sizeof(input_30));
    output = DFT_naive(input, VALUE_CNT);

    /* Compare results */
    for (i = 0; i < VALUE_CNT; i++) {
		if (float_abs(output[i].re - result[i].re) > CFFT_FLOAT_TOLERANCE) {
			printf("[%d].re: 0x%lx, 0x%lx, \n", i, output[i].re, result[i].re);
			error_cnt++;
		}
		if (float_abs(output[i].im - result[i].im) > CFFT_FLOAT_TOLERANCE) {
			printf("[%d].im: 0x%lx, 0x%lx, \n", i, output[i].im, result[i].im);
			error_cnt++;
		}
	}
	return error_cnt;
}

/*
 * Define one instance in bench command.
 */
struct cfft_percpu {
	struct cfft_context *ptr;
} __cache_aligned;
static struct cfft_percpu cfft_ctx[MAX_CPU_NUM];

static int cfft_t_pass(void) { return 1; }
static int cfft_t_fail(void) { return 0; }

#ifdef HOSTED
void main (int argc, char *argv[])
#else
int cfft(caddr_t percpu_area)
#endif
{
	int error_cnt;

	cfft_ctx[smp_processor_id()].ptr = (struct cfft_context *)percpu_area;
	error_cnt = cfft_run(cfft_ctx[smp_processor_id()].ptr);
	if (error_cnt == 0) {
		printf("Bench %s Success.\n", __func__);
		return cfft_t_pass();
	} else {
		printf("Bench %s Failed.\n", __func__);
		return cfft_t_fail();
	}
}

__define_testfn(cfft, sizeof(struct cfft_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
