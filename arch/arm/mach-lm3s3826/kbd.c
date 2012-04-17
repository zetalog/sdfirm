#include <target/kbd.h>
#include <target/gpio.h>
#include <target/delay.h>

#ifdef CONFIG_KBD_LM3S3826_SINGLE_PORT
#define KBD_MAX_IRQ	1
#endif
#ifdef CONFIG_KBD_LM3S3826_DOUBLE_PORT
#define KBD_MAX_IRQ	2
#endif

uint8_t __kbd_hw_last_status = 0x0;

static text_byte_t __kbd_hw_ascii_keys[__KBD_HW_MAX_SCANS] = {
	/* ESCAPE  UP  ENTER  DOWN */
	0x1B,	0x26,	0x0D,	0x28,
};

unsigned char kbd_hw_translate_key(uint8_t scancode)
{
	return __kbd_hw_ascii_keys[scancode];
}

static uint8_t __kbd_hw_get_status(void)
{
	uint8_t status = 0x0;

	if (KBD_MAX_IRQ == 1) {
		status = (__kbd_hw_read_port(GPIOA, 0xf<<4))>>4;
	} else if (KBD_MAX_IRQ == 2) {
		status = (__kbd_hw_read_port(GPIOB, 0x3<<2))>>2 |
			(__kbd_hw_read_port(GPIOA, 0x3<<6)>>4);
	}
	return status;
}

static void __kbd_hw_handle_status(uint8_t status)
{
	uint8_t scancode, flag;

	for (scancode = 0; scancode < __KBD_HW_MAX_SCANS; scancode++) {
		flag = 1 << scancode;
		if (bits_raised_any(status, flag)) {
			if (!bits_raised_any(__kbd_hw_last_status, flag)) {
				kbd_event_down(scancode);
				raise_bits(__kbd_hw_last_status, flag);
			}
		} else {
			if (bits_raised_any(__kbd_hw_last_status, flag)) {
				kbd_event_up(scancode);
				unraise_bits(__kbd_hw_last_status, flag);
			}
		}
	}
}

static void __kbd_hw_scan_async(void)
{
	uint8_t status;

	status = __kbd_hw_get_status();
	/* bouncing filter according to data sheet */
        mdelay(10);
	status &= __kbd_hw_get_status();
	__kbd_hw_handle_status(status);
}

void kbd_hw_scan_sync(uint8_t sec)
{
	uint8_t i, j;

	for (i = 0; i < sec; i++) {
		for (j = 0; j < 100; j++) {
			__kbd_hw_scan_async();
		}
	}
}

static void __kbd_hw_config_pin(uint8_t port, uint8_t nr) {
	gpio_config_mux(port, nr, GPIO_MUX_NONE);
	gpio_config_pad(port, nr, GPIO_PAD_PP_WD, 0);

	gpio_disable_irq(port, nr);
	gpio_config_irq(port, nr, GPIO_IRQ_BE);
	gpio_enable_irq(port, nr);
}

static void __kbd_hw_config_pad(void)
{
	uint8_t nr;

	pm_hw_resume_device(DEV_GPIOA, DEV_MODE_ON);

	if (KBD_MAX_IRQ == 1) {
		for (nr = 4; nr < 8; nr++) {
			__kbd_hw_config_pin(GPIOA, nr);
		}
	} else if (KBD_MAX_IRQ == 2) {
		pm_hw_resume_device(DEV_GPIOB, DEV_MODE_ON);

		for (nr = 6; nr < 8; nr++) {
			__kbd_hw_config_pin(GPIOA, nr);
		}
		for (nr = 2; nr < 4; nr++) {
			__kbd_hw_config_pin(GPIOB, nr);
		}
	}
}

static void __kbd_hw_clear_irq(void)
{
	uint8_t status;

	status = gpio_irq_status(GPIOA);
	if (!status) {
		status = gpio_irq_status(GPIOB);
		status = ((status >> 2) & 0x3) << 2;
		gpio_clear_irq(GPIOB, __ffs8(status));
	} else {
		if (KBD_MAX_IRQ == 1) {
			status = ((status >> 4) & 0xf) << 4;
		} else if (KBD_MAX_IRQ == 2) {
			status = ((status >> 6) & 0x3) << 6;
		}
		gpio_clear_irq(GPIOA, __ffs8(status));
	}
}

static void __kbd_hw_handle_irq(void)
{
	__kbd_hw_clear_irq();
	__kbd_hw_scan_async();
}

static void __kbd_hw_irq_init(void)
{
	uint8_t nr, irq;

	for (nr = 0; nr < KBD_MAX_IRQ; nr++) {
		irq = __kbd_hw_port_irq(nr);
		vic_hw_register_irq(irq, __kbd_hw_handle_irq);
		vic_hw_irq_enable(irq);
	}
}

void kbd_hw_ctrl_init(void)
{
	__kbd_hw_config_pad();
	__kbd_hw_irq_init();
}
