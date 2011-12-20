#ifndef __SPI_H_INCLUDE__
#define __SPI_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

/* device ID */
typedef uint8_t spi_t;

#include <driver/spi.h>

#ifdef CONFIG_SPI_MAX_DEVICES
#define NR_SPI_DEVICES	CONFIG_SPI_MAX_DEVICES
#else
#define NR_SPI_DEVICES	1
#endif
#define INVALID_SPI_DID	NR_SPI_DEVICES

struct spi_device {
	uint8_t mode;
	uint32_t max_freq_khz;
	uint8_t chip;
};
typedef struct spi_device spi_device_t;

#define SPI_LSB		0x04
#define SPI_MSB		0x00
#define spi_order(mode)	(mode&SPI_LSB)

#define SPI_CPHA	0x01
#define SPI_CPOL	0x02
#define SPI_MODE_MASK	0x03
#define SPI_MODE_0	(0|0)
#define SPI_MODE_1	(0|SPI_CPHA)
#define SPI_MODE_2	(SPI_CPOL|0)
#define SPI_MODE_3	(SPI_CPOL|SPI_CPHA)
#define spi_mode(mode)	(mode&SPI_MODE_MASK)
#define INVALID_SPI_MODE	0x04

#ifdef CONFIG_SPI_MASTER
#ifdef SPI_HW_FREQ
#define SPI_FREQ_DEF	SPI_HW_FREQ
#else
#error "SPI bus frequency not defined"
#endif
#endif

void spi_write_byte(uint8_t byte);
uint8_t spi_read_byte(void);

spi_t spi_register_device(spi_device_t *dev);
void spi_select_device(spi_t spi);
#define spi_deselect_device()	spi_hw_deselect_chips();

#endif /* __SPI_H_INCLUDE__ */
