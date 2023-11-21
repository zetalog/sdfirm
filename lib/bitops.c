#include <target/generic.h>
#include <target/atomic.h>
#include <target/panic.h>
#include <target/irq.h>

#ifdef CONFIG_BIT_HWEIGHT64
uint8_t hweight64(uint64_t quad)
{
	uint64_t res;

	res = quad - ((quad >> 1) & 0x5555555555555555ul);
	res = (res & 0x3333333333333333ul) +
	      ((res >> 2) & 0x3333333333333333ul);
	res = (res + (res >> 4)) & 0x0F0F0F0F0F0F0F0Ful;
	res = res + (res >> 8);
	res = res + (res >> 16);
	return (uint8_t)((res + (res >> 32)) & 0x00000000000000FFul);
}
#endif

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

/**
 * set_bit - Set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * Unlike set_bit(), this function is non-atomic and may be reordered.
 * If it's called on the same region of memory simultaneously, the effect
 * may be that only one operation succeeds.
 */
void __set_bit(bits_t nr, volatile bits_t *addr)
{
	bits_t mask = BITOP_MASK(nr);
	bits_t *p = ((bits_t *)addr) + BITOP_WORD(nr);
	*p |= mask;
}

bool __test_and_set_bit(bits_t nr, volatile bits_t *addr)
{
	bits_t mask = BITOP_MASK(nr);
	bits_t *p = ((bits_t *)addr) + BITOP_WORD(nr);
	bits_t old = *p;

	*p = old | mask;
	return (old & mask) != 0;
}

void __clear_bit(bits_t nr, volatile bits_t *addr)
{
	bits_t mask = BITOP_MASK(nr);
	bits_t *p = ((bits_t *)addr) + BITOP_WORD(nr);
	*p &= ~mask;
}

#ifdef CONFIG_SMP
void set_bit(bits_t nr, volatile bits_t *addr)
{
	bits_t mask = BITOP_MASK(nr);
	bits_t *p = ((bits_t *)addr) + BITOP_WORD(nr);
	atomic_or(mask, (atomic_t *)p);
}

void clear_bit(bits_t nr, volatile bits_t *addr)
{
	bits_t mask = BITOP_MASK(nr);
	bits_t *p = ((bits_t *)addr) + BITOP_WORD(nr);
	atomic_andnot(mask, (atomic_t *)p);
}

bool test_and_set_bit(bits_t nr, volatile bits_t *addr)
{
	bits_t mask = BITOP_MASK(nr);
	bits_t *p = ((bits_t *)addr) + BITOP_WORD(nr);
	bits_t old = *p;

	old = atomic_or_return(mask, (atomic_t *)p);
	return (old & mask) != 0;
}
#endif

/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
boolean test_bit(bits_t nr, const bits_t *addr)
{
	return (bits_t)1 & (addr[BITOP_WORD(nr)] >> (nr & (BITS_PER_UNIT-1)));
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
	offset %= BITS_PER_UNIT;
	if (offset) {
		tmp = *(p++);
		tmp &= ((bits_t)(~((bits_t)0)) << offset);
		if (size < BITS_PER_UNIT)
			goto found_first;
		if ((bits_t)tmp)
			goto found_middle;
		size -= BITS_PER_UNIT;
		result += BITS_PER_UNIT;
	}
	while (size & ~(BITS_PER_UNIT-1)) {
		if ((bits_t)(tmp = *(p++)))
			goto found_middle;
		result += BITS_PER_UNIT;
		size -= BITS_PER_UNIT;
	}
	if (!size)
		return result;
	tmp = *p;

found_first:
	tmp &= ((bits_t)(~((bits_t)0)) >> (BITS_PER_UNIT - size));
	if (tmp == (bits_t)0)
		return result + size;
found_middle:
	return result + __ffs_unit(tmp);
}

