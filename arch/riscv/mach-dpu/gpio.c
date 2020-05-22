#include <target/gpio.h>
#include <target/irq.h>
#include <target/cmdline.h>

struct dpu_gpio_irq {
	irq_t irq;
	irq_handler handler;
	uint8_t trigger_pin;
	bool triggered;
};

static struct dpu_gpio_irq dpu_gpio_irqs[];

static void dpu_gpio_handler(struct dpu_gpio_irq *girq)
{
	girq->triggered = true;
	irqc_mask_irq(girq->irq);
	printf("GPIO%d IRQ\n", girq->irq - IRQ_GPIO0);
}

static void dpu_gpio0_handler(void)
{
	dpu_gpio_handler(&dpu_gpio_irqs[0]);
}

static void dpu_gpio1_handler(void)
{
	dpu_gpio_handler(&dpu_gpio_irqs[1]);
}

static void dpu_gpio2_handler(void)
{
	dpu_gpio_handler(&dpu_gpio_irqs[2]);
}

static void dpu_gpio3_handler(void)
{
	dpu_gpio_handler(&dpu_gpio_irqs[3]);
}

static void dpu_gpio4_handler(void)
{
	dpu_gpio_handler(&dpu_gpio_irqs[4]);
}

static void dpu_gpio5_handler(void)
{
	dpu_gpio_handler(&dpu_gpio_irqs[5]);
}

static void dpu_gpio6_handler(void)
{
	dpu_gpio_handler(&dpu_gpio_irqs[6]);
}

static void dpu_gpio7_handler(void)
{
	dpu_gpio_handler(&dpu_gpio_irqs[7]);
}

static struct dpu_gpio_irq dpu_gpio_irqs[] = {
	{ IRQ_GPIO0, dpu_gpio0_handler, 1 },
	{ IRQ_GPIO1, dpu_gpio1_handler, 0 },
	{ IRQ_GPIO2, dpu_gpio2_handler, 3 },
	{ IRQ_GPIO3, dpu_gpio3_handler, 2 },
	{ IRQ_GPIO4, dpu_gpio4_handler, 5 },
	{ IRQ_GPIO5, dpu_gpio5_handler, 4 },
	{ IRQ_GPIO6, dpu_gpio6_handler, 7 },
	{ IRQ_GPIO7, dpu_gpio7_handler, 6 },
};

static void dpu_gpio_irq_trigger(int pin, bool high)
{
	struct dpu_gpio_irq *girq;

	if (pin >= 8)
		return;

	girq = &(dpu_gpio_irqs[pin]);
	girq->triggered = false;
	gpio_config_pad(GPIOA, pin, GPIO_PAD_PP, GPIO_DRIVE_IN);
	gpio_config_pad(GPIOA, girq->trigger_pin, GPIO_PAD_PP, 2);
	gpio_config_irq(GPIOA, pin, high ? GPIO_IRQ_HL : GPIO_IRQ_LL);
	gpio_write_pin(GPIOA, girq->trigger_pin, high ? 1 : 0);
	while (!girq->triggered) {
		irq_local_enable();
		irq_local_disable();
	}
	gpio_write_pin(GPIOA, girq->trigger_pin, high ? 0 : 1);
	irqc_ack_irq(girq->irq);
	irqc_unmask_irq(girq->irq);
}

void dpu_gpio_irq_init(void)
{
	int pin;
	struct dpu_gpio_irq *girq;

	for (pin = 0; pin < 8; pin++) {
		girq = &dpu_gpio_irqs[pin];
		irqc_configure_irq(girq->irq, 0, IRQ_LEVEL_TRIGGERED);
		irq_register_vector(girq->irq, girq->handler);
		irqc_enable_irq(girq->irq);
	}
}

static int do_gpiot_irq(int argc, char **argv)
{
	int gpio;
	bool high = true;

	if (argc < 3)
		return -EINVAL;
	gpio = strtoul(argv[2], NULL, 0);
	if (gpio < 0 || gpio >= 8) {
		printf("Invalid gpio %s.\n", argv[2]);
		return -EINVAL;
	}

	if (argc > 3) {
		if (strcmp(argv[3], "low") == 0)
			high = false;
	}
	dpu_gpio_irq_trigger(gpio, high);
	return 0;
}

static int do_gpiot(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "irq") == 0)
		return do_gpiot_irq(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(gpiot, do_gpiot, "Genral purpose input/output testing",
	"gpiot irq gpio [high|low]\n"
	"    -testing GPIO IRQ\n"
);
