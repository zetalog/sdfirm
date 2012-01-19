/*
 * crc32.h
 * See linux/lib/crc32.c for license and changes
 */
#ifndef __CRC32_H_INCLUDE__
#define __CRC32_H_INCLUDE__

#include <sys/types.h>
#include <host/types.h>

extern uint32_t  crc32_le(uint32_t crc, unsigned char const *p, size_t len);
extern uint32_t  crc32_be(uint32_t crc, unsigned char const *p, size_t len);

#define crc32(seed, data, length)  crc32_le(seed, (unsigned char const *)data, length)

#endif /* __CRC32_H_INCLUDE__ */
