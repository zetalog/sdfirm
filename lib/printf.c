#include <stdio.h>
#include <target/uart.h>

#ifdef CONFIG_CONSOLE_PRINT_BUFFER_SIZE
#define CONSOLE_PRINT_BUFFER_SIZE	CONFIG_CONSOLE_PRINT_BUFFER_SIZE
#else
#define CONSOLE_PRINT_BUFFER_SIZE	128
#endif

#ifdef CONFIG_PRINT_VPRINTF
int vprintf(const char *fmt, va_list arg)
{
	int len, i, space;
	char print_buffer[CONSOLE_PRINT_BUFFER_SIZE];

	len = vsnprintf(print_buffer, sizeof(print_buffer), fmt, arg);
	space = 0;
	for (i = 0; i < len && space < CONSOLE_PRINT_BUFFER_SIZE; i++) {
		putchar(print_buffer[i]);
		space++;
	}
	return len;
}
#endif

#ifdef CONFIG_PRINT_PRINTF
int printf(const char *fmt, ...)
{
	va_list arg;
	int len;

	va_start(arg, fmt);
	len = vprintf(fmt, arg);
	va_end(arg);
	return len;
}
#endif

int puts(const char *s)
{
	int len = 0;

	while (*s) {
		len++;
		putchar(*s++);
	}
	return len;
}
