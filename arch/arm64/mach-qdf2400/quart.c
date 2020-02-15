#include <target/uart.h>
#include <target/irq.h>
#include <target/gpio.h>
#include <target/clk.h>
#include <target/console.h>
#include <target/panic.h>

#define UART_DM_QUART_TX_FIFO_SIZE	256
#define UART_DM_ACUART_TX_FIFO_SIZE	64

#define UART_DM_CLK_SRC0_FREQ		UL(20000000)
#define UART_DM_CLK_SRC1_FREQ		UL(1000000000)
#ifdef CONFIG_QUART_AMPLIFIED_BAUDRATE
#define UART_DM_BAUDRATE		\
	(UART_DM_CLK_SRC1_FREQ/UART_DM_CLK_SEL_DIV_MIN)
#else
#define UART_DM_BAUDRATE		UART_CON_BAUDRATE
#endif

#define UART_DM_NR_CLOCK_DIVISORS	16

uint16_t uart_dm_clk_divs[UART_DM_NR_CLOCK_DIVISORS] = {
	24576,
	12288,
	6144,
	3072,
	1536,
	768,
	512,
	384,
	256,
	192,
	128,
	96,
	64,
	48,
	32,
	16,
};

static uint8_t __uart_dm_select_div(uint32_t baudrate, uint32_t hz)
{
	uint8_t i, idiv = UART_DM_NR_CLOCK_DIVISORS;

	for (i = UART_DM_NR_CLOCK_DIVISORS; i > 0; i--) {
		/* MND div_max is same as MND_MASK */
		if ((hz / uart_dm_clk_divs[i - 1] >= baudrate) &&
		    ((hz / (baudrate * uart_dm_clk_divs[i - 1])) <=
		     (RCG_HID_DIV_MAX * PCC_UART_MND_MASK))) {
			idiv = i - 1;
			break;
		}
	}
	return idiv;
}

#define QDF2400_UART(n)							\
static inline void __uart##n##_dm_ack_irq(void)				\
{									\
	irq_t uart_irq = UART_DM_IRQ(n);				\
	irqc_enable_irq(uart_irq);					\
}									\
static inline void __uart##n##_dm_handle_irq(void)			\
{									\
	irq_t uart_irq = UART_DM_IRQ(n);				\
	irqc_disable_irq(uart_irq);					\
	console_handle_irq();						\
}									\
static inline void __uart##n##_dm_config_irq(void)			\
{									\
	irq_t uart_irq = UART_DM_IRQ(n);				\
	irqc_configure_irq(uart_irq, 0, IRQ_LEVEL_TRIGGERED);		\
	irq_register_vector(uart_irq, __uart##n##_dm_handle_irq);	\
	irqc_enable_irq(uart_irq);					\
}									\
static inline void __uart##n##_dm_config_clock(uint32_t __hz,		\
					       uint32_t __input_hz)	\
{									\
	uint8_t __mode, __clk;						\
	if (__input_hz == UART_DM_CLK_SRC1_FREQ) {			\
		__clk = 1;						\
		__mode = RCG_MODE_BYPASS;				\
	} else {							\
		__clk = 0;						\
		__mode = RCG_MODE_DUAL_EDGE;				\
	}								\
	__clk_generate_mnd(PCC_UART_CMD_RCGR(UART_DM_PCC_UART(n)),	\
			   __clk, __input_hz, __hz,			\
			   __mode, PCC_UART_MND_MASK);			\
	__clk_enable_root(PCC_UART_CMD_RCGR(UART_DM_PCC_UART(n)));	\
	__clk_update_root(PCC_UART_CMD_RCGR(UART_DM_PCC_UART(n)));	\
}									\
static inline void __uart##n##_dm_config_baudrate(uint32_t __baudrate)	\
{									\
	uint8_t __sel;							\
	uint32_t __hz = UART_DM_CLK_SRC0_FREQ;				\
	if (__baudrate > __hz)						\
		__hz = UART_DM_CLK_SRC1_FREQ;				\
	__sel = __uart_dm_select_div(__baudrate, __hz);			\
	__uart##n##_dm_config_clock(__baudrate *			\
				    uart_dm_clk_divs[__sel],		\
				    __hz);				\
	__raw_writel(UART_DM_RX_CLK_SEL(__sel) |			\
		     UART_DM_TX_CLK_SEL(__sel),				\
		     UART_DM_CSR(n));					\
}									\
static inline void __uart##n##_dm_config_gpio(void)			\
{									\
	__uart_dm_config_pad(UART_DM_GPIO_TX_UART(n),			\
			     GPIO_PAD_PULL_DOWN,			\
			     UART_DM_GPIO_MUX_UART(n));			\
	__uart_dm_config_pad(UART_DM_GPIO_RX_UART(n),			\
			     GPIO_PAD_PULL_DOWN,			\
			     UART_DM_GPIO_MUX_UART(n));			\
	__uart_dm_config_pad(UART_DM_GPIO_RTS_UART(n),			\
			     GPIO_PAD_PULL_UP,				\
			     UART_DM_GPIO_MUX_UART(n));			\
	__uart_dm_config_pad(UART_DM_GPIO_CTS_UART(n),			\
			     GPIO_PAD_PULL_UP,				\
			     UART_DM_GPIO_MUX_UART(n));			\
}

