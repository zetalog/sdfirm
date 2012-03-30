#include <target/ifd.h>
#include <target/delay.h>

static inline void __uart1_hw_config_pins(void)
{
	pm_hw_resume_device(DEV_GPIOB, DEV_MODE_ON);
	/* configure UART1 RX pin */
	gpio_config_mux(PORT_IO, PIN_IO, GPIOB4_MUX_U1RX);
	gpio_config_pad(PORT_IO, PIN_IO, GPIO_PAD_PP, 2);
	/* configure UART1 TX pin */
	gpio_config_mux(PORT_CLK, PIN_CLK, GPIOB5_MUX_U1TX);
	gpio_config_pad(PORT_CLK, PIN_CLK, GPIO_PAD_PP, 2);
}

void __ifd_hw_ctrl_disable(uint8_t n)
{
	while (__raw_readl(UARTFR(n)) & _BV(BUSY));
	/* disable the FIFO */
	__uart_hw_fifo_disable(n);
	/* disable the IFD */
	__uart_hw_ifd_enable(n);
}

void __ifd_hw_ctrl_enable(uint8_t n)
{
	/* enable the FIFO */
	__uart_hw_fifo_enable(n);
	/* enable the IFD */
	__uart_hw_ifd_disable(n);
}

void __ifd_hw_start_wt(uint8_t n, uint16_t ms)
{
	__uart_hw_wt_enable(n);
}

void __ifd_hw_stop_wt(uint8_t n)
{
	__uart_hw_wt_disable(n);
}

void ifds_hw_ctrl_init(void)
{
	pm_hw_resume_device(DEV_UART1, DEV_MODE_ON);
	/* config PIN IO/CLK */
	__uart1_hw_config_pins();

	pm_hw_resume_device(DEV_GPIOF, DEV_MODE_ON);
	/* config PIN VCC */
	gpio_config_mux(PORT_VCC, PIN_VCC, GPIO_MUX_NONE);
	gpio_config_pad(PORT_VCC, PIN_VCC, GPIO_PAD_PP, 2);
	/* config PIN PRES */
	gpio_config_mux(PORT_PRES, PIN_PRES, GPIO_MUX_NONE);
	gpio_config_pad(PORT_PRES, PIN_PRES, GPIO_PAD_PP, 2);
	/* config PIN VCCSEL1 */
	gpio_config_mux(PORT_VCCSEL, PIN_VCCSEL1, GPIO_MUX_NONE);
	gpio_config_pad(PORT_VCCSEL, PIN_VCCSEL1, GPIO_PAD_PP, 2);
	/* config PIN VCCSEL2 */
	gpio_config_mux(PORT_VCCSEL, PIN_VCCSEL2, GPIO_MUX_NONE);
	gpio_config_pad(PORT_VCCSEL, PIN_VCCSEL2, GPIO_PAD_PP, 2);

	pm_hw_resume_device(DEV_GPIOE, DEV_MODE_ON);
	/* config PIN RST */
	gpio_config_mux(PORT_RST, PIN_RST, GPIO_MUX_NONE);
	gpio_config_pad(PORT_RST, PIN_RST, GPIO_PAD_PP, 2);
}
