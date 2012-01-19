#ifndef __SPI_AT8XC5122_H_INCLUDE__
#define __SPI_AT8XC5122_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/irq.h>

#ifndef ARCH_HAVE_SPI
#define ARCH_HAVE_SPI		1
#else
#error "Multiple SPI controller defined"
#endif

#include <target/spi.h>

Sfr(SPCON,	0xC3);		/* Serial Peripheral Control */
Sfr(SPSTA,	0xC4);		/* Serial Peripheral Status-Control */
Sfr(SPDAT,	0xC5);		/* Serial Peripheral Data */

#define MSK_SPCON_SPR2		0x80	/* SPCON */
#define MSK_SPCON_SPEN		0x40
#define MSK_SPCON_SSDIS		0x20
#define MSK_SPCON_MSTR		0x10
#define MSK_SPCON_CPOL		0x08
#define MSK_SPCON_CPHA		0x04
#define MSK_SPCON_SPR1		0x02
#define MSK_SPCON_SPR0		0x01
#define MSK_SPSTA_SPIF		0x80	/* SPSTA */
#define MSK_SPSTA_WCOL		0x40
#define MSK_SPSTA_MODF		0x10

#endif /* __SPI_AT8XC5122_H_INCLUDE__ */
