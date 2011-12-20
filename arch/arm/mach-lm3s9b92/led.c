#include <target/led.h>
#include <target/delay.h>

void led_hw_light_on(led_no_t no, uint8_t color)
{
	gpio_hw_write_pin(GPIOD, 0, 0x01);
}

void led_hw_light_off(led_no_t no)
{
	gpio_hw_write_pin(GPIOD, 0, 0x00);
}

void led_hw_ctrl_init(void)
{
	pm_hw_resume_device(DEV_GPIOD, DEV_MODE_ON);
	/* insert a few cycles after enabling the peripheral */
	udelay(1);
	gpio_hw_config_pad(GPIOD, 0, GPIO_DIR_OUT, GPIO_PAD_PP, GPIO_DRIVE_2MA);
}
