#ifndef __C_FFT_H_INCLUDE__
#define __C_FFT_H_INCLUDE__

struct c_fft_context {
	int result;
};

int c_fft_run(struct c_fft_context *cntx);

#endif /* __C_FFT_H_INCLUDE__ */
