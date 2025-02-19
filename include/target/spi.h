#ifndef __SPI_H_INCLUDE__
#define __SPI_H_INCLUDE__

#include <target/generic.h>

/* device ID */
typedef uint8_t spi_t;

#include <driver/spi.h>

#ifdef CONFIG_SPI_MAX_DEVICES
#define NR_SPI_DEVICES		CONFIG_SPI_MAX_DEVICES
#else
#define NR_SPI_DEVICES		1
#endif
#define INVALID_SPI_DID	NR_SPI_DEVICES

struct spi_device {
	uint8_t mode;
	uint32_t max_freq_khz;
	uint8_t chip;
};
typedef struct spi_device spi_device_t;

#define SPI_LSB			0x01
#define SPI_MSB			0x00
#define spi_order_OFFSET	2
#define spi_order_MASK		REG_1BIT_MASK
#define spi_order(mode)		_GET_FV(spi_order, mode)
#define SPI_ORDER(value)	_SET_FV(spi_order, value)

#define SPI_CPHA		0x01
#define SPI_CPOL		0x02
#define spi_mode_OFFSET		0
#define spi_mode_MASK		REG_2BIT_MASK
#define SPI_MODE_0		(0|0)
#define SPI_MODE_1		(0|SPI_CPHA)
#define SPI_MODE_2		(SPI_CPOL|0)
#define SPI_MODE_3		(SPI_CPOL|SPI_CPHA)
#define spi_mode(mode)		_GET_FV(spi_mode, mode)
#define SPI_MODE(value)		_SET_FV(spi_mode, value)
#define INVALID_SPI_MODE	0x04

/* Flash protocol */
#define SPI_FLASH_3WIRE		0x00 /* single data line */
#define SPI_FLASH_4WIRE		0x01 /* dual data line */
#define SPI_FLASH_6WIRE		0x02 /* quad data line */
#define spi_flash_OFFSET	3
#define spi_flash_MASK		REG_2BIT_MASK
#define spi_flash(mode)		_GET_FV(spi_flash, mode)
#define SPI_FLASH(value)	_SET_FV(spi_flash, value)

#define SPI_MAX_FREQ		SPI_HW_MAX_FREQ
#define SPI_MAX_FREQ_MHZ	(UL(1000) * SPI_MAX_FREQ)

void spi_write_byte(uint8_t byte);
uint8_t spi_read_byte(void);
#define spi_tx(byte)		spi_write_byte(byte)
#define spi_rx()		spi_read_byte()
uint8_t spi_txrx(uint8_t byte);

#ifdef CONFIG_SPI_MASTER
spi_t spi_register_device(spi_device_t *dev);
void spi_select_device(spi_t spi);
#define spi_deselect_device()	spi_hw_deselect_chips();
#else
#define spi_register_device(dev)	(-1)
#define spi_select_device(spi)		do { } while (0)
#define spi_deselect_device(spi)	do { } while (0)
#endif

#endif /* __SPI_H_INCLUDE__ */
