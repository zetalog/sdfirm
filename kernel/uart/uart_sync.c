#include <target/uart.h>

void uart_sync_init(void)
{
	uart_hw_sync_init();
	uart_hw_sync_stop();
	uart_hw_set_params(UART_PARAMS, UART_BAUDRATE);
	uart_hw_sync_start();
}
