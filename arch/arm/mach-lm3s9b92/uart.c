#include <target/uart.h>
#include <target/irq.h>

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

static void __uart_hw_config_params(uint8_t n, uint8_t params)
{
	unsigned long cfg;

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

	/* UARTLCRH write must follows UART(I|F)BRD writes */
	__raw_writel_mask(cfg, 0xEE, UARTLCRH(n));
}

void __uart_hw_ctrl_config(uint8_t n,
			   uint8_t params,
			   uint32_t baudrate)
{
	__uart_hw_config_baudrate(n, baudrate);
	__uart_hw_config_params(n, params);
}

#ifdef CONFIG_UART_SYNC
static inline void __uart_hw_config_pins(void)
{
	pm_hw_resume_device(DEV_GPIOA, DEV_MODE_ON);
	/* configure UART0 RX pin */
	gpio_config_mux(GPIOA, 0, GPIOA0_MUX_U0RX);
	gpio_config_pad(GPIOA, 0, GPIO_PAD_PP, 2);
	/* configure UART0 TX pin */
	gpio_config_mux(GPIOA, 1, GPIOA1_MUX_U0TX);
	gpio_config_pad(GPIOA, 1, GPIO_PAD_PP, 2);
}

void uart_hw_sync_write(uint8_t byte)
{
	if (pm_hw_device_mode(DEV_UART0) != DEV_MODE_OFF) {
		while (__uart_hw_write_full(0));
		__uart_hw_write_byte(0, byte);
	}
}

uint8_t uart_hw_sync_read(void)
{
	if (pm_hw_device_mode(DEV_UART0) != DEV_MODE_OFF) {
		while (__uart_hw_read_empty(0));
		return __uart_hw_read_byte(0);
	}
	return 0;
}

/* XXX: UARTCTL/UARTLCRH Register Access Ordering
 *
 * The UARTCTL register should not be changed while the UART is enabled or
 * else the results are unpredictable. The following sequence is
 * recommended for making changes to the UARTCTL register.
 * 1. Disable the UART.
 * 2. Wait for the end of transmission or reception of the current
 *    character.
 * 3. Flush the transmit FIFO by clearing bit 4 (FEN) in the line control
 *    register (UARTLCRH).
 * 4. Reprogram the control register.
 * 5. Enable the UART.
 */
void uart_hw_sync_stop(void)
{
	__uart_hw_uart_disable(0);
	__uart_hw_ctrl_disable(0);
}

void uart_hw_sync_start(void)
{
	__uart_hw_ctrl_enable(0);
	__uart_hw_uart_enable(0);
}

void uart_hw_sync_config(uint8_t params,
			 uint32_t baudrate)
{
	__uart_hw_ctrl_config(0, params, baudrate);
}

void uart_hw_sync_init(void)
{
	pm_hw_resume_device(DEV_UART0, DEV_MODE_ON);
	__uart_hw_config_pins();
}
#endif

#ifdef CONFIG_UART_ASYNC
uart_hw_gpio_t uart_hw_gpios[NR_UART_PORTS] = {
	{__UART0_HW_DEV_GPIO, __UART0_HW_DEV_UART,
	 __UART0_HW_RX_PORT, __UART0_HW_RX_PIN, __UART0_HW_RX_MUX,
	 __UART0_HW_TX_PORT, __UART0_HW_TX_PIN, __UART0_HW_TX_MUX},
#if NR_UART_PORTS > 1
	{__UART1_HW_DEV_GPIO, __UART1_HW_DEV_UART,
	 __UART1_HW_RX_PORT, __UART1_HW_RX_PIN, __UART1_HW_RX_MUX,
	 __UART1_HW_TX_PORT, __UART1_HW_TX_PIN, __UART1_HW_TX_MUX},
#endif
#if NR_UART_PORTS > 2
	{__UART2_HW_DEV_GPIO, __UART2_HW_DEV_UART,
	 __UART2_HW_RX_PORT, __UART2_HW_RX_PIN, __UART2_HW_RX_MUX,
	 __UART2_HW_TX_PORT, __UART2_HW_TX_PIN, __UART2_HW_TX_MUX},
#endif
};

static void __uart_hw_resume_port(uint8_t n)
{
	uart_hw_gpio_t *pins;

	pins = &uart_hw_gpios[n];
	/* enable UART port */
	pm_hw_resume_device(pins->uart, DEV_MODE_ON);
	/* enable GPIO for PIN configurations */
	pm_hw_resume_device(pins->gpio, DEV_MODE_ON);
	/* configure UART RX pin */
	gpio_config_mux(pins->rx_port, pins->rx_pin, pins->rx_mux);
	gpio_config_pad(pins->rx_port, pins->rx_pin, GPIO_PAD_PP, 2);
	/* configure UART TX pin */
	gpio_config_mux(pins->tx_port, pins->tx_pin, pins->tx_mux);
	gpio_config_pad(pins->tx_port, pins->tx_pin, GPIO_PAD_PP, 2);
}

static uart_pid_t __uart_hw_pids[NR_UART_PORTS];

