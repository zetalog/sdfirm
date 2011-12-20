#include <target/led.h>

DECLARE_BITMAP(led_reg_bits, NR_LED_LIGHTS);

static boolean led_light_claimed(led_no_t no)
{
	return test_bit(no, led_reg_bits);
}

led_no_t led_claim_light(void)
{
	led_no_t no;
	for (no = 0; no < NR_LED_LIGHTS; no++) {
		if (!led_light_claimed(no)) {
			set_bit(no, led_reg_bits);
			break;
		}
	}
	return no;
}

void led_init(void)
{
	led_hw_ctrl_init();
}
