#include <target/generic.h>

#define MSB_IS_SET(x)	((boolean)(x>>((sizeof(x)<<3)-1))&1)

#ifndef ARCH_HAVE_DIV16U
uint16_t div16u(uint16_t x, uint16_t y)
{
	uint16_t reste = 0;
	uint8_t count = 16;
	boolean c;
	
	do {
		/* reste: x <- 0 */
		c = MSB_IS_SET(x);
		x <<= 1;
		reste <<= 1;
		if (c)
			reste |= 1;
		
		if (reste >= y) {
			reste -= y;
			/* x <- (result = 1) */
			x |= 1;
		}
	} while (--count);
	return x;
}
#endif

#ifndef ARCH_HAVE_DIV32U
uint32_t div32u(uint32_t x, uint32_t y)
{
	uint32_t reste = 0;
	uint8_t count = 32;
	boolean c;
	
	do {
		/* reste: x <- 0 */
		c = MSB_IS_SET(x);
		x <<= 1;
		reste <<= 1;
		if (c)
			reste |= 1;
		
		if (reste >= y) {
			reste -= y;
			/* x <- (result = 1) */
			x |= 1;
		}
	} while (--count);
	return x;
}
#endif

#ifdef CONFIG_MATH_DIV64
#ifndef	ARCH_HAVE_DIV64U
uint64_t div64u(uint64_t x, uint64_t y)
{
	uint64_t reste = 0;
	uint8_t count = 64;
	boolean c;
	
	do {
		/* reste: x <- 0 */
		c = MSB_IS_SET(x);
		x <<= 1;
		reste <<= 1;
		if (c)
			reste |= 1;
		
		if (reste >= y) {
			reste -= y;
			/* x <- (result = 1) */
			x |= 1;
		}
	} while (--count);
	return x;
}
#endif
#endif

#ifndef ARCH_HAVE_MUL16U
uint32_t mul16u(uint16_t x, uint16_t y)
{
	uint32_t a;
	uint32_t b;
	uint8_t mul1, mul2;

	mul1 = LOBYTE(x);
	mul2 = LOBYTE(y);
	a = (uint32_t)(mul1 * mul2) & 0x0000FFFF;

	mul2 = HIBYTE(y);
	b = (uint32_t)(mul1 * mul2) & 0x0000FFFF;

	a += b << 8;

	mul1 = HIBYTE(x);
	b = (uint32_t)(mul1 * mul2) & 0x0000FFFF;

	a += b << 16;

	mul2 = LOBYTE(y);
	b = (uint32_t)(mul1 * mul2) & 0x0000FFFF;

	a += b << 8;

	return a;
}
#endif

#ifdef CONFIG_MATH_MUL32
#ifndef	ARCH_HAVE_MUL32U
uint64_t mul32u(uint32_t x, uint32_t y)
{
	uint64_t a;
	uint64_t b;

	a = (uint64_t)(mul16u(LOWORD(x),LOWORD(y)) & 0x00000000FFFFFFFF);
	
	b = (uint64_t)(mul16u(LOWORD(x),HIWORD(y)) & 0x00000000FFFFFFFF);
	a += b << 16;

	b = (uint64_t)(mul16u(HIWORD(x),HIWORD(y)) & 0x00000000FFFFFFFF);
	a += b << 32;
	
	b = (uint64_t)(mul16u(HIWORD(x),LOWORD(y)) & 0x00000000FFFFFFFF);
	a += b << 16;
	
	return a;
}
#endif
#endif

#ifndef ARCH_HAVE_MOD16U
uint16_t mod16u(uint16_t x, uint16_t y)
{
	uint8_t count = 1;

	if (!y) return (uint16_t)-1;

	while (!(y & (1UL << (8*sizeof(uint16_t)-1)))) {
		y <<= 1;
		++count;
	}

	while (count) {
		if (x >= y) {
			x -= y;
		}
		y >>= 1;
		--count;
	}
	return x;
}
#endif

#ifdef CONFIG_MATH_MOD32
#ifndef	ARCH_HAVE_MOD32U
uint32_t mod32u(uint32_t x, uint32_t y)
{
	uint8_t count = 1;

	if (!y) return (uint32_t)-1;

	while (!(y & (1UL << (8*sizeof(uint32_t)-1)))) {
		y <<= 1;
		++count;
	}

	while (count) {
		if (x >= y) {
			x -= y;
		}
		y >>= 1;
		--count;
	}
	return x;
}
#endif
#endif

#ifdef CONFIG_MATH_MOD64
#ifndef	ARCH_HAVE_MOD64U
uint64_t mod64u(uint64_t x, uint64_t y)
{
	uint8_t count = 1;

	if (!y) return (uint32_t)-1;

	while (!(y & (uint64_t)(ULL(1) << (8*sizeof(uint64_t)-1)))) {
		y <<= 1;
		++count;
	}

	while (count) {
		if (x >= y) {
			x -= y;
		}
		y >>= 1;
		--count;
	}
	return x;
}
#endif
#endif
