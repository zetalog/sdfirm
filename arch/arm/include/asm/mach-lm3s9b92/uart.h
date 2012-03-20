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
#define DRFE		8
#define DRPE		9
#define DRBE		10
#define DROE		11
#define __UARTRSR	0x004
#define REG_UARTRSR(n)	UART_REG(n, __UARTRSR)
#define __UARTECR	0x004
#define REG_UARTECR(n)	UART_REG(n, __UARTECR)
#define SRFE		0
#define SRPE		1
#define SRBE		2
#define SROE		3
#define __UARTFR	0x018
#define REG_UARTFR(n)	UART_REG(n, __UARTFR)
#define RI		8
#define TXFE		7
#define RXFF		6
#define TXFF		5
#define RXFE		4
#define BUSY		3
#define DCD		2
#define DSR		1
#define CTS		0
#define __UARTILPR	0x020
#define REG_UARTILPR(n)	UART_REG(n, __UARTILPR)
#define __UARTIBRD	0x024
#define REG_UARTIBRD(n)	UART_REG(n, __UARTIBRD)
#define __UARTFBRD	0x028
#define REG_UARTFBRD(n)	UART_REG(n, __UARTFBRD)
#define __UARTLCRH	0x02C
#define REG_UARTLCRH(n)	UART_REG(n, __UARTLCRH)
#define SPS		7
#define WLEN		5 /* width = 2bits */
#define FEN		4
#define STP2		3
#define EPS		2
#define PEN		1
#define BRK		0
#define __UARTCTL	0x030
#define REG_UARTCTL(n)	UART_REG(n, __UARTCTL)
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
#define __UARTIFLS	0x034
#define REG_UARTIFLS(n)	UART_REG(n, __UARTIFLS)
#define __UARTIM	0x038
#define REG_UARTIM(n)	UART_REG(n, __UARTIM)
#define __UARTRIS	0x03C
#define REG_UARTRIS(n)	UART_REG(n, __UARTRIS)
#define __UARTMIS	0x040
#define REG_UARTMIS(n)	UART_REG(n, __UARTMIS)
#define __UARTICR	0x044
#define REG_UARTICR(n)	UART_REG(n, __UARTICR)
#define __UARTDMACTL	0x048
#define REG_UARTDMACTL(n)	UART_REG(n, __UARTDMACTL)

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

static inline uint32_t __uart_hw_config_param(uint8_t params)
{
	uint32_t cfg;
	cfg = (uart_bits(params)-5) << WLEN;
	switch (uart_parity(params)) {
	case UART_PARITY_EVEN:
		cfg |= _BV(EPS);
	case UART_PARITY_ODD:
		cfg |= _BV(PEN);
		break;
	}
	if (uart_stopb(params))
		cfg |= _BV(STP2);
	return cfg;
}

static inline void __uart_hw_ctrl_disable(uint8_t n)
{
	while (__raw_readl(REG_UARTFR(n)) & _BV(BUSY));
	/* disable the UART */
	__raw_clearl(_BV(UARTEN) | _BV(TXE) | _BV(RXE), REG_UARTCTL(n));
}

static inline void __uart_hw_ctrl_enable(uint8_t params, uint8_t n)
{
	/* disable the FIFO and BRK */
	__raw_writel(__uart_hw_config_param(params),
		     REG_UARTLCRH(n));
	/* enable RX, TX, and the UART */
	__raw_writel(_BV(UARTEN) | _BV(TXE) | _BV(RXE), REG_UARTCTL(n));
	/* clear the flags register */
	/* __raw_writel(0, UART0FR); */
}

static void __uart_hw_config_baudrate(uint32_t baudrate)
{
	unsigned long cfg;
	/* is the required baud rate greater than the maximum rate */
	/* supported without the use of high speed mode? */
	if (baudrate > mul16u((uint16_t)div32u(CLK_SYS, 16), 1000)) {
		/* enable high speed mode */
		__raw_setl_atomic(HSE, REG_UARTCTL(0));
		baudrate = div32u(baudrate, 2);
	} else {
		/* disable high speed mode */
		__raw_clearl_atomic(HSE, REG_UARTCTL(0));
	}
	cfg = div32u(div32u((uint32_t)(mul32u(CLK_SYS, 20)),
			    div32u(baudrate, 400)) + 1, 2);
	/* set the baud rate */
	__raw_writel(cfg >> __UART_HW_FBRD_OFFSET, REG_UARTIBRD(0));
	__raw_writel(cfg & __UART_HW_FBRD_MASK, REG_UARTFBRD(0));
}

static inline void __uart_hw_ctrl_config(uint8_t params,
					 uint8_t n,
					 uint32_t baudrate)
{
	__uart_hw_config_baudrate(baudrate);
	/* UARTLCRH write must follows UART(I|F)BRD writes */
	__raw_writel_mask(__uart_hw_config_param(params), 0xEE,
			  REG_UARTLCRH(n));
}

static inline boolean __uart_hw_status_error(uint8_t n)
{
	return __raw_readl(REG_UARTRSR(n));
}

static inline void __uart_hw_write_byte(uint8_t byte, uint8_t n)
{
	while (__raw_readl(REG_UARTFR(n)) & _BV(TXFF))
		;
	__raw_writeb(byte, REG_UARTDR(n));
}

static inline uint8_t __uart_hw_read_byte(uint8_t n)
{
	while (__raw_readl(REG_UARTFR(n)) & _BV(RXFE))
		;
	return __raw_readb(REG_UARTDR(n));
}

static inline void __uart_hw_smart_start(uint8_t n)
{
	__raw_writel(__uart_hw_config_param(__UART_HW_SMART_PARAM),
					    REG_UARTLCRH(n));
	__raw_writel(_BV(UARTEN) | _BV(SMART), REG_UARTCTL(n));
}

static inline void __uart_hw_smart_stop(uint8_t n)
{
	while (__raw_readl(REG_UARTFR(n)) & _BV(BUSY))
		;
	__raw_clearl(_BV(UARTEN) | _BV(SMART), REG_UARTCTL(n));
}

static inline boolean __uart_hw_smart_empty(uint8_t n)
{
	return __raw_readl(REG_UARTFR(n)) & _BV(RXFE);
}

static inline uint8_t __uart_hw_smart_read(uint8_t n)
{
	return __raw_readb(REG_UARTDR(n));
}

void uart_hw_sync_init(void);
void uart_hw_sync_start(void);
void uart_hw_sync_stop(void);
void uart_hw_sync_write(uint8_t byte);
uint8_t uart_hw_sync_read(void);

void uart_hw_async_init(void);

void uart_hw_set_params(uint8_t params,
			uint8_t n, uint32_t baudrate);

#endif /* __UART_LM3S9B92_H_INCLUDE__ */
