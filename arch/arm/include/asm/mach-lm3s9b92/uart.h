#ifndef __UART_LM3S9B92_H_INCLUDE__
#define __UART_LM3S9B92_H_INCLUDE__

#include <asm/mach/usart.h>

#ifdef CONFIG_UART_LM3S9B92
#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif
#endif

struct uart_hw_gpio {
	uint8_t gpio;
	uint8_t uart;
	uint8_t rx_port;
	uint8_t rx_pin;
	uint8_t rx_mux;
	uint8_t tx_port;
	uint8_t tx_pin;
	uint8_t tx_mux;
};
__TEXT_TYPE__(const struct uart_hw_gpio, uart_hw_gpio_t);

#define __UART0_HW_DEV_GPIO	DEV_GPIOA
#define __UART0_HW_DEV_UART	DEV_UART0
#define __UART0_HW_RX_PORT	GPIOA
#define __UART0_HW_RX_PIN	0
#define __UART0_HW_RX_MUX	GPIOA0_MUX_U0RX
#define __UART0_HW_TX_PORT	GPIOA
#define __UART0_HW_TX_PIN	1
#define __UART0_HW_TX_MUX	GPIOA1_MUX_U0TX

#define __UART1_HW_DEV_UART	DEV_UART1
#ifdef CONFIG_UART_LM3S9B92_UART1_PA0
#define __UART1_HW_DEV_GPIO	DEV_GPIOA
#define __UART1_HW_RX_PORT	GPIOA
#define __UART1_HW_RX_PIN	0
#define __UART1_HW_RX_MUX	GPIOA0_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOA
#define __UART1_HW_TX_PIN	1
#define __UART1_HW_TX_MUX	GPIOA1_MUX_U1TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART1_PB0
#define __UART1_HW_DEV_GPIO	DEV_GPIOB
#define __UART1_HW_RX_PORT	GPIOB
#define __UART1_HW_RX_PIN	0
#define __UART1_HW_RX_MUX	GPIOB0_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOB
#define __UART1_HW_TX_PIN	1
#define __UART1_HW_TX_MUX	GPIOB1_MUX_U1TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART1_PB4
#define __UART1_HW_DEV_GPIO	DEV_GPIOB
#define __UART1_HW_RX_PORT	GPIOB
#define __UART1_HW_RX_PIN	4
#define __UART1_HW_RX_MUX	GPIOB4_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOB
#define __UART1_HW_TX_PIN	5
#define __UART1_HW_TX_MUX	GPIOB5_MUX_U1TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART1_PC6
#define __UART1_HW_DEV_GPIO	DEV_GPIOC
#define __UART1_HW_RX_PORT	GPIOC
#define __UART1_HW_RX_PIN	6
#define __UART1_HW_RX_MUX	GPIOC6_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOC
#define __UART1_HW_TX_PIN	7
#define __UART1_HW_TX_MUX	GPIOC7_MUX_U1TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART1_PD0
#define __UART1_HW_DEV_GPIO	DEV_GPIOD
#define __UART1_HW_RX_PORT	GPIOD
#define __UART1_HW_RX_PIN	0
#define __UART1_HW_RX_MUX	GPIOD0_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOD
#define __UART1_HW_TX_PIN	1
#define __UART1_HW_TX_MUX	GPIOD1_MUX_U1TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART1_PD2
#define __UART1_HW_DEV_GPIO	DEV_GPIOD
#define __UART1_HW_RX_PORT	GPIOD
#define __UART1_HW_RX_PIN	2
#define __UART1_HW_RX_MUX	GPIOD2_MUX_U1RX
#define __UART1_HW_TX_PORT	GPIOD
#define __UART1_HW_TX_PIN	3
#define __UART1_HW_TX_MUX	GPIOD3_MUX_U1TX
#endif

#define __UART2_HW_DEV_UART	DEV_UART1
#ifdef CONFIG_UART_LM3S9B92_UART2_PD0
#define __UART2_HW_DEV_GPIO	DEV_GPIOD
#define __UART2_HW_RX_PORT	GPIOD
#define __UART2_HW_RX_PIN	0
#define __UART2_HW_RX_MUX	GPIOD0_MUX_U2RX
#define __UART2_HW_TX_PORT	GPIOD
#define __UART2_HW_TX_PIN	1
#define __UART2_HW_TX_MUX	GPIOD1_MUX_U2TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART2_PD5
#define __UART2_HW_DEV_GPIO	DEV_GPIOD
#define __UART2_HW_RX_PORT	GPIOD
#define __UART2_HW_RX_PIN	5
#define __UART2_HW_RX_MUX	GPIOD5_MUX_U2RX
#define __UART2_HW_TX_PORT	GPIOD
#define __UART2_HW_TX_PIN	6
#define __UART2_HW_TX_MUX	GPIOD6_MUX_U2TX
#endif
#ifdef CONFIG_UART_LM3S9B92_UART2_PG0
#define __UART2_HW_DEV_GPIO	DEV_GPIOG
#define __UART2_HW_RX_PORT	GPIOG
#define __UART2_HW_RX_PIN	0
#define __UART2_HW_RX_MUX	GPIOG0_MUX_U2RX
#define __UART2_HW_TX_PORT	GPIOG
#define __UART2_HW_TX_PIN	1
#define __UART2_HW_TX_MUX	GPIOG1_MUX_U2TX
#endif

#ifdef CONFIG_DEBUG_PRINT
void uart_hw_dbg_init(void);
void uart_hw_dbg_start(void);
void uart_hw_dbg_stop(void);
void uart_hw_dbg_write(uint8_t byte);
void uart_hw_dbg_config(uint8_t params, uint32_t baudrate);
#endif

#ifdef CONFIG_UART
void uart_hw_ctrl_init(void);
#endif

#endif /* __UART_LM3S9B92_H_INCLUDE__ */
