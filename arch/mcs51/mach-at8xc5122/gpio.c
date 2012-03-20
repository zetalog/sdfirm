#include <target/gpio.h>

void __gpio_hw_clear_pin(uint8_t port, uint8_t pin)
{
	uint8_t val = _BV(pin);

	switch (port) {
	case GPIOA:
		P0 &= ~val;
		break;
	case GPIOB:
		P1 &= ~val;
		break;
	case GPIOC:
		P2 &= ~val;
		break;
	case GPIOD:
		P3 &= ~val;
		break;
	case GPIOE:
		P4 &= ~val;
		break;
	case GPIOF:
		P5 &= ~val;
		break;
	}
}

void __gpio_hw_set_pin(uint8_t port, uint8_t pin)
{
	uint8_t val = _BV(pin);

	switch (port) {
	case GPIOA:
		P0 |= val;
		break;
	case GPIOB:
		P1 |= val;
		break;
	case GPIOC:
		P2 |= val;
		break;
	case GPIOD:
		P3 |= val;
		break;
	case GPIOE:
		P4 |= val;
		break;
	case GPIOF:
		P5 |= val;
		break;
	}
}

uint8_t gpio_hw_read_pin(uint8_t port, uint8_t pin)
{
	uint8_t val = _BV(pin);

	switch (port) {
	case GPIOA:
		return (P0 & val) >> pin;
	case GPIOB:
		return (P1 & val) >> pin;
	case GPIOC:
		return (P2 & val) >> pin;
	case GPIOD:
		return (P3 & val) >> pin;
	case GPIOE:
		return (P4 & val) >> pin;
	case GPIOF:
		return (P5 & val) >> pin;
	}
	return 0;
}

void gpio_hw_write_pin(uint8_t port, uint8_t pin, uint8_t val)
{
	if (val == 0)
		__gpio_hw_clear_pin(port, pin);
	else
		__gpio_hw_set_pin(port, pin);
}

void gpio_hw_write_port(uint8_t port, uint8_t val)
{
	switch (port) {
	case GPIOA:
		P0 = val;
		break;
	case GPIOB:
		P1 = val;
		break;
	case GPIOC:
		P2 = val;
		break;
	case GPIOD:
		P3 = val;
		break;
	case GPIOE:
		P4 = val;
		break;
	case GPIOF:
		P5 = val;
		break;
	}
}

uint8_t gpio_hw_read_port(uint8_t port)
{
	switch (port) {
	case GPIOA:
		return P0;
	case GPIOB:
		return P1;
	case GPIOC:
		return P2;
	case GPIOD:
		return P3;
	case GPIOE:
		return P4;
	case GPIOF:
		return P5;
	}
	return 0;
}
