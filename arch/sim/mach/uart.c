#include "mach.h"
#include <target/uart.h>

void uart_hw_write_byte(uint8_t byte)
{
	sim_uart_write_byte(byte);
}

uint8_t uart_hw_read_byte(void)
{
	return 0;
}

void uart_hw_ctrl_start(void)
{
}

void uart_hw_ctrl_stop(void)
{
}

void uart_hw_set_params(uint8_t params, uint32_t baudrate)
{
}

void uart_hw_ctrl_init(void)
{
}
