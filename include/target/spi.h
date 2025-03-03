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

#define SPI_MODE_CALL		0x80
#define SPI_MODE_SLAVE		0x00
#define SPI_MODE_MASTER		0x02
#define SPI_BUS_MASK		0x02
#define SPI_MODE_TX		0x00
#define SPI_MODE_RX		0x01
#define SPI_DIR_MASK		0x01

#define SPI_STATUS_IDLE		0x00
#define SPI_STATUS_START	0x01
#define SPI_STATUS_ACK		0x02
#define SPI_STATUS_NACK		0x03
#define SPI_STATUS_ARBI		0x04
#define SPI_STATUS_STOP		0x05

#define SPI_STATE_IDLE		0x00
#define SPI_STATE_WAIT		0x01
#define SPI_STATE_READ		0x02
#define SPI_STATE_WRITE		0x03

#define SPI_EVENT_IDLE		_BV(SPI_STATUS_IDLE)
#define SPI_EVENT_START		_BV(SPI_STATUS_START)
#define SPI_EVENT_PAUSE		_BV(SPI_STATUS_ACK)
#define SPI_EVENT_ABORT		_BV(SPI_STATUS_NACK)
#define SPI_EVENT_ARB		_BV(SPI_STATUS_ARBI)
#define SPI_EVENT_STOP		_BV(SPI_STATUS_STOP)

#define SPI_BUS(x)		(x & SPI_BUS_MASK)
#define SPI_DIR(x)		(x & SPI_DIR_MASK)
#define SPI_BUS_DIR(x)		(x & (SPI_BUS_MASK | SPI_DIR_MASK))
#define SPI_SET_BUS(x, b)	(x &= ~SPI_BUS_MASK, x |= b)
#define SPI_SET_DIR(x, d)	(x &= ~SPI_DIR_MASK, x |= d)
#define SPI_CALL(x)		(x & SPI_MODE_CALL)

#define SPI_MODE_SLAVE_TX	(SPI_MODE_SLAVE | SPI_MODE_TX)
#define SPI_MODE_MASTER_TX	(SPI_MODE_MASTER | SPI_MODE_TX)
#define SPI_MODE_SLAVE_RX	(SPI_MODE_SLAVE | SPI_MODE_RX)
#define SPI_MODE_MASTER_RX	(SPI_MODE_MASTER | SPI_MODE_RX)

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
