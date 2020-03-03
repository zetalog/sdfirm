#ifndef __SHA256_H_INCLUDE__
#define __SHA256_H_INCLUDE__

#include <stdint.h>

#define SHA256_BYTES    32

struct sha256_context {
	uint8_t  buf[64];
	uint32_t hash[8];
	uint32_t bits[2];
	uint32_t len;
};

void sha256_run(struct sha256_context *ctx, 
				const void *data, size_t len, uint8_t *hash);

#endif