static uint8_t __uart_hw_pid2port(uart_pid_t pid)
{
	uint8_t n;

	for (n = 0; n < NR_UART_PORTS; n++) {
		if (pid == __uart_hw_pids[n])
			break;
	}

	return n;
}

static void uart_hw_async_dummy(void)
{
}

static boolean uart_hw_rx_poll(void)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	return !__uart_hw_read_empty(n);
}

static boolean uart_hw_tx_poll(void)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	return !__uart_hw_write_full(n);
}

static void uart_hw_rx_getch(uint8_t *byte)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	*byte = __uart_hw_read_byte(n);
}

static void uart_hw_tx_putch(uint8_t *byte)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	__uart_hw_write_byte(n, *byte);
}

static void uart_hw_rx_start(void)
{
	uart_read_submit(uart_pid, 1);
}

static void uart_hw_tx_start(void)
{
	/* uart_write_submit(uart_pid, 1); */
}

static void uart_hw_handle_irq(void)
{
	uint8_t n;

	for (n = 0; n < NR_UART_PORTS; n++) {
		uint8_t pid = __uart_hw_pids[n];
		uint32_t ris;
		
		ris = __uart_hw_irq_status(n);
		__uart_hw_irq_unraise(n, ris);
		if (ris & _BV(TXI)) {
			__uart_hw_irq_unraise(n, _BV(TXI));
			uart_write_byte(pid);
		}
		if (ris & _BV(RXI)) {
			__uart_hw_irq_unraise(n, _BV(RXI));
			uart_read_byte(pid);
		}
	}
}

void uart_hw_async_start(void)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	__uart_hw_ctrl_enable(n);
	__uart_hw_uart_enable(n);
}

void uart_hw_async_stop(void)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	__uart_hw_uart_disable(n);
	__uart_hw_ctrl_disable(n);
}

#ifdef SYS_REALTIME
#define uart_hw_tx_open		uart_hw_async_dummy
#define uart_hw_tx_close	uart_hw_async_dummy
#define uart_hw_rx_open		uart_hw_async_dummy
#define uart_hw_rx_close	uart_hw_async_dummy
#define uart_hw_irq_init(n)

void uart_hw_irq_poll(void)
{
	uart_hw_handle_irq();
}
#else
static void uart_hw_tx_open(void)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	__uart_hw_irq_enable(n, _BV(TXI));
}

static void uart_hw_tx_close(void)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	__uart_hw_irq_disable(n, _BV(TXI));
}

static void uart_hw_rx_open(void)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	__uart_hw_irq_enable(n, _BV(RXI));
}

static void uart_hw_rx_close(void)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	__uart_hw_irq_disable(n, _BV(RXI));
}

static void __uart_hw_handle_irq(void)
{
	uart_hw_handle_irq();
}

uint8_t __uart_hw_port_irq(uint8_t n)
{
	if (n == 2)
		return IRQ_UART2;
	else
		return IRQ_UART0 + n;
}

void uart_hw_irq_init(uint8_t n)
{
	uint8_t irq;

	irq = __uart_hw_port_irq(n);
	vic_hw_register_irq(irq, __uart_hw_handle_irq);
	vic_hw_irq_enable(irq);
}
#endif

void uart_hw_async_config(uint8_t params,
			  uint32_t baudrate)
{
	uint8_t n = __uart_hw_pid2port(uart_pid);
	__uart_hw_ctrl_config(n, params, baudrate);
}

bulk_channel_t __uart_hw_tx = {
	O_WRCMPL,
	1,
	8,
	uart_hw_tx_open,
	uart_hw_tx_close,
	uart_hw_tx_start,
	uart_hw_async_dummy,
	uart_hw_async_dummy,
	uart_hw_async_dummy,
	uart_async_select,
	uart_hw_tx_poll,
	uart_hw_tx_putch,
	NULL,
};

bulk_channel_t __uart_hw_rx = {
	O_RDAVAL,
	1,
	8,
	uart_hw_rx_open,
	uart_hw_rx_close,
	uart_async_start,
	uart_async_stop,
	uart_async_halt,
	uart_hw_async_dummy,
	uart_async_select,
	uart_async_poll,
	uart_async_read,
	NULL,
};

static uart_port_t __uart_hw_port = {
	uart_hw_async_start,
	uart_hw_async_stop,
	uart_hw_async_config,
	uart_hw_rx_start,
	uart_hw_async_dummy,
	uart_hw_rx_getch,
	uart_hw_rx_poll,
	(bulk_channel_t *)(&__uart_hw_tx),
	(bulk_channel_t *)(&__uart_hw_rx),
};

void uart_hw_async_init(void)
{
	uint8_t n;

	for (n = 0; n < NR_UART_PORTS; n++) {
		/* resume UART power */
		__uart_hw_resume_port(n);
		/* register UART IRQ */
		uart_hw_irq_init(n);
		/* register UART port */
		__uart_hw_pids[n] = uart_register_port(&__uart_hw_port);
	}
}
#endif
