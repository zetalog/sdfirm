#ifndef __MD5_H_INCLUDE__
#define __MD5_H_INCLUDE__

#include <stdint.h>

#define MD5_BYTES    16

struct md5_context {
    uint8_t result[MD5_BYTES];
};

void md5_run(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest);

#endif
