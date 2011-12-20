#ifndef __CRC16_CCITT_H_INCLUDE__
#define __CRC16_CCITT_H_INCLUDE__

#include <target/types.h>

void crc_ccitt_init(void);
void crc_ccitt_update(uint8_t byte, boolean msb);
uint16_t crc_ccitt_final(void);

#endif /* __CRC16_CCITT_H_INCLUDE__ */
