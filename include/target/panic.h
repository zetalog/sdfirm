#ifndef __PANIC_H_INCLUDE__
#define __PANIC_H_INCLUDE__

#include <target/config.h>
#include <target/io.h>

#ifdef CONFIG_DEBUG_PRINT
#define dbg_dump(x)		io_putchar(x)
void dbg_print(uint8_t tag, uint8_t val);
void dbg_dump_str(const text_char_t *str);
#else
#define dbg_dump(x)
#define dbg_dump_str(x)
#define dbg_print(tag, val)
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
