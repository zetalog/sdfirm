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
#define GPIO_PAD_PUSH_PULL	0x00
#define GPIO_PAD_OPEN_DRAIN	0x01
#define GPIO_PAD_PULL_UP	0x02
#define GPIO_PAD_PULL_DOWN	0x04
#define GPIO_PAD_ANALOG_IO	0x00
#define GPIO_PAD_DIGITAL_IO	0x08
#define GPIO_PAD_WEAK_PULL	0x00
#define GPIO_PAD_MEDIUM_PULL	0x10

#define GPIO_PAD_PP		(GPIO_PAD_DIGITAL_IO | GPIO_PAD_PUSH_PULL)
#define GPIO_PAD_OD		(GPIO_PAD_DIGITAL_IO | GPIO_PAD_OPEN_DRAIN)
/* push pull with weak pull up */
#define GPIO_PAD_PP_WU		\
	(GPIO_PAD_PP | GPIO_PAD_PULL_UP | GPIO_PAD_WEAK_PULL)
/* push pull with weak pull down */
#define GPIO_PAD_PP_WD		\
	(GPIO_PAD_PP | GPIO_PAD_PULL_DOWN | GPIO_PAD_WEAK_PULL)
/* push pull with medium pull up */
#define GPIO_PAD_PP_MU		\
	(GPIO_PAD_PP | GPIO_PAD_PULL_UP | GPIO_PAD_MEDIUM_PULL)
/* push pull with medium pull down */
#define GPIO_PAD_PP_MD		\
	(GPIO_PAD_PP | GPIO_PAD_PULL_DOWN | GPIO_PAD_MEDIUM_PULL)
/* open drain with weak pull up */
#define GPIO_PAD_OD_WU		\
	(GPIO_PAD_OD | GPIO_PAD_PULL_UP | GPIO_PAD_WEAK_PULL)
/* open drain with weak pull down */
#define GPIO_PAD_OD_WD		\
	(GPIO_PAD_OD | GPIO_PAD_PULL_DOWN | GPIO_PAD_WEAK_PULL)
/* open drain with medium pull up */
#define GPIO_PAD_OD_MU		\
	(GPIO_PAD_OD | GPIO_PAD_PULL_UP | GPIO_PAD_MEDIUM_PULL)
/* open drain with medium pull down */
#define GPIO_PAD_OD_MD		\
	(GPIO_PAD_OD | GPIO_PAD_PULL_DOWN | GPIO_PAD_MEDIUM_PULL)

#include <driver/gpio.h>

#define gpio_read_pin(port, pin)			\
	gpio_hw_read_pin(port, pin)
#define gpio_write_pin(port, pin, val)			\
	gpio_hw_write_pin(port, pin, val)
#define gpio_read_port(port, mask)			\
	gpio_hw_read_port(port, mask)
#define gpio_write_port(port, mask, val)		\
	gpio_hw_write_port(port, mask, val)
#define gpio_config_pad(port, pin, pad, ma)		\
	gpio_hw_config_pad(port, pin, pad, ma)
#define gpio_config_mux(port, pin, mux)			\
	gpio_hw_config_mux(port, pin, mux)

#endif /* __GPIO_H_INCLUDE__ */
