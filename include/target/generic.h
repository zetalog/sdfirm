#ifndef __GENERIC_H_INCLUDE__
#define __GENERIC_H_INCLUDE__

#include <target/compiler.h>
#include <target/types.h>
#include <target/const.h>
#include <target/sizes.h>

#ifndef __ASSEMBLY__
#include <target/bitops.h>
#include <target/list.h>
#include <target/string.h>
#include <target/muldiv.h>
#include <target/version.h>
#include <target/circbf.h>

void system_init(void);
void system_suspend(void);
void system_resume(void);

#include <target/devid.h>

#ifdef CONFIG_CONSOLE_VERBOSE
__printf_chk(1) void con_log(const char *fmt, ...);
#else
#define con_log(...)			do { } while (0)
#endif
#ifdef CONFIG_CONSOLE_DEBUG
__printf_chk(1) void con_dbg(const char *fmt, ...);
#else
#define con_dbg(...)			do { } while (0)
#endif

#define OPEN_READ		0x00
#define OPEN_WRITE		0x01
#endif /* __ASSEMBLY__ */

#endif /* __GENERIC_H_INCLUDE__ */
