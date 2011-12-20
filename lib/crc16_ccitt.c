#include <target/crc16_ccitt.h>

union {
	uint16_t val;
	struct {
		uint16_t bit0 : 1;
		uint16_t bit1 : 1;
		uint16_t bit2 : 1;
		uint16_t bit3 : 1;
		uint16_t bit4 : 1;
		uint16_t bit5 : 1;
		uint16_t bit6 : 1;
		uint16_t bit7 : 1;
		uint16_t bit8 : 1;
		uint16_t bit9 : 1;
		uint16_t bit10 : 1;
		uint16_t bit11 : 1;
		uint16_t bit12 : 1;
		uint16_t bit13 : 1;
		uint16_t bit14 : 1;
		uint16_t bit15 : 1;
	} bits;
} crc_reg16;

#define CRC_CCITT_POLY_LSB	0x8408
#define CRC_CCITT_POLY_MSB	0x1021

static uint16_t crc_ccitt_poly(boolean msb)
{
	if (msb)
		return CRC_CCITT_POLY_MSB;
	else
		return CRC_CCITT_POLY_LSB;
}

void crc_ccitt_init(void)
{
	crc_reg16.val = 0xFFFF;
}

void crc_ccitt_update(uint8_t byte, boolean msb)
{
	uint8_t j, odd = 0;

	for (j = 0; j < 8; j++) {
		odd = (((crc_reg16.bits.bit0) ^ ((byte >> j) & 0x1)) & 0x1);
		crc_reg16.val >>= 1;
		if (odd)
			crc_reg16.val ^= crc_ccitt_poly(msb);
	}
}

uint16_t crc_ccitt_final(void)
{
	return ~crc_reg16.val;
}
