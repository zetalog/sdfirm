#include <target/uart.h>
#include <target/irq.h>

static void pl01x_ctrl_config(uint8_t n, uint8_t params, uint32_t baudrate)
{
	pl01x_config_baudrate(n, baudrate);
	pl01x_config_params(n, params);
}

#ifdef CONFIG_DEBUG_PRINT
void uart_hw_dbg_write(uint8_t byte)
{
	while (pl01x_write_full(0));
	pl01x_write_data(0, byte);
}

void uart_hw_dbg_start(void)
{
	pl01x_ctrl_enable(0);
	pl01x_uart_enable(0);
}

void uart_hw_dbg_stop(void)
{
	pl01x_uart_disable(0);
	pl01x_ctrl_disable(0);
}

void uart_hw_dbg_config(uint8_t params, uint32_t baudrate)
{
	pl01x_ctrl_config(0, params, baudrate);
}

void uart_hw_dbg_init(void)
{
	pl01x_ctrl_disable_all(0);
}
#endif
