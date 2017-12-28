#include <target/uart.h>

/* compute the baud rate divisor
 * BRDDIV = BRDi + BRDf = CLK_UART / (16 * baudrate)
 *
 * we will compute 64*BRDDIV+0.5 to get DIVINT & DIVFRAC value
 * DIVINT  = (64*BRDi+0.5) / 64
 * DIVFRAC = (64*BRDf+0.5) % 64
 *
 * DIVCFG = 64*BRDDIV+0.5
 *        = ((64 * CLK_UART * 1000) / (16 * baudrate)) + 0.5
 *        = ((CLK_UART * 64000) / (16 * baudrate)) + 0.5
 *        = ((CLK_UART * 4000) / (baudrate)) + 0.5
 *        = (((CLK_UART * 20) / (baudrate/400)) + 1) / 2
 */
/* Since fractional part is based on the 64 (2^6) times of the BRD,
 * 6 is define as FBRD offset here.
 */
#define PL01X_BRDDIV_20CLK	CLK_UART*20
#define PL01X_BRDDIV_OFFSET	6
#define PL01X_BRDDIV_MASK	((1<<PL01X_BRDDIV_OFFSET)-1)

void pl01x_config_baudrate(uint8_t n, uint32_t baudrate)
{
	uint32_t cfg;

	cfg = div32u(1 + div32u(PL01X_BRDDIV_20CLK,
				div32u(baudrate, 400)),
		     2);
	__raw_writew(cfg >> PL01X_BRDDIV_OFFSET, UARTIBRD(n));
	__raw_writeb(cfg  & PL01X_BRDDIV_MASK, UARTFBRD(n));
}

void pl01x_config_params(uint8_t n, uint8_t params)
{
	uint8_t cfg;

	cfg = UART_WLEN(uart_bits(params)-5);
	switch (uart_parity(params)) {
	case UART_PARITY_EVEN:
		cfg |= UART_EPS;
	case UART_PARITY_ODD:
		cfg |= UART_PEN;
		break;
	}
	if (uart_stopb(params))
		cfg |= UART_STP2;
	__raw_writeb_mask(cfg, 0xEE, UARTLCR_H(n));
}
