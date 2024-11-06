#include <target/gpio.h>
#include <target/panic.h>

uint8_t __gpio_hw_od;
uint8_t __gpio_hw_od5;

uint8_t __gpio_hw_port_reg(uint8_t port)
{
	if (port == GPIOF)
		return PORT5;
	else
		return PORT0+((port)<<4);
}

void __gpio_hw_clear_pin(uint8_t port, uint8_t pin)
{
	uint8_t reg = __gpio_hw_port_reg(port);
	__raw_clearb_atomic(pin, reg);
}

void __gpio_hw_set_pin(uint8_t port, uint8_t pin)
{
	uint8_t reg = __gpio_hw_port_reg(port);
	__raw_setb_atomic(pin, reg);
}

boolean __gpio_hw_test_od_port(uint8_t port)
{
	BUG_ON(port == GPIOF);
	return (port != GPIOA) && (_BV(port) & __gpio_hw_od);
}

boolean __gpio_hw_test_od_pin(uint8_t port, uint8_t mask)
{
	if (port != GPIOF)
		return __gpio_hw_test_od_port(port);
	else
		return mask & __gpio_hw_od5;
}

uint8_t gpio_hw_read_pin(uint8_t port, uint8_t pin)
{
	uint8_t reg = __gpio_hw_port_reg(port);
	if (__gpio_hw_test_od_pin(port, _BV(pin)))
		__raw_setb_atomic(pin, reg);
	return __raw_testb_atomic(pin, reg);
}

void gpio_hw_write_pin(uint8_t port, uint8_t pin, uint8_t val)
{
	if (val == 0)
		__gpio_hw_clear_pin(port, pin);
	else
		__gpio_hw_set_pin(port, pin);
}

void gpio_hw_write_port(uint8_t port, uint8_t mask,
			uint8_t val)
{
	uint8_t reg = __gpio_hw_port_reg(port);
	uint8_t pin;

	for (pin = 0; pin < 8; pin++) {
		/* P0 = val; */
		if (_BV(pin) & mask)
			__raw_setb_atomic(pin, reg);
	}
}

uint8_t gpio_hw_read_port(uint8_t port, uint8_t mask)
{
	uint8_t reg = __gpio_hw_port_reg(port);
	uint8_t pin;

	if (port == GPIOF) {
		for (pin = 0; pin < 8; pin++) {
			if (__gpio_hw_test_od_pin(port, _BV(pin) & mask))
				__raw_setb_atomic(pin, reg);
		}
	} else {
		if (__gpio_hw_test_od_port(port))
			__raw_writeb(0xFF, reg);
	}
	return __raw_readb(reg);
}

void __gpio_hw_config_pad(uint8_t port, uint8_t pin,
			  uint8_t val)
{
	if (port < GPIOE) {
		BUG_ON(pin != 0);
		__gpio_hw_clear_mod0(port << 1);
		__gpio_hw_set_mod0(port << 1, val);
	} else {
		uint8_t off;

		if (port == GPIOE) {
			BUG_ON(pin != 0);
			off = 0;
		} else {
			BUG_ON(pin != 0 && pin != 3 && pin != 6);
			off = (pin/3 << 1) + 1;
		}
		__gpio_hw_clear_mod1(off);
		__gpio_hw_set_mod1(off, val);
	}
}

void __gpio_hw_clear_od(uint8_t port, uint8_t pin)
{
	if (port != GPIOF) {
		__gpio_hw_od &= ~_BV(port);
	} else {
		if (pin == 0)
			__gpio_hw_od5 &= ~0x7;
		if (pin == 3)
			__gpio_hw_od5 &= ~0x38;
		if (pin == 6)
			__gpio_hw_od5 &= ~0xC0;
	}
}

void __gpio_hw_set_od(uint8_t port, uint8_t pin)
{
	if (port != GPIOF) {
		__gpio_hw_od |= _BV(port);
	} else {
		if (pin == 0)
			__gpio_hw_od5 |= 0x7;
		if (pin == 3)
			__gpio_hw_od5 |= 0x38;
		if (pin == 6)
			__gpio_hw_od5 |= 0xC0;
	}
}

void gpio_hw_config_pad(uint8_t port, uint8_t pin,
			pad_cfg_t gpad, uint8_t drv)
{
	uint8_t val = 0xFF;
	uint8_t pad = (uint8_t)gpad;

	__gpio_hw_clear_od(port, pin);
	if (pad == GPIO_PAD_OD) {
		__gpio_hw_set_od(port, pin);
		val = __GPIO_HW_P_OD_INOUT;
		goto done;
	}
	if (pad == GPIO_PAD_KB) {
		BUG_ON(port != GPIOF);
		val = __GPIO_HW_P_KB_OUT;
		goto done;
	}
	if (pad == GPIO_PAD_PP) {
		BUG_ON(port == GPIOB);
		if (port == GPIOA)
			val = __GPIO_HW_P0_PP_OUT;
		else
			val = __GPIO_HW_P_PP_OUT;
		goto done;
	}
	if (pad == GPIO_PAD_PP_WU) {
		BUG_ON(port < GPIOD);
		BUG_ON(port == GPIOF && pin == 6);
		val = __GPIO_HW_P_PPWU_IN;
		goto done;
	}
	if (pad == GPIO_PAD_PP_WD) {
		BUG_ON(port != GPIOC && port != GPIOF);
		BUG_ON(port == GPIOF && pin == 0);
		if (port == GPIOC)
			val = __GPIO_HW_P2_PPWD_IN;
		else
			val = __GPIO_HW_P5_PPWD_IN;
		goto done;
	}
	if (pad == GPIO_PAD_PP_MU) {
		BUG_ON(port != GPIOE && port != GPIOF);
		BUG_ON(port == GPIOF && pin != 0);
		if (port == GPIOE)
			val = __GPIO_HW_P4_PPMU_IN;
		else
			val = __GPIO_HW_P5L_PPMU_IN;
		goto done;
	}
	/* unexpected configurations */
	BUG();
done:
	BUG_ON(val == 0xFF);
	__gpio_hw_config_pad(port, pin, val);
}
