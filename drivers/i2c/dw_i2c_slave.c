#include <target/i2c.h>
#include <target/clk.h>
#include <target/console.h>
#include <target/irq.h>






uint8_t dw_i2c_slave_read_byte(void)
{

}

void dw_i2c_handle_irq(void)
{

}

void dw_i2c_irq_handler(irq_t irq)
{
	dw_i2c_slave_select(irq - IRQ_I2C0);
	dw_i2c_handle_irq();
}

void dw_i2c_irq_init(void)
{
	irqc_configure_irq(IRQ_I2C0 + dw_i2cd, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_I2C0 + dw_i2cd, dw_i2c_irq_handler);
	irqc_enable_irq(IRQ_I2C0 + dw_i2cd);
}

void __dw_i2c_slave_init(void)
{
	dw_i2c_ctrl_disable();
	__raw_writel((IC_CON_SLAVE_DISABLE & IC_CON_MASTER_MODE) |
			IC_CON_SPEED(IC_CON_SPEED_STD) | IC_CON_RESTART_EN,
			IC_CON(dw_i2cd));
	__raw_writel(IC_INTR_ALL, IC_INTR_MASK(dw_i2cd));
	dw_i2c_ctrl_enable();
	dw_i2c.state = DW_I2C_DRIVER_INIT;
}

void dw_i2c_slave_init(void)
{
	dw_i2c.addr_mode = 0;
	dw_i2c.state = DW_I2C_DRIVER_INVALID;
	__dw_i2c_slave_init();
	dw_i2c_irq_init();
}
