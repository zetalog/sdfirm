#ifndef __USART_LM3S9B92_H_INCLUDE__
#define __USART_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/clk.h>
#include <asm/mach/pm.h>
#include <asm/mach/irq.h>
#include <target/gpio.h>

#define UART_BASE	0x4000C000
#define UART(n, o)	(UART_BASE+((n)<<12)+(o))

#define UARTDR(n)	UART(n, 0x000)
#define DRFE		8
#define DRPE		9
#define DRBE		10
#define DROE		11
#define UARTRSR(n)	UART(n, 0x004)
#define UARTECR(n)	UART(n, 0x004)
#define SRFE		0
#define SRPE		1
#define SRBE		2
#define SROE		3
#define UARTFR(n)	UART(n, 0x018)
#define RI		8
#define TXFE		7
#define RXFF		6
#define TXFF		5
#define RXFE		4
#define BUSY		3
#define DCD		2
#define DSR		1
#define CTS		0
#define UARTILPR(n)	UART(n, 0x020)
#define UARTIBRD(n)	UART(n, 0x024)
#define UARTFBRD(n)	UART(n, 0x028)
#define UARTLCRH(n)	UART(n, 0x02C)
#define SPS		7
#define WLEN		5 /* width = 2bits */
#define FEN		4
#define STP2		3
#define EPS		2
#define PEN		1
#define BRK		0
#define UARTCTL(n)	UART(n, 0x030)
#define CTSEN		15
#define RTSEN		14
#define RTS		11
#define DTR		10
#define RXE		9
#define TXE		8
#define LBE		7
#define LIN		6
#define HSE		5
#define EOT		4
#define SMART		3
#define SIRLP		2
#define SIREN		1
#define UARTEN		0
#define UARTIFLS(n)	UART(n, 0x034)
#define UARTIM(n)	UART(n, 0x038)
#define UARTRIS(n)	UART(n, 0x03C)
#define UARTMIS(n)	UART(n, 0x040)
#define UARTICR(n)	UART(n, 0x044)
#define LME5I		15
#define LME1I		14
#define LMSBI		13
#define OEI		10		
#define BEI		9
#define PEI		8
#define FEI		7
#define RTI		6
#define TXI		5
#define RXI		4
#define DSRI		3
#define DCDI		2
#define CTSI		1
#define RII		0

#define UARTDMACTL(n)	UART(n, 0x048)

/* Since fractional part is based on the 64 (2^6) times of the BRD,
 * 6 is define as FBRD offset here.
 */
/* compute the baud rate divisor
 * BRD = IBRD + FBRD = SysClk / (ClkDiv * baudrate)
 *
 * we will compute 64*BRD+0.5 to get DIVINT & DIVFRAC value
 * DIVINT = (64*BRD+0.5) / 64 = BRD
 * DIVFRAC = (64*BRD+0.5) % 64
 *
 * 64*BRD+0.5 = ((64 * SysClk) / (ClkDiv * baudrate)) + 0.5
 *            = ((CLK_SYS * 64000) / (16 * baudrate)) + 0.5
 *            = ((CLK_SYS * 4000) / (baudrate)) + 0.5
 *            = (((CLK_SYS * 20) / (baudrate/400)) + 1) / 2
 */
#define __UART_HW_FBRD_OFFSET	6
#define __UART_HW_FBRD_MASK	((1<<__UART_HW_FBRD_OFFSET)-1)

/* UART0 */
/* GPIO PIN A0 */
#define GPIOA0_MUX_U0RX		0x01
/* GPIO PIN A1 */
#define GPIOA1_MUX_U0TX		0x01

