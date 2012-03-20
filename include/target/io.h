#ifndef __IO_H_INCLUDE__
#define __IO_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>

#ifdef CONFIG_INPUT_NONE
#define io_getchar()	0
#else
uint8_t io_getchar(void);
#endif

#ifdef CONFIG_OUTPUT_NONE
#define io_putchar(val)
#else
void io_putchar(uint8_t val);
#endif

#endif /* __IO_H_INCLUDE__ */
