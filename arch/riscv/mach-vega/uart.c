#include <target/uart.h>
#include <target/console.h>

clk_t uart_hw_board_init(void)
{
	int i;
	clk_t clks[NR_UART_CLKS] = uart_clks;

	for (i = 0; i < (NR_UART_CLKS - 1); i++) {
		if (clks[i] != invalid_clk)
			clk_enable(clks[i]);
	}
	gpio_config_mux(UART_PIN_PORT(UART_PIN_RX),
			UART_PIN_PIN(UART_PIN_RX), UART_MUX);
	gpio_config_mux(UART_PIN_PORT(UART_PIN_TX),
			UART_PIN_PIN(UART_PIN_TX), UART_MUX);
	return clks[NR_UART_CLKS - 1];
}

void uart_hw_con_init(void)
{
	clk_t clk;

	clk = uart_hw_board_init();
	clk_enable(clk);
	lpuart_ctrl_init(UART_CON_ID, clk_get_frequency(clk),
			 UART_DEF_PARAMS, UART_CON_BAUDRATE);
}
