#ifndef __SPI_AT90SCR100_H_INCLUDE__
#define __SPI_AT90SCR100_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/gpio.h>

#ifdef CONFIG_SPI_AT90SCR100
#ifndef ARCH_HAVE_SPI
#define ARCH_HAVE_SPI		1
#else
#error "Multiple SPI controller defined"
#endif
#endif

/* SPI control register */
#define SPCR	_SFR_IO8(0x2C)
#define SPIE    7
#define SPE     6
#define DORD    5
#define MSTR    4
#define CPOL    3
#define CPHA    2
#define SPR1    1
#define SPR0    0

/* SPI status register */
#define SPSR	_SFR_IO8(0x2D)
#define SPIF    7
#define WCOL    6
#define SPI2X   0

/* SPI data register */
#define SPDR	_SFR_IO8(0x2E)

/* SPI modes */
#define SPI_CPHA_LEADING	(0)
#define SPI_CPHA_TRAILING	(_BV(CPHA))
#define SPI_CPOL_LOW		(0)
#define SPI_CPOL_HIGH		(_BV(CPOL))
#define SPI_MODE_0_SPCR		(SPI_CPOL_LOW|SPI_CPHA_LEADING)
#define SPI_MODE_1_SPCR		(SPI_CPOL_LOW|SPI_CPHA_TRAILING)
#define SPI_MODE_2_SPCR		(SPI_CPOL_HIGH|SPI_CPHA_LEADING)
#define SPI_MODE_3_SPCR		(SPI_CPOL_HIGH|SPI_CPHA_TRAILING)

#define SPI_CLOCK_MASK_SPCR	(_BV(SPR1)|_BV(SPR0))
#define SPI_CLOCK_MASK_SPSR	(_BV(SPI2X))
#define SPI_MODE_MASK_SPCR	0x3C
#define SPI_MODE_OFFSET_SPCR	2

#define SPI_DDR			DDRB
#define DD_MOSI			DDB2
#define DD_MISO			DDB3
#define DD_SCK			DDB1
#define DD_SS			DDB0

#endif /* __SPI_AT90SCR100_H_INCLUDE__ */
