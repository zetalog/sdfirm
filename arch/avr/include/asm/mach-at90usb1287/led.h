#ifndef __LED_AT90USB1287_H_INCLUDE__
#define __LED_AT90USB1287_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/gpio.h>

#ifndef ARCH_HAVE_LED
#define ARCH_HAVE_LED			1
#else
#error "Multiple LED controller defined"
#endif

#define LED_PORT		PORTD
#define LED_DDR			DDRD
#define LED_PIN			PIND

#define LED_BITS		4
#define LED_OFFSET		4
#define LED_MASK		((1<<LED_BITS)-1)<<LED_OFFSET

#ifdef CONFIG_LED_AT90USB1287_MAX_LIGHTS
#define NR_LED_HW_LIGHTS	CONFIG_LED_AT90USB1287_MAX_LIGHTS
#else
#define NR_LED_HW_LIGHTS	4
#endif
#ifdef CONFIG_LED_AT90USB1287_MAX_COLORS
#define NR_LED_HW_COLORS	CONFIG_LED_AT90USB1287_MAX_COLORS
#else
#define NR_LED_HW_COLORS	1
#endif

#define LED_COLOR_GREEN		0
#define LED_COLOR_RED		1

typedef uint8_t led_no_t;

#define led_hw_ctrl_init()	(LED_DDR |= LED_MASK)
void led_hw_light_on(led_no_t no, uint8_t color);
void led_hw_light_off(led_no_t no);

#endif /* __LED_AT90USB1287_H_INCLUDE__ */