#ifdef CONFIG_BIT_FIND_CLEAR
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
	offset %= BITS_PER_UNIT;
	if (offset) {
		tmp = *(p++);
		tmp |= (bits_t)(~((bits_t)0)) >> (BITS_PER_UNIT - offset);
		if (size < BITS_PER_UNIT)
			goto found_first;
		if ((bits_t)(~tmp))
			goto found_middle;
		size -= BITS_PER_UNIT;
		result += BITS_PER_UNIT;
	}
	while (size & ~(BITS_PER_UNIT-1)) {
		if ((bits_t)(~(tmp = *(p++))))
			goto found_middle;
		result += BITS_PER_UNIT;
		size -= BITS_PER_UNIT;
	}
	if (!size)
		return result;
	tmp = *p;

found_first:
	tmp |= (bits_t)(~((bits_t)0)) << size;
	if (tmp == (bits_t)(~((bits_t)0)))	
		return result + size;
found_middle:
	return result + __ffz_unit(tmp);
}
#endif

#ifdef CONFIG_BIT_FLS8
uint8_t __fls8(uint8_t x)
{
	uint8_t r = 7;

	if (!x)
		return 0;
	if (!(x & (uint8_t)0xf0)) {
		x <<= 4;
		r -= 4;
	}
	if (!(x & (uint8_t)0xc0)) {
		x <<= 2;
		r -= 2;
	}
	if (!(x & (uint8_t)0x80)) {
		x <<= 1;
		r -= 1;
	}
	return r;
}
#endif

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

#ifdef CONFIG_BIT_FFS16
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
#endif

#ifdef CONFIG_BIT_FFS32
uint8_t __ffs32(uint32_t dword)
{
	uint8_t num = 0;

	if (!dword)
		return 0;

	if ((dword & 0xffff) == 0) {
		num += 16;
		dword >>= 16;
	}
	if ((dword & 0xff) == 0) {
		num += 8;
		dword >>= 8;
	}
	if ((dword & 0xf) == 0) {
		num += 4;
		dword >>= 4;
	}
	if ((dword & 0x3) == 0) {
		num += 2;
		dword >>= 2;
	}
	if ((dword & 0x1) == 0)
		num += 1;
	return num;
}
#endif

#ifdef CONFIG_BIT_FFS64
uint8_t __ffs64(uint64_t qword)
{
	uint8_t num = 0;

	if (!qword)
		return 0;

	if ((qword & 0xffffffff) == 0) {
		num += 32;
		qword >>= 32;
	}
	if ((qword & 0xffff) == 0) {
		num += 16;
		qword >>= 16;
	}
	if ((qword & 0xff) == 0) {
		num += 8;
		qword >>= 8;
	}
	if ((qword & 0xf) == 0) {
		num += 4;
		qword >>= 4;
	}
	if ((qword & 0x3) == 0) {
		num += 2;
		qword >>= 2;
	}
	if ((qword & 0x1) == 0)
		num += 1;
	return num;
}
#endif

