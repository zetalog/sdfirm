#ifndef __I2C_DPU_H_INCLUDE__
#define __I2C_DPU_H_INCLUDE__

#include <target/clk.h>

/* Required implementation specific definitions:
 *
 * DW_I2Cx_BASE: the base address of the I2C register block
 */
#define DW_I2C_BASE(n)		(I2C0_BASE + 0x100 * n)
#define DW_I2C_FREQ		APB_CLK_FREQ
#define DW_I2C_IRQ(n)		(IRQ_I2C0 + (n))

#ifdef CONFIG_DW_I2C
#include <driver/dw_i2c.h>
#ifndef ARCH_HAVE_I2C
#define ARCH_HAVE_I2C		1
#else
#error "Multiple I2C controller defined"
#endif
#endif

#ifdef ARCH_HAVE_I2C
#define I2C_HW_MAX_MASTERS		2
#define I2C_HW_MAX_SLAVES		1
#define i2c_hw_master_select(i2c)	dw_i2c_master_select(i2c)
#define i2c_hw_ctrl_init()				\
	do {						\
		clk_enable(srst_i2c0 + i2c_mid);	\
		dw_i2c_master_init();			\
	} while (0)
#define i2c_hw_set_address(addr, call)	dw_i2c_set_address(addr, call)
#define i2c_hw_set_frequency(khz)	dw_i2c_set_frequency(khz)
#define i2c_hw_start_condition(sr)	dw_i2c_start_condition(sr)
#define i2c_hw_stop_condition()		dw_i2c_stop_condition()
#define i2c_hw_read_byte()		dw_i2c_read_byte()
#define i2c_hw_write_byte(byte)		dw_i2c_write_byte(byte)
#define i2c_hw_transfer_reset()		dw_i2c_transfer_reset()
#define i2c_hw_handle_irq()		dw_i2c_handle_irq()
#ifndef SYS_REALTIME
#define i2c_hw_irq_init()		dw_i2c_irq_init()
#endif /* SYS_REALTIME */
#endif /* ARCH_HAVE_I2C */

#endif /* __I2C_DPU_H_INCLUDE__ */
