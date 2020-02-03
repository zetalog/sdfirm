#include <target/uart.h>
#include <target/gpio.h>
#include <target/bh.h>
#include <target/irq.h>
#include <target/console.h>
#include <target/readline.h>
#include <target/circbf.h>
#include <target/spinlock.h>

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

#ifdef CONFIG_CONSOLE_OUTPUT_CR
void append_cr(int c)
{
	if (c == '\n')
		uart_hw_con_write('\r');
}
#else
#define append_cr(c)	do { } while (0)
#endif

int putchar(int c)
{
	append_cr(c);
	uart_hw_con_write(c);
	return 1;
}

int getchar(void)
{
	while (!uart_hw_con_poll());
	return (int)uart_hw_con_read();
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

void console_input_handler(void)
{
	while (console_console_poll() && console_console_handler)
		console_console_handler();
}
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

#ifdef SYS_REALTIME
#define console_poll_init()	(irq_register_poller(console_bh))
#define console_irq_ack()
#define console_irq_init()
#else
#define console_poll_init()
#define console_irq_init()	uart_hw_irq_init()
#define console_irq_ack()	uart_hw_irq_ack()
#endif

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

DEFINE_SPINLOCK(console_lock);

static void console_bh_handler(uint8_t events)
{
	irq_flags_t flags;

	if (events == BH_POLLIRQ) {
		console_output_handler();
		console_input_handler();
		return;
	}
	if (console_events == CONSOLE_IRQ) {
		console_events = 0;
		spin_lock_irqsave(&console_lock, flags);
		console_input_handler();
		spin_unlock_irqrestore(&console_lock, flags);
		console_irq_ack();
	}
}

#ifdef CONFIG_MMU_IDMAP_DEVICE
bool console_enabled = true;
#else
bool console_enabled = false;
#endif

void con_printf(const char *fmt, ...)
{
	va_list arg;

	if (!console_enabled)
		return;

	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);
}

#ifdef CONFIG_CONSOLE_DEBUG
void con_dbg(const char *fmt, ...)
{
	va_list arg;

	if (!console_enabled)
		return;

	va_start(arg, fmt);
	vprintf(fmt, arg);
	va_end(arg);
}
#endif

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

void early_console_init(void)
{
	gpio_hw_mmu_init();
	uart_hw_mmu_init();
	console_enabled = true;
}

void console_init(void)
{
	console_output_init();
}

void late_console_init(void)
{
	console_input_init();
	console_bh = bh_register_handler(console_bh_handler);
	console_irq_init();
	console_poll_init();
	printf("Welcome to sdfirm\n");
}
