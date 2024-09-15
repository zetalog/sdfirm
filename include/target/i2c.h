#ifndef __I2C_H_INCLUDE__
#define __I2C_H_INCLUDE__

#include <target/generic.h>

typedef uint8_t i2c_t;

#include <driver/i2c.h>

#if CONFIG_I2C_MAX_MASTERS > I2C_HW_MAX_MASTERS
#define NR_I2C_MASTERS		I2C_HW_MAX_MASTERS
#else
#define NR_I2C_MASTERS		CONFIG_I2C_MAX_MASTERS
#endif
#define INVALID_I2C		NR_I2C_MASTERS

typedef uint16_t i2c_event_t;

struct i2c_device {
	i2c_io_cb iocb;
};
typedef struct i2c_device i2c_device_t;

struct i2c_master {
	i2c_addr_t target;
	i2c_addr_t address;
	uint16_t freq;
	uint8_t mode;

	/* TODO: RX/TX split transfer */
	i2c_len_t rxsubmit;
	i2c_len_t txsubmit;

	i2c_len_t limit;
	i2c_len_t current;
	i2c_len_t commit;

	uint8_t status;
	uint8_t state;
	i2c_event_t event;
	i2c_device_t *device;
	i2c_addr_t abrt_slave;
};

#if NR_I2C_MASTERS > 1
extern struct i2c_master i2c_masters[NR_I2C_MASTERS];
extern i2c_t i2c_mid;

#define i2c_target	i2c_masters[i2c_mid].target
#define i2c_address	i2c_masters[i2c_mid].address
#define i2c_freq	i2c_masters[i2c_mid].freq
#define i2c_mode	i2c_masters[i2c_mid].mode
#define i2c_txsubmit	i2c_masters[i2c_mid].txsubmit
#define i2c_rxsubmit	i2c_masters[i2c_mid].rxsubmit
#define i2c_limit	i2c_masters[i2c_mid].limit
#define i2c_current	i2c_masters[i2c_mid].current
#define i2c_commit	i2c_masters[i2c_mid].commit
#define i2c_status	i2c_masters[i2c_mid].status
#define i2c_device	i2c_masters[i2c_mid].device
#define i2c_state	i2c_masters[i2c_mid].state
#define i2c_event	i2c_masters[i2c_mid].event
#define i2c_abrt_slave	i2c_masters[i2c_mid].abrt_slave

void i2c_master_select(i2c_t i2c);
i2c_t i2c_master_save(i2c_t i2c);
#define i2c_master_restore(i2c)		i2c_master_select(i2c)
#else
extern i2c_addr_t i2c_target;
extern i2c_addr_t i2c_address;
extern uint16_t i2c_freq;
extern uint8_t i2c_mode;
extern i2c_len_t i2c_txsubmit;
extern i2c_len_t i2c_rxsubmit;
extern i2c_len_t i2c_limit;
extern i2c_len_t i2c_current;
extern i2c_len_t i2c_commit;
extern uint8_t i2c_status;
extern uint8_t i2c_state;
extern i2c_event_t i2c_event;
extern i2c_device_t *i2c_device;
extern i2c_addr_t i2c_abrt_slave;

#define i2c_mid				0
#define i2c_master_save(i2c)		0
#define i2c_master_restore(i2c)		do { } while (0)
#endif

#ifdef I2C_HW_FREQ
#define I2C_FREQ		I2C_HW_FREQ
#else
#define I2C_FREQ		300
#endif

#define I2C_ADDR_CALL		0x00
#define I2C_ADDR_DEFAUL		0x01
#define I2C_ADDR_OFFSET		1
#define I2C_ADDR_LEN		sizeof(i2c_addr_t)
#ifdef CONFIG_I2C_10BIT_ADDRESS
#else
#define I2C_ADDR_MASK		0xFE
#endif
#define I2C_ADDR_DEVID		0x7C	/* 1111100 */

#define I2C_DEVID_INVAL		0x000000
#define I2C_DEVID_LEN		3

#define I2C_CALL_PROG_RESET	0x06
#define I2C_CALL_PROG		0x04
#define I2C_CALL_INVAL		0x00

#define I2C_MODE_CALL		0x80
#define I2C_MODE_SLAVE		0x00
#define I2C_MODE_MASTER		0x02
#define I2C_BUS_MASK		0x02
#define I2C_MODE_TX		0x00
#define I2C_MODE_RX		0x01
#define I2C_DIR_MASK		0x01

#define I2C_STATUS_IDLE		0x00
#define I2C_STATUS_START	0x01
#define I2C_STATUS_ACK		0x02
#define I2C_STATUS_NACK		0x03
#define I2C_STATUS_ARBI		0x04
#define I2C_STATUS_STOP		0x05
#define I2C_STATUS_RX_AVAL	0x06
#define I2C_STATUS_TX_AVAL	0x07
#define I2C_STATUS_RX_CMPL	0x08
#define I2C_STATUS_TX_CMPL	0x09

#define I2C_STATE_IDLE		0x00
#define I2C_STATE_WAIT		0x01
#define I2C_STATE_READ		0x02
#define I2C_STATE_WRITE		0x03

