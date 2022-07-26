//#include <types.h>
#include <std/stdint.h>
#include <io.h>
//include <sysma.h>

#define UART_BASE	0x83000000
#define UART_REG(n)	(UART_BASE + n)

#define THR		0x00
#define USR		0x7c
#define DLL		0x00
#define DLH		0x04
#define IER		0x04
#define FCR		0x08
#define LCR		0x0C
#define MCR		0x10

void dw_uart_init(void)
{
	// MCR: AFCE=0, RTS=0, DRT=0
	__raw_writel(0x0, UART_REG(MCR));
	__raw_writel(0xa3, UART_REG(LCR));

	// Set Baudrate
	__raw_writel(0x1, UART_REG(DLL));
	__raw_writel(0x0, UART_REG(DLH));

	__raw_writel(0x23, UART_REG(LCR));
	__raw_writel(0x0, UART_REG(FCR));

	__raw_writel(0x0, UART_REG(IER));
}

void console_putc(char str)
{
	volatile uint32_t status;

	do {
		status = __raw_readl(UART_REG(USR));
	} while ((status & 0x1) == 0x1);

	__raw_writel(str, (UART_REG(THR)));
}
