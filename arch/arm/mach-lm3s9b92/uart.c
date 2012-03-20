#include <target/uart.h>

void __uart_hw_smart_start(uint8_t n)
{
	__raw_writel(__uart_hw_config_param(__UART_HW_SMART_PARAM),
					    UARTLCRH(n));
	__raw_writel(_BV(UARTEN) | _BV(SMART), UARTCTL(n));
}

void __uart_hw_smart_stop(uint8_t n)
{
	while (__raw_readl(UARTFR(n)) & _BV(BUSY))
		;
	__raw_clearl(_BV(UARTEN) | _BV(SMART), UARTCTL(n));
}

uint32_t __uart_hw_config_param(uint8_t params)
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

void __uart_hw_ctrl_disable(uint8_t n)
{
	while (__raw_readl(UARTFR(n)) & _BV(BUSY));
	/* disable the UART */
	__raw_clearl(_BV(UARTEN) | _BV(TXE) | _BV(RXE), UARTCTL(n));
}

void __uart_hw_ctrl_enable(uint8_t n, uint8_t params)
{
	/* disable the FIFO and BRK */
	__raw_writel(__uart_hw_config_param(params),
		     UARTLCRH(n));
	/* enable RX, TX, and the UART */
	__raw_writel(_BV(UARTEN) | _BV(TXE) | _BV(RXE), UARTCTL(n));
	/* clear the flags register */
	/* __raw_writel(0, UART0FR); */
}

static void __uart_hw_config_baudrate(uint8_t n, uint32_t baudrate)
{
	unsigned long cfg;
	/* is the required baud rate greater than the maximum rate */
	/* supported without the use of high speed mode? */
	if (baudrate > mul16u((uint16_t)div32u(CLK_SYS, 16), 1000)) {
		/* enable high speed mode */
		__raw_setl_atomic(HSE, UARTCTL(n));
		baudrate = div32u(baudrate, 2);
	} else {
		/* disable high speed mode */
		__raw_clearl_atomic(HSE, UARTCTL(n));
	}
	cfg = div32u(div32u((uint32_t)(mul32u(CLK_SYS, 20)),
			    div32u(baudrate, 400)) + 1, 2);
	/* set the baud rate */
	__raw_writel(cfg >> __UART_HW_FBRD_OFFSET, UARTIBRD(n));
	__raw_writel(cfg & __UART_HW_FBRD_MASK, UARTFBRD(n));
}

void __uart_hw_ctrl_config(uint8_t n,
			   uint8_t params,
			   uint32_t baudrate)
{
	__uart_hw_config_baudrate(n, baudrate);
	/* UARTLCRH write must follows UART(I|F)BRD writes */
	__raw_writel_mask(__uart_hw_config_param(params), 0xEE,
			  UARTLCRH(n));
}

void uart_hw_set_params(uint8_t params,
			uint32_t baudrate)
{
	__uart_hw_ctrl_config(0, params, baudrate);
}

#ifdef CONFIG_UART_SYNC
static inline void __uart_hw_config_pins(void)
{
	pm_hw_resume_device(DEV_GPIOA, DEV_MODE_ON);
	/* configure UART0 RX pin */
	gpio_hw_config_mux(GPIOA, 0, GPIOA0_MUX_U0RX);
	gpio_hw_config_pad(GPIOA, 0, GPIO_DIR_HW,
			   GPIO_PAD_PP, GPIO_DRIVE_2MA);
	/* configure UART0 TX pin */
	gpio_hw_config_mux(GPIOA, 1, GPIOA1_MUX_U0TX);
	gpio_hw_config_pad(GPIOA, 1, GPIO_DIR_HW,
			   GPIO_PAD_PP, GPIO_DRIVE_2MA);
}

void uart_hw_sync_write(uint8_t byte)
{
	if (pm_hw_device_mode(DEV_UART0) != DEV_MODE_OFF)
		__uart_hw_sync_write(byte, 0);
}

uint8_t uart_hw_sync_read(void)
{
	if (pm_hw_device_mode(DEV_UART0) != DEV_MODE_OFF)
		return __uart_hw_sync_read(0);
	return 0;
}

void uart_hw_sync_start(void)
{
	__uart_hw_ctrl_enable(0, UART_SYNC_PARAMS);
}

void uart_hw_sync_stop(void)
{
	__uart_hw_ctrl_disable(0);
}

void uart_hw_sync_init(void)
{
	pm_hw_resume_device(DEV_UART0, DEV_MODE_ON);
	__uart_hw_config_pins();
}
#endif

#ifdef CONFIG_UART_ASYNC
struct uart_hw_gpio uart_hw_gpios[NR_UART_PORTS] = {
	{__UART0_HW_DEV,
	 __UART0_HW_RX_PORT, __UART0_HW_RX_PIN, __UART0_HW_RX_MUX,
	 __UART0_HW_TX_PORT, __UART0_HW_TX_PIN, __UART0_HW_TX_MUX},
#if NR_UART_PORTS > 1
	{__UART1_HW_DEV,
	 __UART1_HW_RX_PORT, __UART1_HW_RX_PIN, __UART1_HW_RX_MUX,
	 __UART1_HW_TX_PORT, __UART1_HW_TX_PIN, __UART1_HW_TX_MUX},
#endif
#if NR_UART_PORTS > 2
	{__UART2_HW_DEV,
	 __UART2_HW_RX_PORT, __UART2_HW_RX_PIN, __UART2_HW_RX_MUX,
	 __UART2_HW_TX_PORT, __UART2_HW_TX_PIN, __UART2_HW_TX_MUX},
#endif
};

void uart_hw_async_init(void)
{
	uint8_t n;

	for (n = 0; n < NR_UART_PORTS; n++) {
		pm_hw_resume_device(uart_hw_gpios[n].dev, DEV_MODE_ON);
		/* configure UART0 RX pin */
		gpio_hw_config_mux(uart_hw_gpios[n].rx_port,
				   uart_hw_gpios[n].rx_pin,
				   uart_hw_gpios[n].rx_mux);
		gpio_hw_config_pad(uart_hw_gpios[n].rx_port,
				   uart_hw_gpios[n].rx_pin,
				   GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
		/* configure UART0 TX pin */
		gpio_hw_config_mux(uart_hw_gpios[n].tx_port,
				   uart_hw_gpios[n].tx_pin,
				   uart_hw_gpios[n].tx_mux);
		gpio_hw_config_pad(uart_hw_gpios[n].tx_port,
				   uart_hw_gpios[n].tx_pin,
				   GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
	}
}
#endif
