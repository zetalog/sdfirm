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
#define UART(offset)	(UART_BASE + offset)
#define UART_REG(n, o)	(UART_BASE+((n)<<12)+(o))

#define __UARTDR	0x000
#define REG_UARTDR(n)	UART_REG(n, __UARTDR)
#define UARTDR(n)	UART(0x##n##000)
#define DRFE		8
#define DRPE		9
#define DRBE		10
#define DROE		11
#define UARTRSR(n)	UART(0x##n##004)
#define UARTECR(n)	UART(0x##n##004)
#define SRFE		0
#define SRPE		1
#define SRBE		2
#define SROE		3
#define __UARTFR	0x018
#define REG_UARTFR(n)	UART_REG(n, __UARTFR)
#define UARTFR(n)	UART(0x##n##018)
#define RI		8
#define TXFE		7
#define RXFF		6
#define TXFF		5
#define RXFE		4
#define BUSY		3
#define DCD		2
#define DSR		1
#define CTS		0
#define UARTILPR(n)	UART(0x##n##020)
#define UARTIBRD(n)	UART(0x##n##024)
#define UARTFBRD(n)	UART(0x##n##028)
#define UARTLCRH(n)	UART(0x##n##02C)
#define SPS		7
#define WLEN		5 /* width = 2bits */
#define FEN		4
#define STP2		3
#define EPS		2
#define PEN		1
#define BRK		0
#define __UARTCTL	0x030
#define REG_UARTCTL(n)	UART_REG(n, __UARTCTL)
#define UARTCTL(n)	UART(0x##n##030)
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
#define UARTIFLS(n)	UART(0x##n##034)
#define UARTIM(n)	UART(0x##n##038)
#define UARTRIS(n)	UART(0x##n##03C)
#define UARTMIS(n)	UART(0x##n##040)
#define UARTICR(n)	UART(0x##n##044)
#define UARTDMACTL(n)	UART(0x##n##048)

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

/* UART2 */
/* GPIO PIN D0 */
#define GPIOD0_MUX_U2RX		0x04
/* GPIO PIN D1 */
#define GPIOD1_MUX_U2TX		0x04

#define __UART0_HW_DEV		DEV_GPIOA
#define __UART0_HW_RX_PORT	GPIOA
#define __UART0_HW_RX_PIN	1
#define __UART0_HW_RX_MUX	GPIOA0_MUX_U0RX
#define __UART0_HW_TX_PORT	GPIOA
#define __UART0_HW_TX_PIN	0
#define __UART0_HW_TX_MUX	GPIOA1_MUX_U0TX

#ifdef CONFIG_UART_LM3S9B92_UART1_PA0
#define __UART1_HW_DEV		DEV_GPIOA
#define __UART1_HW_RX_PORT	GPIOA
#define __UART1_HW_RX_PIN	1
#define __UART1_HW_RX_MUX	GPIOA0_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOA
#define __UART1_HW_TX_PIN	0
#define __UART1_HW_TX_MUX	GPIOA0_MUX_U1TX
#endif

#ifdef CONFIG_UART_LM3S9B92_UART1_PD0
#define __UART2_HW_DEV		DEV_GPIOD
#define __UART2_HW_RX_PORT	GPIOD
#define __UART2_HW_RX_PIN	1
#define __UART2_HW_RX_MUX	GPIOD0_MUX_U2RX
#define __UART2_HW_TX_PORT	GPIOD
#define __UART2_HW_TX_PIN	0
#define __UART2_HW_TX_MUX	GPIOD0_MUX_U2TX
#endif

