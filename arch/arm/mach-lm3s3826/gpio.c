#include <target/generic.h>
#include <target/arch.h>

#ifdef CONFIG_GPIO_LM3S9B92_AHB
unsigned long __gpio_hw_port_reg(uint8_t port, unsigned short offset)
{
	return GPIOA_BASE + ((port)<<12)+offset;
}

#define __gpio_hw_port_init()	__raw_writel(0x000001FF, GPIOHBCTL)
#else
unsigned long __gpio_hw_port_reg(uint8_t port, unsigned short offset)
{
	unsigned long reg;

	if (port < 4)
		reg = GPIOA_BASE + ((port)<<12);
	else
		reg = GPIOE_BASE + ((port-4)<<12);

	return reg+offset;
}

#define __gpio_hw_port_init()
#endif

void gpio_hw_config_mux(uint8_t port, uint8_t pin, uint8_t mux)
{
	unsigned long reg;
	uint8_t shift = pin<<2;

	reg = __gpio_hw_port_reg(port, GPIOPCTL);
	__raw_clearl(0xf << shift, reg);
	__raw_setl((mux & 0xf) << shift, reg);
	reg = __gpio_hw_port_reg(port, GPIOAFSEL);
	if (mux == GPIO_MUX_NONE)
		__raw_clearl_atomic(pin, reg);
	else
		__raw_setl_atomic(pin, reg);
}

void gpio_hw_config_pad(uint8_t port, uint8_t pin,
			uint8_t pad, uint8_t drv)
{
	unsigned long reg;

	/* configure PIN IO type */
	/* digital IO */
	/* push-pull & open-drain settings */
	reg = __gpio_hw_port_reg(port, GPIODEN);
	if (pad & GPIO_PAD_DIGITAL_IO) {
		__raw_setl_atomic(pin, reg);
	} else {
		__raw_clearl_atomic(pin, reg);
	}
	reg = __gpio_hw_port_reg(port, GPIOODR);
	if (pad & GPIO_PAD_OPEN_DRAIN) {
		__raw_setl_atomic(pin, reg);
	} else {
		__raw_clearl_atomic(pin, reg);
	}
	reg = __gpio_hw_port_reg(port, GPIOPDR);
	if (pad & GPIO_PAD_PULL_DOWN) {
		__raw_setl_atomic(pin, reg);
	} else {
		__raw_clearl_atomic(pin, reg);
	}
	reg = __gpio_hw_port_reg(port, GPIOPUR);
	if (pad & GPIO_PAD_PULL_UP) {
		__raw_setl_atomic(pin, reg);
	} else {
		__raw_clearl_atomic(pin, reg);
	}
	/* analog IO */
	reg = __gpio_hw_port_reg(port, GPIOAMSEL);
	if (pad == GPIO_PAD_ANALOG_IO) {
		/* only valid for port D and E */
		BUG_ON((port != GPIOD) && (port != GPIOE));
		__raw_setl_atomic(pin, reg);
	} else {
		__raw_clearl_atomic(pin, reg);
	}
	reg = __gpio_hw_port_reg(port, GPIOSLR);
	if (pad & GPIO_PAD_SLEW_RATE) {
		__raw_setl_atomic(pin, reg);
	} else {
		__raw_clearl_atomic(pin, reg);
	}

	/* configure PIN OUT drive strength & PIN IN */
	reg = __gpio_hw_port_reg(port, GPIODIR);
	if (drv == GPIO_DRIVE_IN) {
		__raw_clearl_atomic(pin, reg);
	} else {
		__raw_setl_atomic(pin, reg);
	}

	reg = __gpio_hw_port_reg(port, GPIODR2R);
	__raw_clearl_atomic(pin, reg);
	if (drv <= 2) {
		/* XXX: GPIO Bi-directional Drive
		 * If a PIN can be both read and written, it should be
		 * a digital IO, 2mA drive should be enough for such
		 * PINs.
		 */
		__raw_setl_atomic(pin, reg);
	}
	reg = __gpio_hw_port_reg(port, GPIODR4R);
	__raw_clearl_atomic(pin, reg);
	if (drv > 2 && drv <= 4) {
		__raw_setl_atomic(pin, reg);
	}
	reg = __gpio_hw_port_reg(port, GPIODR8R);
	__raw_clearl_atomic(pin, reg);
	if (drv > 4) {
		__raw_setl_atomic(pin, reg);
	}
}

