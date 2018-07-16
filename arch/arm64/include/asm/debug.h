#ifndef __DCC_ARM64_H_INCLUDE__
#define __DCC_ARM64_H_INCLUDE__

#include <asm/reg.h>

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

#endif /* __DCC_ARM64_H_INCLUDE__ */
