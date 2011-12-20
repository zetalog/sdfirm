#ifndef __CRC32_TABLE_H_INCLUDE__
#define __CRC32_TABLE_H_INCLUDE__

#include <target/types.h>

#define crc32_le_byte(accum, delta)					\
	do {								\
		(accum) = crc32_le_table[((accum)^(delta))&0xff]^	\
			  ((accum)>>8);					\
	} while (0)

extern uint32_t crc32_le_table[];

#endif /* __CRC32_TABLE_H_INCLUDE__ */
