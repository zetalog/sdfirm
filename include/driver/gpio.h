#ifndef __GPIO_DRIVER_H_INCLUDE__
#define __GPIO_DRIVER_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#ifdef CONFIG_ARCH_HAS_GPIO
#include <asm/mach/gpio.h>
#endif

#ifndef ARCH_HAVE_GPIO
#define gpio_hw_ctrl_init()
#define gpio_hw_read_pin(port, pin)		0
#define gpio_hw_write_pin(port, pin, val)
#define gpio_hw_read_port(port)			0
#define gpio_hw_write_port(port, val)
/* NOTE: No Pad/Mux Interfaces
 *
 * driver are allowed not to provide following functions for convinience.
 */
#define gpio_hw_config_pad(port, pin, dir, pad, ma)
#define gpio_hw_config_mux(port, pin, mux)
#endif

#endif /* __GPIO_DRIVER_H_INCLUDE__ */
