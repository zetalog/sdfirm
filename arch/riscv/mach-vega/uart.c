#include <target/clk.h>
#include <target/gpio.h>
#include <target/uart.h>
#include <target/console.h>

void uart_hw_con_init(void)
{
	clk_t clk;

	switch (UART_CON_ID) {
	case 0:
		clk_enable(portc_clk);
		gpio_config_mux(3, 7, PTC7_MUX_LPUART0_RX);
		gpio_config_mux(3, 8, PTC8_MUX_LPUART0_TX);
		clk = lpuart0_clk;
		break;
	case 1:
	case 2:
	case 3:
		break;
	}
	clk_enable(clk);
	lpuart_ctrl_init(UART_CON_ID, clk_get_frequency(clk),
			 UART_DEF_PARAMS, UART_CON_BAUDRATE);
}