uint8_t gpio_hw_read_pin(uint8_t port, uint8_t pin)
{
	unsigned long reg;

	/* configure PIN directions */
	reg = __gpio_hw_port_reg(port, GPIODIR);
	__raw_clearl_atomic(pin, reg);
	reg = __gpio_hw_port_reg(port, GPIODATA);
	return (uint8_t)(__raw_readl(reg + (_BV(pin) << 2)) >> pin);
}

void gpio_hw_write_pin(uint8_t port, uint8_t pin, uint8_t val)
{
	unsigned long reg;

	/* configure PIN directions */
	reg = __gpio_hw_port_reg(port, GPIODIR);
	__raw_setl_atomic(pin, reg);
	reg = __gpio_hw_port_reg(port, GPIODATA);
	__raw_writel(((uint32_t)val) << pin, reg + (_BV(pin) << 2));
}

uint8_t gpio_hw_read_port(uint8_t port, uint8_t mask)
{
	unsigned long reg;

	/* configure PIN directions */
	reg = __gpio_hw_port_reg(port, GPIODIR);
	__raw_writel_mask(0x0, mask, reg);
	reg = __gpio_hw_port_reg(port, GPIODATA);
	return (uint8_t)(__raw_readl(reg + (mask << 2)));
}

void gpio_hw_write_port(uint8_t port, uint8_t mask,
			uint8_t val)
{
	unsigned long reg;

	/* configure PIN directions */
	reg = __gpio_hw_port_reg(port, GPIODIR);
	__raw_writel_mask(0xFF, mask, reg);
	reg = __gpio_hw_port_reg(port, GPIODATA);
	__raw_writel(((uint32_t)val), reg + (mask << 2));
}

void gpio_hw_config_irq(uint8_t port, uint8_t pin, uint8_t mode)
{
	unsigned long reg;

	if (bits_raised(mode, GPIO_IRQ_HIGH | GPIO_IRQ_LOW)) {
		reg = __gpio_hw_port_reg(port, GPIOIS);
		__raw_clearl_atomic(pin, reg);
		reg = __gpio_hw_port_reg(port, GPIOIBE);
		__raw_setl_atomic(pin, reg);
	} else {
		reg = __gpio_hw_port_reg(port, GPIOIBE);
		__raw_clearl_atomic(pin, reg);

		if (bits_raised(mode, GPIO_IRQ_EDGE_TRIG)) {
			reg = __gpio_hw_port_reg(port, GPIOIS);
			__raw_clearl_atomic(pin, reg);
		} else {
			reg = __gpio_hw_port_reg(port, GPIOIS);
			__raw_setl_atomic(pin, reg);
		}
		if (bits_raised(mode, GPIO_IRQ_LOW)) {
			reg = __gpio_hw_port_reg(port, GPIOIEV);
			__raw_clearl_atomic(pin, reg);
		} else {
			reg = __gpio_hw_port_reg(port, GPIOIEV);
			__raw_setl_atomic(pin, reg);
		}
	}
}

void gpio_hw_enable_irq(uint8_t port, uint8_t pin)
{
	unsigned long reg;

	reg = __gpio_hw_port_reg(port, GPIOIM);
	__raw_setl_atomic(pin, reg);
}

void gpio_hw_disable_irq(uint8_t port, uint8_t pin)
{
	unsigned long reg;

	reg = __gpio_hw_port_reg(port, GPIOIM);
	__raw_clearl_atomic(pin, reg);
}

uint8_t gpio_hw_irq_status(uint8_t port)
{
	unsigned long reg;

	reg = __gpio_hw_port_reg(port, GPIORIS);
	return	(uint8_t)__raw_readl(reg);
}

void gpio_hw_clear_irq(uint8_t port, uint8_t pin)
{
	unsigned long reg;

	reg = __gpio_hw_port_reg(port, GPIOICR);
	__raw_setl_atomic(pin, reg);
}

#ifdef CONFIG_PORTING_GPIO
void gpio_hw_porting_init(void)
{
	pm_hw_resume_device(DEV_GPIOA+CONFIG_PORTING_MINOR,
			    DEV_MODE_ON);
}
#endif

void gpio_hw_ctrl_init(void)
{
	__gpio_hw_port_init();
}
