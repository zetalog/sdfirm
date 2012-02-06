#include <driver/pn53x.h>
#include <target/spi.h>
#include <target/arch.h>

/*
 * PN53X Classic SPI communication Preamble
 * (without Handshake mechanism combination)
 */
#define PN53X_CMD_SR		0x02
#define PN53X_CMD_DW		0x01
#define PN53X_CMD_DR		0x03

#define PN53X_SPI_FREQ		5000

spi_t spi;

spi_device_t spi_pn53x = {
	SPI_MODE_0,
	PN53X_SPI_FREQ,
	PN53X_HW_SPI_CHIP,
};

boolean pn53x_hw_poll_ready(void)
{
	spi_write_byte(PN53X_CMD_SR);
	return ((spi_read_byte() & 0x01) ? true : false);
}

void pn53x_hw_read_cmpl(scs_size_t ne)
{
	spi_deselect_device();
}

void pn53x_hw_write_cmpl(scs_size_t nc)
{
	spi_deselect_device();
}

uint8_t pn53x_hw_xchg_read(scs_off_t index)
{
	if (!index) {
		spi_select_device(spi);
		spi_write_byte(PN53X_CMD_DR);
	}
	return spi_read_byte();
}

void pn53x_hw_xchg_write(scs_off_t index, uint8_t val)
{
	if (!index) {
		spi_select_device(spi);
		spi_write_byte(PN53X_CMD_DW);
	}
	spi_write_byte(val);
}

void pn53x_hw_ctrl_init(void)
{
	spi = spi_register_device(&spi_pn53x);
}
