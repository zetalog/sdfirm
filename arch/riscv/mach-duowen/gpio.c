#include <target/gpio.h>
#include <target/clk.h>

struct duowen_pad {
	uint32_t port;
	uint16_t pin;
	uint8_t func;
	uint8_t pad;
	uint8_t drive;
};

struct duowen_pad duowen_pads[] = {
#ifndef CONFIG_DUOWEN_UART_CON0
	/* uart0 */
	{ GPIO1B, pad_gpio_54, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO1B, pad_gpio_55, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO2A, pad_gpio_80, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_81, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_82, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_83, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_84, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_85, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
#endif
#ifndef CONFIG_DUOWEN_UART_CON1
	/* uart1 */
	{ GPIO1B, pad_gpio_62, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO1B, pad_gpio_63, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO2A, pad_gpio_88, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_89, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_90, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_91, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_92, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_93, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
#endif
#ifndef CONFIG_DUOWEN_UART_CON2
	/* uart2 */
	{ GPIO1C, pad_gpio_70, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO1C, pad_gpio_71, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO2A, pad_gpio_96, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_97, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_98, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_99, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_100, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_101, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
#endif
#ifndef CONFIG_DUOWEN_UART_CON3
	/* uart3 */
	{ GPIO1D, pad_gpio_78, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO1D, pad_gpio_79, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO2A, pad_gpio_104, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_105, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_106, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_107, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_108, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO2A, pad_gpio_109, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
#endif
#ifndef CONFIG_DUOWEN_SSI_FLASH_SPI0
	/* flash0 */
	{ GPIO1B, pad_gpio_48, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO1B, pad_gpio_49, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO1B, pad_gpio_50, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_DOWN, 8 },
	{ GPIO1B, pad_gpio_51, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
	{ GPIO1B, pad_gpio_52, TLMM_PAD_FUNCTION, GPIO_PAD_PULL_UP, 8 },
#endif
};

/* This function is used by SBI to pass pin configurations to the Linux
 * kernel.
 */
void duowen_gpio_init(void)
{
	int i;
	struct duowen_pad *p;

	for (i = 0; i < ARRAY_SIZE(duowen_pads); i++) {
		p = &(duowen_pads[i]);
		gpio_config_pad(p->port, p->pin, p->pad, p->drive);
		gpio_config_mux(p->port, p->pin, p->func);
	}
}

#ifdef CONFIG_DUOWEN_GPIO_PORT
uint8_t gpio_hw_read_pin(uint8_t port, uint16_t pin)
{
	if (port > GPIO_HW_MAX_PORTS)
		return 0;

	return dw_gpio_read_pin(duowen_gpiop_chip(port),
				duowen_gpiop_port(port), pin);
}

void gpio_hw_write_pin(uint8_t port, uint16_t pin, uint8_t val)
{
	if (port > GPIO_HW_MAX_PORTS)
		return;

	dw_gpio_write_pin(duowen_gpiop_chip(port),
			  duowen_gpiop_port(port), pin, val);
}

uint32_t gpio_hw_read_port(uint8_t port, uint32_t mask)
{
	uint32_t val;

	if (port > GPIO_HW_MAX_PORTS)
		return 0;

	val = dw_gpio_read_port(duowen_gpiop_chip(port),
				duowen_gpiop_port(port));
	return val & mask;
}

void gpio_hw_write_port(uint8_t port, uint32_t val)
{
	if (port > GPIO_HW_MAX_PORTS)
		return;

	dw_gpio_write_port(duowen_gpiop_chip(port),
			   duowen_gpiop_port(port), val);
}

void gpio_hw_config_pad(uint8_t port, uint16_t pin,
			uint8_t pad, uint8_t drv)
{
	if (port > GPIO_HW_MAX_PORTS)
		return;

	dw_gpio_config_pad(duowen_gpiop_chip(port),
			   duowen_gpiop_port(port), pin, pad, drv);
	tlmm_config_pad(pin, pad, drv);
}

void gpio_hw_config_mux(uint8_t port, uint16_t pin, uint8_t mux)
{
	if (port > GPIO_HW_MAX_PORTS)
		return;

	tlmm_config_mux(pin, mux);
}

static bool duowen_gpio_is_porta(uint16_t port)
{
	return duowen_gpiop_port(port) == 0;
}

void gpio_hw_config_irq(uint8_t port, uint16_t pin, uint8_t trig)
{
	if (port > GPIO_HW_MAX_PORTS)
		return;

	if (duowen_gpio_is_porta(port))
		dw_gpio_config_irq(duowen_gpiop_chip(port), pin, trig);
}

#ifndef CONFIG_SYS_NOIRQ
void gpio_hw_enable_irq(uint8_t port, uint16_t pin)
{
	if (port > GPIO_HW_MAX_PORTS)
		return;

	if (duowen_gpio_is_porta(port))
		dw_gpio_enable_irq(duowen_gpiop_chip(port), pin);
}

void gpio_hw_disable_irq(uint8_t port, uint16_t pin)
{
	if (port > GPIO_HW_MAX_PORTS)
		return;

	if (duowen_gpio_is_porta(port))
		dw_gpio_disable_irq(duowen_gpiop_chip(port), pin);
}
#endif

void gpio_hw_clear_irq(uint8_t port, uint16_t pin)
{
	if (port > GPIO_HW_MAX_PORTS)
		return;

	if (duowen_gpio_is_porta(port))
		dw_gpio_clear_irq(duowen_gpiop_chip(port), pin);
}

uint8_t gpio_hw_irq_status(uint8_t port, uint16_t pin)
{
	bool girq = 0;

	if (port > GPIO_HW_MAX_PORTS)
		return 0;
	if (duowen_gpio_is_porta(port))
		girq = dw_gpio_irq_status(duowen_gpiop_chip(port), pin);
	return girq;
}
#else /* CONFIG_DUOWEN_GPIO_PORT */
uint16_t duowen_gpios[NR_GPIOS] = {
	pad_gpio_0,
	pad_gpio_1,
	pad_gpio_2,
	pad_gpio_3,
	pad_gpio_4,
	pad_gpio_5,
	pad_gpio_6,
	pad_gpio_7,
	pad_gpio_8,
	pad_gpio_9,
	pad_gpio_10,
	pad_gpio_11,
	pad_gpio_12,
	pad_gpio_13,
	pad_gpio_14,
	pad_gpio_15,
	pad_gpio_16,
	pad_gpio_17,
	pad_gpio_18,
	pad_gpio_19,
	pad_gpio_20,
	pad_gpio_21,
	pad_gpio_22,
	pad_gpio_23,
	pad_gpio_24,
	pad_gpio_25,
	pad_gpio_26,
	pad_gpio_27,
	pad_gpio_28,
	pad_gpio_29,
	pad_gpio_30,
	pad_gpio_31,
	pad_gpio_32,
	pad_gpio_33,
	pad_gpio_34,
	pad_gpio_35,
	pad_gpio_36,
	pad_gpio_37,
	pad_gpio_38,
	pad_gpio_39,
	pad_gpio_40,
	pad_gpio_41,
	pad_gpio_42,
	pad_gpio_43,
	pad_gpio_44,
	pad_gpio_45,
	pad_gpio_46,
	pad_gpio_47,
	pad_gpio_48,
	pad_gpio_49,
	pad_gpio_50,
	pad_gpio_51,
	pad_gpio_52,
	pad_gpio_53,
	pad_gpio_54,
	pad_gpio_55,
	pad_gpio_56,
	pad_gpio_57,
	pad_gpio_58,
	pad_gpio_59,
	pad_gpio_60,
	pad_gpio_61,
	pad_gpio_62,
	pad_gpio_63,
	pad_gpio_64,
	pad_gpio_65,
	pad_gpio_66,
	pad_gpio_67,
	pad_gpio_68,
	pad_gpio_69,
	pad_gpio_70,
	pad_gpio_71,
	pad_gpio_72,
	pad_gpio_73,
	pad_gpio_74,
	pad_gpio_75,
	pad_gpio_76,
	pad_gpio_77,
	pad_gpio_78,
	pad_gpio_79,
	pad_gpio_80,
	pad_gpio_81,
	pad_gpio_82,
	pad_gpio_83,
	pad_gpio_84,
	pad_gpio_85,
	pad_gpio_86,
	pad_gpio_87,
	pad_gpio_88,
	pad_gpio_89,
	pad_gpio_90,
	pad_gpio_91,
	pad_gpio_92,
	pad_gpio_93,
	pad_gpio_94,
	pad_gpio_95,
	pad_gpio_96,
	pad_gpio_97,
	pad_gpio_98,
	pad_gpio_99,
	pad_gpio_100,
	pad_gpio_101,
	pad_gpio_102,
	pad_gpio_103,
	pad_gpio_104,
	pad_gpio_105,
	pad_gpio_106,
	pad_gpio_107,
	pad_gpio_108,
	pad_gpio_109,
	pad_gpio_110,
	pad_gpio_111,
	pad_gpio_112,
	pad_gpio_113,
	pad_gpio_114,
	pad_gpio_115,
	pad_gpio_116,
	pad_gpio_117,
	pad_gpio_118,
	pad_gpio_119,
	pad_gpio_120,
	pad_gpio_121,
	pad_gpio_122,
	pad_gpio_123,
	pad_gpio_124,
	pad_gpio_125,
	pad_gpio_126,
	pad_gpio_127,
	pad_gpio_128,
	pad_gpio_129,
	pad_gpio_130,
	pad_gpio_131,
	pad_gpio_132,
	pad_gpio_133,
	pad_gpio_134,
	pad_gpio_135,
	pad_gpio_136,
	pad_gpio_137,
	pad_gpio_138,
	pad_gpio_139,
	pad_gpio_140,
	pad_gpio_141,
	pad_gpio_142,
	pad_gpio_143,
	pad_gpio_144,
	pad_gpio_145,
	pad_gpio_146,
	pad_gpio_147,
	pad_gpio_148,
	pad_gpio_149,
	pad_gpio_150,
	pad_gpio_151,
	pad_gpio_152,
	pad_gpio_153,
	pad_gpio_154,
	pad_gpio_155,
	pad_gpio_156,
	pad_gpio_157,
	pad_gpio_158,
	pad_gpio_159,
};

static inline uint16_t duowen_gpio_from_pin(uint16_t pin)
{
	uint16_t gpio;

	for (gpio = 0; gpio < ARRAY_SIZE(duowen_gpios); gpio++) {
		if (pin == duowen_gpios[gpio])
			return gpio;
	}
	return INVALID_TLMM_GPIO;
}

uint8_t gpio_hw_read_pin(uint8_t port, uint16_t pin)
{
	uint16_t gpio = duowen_gpio_from_pin(pin);

	if (gpio == INVALID_TLMM_GPIO)
		return 0;

	return dw_gpio_read_pin(duowen_gpio_chip(pin),
				duowen_gpio_port(pin),
				duowen_gpio_pin(pin));
}

void gpio_hw_write_pin(uint8_t port, uint16_t pin, uint8_t val)
{
	uint16_t gpio = duowen_gpio_from_pin(pin);

	if (gpio == INVALID_TLMM_GPIO)
		return;

	dw_gpio_write_pin(duowen_gpio_chip(pin), duowen_gpio_port(pin),
			  duowen_gpio_pin(pin), val);
}

void gpio_hw_config_pad(uint8_t port, uint16_t pin,
			uint8_t pad, uint8_t drv)
{
	uint16_t gpio = duowen_gpio_from_pin(pin);

	if (gpio == INVALID_TLMM_GPIO)
		return;

	dw_gpio_config_pad(duowen_gpio_chip(pin), duowen_gpio_port(pin),
			   duowen_gpio_pin(pin), pad, drv);
	tlmm_config_pad(gpio, pad, drv);
}

void gpio_hw_config_mux(uint8_t port, uint16_t pin, uint8_t mux)
{
	uint16_t gpio = duowen_gpio_from_pin(pin);

	if (gpio == INVALID_TLMM_GPIO)
		return;

	tlmm_config_mux(gpio, mux);
}

static bool duowen_gpio_is_porta(uint16_t gpios)
{
	return duowen_gpio_port(gpios) == 0;
}

void gpio_hw_config_irq(uint8_t port, uint16_t pin, uint8_t trig)
{
	uint16_t gpio = duowen_gpio_from_pin(pin);

	if (gpio == INVALID_TLMM_GPIO)
		return;

	if (duowen_gpio_is_porta(pin))
		dw_gpio_config_irq(duowen_gpio_chip(pin),
				   duowen_gpio_pin(pin), trig);
}

#ifndef CONFIG_SYS_NOIRQ
void gpio_hw_enable_irq(uint8_t port, uint16_t pin)
{
	uint16_t gpio = duowen_gpio_from_pin(pin);

	if (gpio == INVALID_TLMM_GPIO)
		return;

	if (duowen_gpio_is_porta(pin))
		dw_gpio_enable_irq(duowen_gpio_chip(pin),
				   duowen_gpio_pin(pin));
}

void gpio_hw_disable_irq(uint8_t port, uint16_t pin)
{
	uint16_t gpio = duowen_gpio_from_pin(pin);

	if (gpio == INVALID_TLMM_GPIO)
		return;

	if (duowen_gpio_is_porta(pin))
		dw_gpio_disable_irq(duowen_gpio_chip(pin),
				    duowen_gpio_pin(pin));
}
#endif

void gpio_hw_clear_irq(uint8_t port, uint16_t pin)
{
	uint16_t gpio = duowen_gpio_from_pin(pin);

	if (gpio == INVALID_TLMM_GPIO)
		return;

	if (duowen_gpio_is_porta(pin))
		dw_gpio_clear_irq(duowen_gpio_chip(pin),
				  duowen_gpio_pin(pin));
}

uint8_t gpio_hw_irq_status(uint8_t port, uint16_t pin)
{
	uint16_t gpio = duowen_gpio_from_pin(pin);
	bool girq = 0;

	if (gpio == INVALID_TLMM_GPIO)
		return 0;

	if (duowen_gpio_is_porta(pin))
		girq = dw_gpio_irq_status(duowen_gpio_chip(pin),
					  duowen_gpio_pin(pin));
	return girq;
}
#endif /* CONFIG_DUOWEN_GPIO_PORT */

void gpio_hw_ctrl_init(void)
{
	clk_enable(gpio0_clk);
	clk_enable(gpio1_clk);
	clk_enable(gpio2_clk);
}
