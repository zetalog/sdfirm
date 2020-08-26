#ifndef __I2C_H_INCLUDE__
#define __I2C_H_INCLUDE__

#include <target/generic.h>

typedef uint8_t i2c_t;

#define NR_I2C_MASTERS		CONFIG_I2C_MAX_MASTERS
#define INVALID_I2C		NR_I2C_MASTERS

#include <driver/i2c.h>

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

struct i2c_device {
	i2c_io_cb iocb;
};
typedef struct i2c_device i2c_device_t;

#ifdef CONFIG_I2C_MASTER
void i2c_apply_frequency(void);
uint8_t i2c_master_write(i2c_addr_t slave, i2c_len_t txlen);
uint8_t i2c_master_read(i2c_addr_t slave, i2c_len_t rxlen);
void i2c_master_release(void);
void i2c_master_init(void);
#if CONFIG_I2C_MAX_MASTERS > 1
void i2c_master_select(i2c_t i2c);
#else
#define i2c_master_select(i2c)			do { } while (0)
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

/* called by device driver */
void i2c_register_device(i2c_device_t *dev);
uint8_t i2c_read_byte(void);
void i2c_write_byte(uint8_t byte);

/* called by bus controller driver */
void i2c_set_status(uint8_t status);
uint8_t i2c_bus_mode(void);
uint8_t i2c_dir_mode(void);
uint8_t i2c_bus_dir_mode(void);

extern i2c_t i2c_mid;

#endif /* __I2C_H_INCLUDE__ */