static inline void __uart_dm_config_pad(uint8_t gpio, uint8_t pad,
					uint8_t func)
{
	gpio_config_pad(GPIO_HW_PORT, gpio, pad, 8);
	gpio_config_mux(GPIO_HW_PORT, gpio, func);
}

QDF2400_UART(0);
QDF2400_UART(1);
QDF2400_UART(2);
QDF2400_UART(3);
QDF2400_UART(8);
QDF2400_UART(9);
QDF2400_UART(10);
QDF2400_UART(11);

static void __uart_dm_config_baudrate(uint8_t n, uint32_t baudrate)
{
	switch (n) {
	case 0:
		__uart0_dm_config_baudrate(baudrate);
		break;
	case 1:
		__uart1_dm_config_baudrate(baudrate);
		break;
	case 2:
		__uart2_dm_config_baudrate(baudrate);
		break;
	case 3:
		__uart3_dm_config_baudrate(baudrate);
		break;
	case 8:
		__uart8_dm_config_baudrate(baudrate);
		break;
	case 9:
		__uart9_dm_config_baudrate(baudrate);
		break;
	case 10:
		__uart10_dm_config_baudrate(baudrate);
		break;
	case 11:
		__uart11_dm_config_baudrate(baudrate);
		break;
	default:
		break;
	}
}

static void __uart_dm_ack_irq(uint8_t n)
{
	switch (n) {
	case 0:
		__uart0_dm_ack_irq();
		break;
	case 1:
		__uart1_dm_ack_irq();
		break;
	case 2:
		__uart2_dm_ack_irq();
		break;
	case 3:
		__uart3_dm_ack_irq();
		break;
	case 8:
		__uart8_dm_ack_irq();
		break;
	case 9:
		__uart9_dm_ack_irq();
		break;
	case 10:
		__uart10_dm_ack_irq();
		break;
	case 11:
		__uart11_dm_ack_irq();
		break;
	default:
		break;
	}
}

static void __uart_dm_gpio_init(uint8_t n)
{
	switch (n) {
	case 0:
		__uart0_dm_config_gpio();
		break;
	case 1:
		__uart1_dm_config_gpio();
		break;
	case 2:
		__uart2_dm_config_gpio();
		break;
	case 3:
		__uart3_dm_config_gpio();
		break;
	case 8:
		__uart8_dm_config_gpio();
		break;
	case 9:
		__uart9_dm_config_gpio();
		break;
	case 10:
		__uart10_dm_config_gpio();
		break;
	case 11:
		__uart11_dm_config_gpio();
		break;
	default:
		break;
	}
}

static void __uart_dm_init_irq(uint8_t n)
{
	switch (n) {
	case 0:
		__uart0_dm_config_irq();
		break;
	case 1:
		__uart1_dm_config_irq();
		break;
	case 2:
		__uart2_dm_config_irq();
		break;
	case 3:
		__uart3_dm_config_irq();
		break;
	case 8:
		__uart8_dm_config_irq();
		break;
	case 9:
		__uart9_dm_config_irq();
		break;
	case 10:
		__uart10_dm_config_irq();
		break;
	case 11:
		__uart11_dm_config_irq();
		break;
	}
}