#define I2C_EVENT_IDLE		_BV(I2C_STATUS_IDLE)
#define I2C_EVENT_START		_BV(I2C_STATUS_START)
#define I2C_EVENT_PAUSE		_BV(I2C_STATUS_ACK)
#define I2C_EVENT_ABORT		_BV(I2C_STATUS_NACK)
#define I2C_EVENT_ARB		_BV(I2C_STATUS_ARBI)
#define I2C_EVENT_STOP		_BV(I2C_STATUS_STOP)
#define I2C_EVENT_RX_AVAL	_BV(I2C_STATUS_RX_AVAL)
#define I2C_EVENT_TX_AVAL	_BV(I2C_STATUS_TX_AVAL)
#define I2C_EVENT_RX_CMPL	_BV(I2C_STATUS_RX_CMPL)
#define I2C_EVENT_TX_CMPL	_BV(I2C_STATUS_TX_CMPL)

#define I2C_BUS(x)		(x & I2C_BUS_MASK)
#define I2C_DIR(x)		(x & I2C_DIR_MASK)
#define I2C_BUS_DIR(x)		(x & (I2C_BUS_MASK | I2C_DIR_MASK))
#define I2C_SET_BUS(x, b)	(x &= ~I2C_BUS_MASK, x |= b)
#define I2C_SET_DIR(x, d)	(x &= ~I2C_DIR_MASK, x |= d)
#define I2C_CALL(x)		(x & I2C_MODE_CALL)

#define I2C_MODE_SLAVE_TX	(I2C_MODE_SLAVE | I2C_MODE_TX)
#define I2C_MODE_MASTER_TX	(I2C_MODE_MASTER | I2C_MODE_TX)
#define I2C_MODE_SLAVE_RX	(I2C_MODE_SLAVE | I2C_MODE_RX)
#define I2C_MODE_MASTER_RX	(I2C_MODE_MASTER | I2C_MODE_RX)

#ifdef CONFIG_I2C_MASTER
void i2c_apply_frequency(void);
uint8_t i2c_master_write(i2c_addr_t slave, i2c_len_t txlen);
uint8_t i2c_master_read(i2c_addr_t slave, i2c_len_t rxlen);
void i2c_master_submit(i2c_addr_t slave,
		       i2c_len_t txlimit, i2c_len_t rxlimit);
void i2c_master_release(void);
void i2c_master_init(void);
#define i2c_set_frequency(khz) i2c_hw_set_frequency(khz)
#if CONFIG_I2C_MAX_MASTERS > 1
void i2c_master_select(i2c_t i2c);
extern i2c_t i2c_mid;
#else
#define i2c_master_select(i2c)			do { } while (0)
#define i2c_mid					0
#endif
#ifdef CONFIG_I2C_GENERAL_CALL
uint8_t i2c_general_call(uint8_t cbyte, i2c_len_t limit);
#else
#define i2c_general_call(cbyte, limit)		I2C_STATUS_ARBI
#endif
#ifdef CONFIG_I2C_DEVICE_ID
uint32_t i2c_probe_devid(i2c_addr_t slave);
#else
#define i2c_probe_devid(slave)			I2C_DEVID_INVAL
#endif
#else
#define i2c_apply_frequency()
#define i2c_master_write(slave, txlen)		I2C_STATUS_ARBI
#define i2c_master_read(slave, rxlen)		I2C_STATUS_ARBI
#define i2c_set_frequency(khz)			do { } while (0)
#define i2c_general_call(cbyte, limit)		I2C_STATUS_ARBI
#define i2c_master_release()			do { } while (0)
#define i2c_master_select(i2c)			do { } while (0)
#define i2c_probe_devid(slave)			I2C_DEVID_INVAL
#endif

#ifdef CONFIG_I2C_SLAVE
void i2c_set_address(i2c_addr_t addr, boolean call);
void i2c_apply_address(void);
#else
#define i2c_set_address(addr, call)
#define i2c_apply_address()
#endif

void i2c_raise_event(uint8_t event);
void i2c_enter_state(uint8_t state);

/* called by device driver */
void i2c_register_device(i2c_device_t *dev);
uint8_t i2c_read_byte(void);
void i2c_write_byte(uint8_t byte);
bool i2c_last_byte(void);
void i2c_read_bytes(uint8_t *buf, i2c_len_t len);
void i2c_write_bytes(uint8_t *buf, i2c_len_t len);

void i2c_tx_aval(void);

/* called by bus controller driver */
void i2c_set_status(uint8_t status);
uint8_t i2c_bus_mode(void);
uint8_t i2c_dir_mode(void);
uint8_t i2c_bus_dir_mode(void);

void i2c_master_submit_async(i2c_addr_t slave, i2c_len_t txlen, i2c_len_t rxlen);
void i2c_master_commit(i2c_len_t len);
void i2c_master_abort(i2c_addr_t slave);
void i2c_master_start(void);
void i2c_master_stop(void);

#endif /* __I2C_H_INCLUDE__ */
