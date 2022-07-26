#ifndef __CCN512_H_INCLUDE__
#define __CCN512_H_INCLUDE__

#include <std/stdint.h>
#include <io.h>
#include <types.h>
//#include <bits.h>

struct s_ccn512 {
	uint64_t base;
	uint64_t cc_rqf_bitmap;
	uint8_t num_hnf;
};

void ccn512_init(struct s_ccn512 *);

#endif /* __CCN512_H_INCLUDE__ */
