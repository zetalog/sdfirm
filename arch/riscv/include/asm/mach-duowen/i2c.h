#ifndef __I2C_DUOWEN_H_INCLUDE__
#define __I2C_DUOWEN_H_INCLUDE__

#if defined(CONFIG_DW_I2C)
#include <driver/dw_i2c.h>

#ifndef ARCH_HAVE_I2C
#define ARCH_HAVE_I2C		1
#else
#error "Multiple I2C controller defined"
#endif


void i2c_hw_ctrl_init(void);
void i2c_hw_set_frequency(uint16_t khz);
void i2c_hw_set_address(i2c_addr_t addr, boolean call);

void i2c_hw_start_condition(void);
void i2c_hw_stop_condition(void);
void i2c_hw_write_byte(uint8_t byte);
uint8_t i2c_hw_read_byte(void);
void i2c_hw_transfer_reset(void);
#endif

#endif /* __I2C_DUOWEN_H_INCLUDE__ */
