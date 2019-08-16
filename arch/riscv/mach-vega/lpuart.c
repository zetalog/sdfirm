#include <target/uart.h>
#include <target/gpio.h>

void lpuart_ctrl_init(void)
{
	/* Clock Gate Control: 0x01 */
	//CLOCK_EnableClock(kCLOCK_PortC);

	gpio_config_mux(3, 7, PORT_MUX_C7_LPUART0_RX);
	gpio_config_mux(3, 8, PORT_MUX_C8_LPUART0_TX);
}
