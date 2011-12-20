#include <host/bitops.h>

void set_bit(uint8_t nr, bits_t *addr)
{
	uint8_t mask = BITOP_MASK(nr);
	bits_t *p = ((bits_t *)addr) + BITOP_WORD(nr);
	*p |= mask;
}

void clear_bit(uint8_t nr, bits_t *addr)
{
	uint8_t mask = BITOP_MASK(nr);
	bits_t *p = ((bits_t *)addr) + BITOP_WORD(nr);
	*p &= ~mask;
}

int test_bit(int nr, const bits_t *addr)
{
	return (bits_t)1 & (addr[BITOP_WORD(nr)] >> (nr & (BITS_PER_UNIT-1)));
}

uint8_t __ffs16(uint16_t word)
{
	uint8_t num = 0;

	if (!word)
		return 0;

	if ((word & 0xff) == 0) {
		num += 8;
		word >>= 8;
	}
	if ((word & 0xf) == 0) {
		num += 4;
		word >>= 4;
	}
	if ((word & 0x3) == 0) {
		num += 2;
		word >>= 2;
	}
	if ((word & 0x1) == 0)
		num += 1;
	return num;
}

uint8_t __fls16(uint16_t word)
{
	uint8_t num = 16 - 1;

	if (!word)
		return 0;
	if (!(word & 0xff00u)) {
		word <<= 8;
		num -= 8;
	}
	if (!(word & 0xf000u)) {
		word <<= 4;
		num -= 4;
	}
	if (!(word & 0xc000u)) {
		word <<= 2;
		num -= 2;
	}
	if (!(word & 0x8000u)) {
		word <<= 1;
		num -= 1;
	}
	return num;
}

uint8_t __fls32(uint32_t word)
{
	uint8_t num = 32 - 1;

	if (!word)
		return 0;
	if (!(word & 0xffff0000u)) {
		word <<= 16;
		num -= 16;
	}
	if (!(word & 0xff000000u)) {
		word <<= 8;
		num -= 8;
	}
	if (!(word & 0xf0000000u)) {
		word <<= 4;
		num -= 4;
	}
	if (!(word & 0xc0000000u)) {
		word <<= 2;
		num -= 2;
	}
	if (!(word & 0x80000000u)) {
		word <<= 1;
		num -= 1;
	}
	return num;
}

uint32_t __rounddown32(uint32_t n)
{
	return (uint32_t)1 << __fls32(n);
}

uint16_t __roundup16(uint16_t n)
{
	return (uint16_t)1 << (__fls16((uint16_t)(n-1))+1);
}

uint32_t __roundup32(uint32_t n)
{
	return (uint32_t)1 << (__fls32((uint32_t)(n-1))+1);
}

uint8_t hweight16(uint16_t word)
{
	uint16_t res = word - ((word >> 1) & 0x5555);
	res = (res & 0x3333) + ((res >> 2) & 0x3333);
	res = (res + (res >> 4)) & 0x0F0F;
	return (res + (res >> 8)) & 0x00FF;
}

uint8_t hweight8(uint8_t byte)
{
	uint8_t res = byte - ((byte >> 1) & 0x55);
	res = (res & 0x33) + ((res >> 2) & 0x33);
	return (res + (res >> 4)) & 0x0F;
}

uint8_t bitrev8(uint8_t byte)
{
	byte = ((byte & 0x55) << 1 | (byte & 0xAA) >> 1);
	byte = ((byte & 0x33) << 2 | (byte & 0xCC) >> 2);
	byte = ((byte & 0x0F) << 4 | (byte & 0xF0) >> 4);
	return byte;
}

uint16_t bitrev16(uint16_t x)
{
	return ((uint16_t)bitrev8((uint8_t)(x & 0xff)) << 8) |
		(uint16_t)bitrev8((uint8_t)(x >> 8));
}

uint32_t bitrev32(uint32_t x)
{
	return ((uint32_t)bitrev16((uint16_t)(x & 0xffff)) << 16) |
		(uint32_t)bitrev16((uint16_t)(x >> 16));
}

uint8_t __ffs8(uint8_t byte)
{
	uint8_t num = 0;

	if (!byte)
		return 0;

	if ((byte & 0xf) == 0) {
		num += 4;
		byte >>= 4;
	}
	if ((byte & 0x3) == 0) {
		num += 2;
		byte >>= 2;
	}
	if ((byte & 0x1) == 0)
		num += 1;
	return num;
}

uint8_t __ffs32(uint32_t word)
{
	uint8_t num = 0;

	if ((word & 0xffff) == 0) {
		num += 16;
		word >>= 16;
	}
	if ((word & 0xff) == 0) {
		num += 8;
		word >>= 8;
	}
	if ((word & 0xf) == 0) {
		num += 4;
		word >>= 4;
	}
	if ((word & 0x3) == 0) {
		num += 2;
		word >>= 2;
	}
	if ((word & 0x1) == 0)
		num += 1;
	return num;
}

bits_t find_next_set_bit(const bits_t *addr,
			 bits_t size,
			 bits_t offset)
{
	const bits_t *p = addr + BITOP_WORD(offset);
	bits_t result = offset & ~(BITS_PER_UNIT-1);
	bits_t tmp;

	if (offset >= size)
		return size;
	size -= result;
	offset %= (bits_t)BITS_PER_UNIT;
	if (offset) {
		tmp = *(p++);
		tmp &= ((bits_t)(~((bits_t)0)) << offset);
		if (size < BITS_PER_UNIT)
			goto found_first;
		if (tmp)
			goto found_middle;
		size -= BITS_PER_UNIT;
		result += BITS_PER_UNIT;
	}
	while (size & ~(BITS_PER_UNIT-1)) {
		if ((tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_UNIT;
		size -= BITS_PER_UNIT;
	}
	if (!size)
		return result;
	tmp = *p;

found_first:
	tmp &= ((bits_t)(~((bits_t)0)) >> (BITS_PER_UNIT - size));
	if (tmp == (uint8_t)0)		/* Are any bits set? */
		return result + size;	/* Nope. */
found_middle:
	return result + __ffs32(tmp);
}

bits_t find_next_clear_bit(const bits_t *addr,
			   bits_t size,
			   bits_t offset)
{
	const bits_t *p = addr + BITOP_WORD(offset);
	bits_t result = offset & ~(BITS_PER_UNIT-1);
	bits_t tmp;

	if (offset >= size)
		return size;
	size -= result;
	offset %= (bits_t)BITS_PER_UNIT;
	if (offset) {
		tmp = *(p++);
		tmp |= (bits_t)(~((bits_t)0)) >> (BITS_PER_UNIT - offset);
		if (size < BITS_PER_UNIT)
			goto found_first;
		if (~tmp)
			goto found_middle;
		size -= BITS_PER_UNIT;
		result += BITS_PER_UNIT;
	}
	while (size & ~(BITS_PER_UNIT-1)) {
		if (~(tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_UNIT;
		size -= BITS_PER_UNIT;
	}
	if (!size)
		return result;
	tmp = *p;

found_first:
	tmp |= (bits_t)(~((bits_t)0)) << size;
	if (tmp == (bits_t)(~((bits_t)0)))	/* Are any bits zero? */
		return result + size;	/* Nope. */
found_middle:
	return result + __ffz32(tmp);
}
