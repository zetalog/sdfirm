#ifndef __MULDIV_H_INCLUDE__
#define __MULDIV_H_INCLUDE__


#define mul8u(x, y)	(x * y)

static inline uint32_t mul16u(uint16_t x, uint16_t y)
{
	uint32_t a;
	uint32_t b;

	a = (uint32_t)mul8u(LOBYTE(x), LOBYTE(y)) & 0x0000FFFF;

	b = (uint32_t)mul8u(LOBYTE(x), HIBYTE(y)) & 0x0000FFFF;
	a += b << 8;

	b = (uint32_t)mul8u(HIBYTE(x), HIBYTE(y)) & 0x0000FFFF;
	a += b << 16;

	b = (uint32_t)mul8u(HIBYTE(x), LOBYTE(y)) & 0x0000FFFF;
	a += b << 8;

	return a;
}

static inline uint64_t mul32u(uint32_t x, uint32_t y)
{
	uint64_t a;
	uint64_t b;

	a = (uint64_t)(mul16u(LOWORD(x), LOWORD(y)) & 0x00000000FFFFFFFF);

	b = (uint64_t)(mul16u(LOWORD(x), HIWORD(y)) & 0x00000000FFFFFFFF);
	a += b << 16;

	b = (uint64_t)(mul16u(HIWORD(x), HIWORD(y)) & 0x00000000FFFFFFFF);
	a += b << 32;
	
	b = (uint64_t)(mul16u(HIWORD(x), LOWORD(y)) & 0x00000000FFFFFFFF);
	a += b << 16;

	return a;
}

#endif /* __MULDIV_H_INCLUDE__ */
