#include <target/uart.h>
#include <target/gpio.h>
#include <target/clk.h>
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
static void append_cr(int c)
{
	if (c == '\n')
		uart_hw_con_write('\r');
}
#else
#define append_cr(c)	do { } while (0)
#endif

#ifdef CONFIG_CONSOLE_OUTPUT
int putchar(int c)
{
	append_cr(c);
	uart_hw_con_write(c);
	return 1;
}
#endif

#ifdef CONFIG_CONSOLE_INPUT
int getchar(void)
{
	while (!uart_hw_con_poll());
	return (int)uart_hw_con_read();
}
#endif

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
void console_uart_handler(void)
{
	readline_async();
}

bool console_console_poll(void)
{
	return !!uart_hw_con_poll();
}

int console_input_init(void)
{
	readline_init();
	console_register_handler(console_uart_handler);
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

#ifdef CONFIG_CONSOLE_INPUT
#ifdef SYS_REALTIME
#define console_poll_init()	(irq_register_poller(console_bh))
#define console_irq_ack()	do { } while (0)
#define console_irq_init()	do { } while (0)
#else /* SYS_REALTIME */
#define console_poll_init()	do { } while (0)
#define console_irq_init()	uart_hw_irq_init()
#define console_irq_ack()	uart_hw_irq_ack()
#endif /* SYS_REALTIME */
#else /* CONFIG_CONSOLE_INPUT */
#define console_poll_init()	do { } while (0)
#define console_irq_init()	do { } while (0)
#define console_irq_ack()	do { } while (0)
#endif /* CONFIG_CONSOLE_INPUT */

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

void console_early_init(void)
{
	clk_hw_mmu_init();
	gpio_hw_mmu_init();
	uart_hw_mmu_init();
	console_enabled = true;
}

#define BANNER							\
	"   _____ _____  ______ _____ _____  __  __\n"		\
	"  / ____|  __ \\|  ____|_   _|  __ \\|  \\/  |\n"	\
	" | (___ | |  | | |__    | | | |__) | \\  / |\n"	\
	"  \\___ \\| |  | |  __|   | | |  _  /| |\\/| |\n"	\
	"  ____) | |__| | |     _| |_| | \\ \\| |  | |\n"	\
	" |_____/|_____/|_|    |_____|_|  \\_\\_|  |_|\n\n"

static bool console_banner_printed;

void console_init(void)
{
	console_output_init();
	if (!console_banner_printed) {
		printf("\n");
		printf("%s - %s\n",
		       CONFIG_VENDOR_NAME, CONFIG_PRODUCT_NAME);
		printf("%s - %s\n",
		       CONFIG_UNAME_RELEASE, CONFIG_KERNELVERSION);
		printf(BANNER);
		console_banner_printed = true;
	}
}

void console_late_init(void)
{
	console_input_init();
	console_bh = bh_register_handler(console_bh_handler);
	console_irq_init();
	console_poll_init();
}
