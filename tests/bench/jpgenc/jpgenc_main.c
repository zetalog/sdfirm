#include <target/bench.h>
#include <target/jpgenc.h>
#include <target/cpus.h>
#include <target/percpu.h>

#include "jpgenc_inc.h"

#include "test_data.c"

struct jpgenc_percpu {
	struct jpgenc_context *ptr;
} __cache_aligned;
static struct jpgenc_percpu jpgenc_ctx[MAX_CPU_NUM];

static uint8_t *load_image(int *width, int *height);

#ifdef HOSTED
void main (int argc, char *argv[])
#else
int jpgenc(caddr_t percpu_area)
#endif
{
	int w, h;
	uint8_t *img;
	jpec_enc_t *e;
	int len;
	jpgenc_ctx[smp_processor_id()].ptr = (struct jpgenc_context *)percpu_area;

	img = load_image(&w, &h);
	e = jpec_enc_new(img, w, h);
	jpec_enc_run(e, &len);
	printf("Done (%d bytes)\n", len);
	jpec_enc_del(e);

	jpgenc_ctx[smp_processor_id()].ptr->result = 1;
	return 1;
}

static uint8_t *load_image(int *width, int *height)
{
	*width = image_width;
	*height = image_height;
	return image_data;
}

__define_testfn(jpgenc, sizeof(struct jpgenc_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
