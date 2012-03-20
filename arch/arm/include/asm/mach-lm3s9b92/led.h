#ifndef __LED_LM3S9B92_H_INCLUDE__
#define __LED_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <target/gpio.h>

#ifndef ARCH_HAVE_LED
#define ARCH_HAVE_LED			1
#else
#error "Multiple LED controller defined"
#endif

#define NR_LED_HW_LIGHTS	1
#define NR_LED_HW_COLORS	1

#define LED_COLOR_GREEN		0

typedef uint8_t led_no_t;

void led_hw_ctrl_init(void);
void led_hw_light_on(led_no_t no, uint8_t color);
void led_hw_light_off(led_no_t no);

#endif /* __LED_LM3S9B92_H_INCLUDE__ */
