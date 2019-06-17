#include <target/generic.h>
#include <target/uart.h>

#ifdef CONFIG_CONSOLE_OUTPUT
void uart_hw_con_write(uint8_t byte)
{
	pl01x_write_byte(byte);
}
#endif

#ifdef CONFIG_CONSOLE_INPUT
bool uart_hw_con_poll(void)
{
	return pl01x_read_poll();
}

uint8_t uart_hw_con_read(void)
{
	return pl01x_read_byte();
}
#endif

void uart_hw_irq_init(void)
{
	pl01x_irq_init();
}

void uart_hw_irq_ack(void)
{
	pl01x_irq_ack();
}

void uart_hw_con_init(void)
{
	pl01x_con_init();

	/* initialize gpio controller */
	asm volatile(
		"ldr	x4, =0x1c010000\n"
		"ldr	w5, =0xc0700000\n"
		"str	wzr, [x4, #160]\n"
		"str	w5, [x4, #164]\n"
	);
}