/* UART1 */
/* GPIO PIN A0 */
#define GPIOA0_MUX_U1RX		0x09
/* GPIO PIN A1 */
#define GPIOA1_MUX_U1TX		0x09
/* GPIO PIN B0 */
#define GPIOB0_MUX_U1RX		0x05
/* GPIO PIN B1 */
#define GPIOB1_MUX_U1TX		0x05
/* GPIO PIN B4 */
#define GPIOB4_MUX_U1RX		0x07
/* GPIO PIN B5 */
#define GPIOB5_MUX_U1TX		0x07
/* GPIO PIN C6 */
#define GPIOC6_MUX_U1RX		0x05
/* GPIO PIN C7 */
#define GPIOC7_MUX_U1TX		0x05
/* GPIO PIN D0 */
#define GPIOD0_MUX_U1RX		0x05
/* GPIO PIN D1 */
#define GPIOD1_MUX_U1TX		0x05
/* GPIO PIN D2 */
#define GPIOD2_MUX_U1RX		0x01
/* GPIO PIN D3 */
#define GPIOD3_MUX_U1TX		0x01

/* UART2 */
/* GPIO PIN D0 */
#define GPIOD0_MUX_U2RX		0x04
/* GPIO PIN D1 */
#define GPIOD1_MUX_U2TX		0x04
/* GPIO PIN D5 */
#define GPIOD5_MUX_U2RX		0x09
/* GPIO PIN D6 */
#define GPIOD6_MUX_U2TX		0x09
/* GPIO PIN G0 */
#define GPIOG0_MUX_U2RX		0x01
/* GPIO PIN G1 */
#define GPIOG1_MUX_U2TX		0x01

#define __uart_hw_irq_enable(n, flags)	__raw_setl(flags, UARTIM(n))
#define __uart_hw_irq_disable(n, flags)	__raw_clearl(flags, UARTIM(n))
#define __uart_hw_irq_raised(n, flags)	(__raw_readl(UARTRIS(n)) & flags)
#define __uart_hw_irq_unraise(n, flags)	__raw_setl(flags, UARTICR(n))
#define __uart_hw_irq_status(n)		__raw_readl(UARTRIS(n))

#define __uart_hw_ctrl_enable(n)				\
	do {							\
		__raw_setl(_BV(FEN), UARTLCRH(n));		\
		__raw_setl(_BV(UARTEN), UARTCTL(n));		\
	} while (0)
#define __uart_hw_ctrl_disable(n)				\
	do {							\
		__raw_clearl(_BV(UARTEN), UARTCTL(n));		\
		while (__raw_readl(UARTFR(n)) & _BV(BUSY));	\
		__raw_clearl(_BV(FEN), UARTLCRH(n));		\
	} while (0)
#define __uart_hw_write_byte(n, byte)	__raw_writeb(byte, UARTDR(n))
#define __uart_hw_read_byte(n)		__raw_readb(UARTDR(n))
#define __uart_hw_write_full(n)		(__raw_readl(UARTFR(n)) & _BV(TXFF))
#define __uart_hw_read_empty(n)		(__raw_readl(UARTFR(n)) & _BV(RXFE))
void __uart_hw_ctrl_config(uint8_t n, uint8_t params, uint32_t baudrate);

#define __uart_hw_uart_enable(n)	__raw_setl(_BV(TXE) | _BV(RXE), UARTCTL(n))
#define __uart_hw_uart_disable(n)	__raw_clearl(_BV(TXE) | _BV(RXE), UARTCTL(n))
#define __uart_hw_set_break(n)		__raw_setl(_BV(BRK), UARTLCRH(n))
#define __uart_hw_clear_break(n)	__raw_clearl(_BV(BRK), UARTLCRH(n))
#define __uart_hw_status_error(n)	__raw_readl(UARTRSR(n))

#define __UART_HW_SMART_PARAM		\
	(8 | UART_PARITY_EVEN | UART_STOPB_TWO)
#define __uart_hw_ifd_enable(n)		__raw_setl(_BV(SMART), UARTCTL(n))
#define __uart_hw_ifd_disable(n)	__raw_clearl(_BV(SMART), UARTCTL(n))
#define __uart_hw_wt_enable(n)		__uart_hw_irq_enable(n, _BV(RTI))
#define __uart_hw_wt_disable(n)		__uart_hw_irq_disable(n, _BV(RTI))
#define __uart_hw_wt_raised(n)		__uart_hw_irq_raised(n, _BV(RTI))
#define __uart_hw_wt_unraise(n)		__uart_hw_irq_unrais(n, _BV(RTI))

#endif /* __USART_LM3S9B92_H_INCLUDE__ */
