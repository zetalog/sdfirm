#include <target/led.h>

#define __LED_COLOR_PER_LIGHT	(uint8_t)(LED_BITS/NR_LED_HW_LIGHTS)
#define __LED_LIGHT(light)	(led_no_t)(light * __LED_COLOR_PER_LIGHT)
#define __LED_COLOR_ALL		((1<<__LED_COLOR_PER_LIGHT)-1)
#ifdef CONFIG_LED_AT90USB1287_VAR_COLORS
#define __LED_COLOR(light, color)	(color)
#else
#ifdef CONFIG_LED_AT90USB1287_CROSS_COLORS
#define __LED_COLOR(light, color)	\
	(light == 0 ? 1<<(__LED_COLOR_PER_LIGHT-color-1) : 1<<color)
#else
#define __LED_COLOR(light, color)	(1<<color)
#endif
#endif

void led_hw_light_off(led_no_t light)
{
	LED_PORT &= ~(__LED_COLOR_ALL << (__LED_LIGHT(light) + LED_OFFSET));
}

void led_hw_light_on(led_no_t light, uint8_t color)
{
	led_hw_light_off(light);
	LED_PORT |= (__LED_COLOR(light, color) << (__LED_LIGHT(light) + LED_OFFSET));
}
