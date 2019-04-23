#ifndef __STD_STDIO_H_INCLUDE__
#define __STD_STDIO_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>
#include <stdarg.h>

#ifndef __ASSEMBLY__
int sprintf(char *, const char *, ...);
int snprintf(char *, size_t, const char *, ...);
int vsprintf(char *, const char *, va_list);
int vsnprintf(char *, size_t, const char *, va_list);
int getchar(void);
int putchar(int c);
#define putch(c)	putchar(c)
#define getch()		getchar()
#ifdef CONFIG_CONSOLE_OUTPUT
int puts(const char *s);
int printf(const char *format, ...);
int vprintf(const char *format, va_list arg);
#else
static inline int puts(const char *s)
{
	return -1;
}
static inline int printf(const char *format, ...)
{
	return -1;
}
static inline int vprintf(const char *format, va_list arg)
{
	return -1;
}
#endif /* CONFIG_CONSOLE_OUTPUT */
#else /* __ASSEMBLY__ */
.extern getchar
.extern putchar
#endif /* __ASSEMBLY__ */

#endif /* __STD_STDIO_H_INCLUDE__ */
