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
#ifdef CONFIG_JPGENC_SMALL_IMAGE
static uint8_t *load_jpg(int *len); /* Resulting JPG data expected. */
#endif

static int jpgenc_t_pass(void) { return 1; }
static int jpgenc_t_fail(void) { return 0; }

#ifdef HOSTED
void main (int argc, char *argv[])
#else
int jpgenc(caddr_t percpu_area)
#endif
{
	int w, h;
	uint8_t *img;
	int jpg_test_len;
#ifdef CONFIG_JPGENC_SMALL_IMAGE
	const uint8_t *jpg_test;
	int jpg_valid_len;
	uint8_t *jpg_valid;
#endif
	int error_cnt = 0;
	jpec_enc_t *e;
	jpgenc_ctx[smp_processor_id()].ptr = (struct jpgenc_context *)percpu_area;

	img = load_image(&w, &h);
	e = jpec_enc_new(img, w, h);

#ifdef CONFIG_JPGENC_SMALL_IMAGE
	jpg_test = jpec_enc_run(e, &jpg_test_len);

	jpg_valid = load_jpg(&jpg_valid_len);
	if (jpg_test_len != jpg_valid_len) {
		error_cnt = 0xFFFFFFFF;
		goto  _test_exit;
	}

	for (int i = 0; i < jpg_valid_len; i++) {
		if (jpg_valid[i] != jpg_test[i]) {
			error_cnt++;
		}
	}

_test_exit:
#else
	jpec_enc_run(e, &jpg_test_len);
#endif

	jpec_enc_del(e);
	if (error_cnt == 0) {
		printf("%s test Success\n", __func__);
		return jpgenc_t_pass();
	} else {
		printf("%s test Failed\n", __func__);
		return jpgenc_t_fail();
	}
}

static uint8_t *load_image(int *width, int *height)
{
	*width = image_width;
	*height = image_height;
	return image_data;
}

#ifdef CONFIG_JPGENC_SMALL_IMAGE
static uint8_t *load_jpg(int *len)
{
	*len = sizeof(jpg_data);
	return jpg_data;
}
#endif

__define_testfn(jpgenc, sizeof(struct jpgenc_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
