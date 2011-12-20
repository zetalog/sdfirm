#include <target/cram.h>

uint8_t cram_readb(text_word_t *addr)
{
	uint8_t byte;

	R7 = 03h;
	R5 = HIBYTE(*addr);
	R4 = LOBYTE(*addr);

	byte = API_LIB((unsigned char)0x03, (unsigned int)addr,
		       (unsigned char)0x00);
	return byte;
}

void cram_writeb(text_word_t *addr, uint16_t byte)
{
	R7 = 04h;
	R5 = HIBYTE(*addr);
	R4 = LOBYTE(*addr);
	R3 = byte;

	API_LIB((unsigned char)0x04, (unsigned int)addr,
		(unsigned char)byte);
}
