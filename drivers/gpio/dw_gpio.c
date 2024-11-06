#include <target/gpio.h>

void dw_gpio_config_pad(uint8_t n, uint8_t p, uint8_t pin,
			pad_cfg_t pad, uint8_t drv)
{
	if (drv == GPIO_DRIVE_IN)
		__raw_clearl(_BV(pin), GPIO_SWPORT_DDR(n, p));
	else
		__raw_setl(_BV(pin), GPIO_SWPORT_DDR(n, p));
}

void dw_gpio_config_irq(uint8_t n, uint8_t pin, uint8_t mode)
{
	if ((mode & GPIO_IRQ_LEVEL_TRIG) == GPIO_IRQ_LEVEL_TRIG)
		dw_gpio_set_irq_level(n, pin);
	else
		dw_gpio_set_irq_edge(n, pin);
	if (mode & GPIO_IRQ_HIGH)
		dw_gpio_set_irq_high(n, pin);
	else
		dw_gpio_set_irq_low(n, pin);
}

void dw_gpio_write_pin(uint8_t n, uint8_t p, uint8_t pin, uint8_t value)
{
	if (value)
		__raw_setl(_BV(pin), GPIO_SWPORT_DR(n, p));
	else
		__raw_clearl(_BV(pin), GPIO_SWPORT_DR(n, p));
}

void dw_gpio_write_port(uint8_t n, uint8_t p, uint32_t value)
{
	__raw_writel(value, GPIO_SWPORT_DR(n, p));
}

uint32_t dw_gpio_read_port(uint8_t n, uint8_t p)
{
	return __raw_readl(GPIO_SWPORT_DR(n, p));
}
