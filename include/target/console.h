#ifndef __CONSOLE_H_INCLUDE__
#define __CONSOLE_H_INCLUDE__

#ifndef __ASSEMBLY__
#include <target/uart.h>

#ifndef UART_CON_BAUDRATE /* Allow fixed baudrate */
#ifdef CONFIG_CONSOLE_230400
#define UART_CON_BAUDRATE	230400
#endif
#ifdef CONFIG_CONSOLE_115200
#define UART_CON_BAUDRATE	115200
#endif
#ifdef CONFIG_CONSOLE_76800
#define UART_CON_BAUDRATE	76800
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
#endif /* !UART_CON_BAUDRATE */

/* Default baudrate */
#ifndef UART_CON_BAUDRATE
#define UART_CON_BAUDRATE	115200
#endif /* !UART_CON_BAUDRATE */

#ifdef CONFIG_CONSOLE_PRINT_BUFFER_SIZE
#define CONSOLE_PRINT_BUFFER_SIZE	CONFIG_CONSOLE_PRINT_BUFFER_SIZE
#else
#define CONSOLE_PRINT_BUFFER_SIZE	128
#endif
#define CONSOLE_BUFFER_SIZE		512

typedef void (*console_handler)(void);

#ifdef CONFIG_CONSOLE
int console_register_handler(console_handler);
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
int console_output_space(void);
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

/* Console command line related */
void console_handle_irq(void);

void console_early_init(void);
void console_init(void);
void console_late_init(void);
void con_printf(const char *fmt, ...);
#else
#define console_early_init()		do { } while (0)
#define console_init()			do { } while (0)
#define console_late_init()		do { } while (0)
#define con_printf(...)			do { } while (0)
#endif

#ifdef CONFIG_CONSOLE_DEBUG
void con_dbg(const char *fmt, ...);
#else
#define con_dbg(...)			do { } while (0)
#endif

#ifdef CONFIG_MMU_IDMAP_DEVICE
#define idmap_early_con_init()		console_init()
#define fixmap_early_con_init()		console_early_init()
#define fixmap_late_con_init()		do { } while (0)
#else
#define idmap_early_con_init()
#define fixmap_early_con_init()		console_early_init()
#define fixmap_late_con_init()		console_init()
#endif
#endif /* !__ASSEMBLY__ */

#endif /* __CONSOLE_H_INCLUDE__ */