static void __uart_dm_config_params(uint8_t n, uint8_t params,
				    uint32_t fifosize,
				    boolean break_zero_char_off,
				    uint32_t stale_timeout)
{
	unsigned long cfg = 0;

	/* Enable RX auto flow control only
	 * trigger @ 58 words(90%) of 64-word RX FIFO
	 */
	__raw_writel(UART_DM_AUTO_RFR_LEVEL(fifosize * 9 / 10) |
		     UART_DM_RX_RDY_CTL, UART_DM_MR1(n));

	if (break_zero_char_off)
		cfg |= UART_DM_RX_BREAK_ZERO_CHAR_OFF;

	cfg |= UART_DM_BITS_PER_CHAR(UART_DM_ENUM_BITS(uart_bits(params)));
	switch (uart_parity(params)) {
	case UART_PARITY_EVEN:
		cfg |= UART_DM_PARITY_MODE(UART_DM_PARITY_EVEN);
		break;
	case UART_PARITY_ODD:
		cfg |= UART_DM_PARITY_MODE(UART_DM_PARITY_ODD);
		break;
	case UART_PARITY_NONE:
		cfg |= UART_DM_PARITY_MODE(UART_DM_PARITY_NONE);
		break;
	}
	switch (uart_stopb(params)) {
	case UART_STOPB_ONE:
		cfg |= UART_DM_STOP_BIT_LEN(UART_DM_ENUM_1_000);
		break;
	case UART_STOPB_TWO:
		cfg |= UART_DM_STOP_BIT_LEN(UART_DM_ENUM_2_000);
		break;
	}
	__raw_writel(cfg, UART_DM_MR2(n));
	/* 15 characters until stale timeout */
	__raw_writel_mask(UART_DM_STALE_TIMEOUT(stale_timeout),
			  UART_DM_STALE_TIMEOUT_MASK,
			  UART_DM_IPR(n));
	/* Enable RX/TX single character mode */
	__raw_writel(UART_DM_RX_SC_ENABLE | UART_DM_TX_SC_ENABLE,
		     UART_DM_DMEN(n));
}

uint32_t __uart_dm_fifo_size(uint8_t n)
{
	uint32_t generics = __raw_readl(UART_DM_GENERICS(n));
	return 4 * (UL(1) << UART_DM_GENERIC_RAM_ADDR_WIDTH(generics));
}

void __uart_dm_config_fifo(uint8_t n,
			   uint32_t fifosize, uint32_t watermark)
{
	/* Configure TX/RX watermark to 0 */
	__raw_writel(watermark, UART_DM_TFWR(n));
	__raw_writel(watermark, UART_DM_RFWR(n));
	/* Ensure there is room for RX FIFO */
	BUG_ON(__uart_dm_fifo_size(n) <= fifosize);
	__raw_writel(fifosize, UART_DM_BADR(n));
}

void __uart_dm_quart_init(uint8_t n)
{
	__uart_dm_init_racc(n);
	__uart_dm_begin_reset(n);
	__uart_dm_uart_init(n);
	__uart_dm_config_fifo(n, UART_DM_QUART_TX_FIFO_SIZE, 0);
	__uart_dm_config_params(n, UART_DEF_PARAMS,
				UART_DM_QUART_TX_FIFO_SIZE, true, 15);
	__uart_dm_config_baudrate(n, UART_DM_BAUDRATE);
	__uart_dm_uart_enable(n);
	__uart_dm_end_reset(n);
}

/* Implement SBSA using ACUART */
/* Differentiated ACUART configurations */
static void __uart_dm_acuart_variant(uint8_t n)
{
	__raw_writel(UART_DM_RX_DMRX_1BYTE_RES_EN |
		     UART_DM_RX_STALE_IRQ_DMRX_EQUAL |
		     UART_DM_RESERVED_3 |
		     UART_DM_RX_DMRX_LOW_EN |
		     UART_DM_STALE_IRQ_EMPTY |
		     UART_DM_TX_BREAK_DISABLE,
		     UART_DM_BCR(n));
	/* This is critical for QDF2400 */
	__raw_setl(UART_DM_ARM_UART_BUSY_FIX, UART_DM_MR2(n));
}

void __uart_dm_acuart_init(uint8_t n)
{
	__uart_dm_init_racc(n);
	__uart_dm_uart_init(n);
	__uart_dm_config_fifo(n, UART_DM_ACUART_TX_FIFO_SIZE, 0);
	__uart_dm_config_baudrate(n, UART_DM_BAUDRATE);
	__uart_dm_config_params(n, UART_DEF_PARAMS,
				UART_DM_ACUART_TX_FIFO_SIZE, false, 4);
	__uart_dm_acuart_variant(n);
	__uart_dm_uart_enable(n);
	__uart_dm_stale_enable(n);
}

