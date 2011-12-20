#include <target/uart.h>

void uart_init(void)
{
	uart_hw_ctrl_init();
	uart_hw_ctrl_stop();
	uart_hw_set_params(UART_PARAMS, UART_BAUDRATE);
	uart_hw_ctrl_start();
}
