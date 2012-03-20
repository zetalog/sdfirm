#include <target/ifd.h>
#include <target/delay.h>

static inline void __uart1_hw_config_pins(void)
{
	pm_hw_resume_device(DEV_GPIOB, DEV_MODE_ON);
	/* configure UART1 RX pin */
	gpio_config_mux(PORT_IO, PIN_IO, GPIOB4_MUX_U1RX);
	gpio_config_pad(PORT_IO, PIN_IO, GPIO_DIR_NONE,
			GPIO_PAD_PP, 2);
	/* configure UART1 TX pin */
	gpio_config_mux(PORT_CLK, PIN_CLK, GPIOB5_MUX_U1TX);
	gpio_config_pad(PORT_CLK, PIN_CLK, GPIO_DIR_NONE,
			GPIO_PAD_PP, 2);
}

void ifds_hw_ctrl_init(void)
{
	pm_hw_resume_device(DEV_UART1, DEV_MODE_ON);
	/* config PIN IO/CLK */
	__uart1_hw_config_pins();

	pm_hw_resume_device(DEV_GPIOF, DEV_MODE_ON);
	/* config PIN VCC */
	gpio_config_mux(PORT_VCC, PIN_VCC, GPIO_MUX_NONE);
	gpio_config_pad(PORT_VCC, PIN_VCC, GPIO_DIR_OUT,
			GPIO_PAD_PP, 2);
	/* config PIN PRES */
	gpio_config_mux(PORT_PRES, PIN_PRES, GPIO_MUX_NONE);
	gpio_config_pad(PORT_PRES, PIN_PRES, GPIO_DIR_IN,
			GPIO_PAD_PP, 2);
	/* config PIN VCCSEL1 */
	gpio_config_mux(PORT_VCCSEL, PIN_VCCSEL1, GPIO_MUX_NONE);
	gpio_config_pad(PORT_VCCSEL, PIN_VCCSEL1, GPIO_DIR_OUT,
			GPIO_PAD_PP, 2);
	/* config PIN VCCSEL2 */
	gpio_config_mux(PORT_VCCSEL, PIN_VCCSEL2, GPIO_MUX_NONE);
	gpio_config_pad(PORT_VCCSEL, PIN_VCCSEL2, GPIO_DIR_OUT,
			GPIO_PAD_PP, 2);

	pm_hw_resume_device(DEV_GPIOE, DEV_MODE_ON);
	/* config PIN RST */
	gpio_config_mux(PORT_RST, PIN_RST, GPIO_MUX_NONE);
	gpio_config_pad(PORT_RST, PIN_RST, GPIO_DIR_OUT,
			GPIO_PAD_PP, 2);
}
