#ifndef __IO_LM3S9B92_H_INCLUDE__
#define __IO_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>

#define IO_HW_SRAM_BASE		0x20000000
#define IO_HW_SRAM_SIZE		0x00100000
#define IO_HW_SRAM_BAND		0x22000000
#define IO_HW_PERIPH_BASE	0x40000000
#define IO_HW_PERIPH_SIZE	0x00100000
#define IO_HW_PERIPH_BAND	0x42000000

#define __IO_HW_BIT_MASK			(IO_HW_PERIPH_SIZE-1)
#define __IO_HW_BIT_VALID(addr, base, size)	((addr >= base) && (addr < (base+size)))

#if 0
BUG_ON(!(__IO_HW_BIT_VALID(address, IO_HW_SRAM_BASE, IO_HW_SRAM_SIZE) ||
	 __IO_HW_BIT_VALID(address, IO_HW_PERIPH_BASE, IO_HW_PERIPH_SIZE)));
#endif

#define __io_hw_bit_band(offset, address)		\
	(((address & ~__IO_HW_BIT_MASK) | 0x02000000) +	\
	 ((address & __IO_HW_BIT_MASK) << 5) +		\
	 (offset << 2))

#define __io_hw_bit_set(b, a)	__raw_writeb(0x01, __io_hw_bit_band(b, a))
#define __io_hw_bit_clear(b, a)	__raw_writeb(0x00, __io_hw_bit_band(b, a))
#define __io_hw_bit_get(b, a)	__raw_readb(__io_hw_bit_band(b, a))

#if 0
#define ARCH_HAVE_IO_ATOMIC 1
#define __raw_setb_atomic(b, a)		__io_hw_bit_set(b, a)
#define __raw_setw_atomic(b, a)		__io_hw_bit_set(b, a)
#define __raw_setl_atomic(b, a)		__io_hw_bit_set(b, a)
#define __raw_clearb_atomic(b, a)	__io_hw_bit_clear(b, a)
#define __raw_clearw_atomic(b, a)	__io_hw_bit_clear(b, a)
#define __raw_clearl_atomic(b, a)	__io_hw_bit_clear(b, a)
#define __raw_testb_atomic(b, a)	__io_hw_bit_get(b, a)
#define __raw_testw_atomic(b, a)	__io_hw_bit_get(b, a)
#define __raw_testl_atomic(b, a)	__io_hw_bit_get(b, a)
#endif

#endif /* __IO_LM3S9B92_H_INCLUDE__ */
