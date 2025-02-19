#include <target/panic.h>
#include <target/spi.h>

#ifdef SYS_REALTIME
#define spi_poll_init()		__spi_poll_init()
#define spi_irq_init()		do { } while (0)
#else
#define spi_poll_init()		do { } while (0)
#define spi_irq_init()		__spi_irq_init()
#endif

static bh_t spi_bh;
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

static void spi_bh_handler(uint8_t events)
{
	if (events == BH_POLLIRQ) {
		spi_hw_handle_irq();
		return;
	}
}

#ifdef SYS_REALTIME
void __spi_poll_init(void)
{
	irq_register_poller(spi_bh);
}
#else
void __spi_irq_init(void)
{
	spi_hw_irq_init();
}
#endif


void spi_init(void)
{
	spi_bh = bh_register_handler(spi_bh_handler);
	spi_irq_init();
	spi_poll_init();
	spi_hw_ctrl_init();
}
