#ifndef __UART_LM3S9B92_H_INCLUDE__
#define __UART_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/clk.h>
#include <asm/mach/pm.h>
#include <asm/mach/gpio.h>

#ifdef CONFIG_UART_LM3S9B92
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif
#endif

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
#define UARTDMACTL(n)	UART(n, 0x048)

#define __UART_HW_SMART_PARAM	(8 | UART_PARITY_EVEN | UART_STOPB_TWO)

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

struct uart_hw_gpio {
	uint8_t dev;
	uint8_t rx_port;
	uint8_t rx_pin;
	uint8_t rx_mux;
	uint8_t tx_port;
	uint8_t tx_pin;
	uint8_t tx_mux;
};

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

#define __UART0_HW_DEV		DEV_GPIOA
#define __UART0_HW_RX_PORT	GPIOA
#define __UART0_HW_RX_PIN	0
#define __UART0_HW_RX_MUX	GPIOA0_MUX_U0RX
#define __UART0_HW_TX_PORT	GPIOA
#define __UART0_HW_TX_PIN	1
#define __UART0_HW_TX_MUX	GPIOA1_MUX_U0TX

#ifdef CONFIG_UART_LM3S9B92_UART1_PA0
#define __UART1_HW_DEV		DEV_GPIOA
#define __UART1_HW_RX_PORT	GPIOA
#define __UART1_HW_RX_PIN	0
#define __UART1_HW_RX_MUX	GPIOA0_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOA
#define __UART1_HW_TX_PIN	1
#define __UART1_HW_TX_MUX	GPIOA1_MUX_U1TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART1_PB0
#define __UART1_HW_DEV		DEV_GPIOB
#define __UART1_HW_RX_PORT	GPIOB
#define __UART1_HW_RX_PIN	0
#define __UART1_HW_RX_MUX	GPIOB0_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOB
#define __UART1_HW_TX_PIN	1
#define __UART1_HW_TX_MUX	GPIOB1_MUX_U1TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART1_PB4
#define __UART1_HW_DEV		DEV_GPIOB
#define __UART1_HW_RX_PORT	GPIOB
#define __UART1_HW_RX_PIN	4
#define __UART1_HW_RX_MUX	GPIOB4_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOB
#define __UART1_HW_TX_PIN	5
#define __UART1_HW_TX_MUX	GPIOB5_MUX_U1TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART1_PC6
#define __UART1_HW_DEV		DEV_GPIOC
#define __UART1_HW_RX_PORT	GPIOC
#define __UART1_HW_RX_PIN	6
#define __UART1_HW_RX_MUX	GPIOC6_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOC
#define __UART1_HW_TX_PIN	7
#define __UART1_HW_TX_MUX	GPIOC7_MUX_U1TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART1_PD0
#define __UART1_HW_DEV		DEV_GPIOD
#define __UART1_HW_RX_PORT	GPIOD
#define __UART1_HW_RX_PIN	0
#define __UART1_HW_RX_MUX	GPIOD0_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOD
#define __UART1_HW_TX_PIN	1
#define __UART1_HW_TX_MUX	GPIOD1_MUX_U1TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART1_PD2
#define __UART1_HW_DEV		DEV_GPIOD
#define __UART1_HW_RX_PORT	GPIOD
#define __UART1_HW_RX_PIN	2
#define __UART1_HW_RX_MUX	GPIOD2_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOD
#define __UART1_HW_TX_PIN	3
#define __UART1_HW_TX_MUX	GPIOD3_MUX_U1TX
#endif

#ifdef CONFIG_UART_LM3S9B92_UART2_PD0
#define __UART2_HW_DEV		DEV_GPIOD
#define __UART2_HW_RX_PORT	GPIOD
#define __UART2_HW_RX_PIN	0
#define __UART2_HW_RX_MUX	GPIOD0_MUX_U2RX
#define __UART2_HW_TX_PORT	GPIOD
#define __UART2_HW_TX_PIN	1
#define __UART2_HW_TX_MUX	GPIOD1_MUX_U2TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART2_PD5
#define __UART2_HW_DEV		DEV_GPIOD
#define __UART2_HW_RX_PORT	GPIOD
#define __UART2_HW_RX_PIN	5
#define __UART2_HW_RX_MUX	GPIOD5_MUX_U2RX
#define __UART2_HW_TX_PORT	GPIOD
#define __UART2_HW_TX_PIN	6
#define __UART2_HW_TX_MUX	GPIOD6_MUX_U2TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART2_PG0
#define __UART2_HW_DEV		DEV_GPIOG
#define __UART2_HW_RX_PORT	GPIOG
#define __UART2_HW_RX_PIN	0
#define __UART2_HW_RX_MUX	GPIOG0_MUX_U2RX
#define __UART2_HW_TX_PORT	GPIOG
#define __UART2_HW_TX_PIN	1
#define __UART2_HW_TX_MUX	GPIOG1_MUX_U2TX
#endif

void __uart_hw_ctrl_enable(uint8_t n, uint8_t params);
void __uart_hw_ctrl_disable(uint8_t n);
void __uart_hw_ctrl_config(uint8_t n,
			   uint8_t params,
			   uint32_t baudrate);
uint32_t __uart_hw_config_param(uint8_t params);

void __uart_hw_smart_start(uint8_t n);
void __uart_hw_smart_stop(uint8_t n);

static inline boolean __uart_hw_status_error(uint8_t n)
{
	return __raw_readl(UARTRSR(n));
}

static inline void __uart_hw_sync_write(uint8_t byte, uint8_t n)
{
	while (__raw_readl(UARTFR(n)) & _BV(TXFF))
		;
	__raw_writeb(byte, UARTDR(n));
}

static inline uint8_t __uart_hw_sync_read(uint8_t n)
{
	while (__raw_readl(UARTFR(n)) & _BV(RXFE))
		;
	return __raw_readb(UARTDR(n));
}

static inline boolean __uart_hw_smart_empty(uint8_t n)
{
	return __raw_readl(UARTFR(n)) & _BV(RXFE);
}

static inline uint8_t __uart_hw_smart_read(uint8_t n)
{
	return __raw_readb(UARTDR(n));
}

void uart_hw_sync_init(void);
void uart_hw_sync_start(void);
void uart_hw_sync_stop(void);
void uart_hw_sync_write(uint8_t byte);
uint8_t uart_hw_sync_read(void);

void uart_hw_async_init(void);

void uart_hw_set_params(uint8_t params,
			uint32_t baudrate);

#endif /* __UART_LM3S9B92_H_INCLUDE__ */
