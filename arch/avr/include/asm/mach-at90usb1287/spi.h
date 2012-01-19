#ifndef __SPI_AT90USB1287_H_INCLUDE__
#define __SPI_AT90USB1287_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#include <asm/reg.h>
#include <asm/mach/pm.h>
#include <asm/mach/gpio.h>
#include <asm/mach/clk.h>

#ifndef ARCH_HAVE_SPI
#define ARCH_HAVE_SPI		1
#else
#error "Multiple SPI controller defined"
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
#define SPI_PORT		PORTB
#define DD_MISO			DDB3
#define DD_MOSI			DDB2
#define DD_SCK			DDB1
#define DD_SS			DDB0
#define PIN_MISO		PINB3
#define PIN_MOSI		PINB2
#define PIN_SCK			PINB1
#define PIN_SS			PINB0

/* chip selection configuration */
#define SPI_CHIP_DATAFLASH0	0
#define SPI_CHIP_DATAFLASH1	1

/* SPI IRQ control */
#define __spi_hw_is_wcol(status)	(status & _BV(WCOL))
#define __spi_hw_is_spif(status)	(status & _BV(SPIF))
#define __spi_hw_xfr_status()		(SPSR & (_BV(SPIF) | _BV(WCOL)))

/* enabling/disabling module and irq */
#define __spi_hw_ctrl_enable()	(SPCR |=  (_BV(SPE)))
#define __spi_hw_ctrl_disable()	(SPCR &= ~(_BV(SPE)))
#define __spi_hw_irq_enable()	(SPCR |=  (_BV(SPIE))
#define __spi_hw_irq_disable()	(SPCR &= ~(_BV(SPIE))

#ifdef CONFIG_SPI_AT90USB1287_FREQ_8M
#define SPI_HW_FREQ		8000
#endif
#ifdef CONFIG_SPI_AT90USB1287_FREQ_4M
#define SPI_HW_FREQ		4000
#endif
#ifdef CONFIG_SPI_AT90USB1287_FREQ_2M
#define SPI_HW_FREQ		2000
#endif
#ifdef CONFIG_SPI_AT90USB1287_FREQ_1M
#define SPI_HW_FREQ		1000
#endif
#ifdef CONFIG_SPI_AT90USB1287_FREQ_500K
#define SPI_HW_FREQ		500
#endif
#ifdef CONFIG_SPI_AT90USB1287_FREQ_250K
#define SPI_HW_FREQ		250
#endif
#ifdef CONFIG_SPI_AT90USB1287_FREQ_125K
#define SPI_HW_FREQ		125
#endif
/* SPR = 128 is not suitable */

#define __spi_hw_write_byte(byte)		\
	({SPDR = byte; while ((SPSR & (1<<SPIF)) == 0);})
#define spi_hw_write_byte(byte)			\
	__spi_hw_write_byte(byte)
#define spi_hw_read_byte()			\
	(__spi_hw_write_byte(0), SPDR)

void spi_hw_config_mode(uint8_t mode);
void spi_hw_config_freq(uint32_t khz);
void spi_hw_chip_select(uint8_t chip);
void spi_hw_deselect_chips(void);

void spi_hw_ctrl_start(void);
void spi_hw_ctrl_stop(void);
void spi_hw_ctrl_init(void);

#endif /* __SPI_AT90USB1287_H_INCLUDE__ */
