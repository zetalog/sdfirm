#ifndef __CFFT_H_INCLUDE__
#define __CFFT_H_INCLUDE__

struct cfft_context {
	int result;
};

int cfft_run(struct cfft_context *cntx);

#endif /* __CFFT_H_INCLUDE__ */
