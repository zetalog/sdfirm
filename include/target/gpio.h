#ifndef __GPIO_H_INCLUDE__
#define __GPIO_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

/*=========================================================================
 * GPIO mux configuration
 *=======================================================================*/
#define GPIO_MUX_NONE		0

/*=========================================================================
 * GPIO pad configuration
 *=======================================================================*/
#define GPIO_DIR_NONE		0x00  /* peripheral function */
#define GPIO_DIR_IN		0x01  /* GPIO input */
#define GPIO_DIR_OUT		0x02  /* GPIO output */

#define GPIO_PAD_ANALOG_IO	0x00
#define GPIO_PAD_DIGITAL_IO	0x08
#define GPIO_PAD_PULL_DOWN	0x04
#define GPIO_PAD_PULL_UP	0x02
#define GPIO_PAD_PUSH_PULL	0x00
#define GPIO_PAD_OPEN_DRAIN	0x01

#define GPIO_PAD_PP		(GPIO_PAD_DIGITAL_IO | GPIO_PAD_PUSH_PULL)
#define GPIO_PAD_OD		(GPIO_PAD_DIGITAL_IO | GPIO_PAD_OPEN_DRAIN)
/* push pull with weak pull up */
#define GPIO_PAD_PP_WU		(GPIO_PAD_PP | GPIO_PAD_PULL_UP)
/* push pull with weak pull down */
#define GPIO_PAD_PP_WD		(GPIO_PAD_PP | GPIO_PAD_PULL_DOWN)
/* open drain with weak pull up */
#define GPIO_PAD_OD_WU		(GPIO_PAD_OD | GPIO_PAD_PULL_UP)
/* open drain with weak pull down */
#define GPIO_PAD_OD_WD		(GPIO_PAD_OD | GPIO_PAD_PULL_DOWN)

#include <driver/gpio.h>

#define gpio_read_pin(port, pin)			\
	gpio_hw_read_pin(port, pin)
#define gpio_write_pin(port, pin, val)			\
	gpio_hw_write_pin(port, pin, val)
#define gpio_read_port(port)				\
	gpio_hw_read_port(port)
#define gpio_write_port(port, val)			\
	gpio_hw_write_port(port, val)
#define gpio_config_pad(port, pin, dir, pad, ma)	\
	gpio_hw_config_pad(port, pin, dir, pad, ma)
#define gpio_config_mux(port, pin, mux)			\
	gpio_hw_config_mux(port, pin, mux)

#endif /* __GPIO_H_INCLUDE__ */
