#ifndef __I2C_DPU_H_INCLUDE__
#define __I2C_DPU_H_INCLUDE__

#include <target/clk.h>

/* Required implementation specific definitions:
 *
 * DW_I2Cx_BASE: the base address of the I2C register block
 */
#define DW_I2C_BASE(n)		(I2C0_BASE + 0x100 * n)
#define DW_I2C_FREQ		APB_CLK_FREQ

#ifdef CONFIG_DW_I2C
#include <driver/dw_i2c.h>
#endif

void i2c_hw_ctrl_init(void);
void i2c_hw_set_frequency(uint16_t khz);
void i2c_hw_set_address(i2c_addr_t addr, boolean call);

void i2c_hw_start_condition(void);
void i2c_hw_stop_condition(void);
void i2c_hw_write_byte(uint8_t byte);
uint8_t i2c_hw_read_byte(void);
void i2c_hw_transfer_reset(void);
void i2c_hw_master_select(i2c_t i2c);

#endif /* __I2C_DPU_H_INCLUDE__ */
