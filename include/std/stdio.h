#ifndef __STD_STDIO_H_INCLUDE__
#define __STD_STDIO_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>
#include <stdarg.h>

#ifndef __ASSEMBLY__
typedef void	*FILE;
#define stderr	NULL
#define stdout	NULL

#ifndef EOF
#define EOF	(-1)
#endif

__printf_chk(2) int sprintf(char *, const char *, ...);
__printf_chk(3) int snprintf(char *, size_t, const char *, ...);
int vsprintf(char *, const char *, va_list);
int vsnprintf(char *, size_t, const char *, va_list);
#define putch(c)	putchar(c)
#define getch()		getchar()
#ifdef CONFIG_CONSOLE_OUTPUT
int putchar(int c);
int puts(const char *s);
int gets(char *str, unsigned long len);
__printf_chk(1) int printf(const char *format, ...);
int vprintf(const char *format, va_list arg);
int vtxprintf(void (*tx_byte)(unsigned char byte, void *data),
	      const char *fmt, va_list args, void *data);
#else
static inline int putchar(int c)
{
	return -1;
}
static inline int puts(const char *s)
{
	return -1;
}
static inline int gets(char *str, unsigned long len)
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
#ifdef CONFIG_CONSOLE_INPUT
int getchar(void);
#else
static inline int getchar(void)
{
	return -1;
}
#endif /* CONFIG_CONSOLE_INPUT */
#else /* __ASSEMBLY__ */
#ifndef LINKER_SCRIPT
.extern getchar
.extern putchar
#endif
#endif /* __ASSEMBLY__ */

#endif /* __STD_STDIO_H_INCLUDE__ */