#ifdef CONFIG_BIT_FLS16
uint8_t __fls16(uint16_t word)
{
	int num = 16 - 1;

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
#endif

#ifdef CONFIG_BIT_FLS32
uint8_t __fls32(uint32_t dword)
{
	int num = 32 - 1;

	if (!dword)
		return 0;
	if (!(dword & UL(0xffff0000))) {
		dword <<= 16;
		num -= 16;
	}
	if (!(dword & UL(0xff000000))) {
		dword <<= 8;
		num -= 8;
	}
	if (!(dword & UL(0xf0000000))) {
		dword <<= 4;
		num -= 4;
	}
	if (!(dword & UL(0xc0000000))) {
		dword <<= 2;
		num -= 2;
	}
	if (!(dword & UL(0x80000000))) {
		dword <<= 1;
		num -= 1;
	}
	return num;
}
#endif

#ifdef CONFIG_BIT_FLS64
uint8_t __fls64(uint64_t quad)
{
	int num = 64 - 1;

	if (!quad)
		return 0;
	if (!(quad & ULL(0xffffffff00000000))) {
		quad <<= 32;
		num -= 32;
	}
	if (!(quad & ULL(0xffff000000000000))) {
		quad <<= 16;
		num -= 16;
	}
	if (!(quad & ULL(0xff00000000000000))) {
		quad <<= 8;
		num -= 8;
	}
	if (!(quad & ULL(0xf000000000000000))) {
		quad <<= 4;
		num -= 4;
	}
	if (!(quad & ULL(0xc000000000000000))) {
		quad <<= 2;
		num -= 2;
	}
	if (!(quad & ULL(0x8000000000000000))) {
		quad <<= 1;
		num -= 1;
	}
	return num;
}
#endif

#ifndef CONFIG_SMP
unsigned long __xchg(unsigned long x, volatile void *ptr, int size)
{
	unsigned long ret;
	irq_flags_t flags;

	switch (size) {
	case 1:
#ifdef __xchg_u8
		return __xchg_u8(x, ptr);
#else
		irq_local_save(flags);
		ret = *(volatile uint8_t *)ptr;
		*(volatile uint8_t *)ptr = x;
		irq_local_restore(flags);
		return ret;
#endif /* __xchg_u8 */
	case 2:
#ifdef __xchg_u16
		return __xchg_u16(x, ptr);
#else
		irq_local_save(flags);
		ret = *(volatile uint16_t *)ptr;
		*(volatile uint16_t *)ptr = x;
		irq_local_restore(flags);
		return ret;
#endif /* __xchg_u16 */
	case 4:
#ifdef __xchg_u32
		return __xchg_u32(x, ptr);
#else
		irq_local_save(flags);
		ret = *(volatile uint32_t *)ptr;
		*(volatile uint32_t *)ptr = x;
		irq_local_restore(flags);
		return ret;
#endif /* __xchg_u32 */
#ifdef CONFIG_64BIT
	case 8:
#ifdef __xchg_u64
		return __xchg_u64(x, ptr);
#else
		irq_local_save(flags);
		ret = *(volatile uint64_t *)ptr;
		*(volatile uint64_t *)ptr = x;
		irq_local_restore(flags);
		return ret;
#endif /* __xchg_u64 */
#endif /* CONFIG_64BIT */
	default:
		BUG();
		return x;
	}
}

unsigned long __cmpxchg(volatile void *ptr,
			unsigned long old, unsigned long new, int size)
{
	unsigned long ret;
	irq_flags_t flags;

	switch (size) {
	case 1:
#ifdef __cmpxchg_u8
		return __cmpxchg_u8(ptr, old, new, size);
#else
		irq_local_save(flags);
		ret = *(uint8_t *)ptr;
		if (ret == old)
			*(uint8_t *)ptr = (uint8_t)new;
		irq_local_restore(flags);
		return ret;
#endif /* __cmpxchg_u8 */
	case 2:
#ifdef __cmpxchg_u16
		return __cmpxchg_u16(ptr, old, new, size);
#else
		irq_local_save(flags);
		ret = *(uint16_t *)ptr;
		if (ret == old)
			*(uint16_t *)ptr = (uint16_t)new;
		irq_local_restore(flags);
		return ret;
#endif /* __cmpxchg_u16 */
	case 4:
#ifdef __cmpxchg_u32
		return __cmpxchg_u32(ptr, old, new, size);
#else
		irq_local_save(flags);
		ret = *(uint32_t *)ptr;
		if (ret == old)
			*(uint32_t *)ptr = (uint32_t)new;
		irq_local_restore(flags);
		return ret;
#endif /* __cmpxchg_u32 */
#ifdef CONFIG_64BIT
	case 8:
#ifdef __cmpxchg_u64
		return __cmpxchg_u64(ptr, old, new, size);
#else
		irq_local_save(flags);
		ret = *(uint64_t *)ptr;
		if (ret == old)
			*(uint64_t *)ptr = (uint64_t)new;
		irq_local_restore(flags);
		return ret;
#endif /* __cmpxchg_u64 */
#endif /* CONFIG_64BIT */
	default:
		BUG();
		return new;
	}
}
#endif
