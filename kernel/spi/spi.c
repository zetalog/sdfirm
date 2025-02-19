#include <target/panic.h>
#include <target/spi.h>

spi_device_t *spi_devices[NR_SPI_DEVICES];
spi_t spi_last_id = 0;
uint8_t spi_last_mode = INVALID_SPI_MODE;
uint32_t spi_last_freq = 0;

void spi_write_byte(uint8_t byte)
{
	spi_hw_write_byte(byte);
}

uint8_t spi_read_byte(void)
{
	return spi_hw_read_byte();
}

uint8_t spi_txrx(uint8_t byte)
{
	spi_tx(byte);
	return spi_rx();
}

#ifdef CONFIG_SPI_MASTER
spi_t spi_register_device(spi_device_t *dev)
{
	spi_t spi;

	BUG_ON(spi_last_id == INVALID_SPI_DID || !dev);

	spi = spi_last_id;
	spi_devices[spi] = dev;
	spi_last_id++;
	return spi;
}

void __spi_config_master(uint32_t khz, uint8_t mode)
{
	spi_last_mode = mode;
	spi_last_freq = khz;
	spi_hw_ctrl_stop();
	spi_hw_config_mode(mode);
	spi_hw_config_freq(min(khz, SPI_MAX_FREQ));
	spi_hw_ctrl_start();
}

static inline void spi_config_master(uint32_t khz, uint8_t mode)
{
	if ((spi_last_mode != mode) || (spi_last_freq != khz)) {
		__spi_config_master(khz, mode);
	}
}

void spi_select_device(spi_t spi)
{
	BUG_ON(spi >= spi_last_id);
	spi_config_master(spi_devices[spi]->max_freq_khz,
			  spi_devices[spi]->mode);
	spi_hw_chip_select(spi_devices[spi]->chip);
}
#endif

void spi_init(void)
{
	spi_hw_ctrl_init();
}
