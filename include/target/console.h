#ifndef __CONSOLE_H_INCLUDE__
#define __CONSOLE_H_INCLUDE__

#ifdef CONFIG_CONSOLE
#ifdef CONFIG_CONSOLE_115200
#define UART_CON_BAUDRATE	115200
#endif
#ifdef CONFIG_CONSOLE_57600
#define UART_CON_BAUDRATE	57600
#endif
#ifdef CONFIG_CONSOLE_38400
#define UART_CON_BAUDRATE	38400
#endif
#ifdef CONFIG_CONSOLE_19200
#define UART_CON_BAUDRATE	19200
#endif
#ifdef CONFIG_CONSOLE_9600
#define UART_CON_BAUDRATE	9600
#endif
#ifdef CONFIG_CONSOLE_4800
#define UART_CON_BAUDRATE	4800
#endif
#ifdef CONFIG_CONSOLE_2400
#define UART_CON_BAUDRATE	2400
#endif
#ifndef UART_CON_BAUDRATE
#define UART_CON_BAUDRATE	115200
#endif
#endif

#define CONSOLE_BUFFER_SIZE	512

typedef void (*console_handler)(void);

#ifdef CONFIG_CONSOLE
int console_register_handler(console_handler);
int console_output_space(void);
#ifdef CONFIG_CONSOLE_INPUT
void console_input_handler(void);
int console_input_init(void);
#else
#define console_input_handler()		do { } while (0)
static inline int console_input_init(void)
{
	return 0;
}
#endif

#ifdef CONFIG_CONSOLE_OUTPUT
void console_output_handler(void);
int console_output_init(void);
#else
#define console_output_handler()	do { } while (0)
static inline int console_output_init(void)
{
	return 0;
}
static inline int console_output_space(void)
{
	return 1;
}
#endif

void console_handle_irq(void);
void console_init(void);
void console_late_init(void);
#else
#define console_init()			do { } while (0)
#define console_late_init()		do { } while (0)
#endif

#endif /* __CONSOLE_H_INCLUDE__ */