#define LM3S9B92_UART(n)						\
static inline uint32_t __uart##n##_hw_config_param(uint8_t params)	\
{									\
	uint32_t cfg;							\
	cfg = (uart_bits(params)-5) << WLEN;				\
	switch (uart_parity(params)) {					\
	case UART_PARITY_EVEN:						\
		cfg |= _BV(EPS);					\
	case UART_PARITY_ODD:						\
		cfg |= _BV(PEN);					\
		break;							\
	}								\
	if (uart_stopb(params)) {					\
		cfg |= _BV(STP2);					\
	}								\
	return cfg;							\
}									\
static inline void __uart##n##_hw_ctrl_disable(void)			\
{									\
	while (__raw_readl(UARTFR(n)) & _BV(BUSY));			\
	/* disable the UART */						\
	__raw_clearl(_BV(UARTEN) | _BV(TXE) | _BV(RXE), UARTCTL(n));	\
}									\
static inline void __uart##n##_hw_ctrl_enable(uint8_t params)		\
{									\
	/* disable the FIFO and BRK */					\
	__raw_writel(__uart##n##_hw_config_param(params),		\
		     UARTLCRH(n));					\
	/* enable RX, TX, and the UART */				\
	__raw_writel(_BV(UARTEN) | _BV(TXE) | _BV(RXE), UARTCTL(n));	\
	/* clear the flags register */					\
	/* __raw_writel(0, UART0FR); */					\
}									\
static void __uart##n##_hw_config_baudrate(uint32_t baudrate)		\
{									\
	unsigned long cfg;						\
	/* is the required baud rate greater than the maximum rate */	\
	/* supported without the use of high speed mode? */		\
	if (baudrate > mul16u((uint16_t)div32u(CLK_SYS, 16), 1000)) {	\
		/* enable high speed mode */				\
		__raw_setl_atomic(HSE, UARTCTL(0));			\
		baudrate = div32u(baudrate, 2);				\
	} else {							\
		/* disable high speed mode */				\
		__raw_clearl_atomic(HSE, UARTCTL(0));			\
	}								\
	cfg = div32u(div32u((uint32_t)(mul32u(CLK_SYS, 20)),		\
			    div32u(baudrate, 400)) + 1, 2);		\
	/* set the baud rate */						\
	__raw_writel(cfg >> __UART_HW_FBRD_OFFSET, UARTIBRD(0));	\
	__raw_writel(cfg & __UART_HW_FBRD_MASK, UARTFBRD(0));		\
}									\
static inline void __uart##n##_hw_ctrl_config(uint8_t params,		\
					      uint32_t baudrate)	\
{									\
	__uart##n##_hw_config_baudrate(baudrate);			\
	/* UARTLCRH write must follows UART(I|F)BRD writes */		\
	__raw_writel_mask(__uart##n##_hw_config_param(params), 0xEE,	\
			  UARTLCRH(n));					\
}									\
static inline boolean __uart##n##_hw_status_error(void)			\
{									\
	return __raw_readl(UARTRSR(n));					\
}									\
static inline void __uart##n##_hw_write_byte(uint8_t byte)		\
{									\
	while (__raw_readl(UARTFR(n)) & _BV(TXFF));			\
	__raw_writeb(byte, UARTDR(n));					\
}									\
static inline uint8_t __uart##n##_hw_read_byte(void)			\
{									\
	while (__raw_readl(UARTFR(n)) & _BV(RXFE));			\
	return __raw_readb(UARTDR(n));					\
}

#define LM3S9B92_UART_SMART(n)						\
LM3S9B92_UART(n)							\
static inline void __uart##n##_hw_smart_start(void)			\
{									\
	__raw_writel(__uart##n##_hw_config_param(__UART_HW_SMART_PARAM),\
						 UARTLCRH(n));		\
	__raw_writel(_BV(UARTEN) | _BV(SMART), UARTCTL(n));		\
}									\
static inline void __uart##n##_hw_smart_stop(void)			\
{									\
	while (__raw_readl(UARTFR(n)) & _BV(BUSY));			\
	__raw_clearl(_BV(UARTEN) | _BV(SMART), UARTCTL(n));		\
}									\
static inline boolean __uart##n##_hw_smart_empty(void)			\
{									\
	return __raw_readl(UARTFR(n)) & _BV(RXFE);			\
}									\
static inline uint8_t __uart##n##_hw_smart_read(void)			\
{									\
	return __raw_readb(UARTDR(n));					\
}

static inline void __uart_hw_ctrl_disable(uint8_t n)
{
	while (__raw_readl(REG_UARTFR(n)) & _BV(BUSY));
	/* disable the UART */
	__raw_clearl(_BV(UARTEN) | _BV(TXE) | _BV(RXE),
		     REG_UARTCTL(n));
}

void uart_hw_sync_init(void);
void uart_hw_sync_start(void);
void uart_hw_sync_stop(void);
void uart_hw_sync_write(uint8_t byte);
uint8_t uart_hw_sync_read(void);

void uart_hw_async_init(void);

void uart_hw_set_params(uint8_t params, uint32_t baudrate);

#endif /* __UART_LM3S9B92_H_INCLUDE__ */
