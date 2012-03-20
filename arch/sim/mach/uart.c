#include "mach.h"
#include <target/uart.h>

void uart_hw_sync_write(uint8_t byte)
{
	sim_uart_write_byte(byte);
}

uint8_t uart_hw_sync_read(void)
{
	return 0;
}

void uart_hw_sync_start(void)
{
}

void uart_hw_sync_stop(void)
{
}

void uart_hw_sync_config(uint8_t params, uint32_t baudrate)
{
}

void uart_hw_sync_init(void)
{
}
