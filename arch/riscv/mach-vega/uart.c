#include <target/clk.h>
#include <target/gpio.h>
#include <target/uart.h>

void uart_hw_con_init(void)
{
	switch (UART_CON_ID) {
	case 0:
		clk_enable(portc_clk);
		gpio_config_mux(3, 7, PTC7_MUX_LPUART0_RX);
		gpio_config_mux(3, 8, PTC8_MUX_LPUART0_TX);
		clk_enable(lpuart0_clk);
		break;
	case 1:
	case 2:
	case 3:
		break;
	}
	lpuart_ctrl_init(UART_CON_ID);
}
