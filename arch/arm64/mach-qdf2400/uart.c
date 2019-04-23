#include <target/generic.h>
#include <target/uart.h>

#ifdef CONFIG_CONSOLE_OUTPUT
void uart_hw_con_write(uint8_t byte)
{
	if (UART_CON_ID < QUART_ACUART_BASE)
		uart_dm_write_byte(byte);
	else
		pl01x_write_byte(byte);
}
#endif

#ifdef CONFIG_CONSOLE_INPUT
bool uart_hw_con_poll(void)
{
	if (UART_CON_ID < QUART_ACUART_BASE)
		return uart_dm_read_poll();
	else
		return pl01x_read_poll();
}

uint8_t uart_hw_con_read(void)
{
	if (UART_CON_ID < QUART_ACUART_BASE)
		return uart_dm_read_byte();
	else
		return pl01x_read_byte();
}
#endif

void uart_hw_irq_init(void)
{
	if (UART_CON_ID < QUART_ACUART_BASE)
		uart_dm_irq_init();
	else
		pl01x_irq_init();
}

void uart_hw_irq_ack(void)
{
	if (UART_CON_ID < QUART_ACUART_BASE)
		uart_dm_irq_ack();
	else
		pl01x_irq_ack();
}

void uart_hw_con_init(void)
{
	/* Qualcomm SBSA PL01X implementation is based on QUART, so there's
	 * no split controller initialization.
	 */
	uart_dm_ctrl_init();
}
