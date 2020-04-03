/*
*   SHA-256 implementation, Mark 2
*
*   Copyright (c) 2010,2014 Ilya O. Levin, http://www.literatecode.com
*
*   Permission to use, copy, modify, and distribute this software for any
*   purpose with or without fee is hereby granted, provided that the above
*   copyright notice and this permission notice appear in all copies.
*
*   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
*   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
*   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
*   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
*   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
*   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
*   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/* #define MINIMIZE_STACK_IMPACT */

#ifdef __cplusplus
extern "C" {
#endif


#include <target/bench.h>
#include <target/sha256.h>
#include <target/cpus.h>
#include <target/percpu.h>

struct sha256_percpu {
	struct sha256_context *ptr;
} __cache_aligned;
static struct sha256_percpu sha256_ctx[MAX_CPU_NUM];
#define FN_ inline static

static const uint32_t K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

#ifdef MINIMIZE_STACK_IMPACT
static uint32_t W[64];
#endif

/* -------------------------------------------------------------------------- */
FN_ uint8_t _shb(uint32_t x, uint32_t n)
{
	return ( (x >> (n & 31)) & 0xff );
} /* _shb */

/* -------------------------------------------------------------------------- */
FN_ uint32_t _shw(uint32_t x, uint32_t n)
{
	return ( (x << (n & 31)) & 0xffffffff );
} /* _shw */

/* -------------------------------------------------------------------------- */
FN_ uint32_t _r(uint32_t x, uint8_t n)
{
	return ( (x >> n) | _shw(x, 32 - n) );
} /* _r */

/* -------------------------------------------------------------------------- */
FN_ uint32_t _Ch(uint32_t x, uint32_t y, uint32_t z)
{
	return ( (x & y) ^ ((~x) & z) );
} /* _Ch */

/* -------------------------------------------------------------------------- */
FN_ uint32_t _Ma(uint32_t x, uint32_t y, uint32_t z)
{
	return ( (x & y) ^ (x & z) ^ (y & z) );
} /* _Ma */

/* -------------------------------------------------------------------------- */
FN_ uint32_t _S0(uint32_t x)
{
	return ( _r(x, 2) ^ _r(x, 13) ^ _r(x, 22) );
} /* _S0 */

/* -------------------------------------------------------------------------- */
FN_ uint32_t _S1(uint32_t x)
{
	return ( _r(x, 6) ^ _r(x, 11) ^ _r(x, 25) );
} /* _S1 */

/* -------------------------------------------------------------------------- */
FN_ uint32_t _G0(uint32_t x)
{
	return ( _r(x, 7) ^ _r(x, 18) ^ (x >> 3) );
} /* _G0 */

/* -------------------------------------------------------------------------- */
FN_ uint32_t _G1(uint32_t x)
{
	return ( _r(x, 17) ^ _r(x, 19) ^ (x >> 10) );
} /* _G1 */

/* -------------------------------------------------------------------------- */
FN_ uint32_t _word(uint8_t *c)
{
	return ( _shw(c[0], 24) | _shw(c[1], 16) | _shw(c[2], 8) | (c[3]) );
} /* _word */

/* -------------------------------------------------------------------------- */
FN_ void  _addbits(struct sha256_context *ctx, uint32_t n)
{
	if ( ctx->bits[0] > (0xffffffff - n) )
		ctx->bits[1] = (ctx->bits[1] + 1) & 0xFFFFFFFF;
	ctx->bits[0] = (ctx->bits[0] + n) & 0xFFFFFFFF;
} /* _addbits */

/* -------------------------------------------------------------------------- */
static void _hash(struct sha256_context *ctx)
{
	register uint32_t a, b, c, d, e, f, g, h, i;
	uint32_t t[2];
#ifndef MINIMIZE_STACK_IMPACT
	uint32_t W[64];
#endif

	a = ctx->hash[0];
	b = ctx->hash[1];
	c = ctx->hash[2];
	d = ctx->hash[3];
	e = ctx->hash[4];
	f = ctx->hash[5];
	g = ctx->hash[6];
	h = ctx->hash[7];

	for (i = 0; i < 64; i++) {
		if ( i < 16 )
			W[i] = _word(&ctx->buf[_shw(i, 2)]);
		else
			W[i] = _G1(W[i - 2]) + W[i - 7] + _G0(W[i - 15]) + W[i - 16];

		t[0] = h + _S1(e) + _Ch(e, f, g) + K[i] + W[i];
		t[1] = _S0(a) + _Ma(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t[0];
		d = c;
		c = b;
		b = a;
		a = t[0] + t[1];
	}

	ctx->hash[0] += a;
	ctx->hash[1] += b;
	ctx->hash[2] += c;
	ctx->hash[3] += d;
	ctx->hash[4] += e;
	ctx->hash[5] += f;
	ctx->hash[6] += g;
	ctx->hash[7] += h;
} /* _hash */

/* -------------------------------------------------------------------------- */
static void sha256_init(struct sha256_context *ctx)
{
	if ( ctx != NULL ) {
		ctx->bits[0]  = ctx->bits[1] = 0;
		ctx->len      = 0;
		ctx->hash[0] = 0x6a09e667;
		ctx->hash[1] = 0xbb67ae85;
		ctx->hash[2] = 0x3c6ef372;
		ctx->hash[3] = 0xa54ff53a;
		ctx->hash[4] = 0x510e527f;
		ctx->hash[5] = 0x9b05688c;
		ctx->hash[6] = 0x1f83d9ab;
		ctx->hash[7] = 0x5be0cd19;
	}
} /* sha256_init */

/* -------------------------------------------------------------------------- */
static void sha256_hash(struct sha256_context *ctx, const void *data, size_t len)
{
	register size_t i;
	const uint8_t *bytes = (const uint8_t *)data;

	if ( (ctx != NULL) && (bytes != NULL) )
		for (i = 0; i < len; i++) {
			ctx->buf[ctx->len] = bytes[i];
			ctx->len++;
			if (ctx->len == sizeof(ctx->buf) ) {
				_hash(ctx);
				_addbits(ctx, sizeof(ctx->buf) * 8);
				ctx->len = 0;
			}
		}
} /* sha256_hash */

/* -------------------------------------------------------------------------- */
static void sha256_done(struct sha256_context *ctx, uint8_t *hash)
{
	register uint32_t i, j;

	if ( ctx != NULL ) {
		j = ctx->len % sizeof(ctx->buf);
		ctx->buf[j] = 0x80;
		for (i = j + 1; i < sizeof(ctx->buf); i++)
			ctx->buf[i] = 0x00;

		if ( ctx->len > 55 ) {
			_hash(ctx);
			for (j = 0; j < sizeof(ctx->buf); j++)
				ctx->buf[j] = 0x00;
		}

		_addbits(ctx, ctx->len * 8);
		ctx->buf[63] = _shb(ctx->bits[0],  0);
		ctx->buf[62] = _shb(ctx->bits[0],  8);
		ctx->buf[61] = _shb(ctx->bits[0], 16);
		ctx->buf[60] = _shb(ctx->bits[0], 24);
		ctx->buf[59] = _shb(ctx->bits[1],  0);
		ctx->buf[58] = _shb(ctx->bits[1],  8);
		ctx->buf[57] = _shb(ctx->bits[1], 16);
		ctx->buf[56] = _shb(ctx->bits[1], 24);
		_hash(ctx);

		if ( hash != NULL )
			for (i = 0, j = 24; i < 4; i++, j -= 8) {
				hash[i     ] = _shb(ctx->hash[0], j);
				hash[i +  4] = _shb(ctx->hash[1], j);
				hash[i +  8] = _shb(ctx->hash[2], j);
				hash[i + 12] = _shb(ctx->hash[3], j);
				hash[i + 16] = _shb(ctx->hash[4], j);
				hash[i + 20] = _shb(ctx->hash[5], j);
				hash[i + 24] = _shb(ctx->hash[6], j);
				hash[i + 28] = _shb(ctx->hash[7], j);
			}
	}
} /* sha256_done */

/* -------------------------------------------------------------------------- */
void sha256_run(struct sha256_context *ctx, const void *data, size_t len, uint8_t *hash)
{
	if (ctx == NULL) return;
	if (data == NULL) return;
	if (len <= 0) return;
	if (hash == NULL) return;

	sha256_init(ctx);
	sha256_hash(ctx, data, len);
	sha256_done(ctx, hash);
}

static int sha256_t_pass(void) { return 1; }
static int sha256_t_fail(void) { return 0; }

/* ========================================================================== */

#ifdef HOSTED
void main (void)
#else
int sha256(caddr_t percpu_area)
#endif
{
	int error_cnt = 0;
	char *input_buf[] = {
#if 0
		"",
#endif
		"abc",
		"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		"The quick brown fox jumps over the lazy dog",
		"The quick brown fox jumps over the lazy cog", /* avalanche effect test */
		"bhn5bjmoniertqea40wro2upyflkydsibsk8ylkmgbvwi420t44cq034eou1szc1k0mk46oeb7ktzmlxqkbte2sy",
	};
	char *digest_str[] = {
#if 0
		"e3b0c442 98fc1c14 9afbf4c8 996fb924 27ae41e4 649b934c a495991b 7852b855",
#endif
		"ba7816bf 8f01cfea 414140de 5dae2223 b00361a3 96177a9c b410ff61 f20015ad",
		"248d6a61 d20638b8 e5c02693 0c3e6039 a33ce459 64ff2167 f6ecedd4 19db06c1",
		"d7a8fbb3 07d78094 69ca9abc b0082e4f 8d5651e4 6d3cdb76 2d02d0bf 37c9e592",
		"e4c4d8f3 bf76b692 de791a17 3e053211 50f7a345 b46484fe 427f6acc 7ecc81be",
		"9085df2f 02e0cc45 5928d0f5 1b27b4bf 1d9cd260 a66ed1fd a11b0a3f f5756d99"
	};
	uint8_t digest_value[][SHA256_BYTES] = {
#if 0
		{0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 
		 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
		 0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 
		 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55},
#endif
		{0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
		 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23, 
		 0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 
		 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad},
		{0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
		 0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
		 0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
		 0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1},
		{0xd7, 0xa8, 0xfb, 0xb3, 0x07, 0xd7, 0x80, 0x94,
		 0x69, 0xca, 0x9a, 0xbc, 0xb0, 0x08, 0x2e, 0x4f,
		 0x8d, 0x56, 0x51, 0xe4, 0x6d, 0x3c, 0xdb, 0x76,
		 0x2d, 0x02, 0xd0, 0xbf, 0x37, 0xc9, 0xe5, 0x92},
		{0xe4, 0xc4, 0xd8, 0xf3, 0xbf, 0x76, 0xb6, 0x92,
		 0xde, 0x79, 0x1a, 0x17, 0x3e, 0x05, 0x32, 0x11,
		 0x50, 0xf7, 0xa3, 0x45, 0xb4, 0x64, 0x84, 0xfe,
		 0x42, 0x7f, 0x6a, 0xcc, 0x7e, 0xcc, 0x81, 0xbe},
		{0x90, 0x85, 0xdf, 0x2f, 0x02, 0xe0, 0xcc, 0x45,
		 0x59, 0x28, 0xd0, 0xf5, 0x1b, 0x27, 0xb4, 0xbf,
		 0x1d, 0x9c, 0xd2, 0x60, 0xa6, 0x6e, 0xd1, 0xfd, 
		 0xa1, 0x1b, 0x0a, 0x3f, 0xf5, 0x75, 0x6d, 0x99}
	};
	uint8_t hash[SHA256_BYTES];
	size_t i, j;
	int test_cnt = (sizeof(input_buf) / sizeof(input_buf[0]));
#ifdef CONFIG_SHA256_TEST_CNT
	if (test_cnt >= CONFIG_SHA256_TEST_CNT)
		test_cnt = CONFIG_SHA256_TEST_CNT;
#endif

	sha256_ctx[smp_processor_id()].ptr = (struct sha256_context *)percpu_area;

	for (i = 0; i < test_cnt; i++) {
		sha256_run(sha256_ctx[smp_processor_id()].ptr, input_buf[i], strlen(input_buf[i]), hash);
		printf("input = '%s'\ndigest: %s\nresult: ", input_buf[i], digest_str[i]);
		for (j = 0; j < SHA256_BYTES; j++) {
			printf("%02x%s", hash[j], ((j % 4) == 3) ? " " : "");
		}
		printf("\n");
		for (j = 0; j < SHA256_BYTES; j++) {
			if (digest_value[i][j] != hash[j]) {
				error_cnt++;
				printf("Invalid %02x %02x", hash[j], digest_value[i][j]);
				continue;
			}
		}
		printf("\n\n");
	}

	if (error_cnt == 0) {
		printf("Bench %s Success.\n", __func__);
		return sha256_t_pass();
	} else {
		printf("Bench %s Failed.\n", __func__);
		return sha256_t_fail();
	}
}

__define_testfn(sha256, sizeof(struct sha256_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);

#ifdef __cplusplus
}
#endif

