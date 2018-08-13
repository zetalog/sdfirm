#ifndef __PANIC_H_INCLUDE__
#define __PANIC_H_INCLUDE__

#include <target/config.h>

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

void console_init(void);
#else
#define console_init()
#endif

#ifdef CONFIG_DEBUG_PRINT
#ifdef CONFIG_DEBUG_115200
#define UART_DBG_BAUDRATE	115200
#endif
#ifdef CONFIG_DEBUG_57600
#define UART_DBG_BAUDRATE	57600
#endif
#ifdef CONFIG_DEBUG_38400
#define UART_DBG_BAUDRATE	38400
#endif
#ifdef CONFIG_DEBUG_19200
#define UART_DBG_BAUDRATE	19200
#endif
#ifdef CONFIG_DEBUG_9600
#define UART_DBG_BAUDRATE	9600
#endif
#ifdef CONFIG_DEBUG_4800
#define UART_DBG_BAUDRATE	4800
#endif
#ifdef CONFIG_DEBUG_2400
#define UART_DBG_BAUDRATE	2400
#endif
#ifndef UART_DBG_BAUDRATE
#define UART_DBG_BAUDRATE	115200
#endif

#define dbg_dump(x)		dbg_putchar(x)
void dbg_putchar(uint8_t val);
void dbg_print(uint8_t tag, uint8_t val);
void dbg_dump_str(const text_char_t *str);
void debug_init(void);
#else
#define dbg_dump(x)
#define dbg_dump_str(x)
#define dbg_print(tag, val)
#define debug_init()
#endif

#ifdef CONFIG_DEBUG_PANIC
/*
 * XXX: BUG Macros
 * No expression? Use BUG.
 * Expression shouldn't be executed when PANIC is disabled? Use BUG_ON.
 */
void dbg_panic(const text_char_t *file, int line);
#ifndef BUG
#define BUG()						\
	dbg_panic(__FILE__, __LINE__)
#endif
#ifndef BUG_ON
#define BUG_ON(expr)					\
	do {						\
		if (expr)				\
			dbg_panic(__FILE__, __LINE__);	\
	} while (0)
#endif
#else
#ifndef BUG
#define BUG()
#endif
#ifndef BUG_ON
#define BUG_ON(expr)
#endif
#endif

#include <target/dbg_event.h>

#ifdef CONFIG_IDLE_DEBUG
#define idle_debug(tag, val)		dbg_print((tag), (val))
#else
#define idle_debug(tag, val)
#endif

#ifdef CONFIG_PANIC_DEBUG
#define main_debug(tag, val)		dbg_print((tag), (val))
#else
#define main_debug(tag, val)
#endif

#endif /* __PANIC_H_INCLUDE__ */
