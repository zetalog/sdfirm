#include <target/uart.h>

LM3S9B92_UART(0)
static inline void __uart0_hw_config_pins(void)
{
	pm_hw_resume_device(DEV_GPIOA, DEV_MODE_ON);
	/* configure UART0 RX pin */
	gpio_hw_config_mux(GPIOA, 0, GPIOA0_MUX_U0RX);
	gpio_hw_config_pad(GPIOA, 0, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
	/* configure UART0 TX pin */
	gpio_hw_config_mux(GPIOA, 1, GPIOA1_MUX_U0TX);
	gpio_hw_config_pad(GPIOA, 1, GPIO_DIR_HW, GPIO_PAD_PP, GPIO_DRIVE_2MA);
}

void uart_hw_set_params(uint8_t params, uint32_t baudrate)
{
	__uart0_hw_ctrl_config(params, baudrate);
}

void uart_hw_sync_write(uint8_t byte)
{
	if (pm_hw_device_mode(DEV_UART0) != DEV_MODE_OFF)
		__uart0_hw_write_byte(byte);
}

uint8_t uart_hw_sync_read(void)
{
	if (pm_hw_device_mode(DEV_UART0) != DEV_MODE_OFF)
		return __uart0_hw_read_byte();
	return 0;
}

void uart_hw_sync_start(void)
{
	__uart0_hw_ctrl_enable(UART_SYNC_PARAMS);
}

void uart_hw_sync_stop(void)
{
	__uart0_hw_ctrl_disable();
}

void uart_hw_sync_init(void)
{
	pm_hw_resume_device(DEV_UART0, DEV_MODE_ON);
	__uart0_hw_config_pins();
}
