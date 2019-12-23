#ifndef __I2C_DRIVER_H_INCLUDE__
#define __I2C_DRIVER_H_INCLUDE__

#ifdef CONFIG_I2C_10BIT_ADDRESS
typedef uint16_t i2c_addr_t;
#else
typedef uint8_t i2c_addr_t;
#endif
typedef uint16_t i2c_len_t;
/* 24 bits device ID */
typedef uint32_t i2c_devid_t;

typedef void (*i2c_io_cb)(void);

#ifdef CONFIG_TWI_AT90SCR100
#include <asm/mach-at90scr100/twi.h>
#endif

#ifdef CONFIG_DW_I2C
#include <asm/mach-duowen/i2c.h>
#endif


#ifndef ARCH_HAVE_I2C
#define I2C_HW_FREQ		0

#define i2c_hw_ctrl_init()
#define i2c_hw_set_frequency(khz)
#define i2c_hw_set_address(addr, call)

#define i2c_hw_start_condition()
#define i2c_hw_stop_condition()
#define i2c_hw_write_byte(byte)
#define i2c_hw_read_byte()			0
#define i2c_hw_transfer_reset()
#endif

#endif /* __I2C_DRIVER_H_INCLUDE__ */
