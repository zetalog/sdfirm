#ifndef __LED_DRIVER_H_INCLUDE__
#define __LED_DRIVER_H_INCLUDE__

#ifdef CONFIG_LED_AT90USB1287
#include <asm/mach-at90usb1287/led.h>
#endif
#ifdef CONFIG_LED_AT90SCR100
#include <asm/mach-at90scr100/led.h>
#endif
#ifdef CONFIG_LED_LM3S9B92
#include <asm/mach-lm3s9b92/led.h>
#endif

#ifndef ARCH_HAVE_LED
typedef uint8_t led_no_t;
#define NR_LED_HW_LIGHTS	0
#define NR_LED_HW_COLORS	0
#define led_hw_ctrl_init()
#define led_hw_light_on(no, color)
#define led_hw_light_off(no)
#endif

#endif /* __LED_DRIVER_H_INCLUDE__ */
