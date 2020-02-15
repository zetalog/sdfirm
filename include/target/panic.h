#ifndef __PANIC_H_INCLUDE__
#define __PANIC_H_INCLUDE__

#include <target/generic.h>

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
#define BUG()						\
	dbg_panic(__FILE__, __LINE__)
#else
#define BUG()				do { } while (0)
#endif
#define BUG_ON(expr)					\
	do {						\
		if (expr) {				\
			BUG();				\
		}					\
	} while (0)

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

#include <target/console.h>

#endif /* __PANIC_H_INCLUDE__ */
