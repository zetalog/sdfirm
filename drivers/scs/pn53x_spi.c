#include <driver/pn53x.h>
#include <target/spi.h>
#include <target/arch.h>
#include <target/delay.h>

/*
 * PN53X Classic SPI communication Preamble
 * (without Handshake mechanism combination)
 */
#define PN53X_CMD_SR		0x02
#define PN53X_CMD_DW		0x01
#define PN53X_CMD_DR		0x03

#define PN53X_SPI_FREQ		5000

spi_t pn53x_spi;

spi_device_t pn53x_spi_device = {
	(SPI_MODE_0 | SPI_LSB),
	PN53X_SPI_FREQ,
	BOARD_HW_SPI_PN53X,
};

void pn53x_hw_pm_resume(void)
{
	spi_select_device(pn53x_spi);
	mdelay(1);
}

void pn53x_hw_pm_suspend(void)
{
	spi_deselect_device();
}

boolean pn53x_hw_poll_ready(void)
{
	boolean ready;

	pn53x_hw_pm_resume();
	spi_write_byte(PN53X_CMD_SR);
	ready = ((spi_read_byte() & 0x01) ? true : false);
	if (!ready)
		pn53x_hw_pm_suspend();

	return ready;
}

void pn53x_hw_read_cmpl(scs_size_t ne)
{
	pn53x_hw_pm_suspend();
}

void pn53x_hw_write_cmpl(scs_size_t nc)
{
	pn53x_hw_pm_suspend();
}

uint8_t pn53x_hw_xchg_read(scs_off_t index)
{
	if (!index) {
		spi_write_byte(PN53X_CMD_DR);
	}
	return spi_read_byte();
}

void pn53x_hw_xchg_write(scs_off_t index, uint8_t val)
{
	if (!index) {
		pn53x_hw_pm_resume();
		spi_write_byte(PN53X_CMD_DW);
	}
	spi_write_byte(val);
}

void pn53x_hw_ctrl_init(void)
{
	pn53x_spi = spi_register_device(&pn53x_spi_device);
}
