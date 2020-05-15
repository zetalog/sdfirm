#include <target/gpio.h>

void dw_gpio_config_pad(uint8_t n, uint8_t p, uint8_t pin,
			uint8_t pad, uint8_t drv)
{
	if (drv == GPIO_DRIVE_IN)
		__raw_clearl(_BV(pin), GPIO_SWPORT_DDR(n, p));
	else
		__raw_setl(_BV(pin), GPIO_SWPORT_DDR(n, p));
}

void dw_gpio_config_irq(uint8_t n, uint8_t p, uint8_t pin, uint8_t mode)
{
	if ((mode & GPIO_IRQ_LEVEL_TRIG) == GPIO_IRQ_EDGE_TRIG)
		dw_gpio_set_irq_level(n, p, pin);
	else
		dw_gpio_set_irq_edge(n, p, pin);
	if (mode & GPIO_IRQ_HIGH)
		dw_gpio_set_irq_high(n, p, pin);
	else
		dw_gpio_set_irq_low(n, p, pin);
}
