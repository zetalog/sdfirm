#include <target/sbi.h>
#include <target/spinlock.h>

DEFINE_SPINLOCK(sbi_print_lock);
static char sbi_print_buffer[CONSOLE_PRINT_BUFFER_SIZE];
static const struct sbi_platform *sbi_console_plat = NULL;

int sbi_vprintf(const char *fmt, va_list arg)
{
	int len, i, space;
	irq_flags_t flags;

	spin_lock_irqsave(&sbi_print_lock, flags);
	len = vsnprintf(sbi_print_buffer,
			sizeof(sbi_print_buffer), fmt, arg);
	space = 0;
	for (i = 0;
	     i < len && space < CONSOLE_PRINT_BUFFER_SIZE; i++) {
		sbi_platform_console_putc(sbi_console_plat,
					  sbi_print_buffer[i]);
		space++;
	}
	spin_unlock_irqrestore(&sbi_print_lock, flags);
	return len;
}

int sbi_printf(const char *fmt, ...)
{
	va_list arg;
	int len;

	va_start(arg, fmt);
	len = sbi_vprintf(fmt, arg);
	va_end(arg);
	return len;
}

int sbi_console_init(struct sbi_scratch *scratch)
{
	sbi_console_plat = sbi_platform_ptr(scratch);
	console_init();
	return 0;
}
