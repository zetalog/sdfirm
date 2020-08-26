#ifndef __I2C_DUOWEN_H_INCLUDE__
#define __I2C_DUOWEN_H_INCLUDE__

#include <target/clk.h>

/* Required implementation specific definitions:
 *
 * DW_I2Cx_BASE: the base address of the I2C register block
 */
#define DW_I2C_BASE(n)		(I2C0_BASE + 0x100000 * n)
#define DW_I2C_FREQ		SFAB_HALF_CLK_FREQ


#ifdef CONFIG_DW_I2C
#include <driver/dw_i2c.h>
#ifndef ARCH_HAVE_I2C
#define ARCH_HAVE_I2C		1
#else
#error "Multiple I2C controller defined"
#endif
#endif

#define i2c_hw_master_select(i2c)	dw_i2c_master_select(i2c)
#define i2c_hw_ctrl_init()				\
	do {						\
		clk_enable(i2c0_clk + i2c_mid);		\
		dw_i2c_master_init();			\
	} while (0)
#define i2c_hw_set_address(addr, call)	dw_i2c_set_address(addr, call)
#define i2c_hw_set_frequency(khz)	dw_i2c_set_frequency(khz)
#define i2c_hw_start_condition()	dw_i2c_start_condition()
#define i2c_hw_stop_condition()		dw_i2c_stop_condition()
#define i2c_hw_read_byte()		dw_i2c_read_byte()
#define i2c_hw_write_byte(byte)		dw_i2c_write_byte(byte)
#define i2c_hw_transfer_reset()		dw_i2c_transfer_reset()

#endif /* __I2C_DUOWEN_H_INCLUDE__ */
