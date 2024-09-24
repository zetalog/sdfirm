#include <target/gpio.h>
#include <target/cmdline.h>

void gpio_init(void)
{
	gpio_hw_ctrl_init();
}

static int do_gpio_pin(int argc, char *argv[])
{
	int port, pin;
	uint8_t val;
	uint8_t mode = 0;
	uint8_t ma = 10;

	if (argc < 5)
		return -EINVAL;
	port = (uint8_t)strtoull(argv[2], 0, 0);
	pin = (uint8_t)strtoull(argv[3], 0, 0);
	if (strcmp(argv[4], "config") == 0) {
		if (argc < 9)
			return -EINVAL;
		if (strcmp(argv[5], "in") == 0)
			mode |= GPIO_DRIVE_IN;
		if (strcmp(argv[6], "od") == 0)
			mode |= GPIO_PAD_OPEN_DRAIN;
		else
			mode |= GPIO_PAD_PUSH_PULL;
		if (strcmp(argv[7], "up") == 0)
			mode |= GPIO_PAD_PULL_UP;
		else
			mode |= GPIO_PAD_PULL_DOWN;
		if (strcmp(argv[8], "strong") == 0)
			mode |= GPIO_PAD_STRONG_PULL;
		else
			mode |= GPIO_PAD_WEAK_PULL;
		if (argc >= 9)
			ma = (uint8_t)strtoull(argv[9], 0, 0);
		gpio_config_pad(port, pin, mode, ma);
		return 0;
	}
	if (strcmp(argv[4], "read") == 0) {
		printf("GPIO%d-%d: %d\n", port, pin,
		       gpio_read_pin(port, pin) ? 1 : 0);
		return 0;
	}
	if (strcmp(argv[4], "write") == 0) {
		if (argc < 6)
			return -EINVAL;
		val  = (uint8_t)strtoull(argv[5], 0, 0);
		gpio_write_pin(port, pin, !!val);
		return 0;
	}
	return -EINVAL;

}

#ifndef CONFIG_SYS_NOIRQ
static int do_gpio_irq(int argc, char *argv[])
{
	int port, pin;
	uint8_t mode;

	if (argc < 5)
		return -EINVAL;

	port = (uint8_t)strtoull(argv[2], 0, 0);
	pin = (uint8_t)strtoull(argv[3], 0, 0);
	if (strcmp(argv[4], "config") == 0) {
		if (argc < 7)
			return -EINVAL;
		if (strcmp(argv[5], "edge") == 0)
			mode |= GPIO_IRQ_EDGE_TRIG;
		else
			mode |= GPIO_IRQ_LEVEL_TRIG;
		if (strcmp(argv[6], "high") == 0)
			mode |= GPIO_IRQ_HIGH;
		else
			mode |= GPIO_IRQ_LOW
		gpio_config_irq(port, pin, mode);
		return 0;
	}
	if (strcmp(argv[4], "enable") == 0) {
		gpio_enable_irq(port, pin);
		return 0;
	}
	if (strcmp(argv[4], "disable") == 0) {
		gpio_disable_irq(port, pin);
		return 0;
	}
	return -EINVAL;
}
#else
static int do_gpio_irq(int argc, char *argv[])
{
	return -ENODEV;
}
#endif

static int do_gpio(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "pin") == 0)
		return do_gpio_pin(argc, argv);
	if (strcmp(argv[1], "irq") == 0)
		return do_gpio_irq(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(gpio, do_gpio, "general purpose IO (GPIO) commands",
	"gpio pin <port> <pin> config <in|out> <pp|od> <up|down> <weak|strong> [ma]\n"
	"    -configure GPIO pad\n"
	"gpio pin <port> <pin> write <value>\n"
	"    -write GPIO value\n"
	"gpio pin <port> <pin> read\n"
	"    -read GPIO value\n"
	"gpio irq <port> <pin> config <in|out> <edge|level> <high|low>\n"
	"    -configure GPIO interrupt\n"
	"gpio irq <port> <pin> enable\n"
	"    -enable GPIO interrupt\n"
	"gpio irq <port> <pin> disable\n"
	"    -disable GPIO interrupt\n"
);
