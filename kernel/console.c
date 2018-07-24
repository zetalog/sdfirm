#include <stdio.h>
#include <target/uart.h>

#ifdef CONFIG_CONSOLE_OUTPUT
#define __putchar(c)	uart_hw_con_write(c)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define __getchar(c)	uart_hw_con_read()
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
