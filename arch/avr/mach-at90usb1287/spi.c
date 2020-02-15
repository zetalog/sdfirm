#include <target/spi.h>
#include <target/panic.h>

/* XXX: Clear SPIF/WCOL Before Next Transfer
 *
 * According to 17.1.4. SPI Status Register - SPSR, SPIF/WCOL will not be
 * cleared by hardware if no corresponding interrupt vector is executed.
 * SPIF/WCOL is cleared by first reading SPSR with SPIF/WCOL set, then
 * accessing the SPDR.
 * __spi_hw_readable is used to indicate the state in which SPSR has
 * been read with SPIF/WCOL set but SPDR has not been accessed.
 */

uint8_t __spi_hw_wait_io(void)
{
	uint8_t status;
	do {
		status = SPSR;
	} while (!(status & (_BV(SPIF) | _BV(WCOL))));
	return status;
}

#if 0
void spi_hw_write_byte(uint8_t byte)
{
	uint8_t status;

retry:
	SPDR = byte;
	status = __spi_hw_wait_io();
	if (__spi_hw_is_wcol(status)) {
		goto retry;
	}
}

uint8_t spi_hw_read_byte(void)
{
	spi_hw_write_byte(0);
	return SPDR;
}
#endif

/* config PINs */
/*
 * PINB0: SPI Slave Select pin, this pin must be disable (disable
 *        input mode) because it is used by joystick
 *        Note: the joystick can work fine in Output mode, because
 *        the pull-up resistor is activated and the level pin can
 *        be read.
 * PINB1: the SCK signal direction is OUTPUT
 * PINB2: the MOSI signal direction is OUTPUT
 * PINB3: the MISO signal direction is INPUT
 */
#ifdef CONFIG_SPI_MASTER
static void __spi_hw_ctrl_init(void)
{
	SPI_DDR &= ~_BV(DD_MISO);
	SPI_DDR |= (_BV(DD_MOSI) | _BV(DD_SCK) | _BV(DD_SS));
	SPI_PORT |= (_BV(PIN_MISO) | _BV(PIN_MOSI) | _BV(PIN_SCK) | _BV(PIN_SS));
	SPCR |= _BV(MSTR);
	__spi_hw_ctrl_enable();
}
#endif

#ifdef CONFIG_SPI_SLAVE
static void __spi_hw_ctrl_init(void)
{
	SPI_DDR |= _BV(DD_MISO);
	SPI_DDR &= ~(_BV(DD_MOSI) | _BV(DD_SCK) | _BV(DD_SS));
	SPI_PORT |= (_BV(PIN_MISO) | _BV(PIN_MOSI) | _BV(PIN_SCK) | _BV(PIN_SS));
	SPCR &= ~_BV(MSTR);
	__spi_hw_ctrl_enable();
}
#endif

void spi_hw_ctrl_start(void)
{
}

void spi_hw_config_mode(uint8_t mode)
{
	if (spi_order(mode) == SPI_LSB)
		SPCR |= _BV(DORD);
	else
		SPCR &= ~_BV(DORD);
	SPCR &= ~SPI_MODE_MASK_SPCR;
	SPCR |= (spi_mode(mode) << SPI_MODE_OFFSET_SPCR);
}

void __spi_hw_config_clock(uint8_t div)
{
	SPCR &= ~SPI_CLOCK_MASK_SPCR;
	SPCR |= ((((div>>3) & 0x01) << SPR0) | (((div>>5) & 0x01) << SPR1));
	SPSR &= ~SPI_CLOCK_MASK_SPSR;
	SPSR |= (__fls16(div) & 0x01) << SPI2X;
}

void spi_hw_deselect_chips(void)
{
	PORTE |= (_BV(PINE1) | _BV(PINE0));
}

void spi_hw_chip_select(uint8_t chip)
{
	spi_hw_deselect_chips();
	if (chip == SPI_CS_0) {
		PORTE &= ~_BV(PINE0);
	}
	if (chip == SPI_CS_1) {
		PORTE &= ~_BV(PINE1);
	}
}

void spi_hw_ctrl_stop(void)
{
}

void spi_hw_config_freq(uint32_t khz)
{
	uint8_t div;

	BUG_ON(khz > SPI_MAX_FREQ);
	div = div16u(CLK_OSC, khz);
	__spi_hw_config_clock(div);
}

void spi_hw_ctrl_init(void)
{
	AT90USB1287_POWER_ENABLE(PRR0, PRSPI);
	DDRE |= (_BV(DDE0) | _BV(DDE1));
	__spi_hw_ctrl_init();
}
