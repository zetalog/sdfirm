#ifndef __CRAM_H_INCLUDE__
#define __CRAM_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/cram.h>

#ifdef ARCH_HAVE_CRAM
uint8_t cram_readb(text_word_t *addr);
void cram_writeb(text_word_t *addr, uint8_t byte);
#else
#define cram_readb(addr)		0
#define cram_writeb(addr, byte)
#endif

#endif /* __CRAM_H_INCLUDE__ */
