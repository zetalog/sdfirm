/*
 * Simple MD5 implementation
 *
 * Compile with: gcc -o md5 md5.c
 */
#if 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#else 
#include <target/bench.h>
#include <target/cpus.h>
#include <target/percpu.h>
#include <target/md5.h>

#include <string.h>
#include <stdint.h>
#endif

struct md5_percpu {
	struct md5_context *ptr;
} __cache_aligned;
static struct md5_percpu md5_ctx[MAX_CPU_NUM];

static uint8_t internal_buf[1024] = {0};

// Constants are the integer part of the sines of integers (in radians) * 2^32.
const uint32_t k[64] = {
0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };
 
// r specifies the per-round shift amounts
const uint32_t r[] = {7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
                      5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
                      4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
                      6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};
 
// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))
 
void to_bytes(uint32_t val, uint8_t *bytes)
{
    bytes[0] = (uint8_t) val;
    bytes[1] = (uint8_t) (val >> 8);
    bytes[2] = (uint8_t) (val >> 16);
    bytes[3] = (uint8_t) (val >> 24);
}
 
uint32_t to_int32(const uint8_t *bytes)
{
    return (uint32_t) bytes[0]
        | ((uint32_t) bytes[1] << 8)
        | ((uint32_t) bytes[2] << 16)
        | ((uint32_t) bytes[3] << 24);
}
 
void md5_run(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest) {
 
    // These vars will contain the hash
    uint32_t h0, h1, h2, h3;
 
    // Message (to prepare)
    uint8_t *msg = NULL;
 
    size_t new_len, offset;
    uint32_t w[16];
    uint32_t a, b, c, d, i, f, g, temp;
 
    // Initialize variables - simple count in nibbles:
    h0 = 0x67452301;
    h1 = 0xefcdab89;
    h2 = 0x98badcfe;
    h3 = 0x10325476;
 
    //Pre-processing:
    //append "1" bit to message    
    //append "0" bits until message length in bits ≡ 448 (mod 512)
    //append length mod (2^64) to message
 
    for (new_len = initial_len + 1; new_len % (512/8) != 448/8; new_len++)
        ;
#if 0 
    msg = (uint8_t*)malloc(new_len + 8);
#else
    msg = internal_buf;
#endif
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 0x80; // append the "1" bit; most significant bit is "first"
    for (offset = initial_len + 1; offset < new_len; offset++)
        msg[offset] = 0; // append "0" bits
 
    // append the len in bits at the end of the buffer.
    to_bytes(initial_len*8, msg + new_len);
    // initial_len>>29 == initial_len*8>>32, but avoids overflow.
    to_bytes(initial_len>>29, msg + new_len + 4);
 
    // Process the message in successive 512-bit chunks:
    //for each 512-bit chunk of message:
    for(offset=0; offset<new_len; offset += (512/8)) {
 
        // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
        for (i = 0; i < 16; i++)
            w[i] = to_int32(msg + offset + i*4);
 
        // Initialize hash value for this chunk:
        a = h0;
        b = h1;
        c = h2;
        d = h3;
 
        // Main loop:
        for(i = 0; i<64; i++) {
 
            if (i < 16) {
                f = (b & c) | ((~b) & d);
                g = i;
            } else if (i < 32) {
                f = (d & b) | ((~d) & c);
                g = (5*i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3*i + 5) % 16;          
            } else {
                f = c ^ (b | (~d));
                g = (7*i) % 16;
            }
 
            temp = d;
            d = c;
            c = b;
            b = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
            a = temp;
 
        }
 
        // Add this chunk's hash to result so far:
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
 
    }

#if 0 
    // cleanup
    free(msg);
#endif
 
    //var char digest[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)
    to_bytes(h0, digest);
    to_bytes(h1, digest + 4);
    to_bytes(h2, digest + 8);
    to_bytes(h3, digest + 12);
}
 
#ifdef HOSTED
int main(int argc, char **argv) {
#else
int md5(caddr_t percpu_area)
#endif
{
    size_t len;
    int i;
	uint8_t *result;
	int error_cnt = 0;

	char *input_buf[] = {
		"",
		"abc",
		"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		"The quick brown fox jumps over the lazy dog",
		"The quick brown fox jumps over the lazy cog", /* avalanche effect test */
		"bhn5bjmoniertqea40wro2upyflkydsibsk8ylkmgbvwi420t44cq034eou1szc1k0mk46oeb7ktzmlxqkbte2sy",
	};
	uint8_t answer[][MD5_BYTES] = {
		{0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04, 0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e},
		{0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0, 0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72},
		{0x82, 0x15, 0xef, 0x07, 0x96, 0xa2, 0x0b, 0xca, 0xaa, 0xe1, 0x16, 0xd3, 0x87, 0x6c, 0x66, 0x4a},
		{0x9e, 0x10, 0x7d, 0x9d, 0x37, 0x2b, 0xb6, 0x82, 0x6b, 0xd8, 0x1d, 0x35, 0x42, 0xa4, 0x19, 0xd6},
		{0x10, 0x55, 0xd3, 0xe6, 0x98, 0xd2, 0x89, 0xf2, 0xaf, 0x86, 0x63, 0x72, 0x51, 0x27, 0xbd, 0x4b},
		{0xd3, 0xbe, 0x54, 0x69, 0xa8, 0x39, 0x5b, 0x9e, 0xff, 0x16, 0x9d, 0x5c, 0x12, 0x0f, 0x2b, 0x95},
	};
	int test_cnt = (sizeof(input_buf) / sizeof(input_buf[0]));
#ifdef CONFIG_MD5_TEST_CNT
	if (test_cnt >= CONFIG_MD5_TEST_CNT)
		test_cnt = CONFIG_MD5_TEST_CNT;
#endif

	md5_ctx[smp_processor_id()].ptr = (struct md5_context *)percpu_area;
	result = md5_ctx[smp_processor_id()].ptr->result;

    for (i = 0; i < test_cnt; i++) {
		len = strlen(input_buf[i]);
        md5_run((uint8_t *)input_buf[i], len, result);
		for (int j = 0; j < MD5_BYTES; j++) {
        	printf("%2.2x", result[j]);
			if (result[j] != answer[i][j]) error_cnt++;
		}
		printf(" for %s\n", input_buf[i]);
    }
 
	if (error_cnt == 0) {
		printf("MD5 test Success\n");
    	return 1;
	} else {
		printf("MD5 test Failed\n");
		return 0;
	}
}

__define_testfn(md5, sizeof(struct md5_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);

