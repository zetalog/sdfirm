#ifndef __DCC_ARM64_H_INCLUDE__
#define __DCC_ARM64_H_INCLUDE__

#include <asm/reg.h>

#ifdef CONFIG_ARM_DCC
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif
#endif

/* =================================================================
 * D.7.3 Debug registers
 * ================================================================= */
/* D.7.3.16 MDCCSR_EL0, Monitor DCC Status Register */
#define MDCCSR_TXFULL		_BV(29)
#define MDCCSR_RXFULL		_BV(30)

unsigned long __dcc_status(void);
void __dcc_write(unsigned long dbg_data);
unsigned long __dcc_read(void);

#define dcc_getchar_async()	__dcc_read()
#define dcc_putchar_async()	__dcc_write()
#define dcc_poll()		(__dcc_status() & MDCCSR_RXFULL)

int dcc_getchar(void);
int dcc_putchar(int c);

#ifdef CONFIG_CONSOLE
#define uart_hw_con_init()
#ifdef CONFIG_CONSOLE_OUTPUT
#define uart_hw_con_write(byte)	dcc_putchar(byte)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define uart_hw_con_read()	dcc_getchar()
#define uart_hw_con_poll()	dcc_poll()
#endif
#endif

#endif /* __DCC_ARM64_H_INCLUDE__ */
