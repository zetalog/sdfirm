#include "mach.h"
#include <target/uart.h>

void uart_hw_dbg_write(uint8_t byte)
{
	sim_uart_write_byte(byte);
}

void uart_hw_dbg_start(void)
{
}

void uart_hw_dbg_stop(void)
{
}

void uart_hw_dbg_config(uint8_t params, uint32_t baudrate)
{
}

void uart_hw_dbg_init(void)
{
}
