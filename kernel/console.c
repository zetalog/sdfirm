#include <stdio.h>
#include <target/uart.h>
#include <target/bh.h>
#include <target/console.h>
#include <target/readline.h>
#include <target/circbf.h>

#define __console_output_space()	\
	circbf_space(&console_output_buffer, CONSOLE_BUFFER_SIZE)
#define __console_output_count()	\
	circbf_count(&debug_output_buffer, CONSOLE_BUFFER_SIZE)
#define __console_output_in(byte)	\
	circbf_in(&debug_output_buffer, CONSOLE_BUFFER_SIZE, byte);
#define __console_output_out(byte)	\
	circbf_out(&debug_output_buffer, CONSOLE_BUFFER_SIZE, byte);

DECLARE_CIRCBF16(console_output_buffer, CONSOLE_BUFFER_SIZE);
static console_handler console_console_handler = NULL;

#ifdef CONFIG_CONSOLE_OUTPUT
#define __putchar(c)	uart_hw_con_write(c)
#else
#define __putchar(c)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define __getchar(c)	uart_hw_con_read()
#else
#define __getchar(c)	0x00
#endif

int putchar(int c)
{
	__putchar(c);
	return 1;
}

int getchar(void)
{
	return (int)__getchar();
}

#ifdef CONFIG_CONSOLE_OUTPUT
int console_output_space(void)
{
	return CONSOLE_BUFFER_SIZE;
}

void console_output_handler(void)
{
}

int console_output_init(void)
{
	uart_hw_con_init();
	return 0;
}
#endif

#ifdef CONFIG_CONSOLE_INPUT
bool console_console_poll(void)
{
	return !!uart_hw_con_poll();
}

int console_input_init(void)
{
	return 0;
}

int console_irq_init(void)
{
	uart_hw_irq_init();
	return 0;
}

void console_irq_ack(void)
{
	uart_hw_irq_ack();
}

void console_input_handler(void)
{
	while (console_console_poll() && console_console_handler)
		console_console_handler();
}
#else
#define console_irq_ack()
#define console_irq_init()
#endif

int console_register_handler(console_handler handler)
{
	if (console_console_handler)
		return -EBUSY;

	console_console_handler = handler;
	return 0;
}

static int console_bh;
static uint8_t console_events;

#define CONSOLE_POLL		0x01
#define CONSOLE_IRQ		0x02

void console_raise_event(uint8_t event)
{
	console_events |= event;
	bh_resume(console_bh);
}

void console_handle_irq(void)
{
	console_raise_event(CONSOLE_IRQ);
}

static void console_bh_handler(uint8_t events)
{
	events = console_events;
	console_events = 0;

	if (events & CONSOLE_POLL) {
		console_output_handler();
		console_input_handler();
		console_raise_event(CONSOLE_POLL);
	}
	if (events & CONSOLE_IRQ) {
		console_input_handler();
		console_irq_ack();
	}
}

#ifdef CONFIG_CONSOLE_COMMAND
void console_uart_handler(void)
{
	readline_async();
}

void console_late_init(void)
{
	readline_init();
	console_register_handler(console_uart_handler);
}
#endif

void console_init(void)
{
	console_output_init();
	console_input_init();
	console_bh = bh_register_handler(console_bh_handler);
#ifndef CONFIG_SYS_POLL
	console_raise_event(CONSOLE_POLL);
#else
	console_irq_init();
#endif
}
