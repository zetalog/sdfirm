#ifndef __LED_H_INCLUDE__
#define __LED_H_INCLUDE__

#include <target/generic.h>

#include <driver/led.h>

#define NR_LED_LIGHTS	NR_LED_HW_LIGHTS
#define NR_LED_COLORS	NR_LED_HW_COLORS
#define LED_INVALID_NO	NR_LED_LIGHTS

led_no_t led_claim_light(void);
#define led_light_on(no, color)	led_hw_light_on(no, color)
#define led_light_off(no)	led_hw_light_off(no)

#endif /* __LED_H_INCLUDE__ */