#ifdef CONFIG_CONSOLE_OUTPUT
void uart_dm_write_byte(uint8_t byte)
{
	while (!(__raw_readl(UART_DM_ISR(UART_CON_ID)) & UART_DM_TXLEV_IRQ));
	__raw_writel(1, UART_DM_NO_CHARS_FOR_TX(UART_CON_ID));
	__raw_writel(byte, UART_DM_TF(UART_CON_ID, 1));
}
#endif

#ifdef CONFIG_CONSOLE_INPUT
#define RING_SIZE			256
#define BYTES_IN_RING()			\
	(write_index - read_index)
#define ROOM_IN_RING()			\
	(RING_SIZE - BYTES_IN_RING() - 1)
#define RING_GET_BYTE()			\
	(ring_buffer[read_index++ & (RING_SIZE - 1)])
#define RING_PUT_BYTE(data)						\
	do {								\
		ring_buffer[write_index++ & (RING_SIZE - 1)] = data;	\
	} while (0)

/* Must be greater and equal to RX_FIFO_SIZE */
#define UART_DM_RX_XFER_SIZE	4096
static uint32_t uart_dm_rx_bytes_left = 0;
static uint8_t ring_buffer[RING_SIZE];
static uint32_t read_index = 0;
static uint32_t write_index = 0;

static void __uart_dm_rx_start(uint8_t n)
{
	__raw_writel(UART_DM_RX_XFER_SIZE, UART_DM_DMRX(n));
	__raw_writel(UART_DM_CMD_GENERAL(ENABLE_STALE_IRQ),
		     UART_DM_CR(n));
	uart_dm_rx_bytes_left = UART_DM_RX_XFER_SIZE;
}

static void __uart_dm_rx_service(uint8_t n)
{
	uint32_t rxfs, isr, snap;
	uint32_t words_in_fifo;
	uint32_t bytes_to_copy;
	uint32_t fifo_words;

	if (uart_dm_rx_bytes_left == 0)
		__uart_dm_rx_start(n);
	while (1) {
		rxfs = __raw_readl(UART_DM_RXFS(n));
		isr = __raw_readl(UART_DM_ISR(n));
		if (isr & UART_DM_RXSTALE_IRQ) {
			__raw_writel(UART_DM_CMD_CHANNEL(CLEAR_STALE_IRQ),
				     UART_DM_CR(n));
			snap = __raw_readl(UART_DM_RX_TOTAL_SNAP(n));
			uart_dm_rx_bytes_left = UART_DM_RX_XFER_SIZE - snap;
		}
		words_in_fifo = UART_DM_FIFO_STATE(rxfs);
		if (words_in_fifo == 0)
			bytes_to_copy = 0;
		else if (words_in_fifo < uart_dm_rx_bytes_left)
			bytes_to_copy = words_in_fifo;
		else
			bytes_to_copy = uart_dm_rx_bytes_left;
		if (ROOM_IN_RING() < bytes_to_copy)
			bytes_to_copy = ROOM_IN_RING();
		if (bytes_to_copy == 0)
			break;
		while (bytes_to_copy) {
			fifo_words = __raw_readl(UART_DM_RF(n, 1));
			RING_PUT_BYTE((uint8_t)fifo_words);
			bytes_to_copy -= 1;
			uart_dm_rx_bytes_left -= 1;
		}
		if (uart_dm_rx_bytes_left == 0)
			__uart_dm_rx_start(n);
	}
}

boolean uart_dm_read_poll(void)
{
	__uart_dm_rx_service(UART_CON_ID);
	return !!(BYTES_IN_RING() > 0);
}

uint8_t uart_dm_read_byte(void)
{
	while (1) {
		__uart_dm_rx_service(UART_CON_ID);
		if (BYTES_IN_RING())
			return RING_GET_BYTE();
	}
}
#endif

void uart_dm_irq_init(void)
{
	uint8_t n = UART_CON_ID;

	__uart_dm_disable_all_irqs(n);
	__uart_dm_enable_irqs(n, UART_DM_RXLEV_IRQ | UART_DM_RXSTALE_IRQ);
	__uart_dm_init_irq(n);
}

void uart_dm_irq_ack(void)
{
	__uart_dm_ack_irq(UART_CON_ID);
}

void __uart_dm_ctrl_init(uint8_t n)
{
	__clk_enable_branch(UART_DM_PCC_BLSP(n));
	__uart_dm_gpio_init(n);
	if (n < QUART_ACUART_BASE)
		__uart_dm_quart_init(n);
	else
		__uart_dm_acuart_init(n);
}

void uart_dm_ctrl_init(void)
{
	__uart_dm_ctrl_init(UART_CON_ID);
}
