#ifndef __SPI_H_INCLUDE__
#define __SPI_H_INCLUDE__

#include <target/generic.h>

typedef uint8_t spi_t;

#include <driver/spi.h>

#ifdef CONFIG_SPI_MAX_MASTERS
#define NR_SPI_MASTERS		CONFIG_SPI_MAX_MASTERS
#else
#define NR_SPI_MASTERS		1
#endif

#ifdef CONFIG_SPI_MAX_DEVICES
#define NR_SPI_DEVICES		CONFIG_SPI_MAX_DEVICES
#else
#define NR_SPI_DEVICES		1
#endif
#define INVALID_SPI_DID		NR_SPI_DEVICES

typedef uint16_t spi_event_t;

struct spi_device {
	uint8_t mode;
	uint32_t max_freq_khz;
	uint8_t chip;
	spi_io_cb iocb;
};
typedef struct spi_device spi_device_t;

struct spi_master {
	spi_t spi_last_id;
	uint8_t spi_last_mode;
	uint32_t last_freq;
	spi_len_t rxsubmit;
	spi_len_t txsubmit;
	spi_len_t limit;
	spi_len_t current;
	spi_len_t commit;

	uint8_t status;
	uint8_t state;
	spi_event_t event;
	spi_device_t *devices[NR_SPI_DEVICES];
};

#if NR_SPI_MASTERS > 1
extern struct spi_master spi_masters[NR_SPI_MASTERS];
extern spi_t spi_mid;

#define spi_last_id				spi_masters[spi_mid].spi_last_id
#define spi_last_mode				spi_masters[spi_mid].spi_last_mode
#define spi_last_freq				spi_masters[spi_mid].last_freq
#define spi_rxsubmit				spi_masters[spi_mid].rxsubmit
#define spi_txsubmit				spi_masters[spi_mid].txsubmit
#define spi_limit				spi_masters[spi_mid].limit
#define spi_current				spi_masters[spi_mid].current
#define spi_commit				spi_masters[spi_mid].commit
#define spi_status				spi_masters[spi_mid].status
#define spi_state				spi_masters[spi_mid].state
#define spi_event				spi_masters[spi_mid].event
#define spi_device[(n)]				spi_masters[spi_mid].device[n]

void spi_master_select(spi_t spi);
spi_t spi_master_save(spi_t spi);
#define spi_master_restore(spi)		spi_master_select(spi)
#else
extern spi_device_t *spi_devices[NR_SPI_DEVICES];
extern spi_t spi_last_mode;
extern spi_t spi_last_id;
extern uint8_t spi_chip;
extern uint32_t spi_max_freq_khz;
extern uint32_t spi_last_freq;
extern uint8_t spi_mode;
extern spi_len_t spi_rxsubmit;
extern spi_len_t spi_txsubmit;
extern spi_len_t spi_limit;
extern spi_len_t spi_current;
extern spi_len_t spi_commit;
extern uint8_t spi_status;
extern uint8_t spi_state;
extern spi_event_t spi_event;
extern spi_device_t *spi_device;

#define spi_mid				0
#define spi_master_save(spi)		0
#define spi_master_restore(spi)		do { } while (0)
#endif

#ifdef SPI_HW_FREQ
#define SPI_FREQ		SPI_HW_FREQ
#else
#define SPI_FREQ		300
#endif

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

#define SPI_MODE_SLAVE		0x00
#define SPI_MODE_MASTER		0x02
#define SPI_BUS_MASK		0x02
#define SPI_MODE_TX		0x00
#define SPI_MODE_RX		0x01
#define SPI_DIR_MASK		0x01

#define SPI_STATUS_IDLE		0x00
#define SPI_STATUS_READ		0x01
#define SPI_STATUS_WRITE	0x02

#define SPI_STATE_IDLE		0x00
#define SPI_STATE_READ		0x01
#define SPI_STATE_WRITE		0x02

#define SPI_EVENT_IDLE		_BV(SPI_STATUS_IDLE)
#define SPI_EVENT_READ		_BV(SPI_STATUS_READ)
#define SPI_EVENT_WRITE		_BV(SPI_STATUS_WRITE)

#define SPI_BUS(x)		(x & SPI_BUS_MASK)
#define SPI_DIR(x)		(x & SPI_DIR_MASK)
#define SPI_BUS_DIR(x)		(x & (SPI_BUS_MASK | SPI_DIR_MASK))
#define SPI_SET_BUS(x, b)	(x &= ~SPI_BUS_MASK, x |= b)
#define SPI_SET_DIR(x, d)	(x &= ~SPI_DIR_MASK, x |= d)
#define SPI_CALL(x)		(x & SPI_MODE_CALL)

#define SPI_ADDR_LEN		sizeof(uint8_t)

#define SPI_MODE_SLAVE_TX	(SPI_MODE_SLAVE | SPI_MODE_TX)
#define SPI_MODE_MASTER_TX	(SPI_MODE_MASTER | SPI_MODE_TX)
#define SPI_MODE_SLAVE_RX	(SPI_MODE_SLAVE | SPI_MODE_RX)
#define SPI_MODE_MASTER_RX	(SPI_MODE_MASTER | SPI_MODE_RX)

#define spi_addr_mode(addr, mode)	(((addr) << 1) | (mode))
#define spi_addr(addr_mode)		((addr_mode) >> 1)
#define spimode_mode(addr_mode)		((addr_mode) & 0x01)

void spi_write_byte(uint8_t byte);
uint8_t spi_read_byte(void);
void spi_submit_write(spi_len_t txlen);
void spi_submit_read(spi_len_t rxlen);
#define spi_tx(byte)		spi_write_byte(byte)
#define spi_rx()		spi_read_byte()
uint8_t spi_txrx(uint8_t byte);

void spi_raise_event(uint8_t event);
void spi_enter_state(uint8_t state);

void spi_sync_status(void);

void spi_set_status(uint8_t status);

spi_t spi_register_device(spi_device_t *dev);

#ifdef CONFIG_SPI_MASTER
void spi_select_device(spi_t spi);
#define spi_deselect_device()	spi_hw_deselect_chips();
#define spi_set_address(addr, call)
#define spi_apply_address()
#else
#define spi_select_device(spi)		do { } while (0)
#define spi_deselect_device(spi)	do { } while (0)
void spi_set_address(uint8_t addr, boolean call);
void spi_apply_address(void);
#endif

void spi_config_mode(uint8_t mode);

#endif /* __SPI_H_INCLUDE__ */
