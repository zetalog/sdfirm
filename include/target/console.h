#ifndef __CONSOLE_H_INCLUDE__
#define __CONSOLE_H_INCLUDE__

#ifndef __ASSEMBLY__
#include <target/uart.h>

#ifndef UART_CON_BAUDRATE /* Allow fixed baudrate */
#ifdef CONFIG_CONSOLE_4000000
#define UART_CON_BAUDRATE	4000000
#endif
#ifdef CONFIG_CONSOLE_3500000
#define UART_CON_BAUDRATE	3500000
#endif
#ifdef CONFIG_CONSOLE_3000000
#define UART_CON_BAUDRATE	3000000
#endif
#ifdef CONFIG_CONSOLE_2500000
#define UART_CON_BAUDRATE	2500000
#endif
#ifdef CONFIG_CONSOLE_2000000
#define UART_CON_BAUDRATE	2000000
#endif
#ifdef CONFIG_CONSOLE_1500000
#define UART_CON_BAUDRATE	1500000
#endif
#ifdef CONFIG_CONSOLE_1000000
#define UART_CON_BAUDRATE	1000000
#endif
#ifdef CONFIG_CONSOLE_500000
#define UART_CON_BAUDRATE	500000
#endif
#ifdef CONFIG_CONSOLE_1152000
#define UART_CON_BAUDRATE	1152000
#endif
#ifdef CONFIG_CONSOLE_921600
#define UART_CON_BAUDRATE	921600
#endif
#ifdef CONFIG_CONSOLE_576000
#define UART_CON_BAUDRATE	576000
#endif
#ifdef CONFIG_CONSOLE_460800
#define UART_CON_BAUDRATE	460800
#endif
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
extern int console_bh;
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
size_t hexstrtobin(const char *str, uint8_t *buf, size_t len);
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

void console_init(void);
void console_late_init(void);
__printf_chk(1) void con_err(const char *fmt, ...);
#else
#define console_bh				INVALID_BH
#define console_init()				do { } while (0)
#define console_late_init()			do { } while (0)
#define con_err(...)				do { } while (0)
#define hexstrtobin(str, buf, len)		0
#endif

#ifdef CONFIG_STRING_HEXDUMP
void hexdump(caddr_t addr, const void *data, uint8_t width, size_t count);
#else
#define hexdump(addr, data, width, count)	do { } while (0)
#endif

#ifdef CONFIG_MMU_IDMAP_DEVICE
#define idmap_early_con_init()		console_init()
#define fixmap_late_con_init()		do { } while (0)
#else
#define idmap_early_con_init()
#define fixmap_late_con_init()		console_init()
#endif
#endif /* !__ASSEMBLY__ */

#endif /* __CONSOLE_H_INCLUDE__ */
