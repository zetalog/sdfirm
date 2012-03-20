#include <target/config.h>
#include <target/generic.h>
#include <target/arch.h>

#ifdef CONFIG_INPUT_UART
#include <target/uart.h>
uint8_t io_getchar(void)
{
	return uart_getchar();
}
#endif
#ifdef CONFIG_INPUT_KBD
#include <target/kbd.h>
uint8_t io_getchar(void)
{
	return kbd_getchar();
}
#endif

#ifdef CONFIG_OUTPUT_UART_SYNC
#include <target/uart.h>
void io_putchar(uint8_t val)
{
	uart_putchar(val);
}
#endif
#ifdef CONFIG_OUTPUT_LCD
#include <target/lcd.h>
void io_putchar(uint8_t val)
{
	lcd_putchar(val);
}
#endif

#ifdef CONFIG_DISPLAY_EZIO
extern void appl_ezio_init(void);
#else
#define appl_ezio_init()
#endif

void appl_init(void)
{
	appl_ezio_init();
}

