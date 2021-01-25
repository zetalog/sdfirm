/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2009-2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)gpio.h: general purpose input/output (GPIO) interfaces
 * $Id: gpio.h,v 1.1 2019-08-17 07:24:00 zhenglv Exp $
 */

#ifndef __GPIO_H_INCLUDE__
#define __GPIO_H_INCLUDE__

#include <target/generic.h>

/*=========================================================================
 * GPIO special drive
 *=======================================================================*/
#define GPIO_DRIVE_IN		0

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
#define GPIO_PAD_STRONG_PULL	0x20
#define GPIO_PAD_SLEW_RATE	0x40
#define GPIO_PAD_SCHMITT_TRIG	0x80
/* Re-use schmitt trigger unless being complained */
#define GPIO_PAD_KEEPER		0x80
#define GPIO_PAD_PULL_MASK	\
	(GPIO_PAD_PULL_UP | GPIO_PAD_PULL_DOWN)

#define GPIO_PAD_PP		(GPIO_PAD_DIGITAL_IO | GPIO_PAD_PUSH_PULL)
#define GPIO_PAD_OD		(GPIO_PAD_DIGITAL_IO | GPIO_PAD_OPEN_DRAIN)
#define GPIO_PAD_KB		(GPIO_PAD_PP | GPIO_PAD_SLEW_RATE)
#define GPIO_PAD_WU		(GPIO_PAD_PULL_UP | GPIO_PAD_WEAK_PULL)
#define GPIO_PAD_WD		(GPIO_PAD_PULL_DOWN | GPIO_PAD_WEAK_PULL)
#define GPIO_PAD_MU		(GPIO_PAD_PULL_UP | GPIO_PAD_MEDIUM_PULL)
#define GPIO_PAD_MD		(GPIO_PAD_PULL_DOWN | GPIO_PAD_MEDIUM_PULL)
#define GPIO_PAD_SU		(GPIO_PAD_PULL_UP | GPIO_PAD_STRONG_PULL)
#define GPIO_PAD_SD		(GPIO_PAD_PULL_DOWN | GPIO_PAD_STRONG_PULL)

/* push pull with weak pull up */
#define GPIO_PAD_PP_WU		(GPIO_PAD_PP | GPIO_PAD_WU)
/* push pull with weak pull down */
#define GPIO_PAD_PP_WD		(GPIO_PAD_PP | GPIO_PAD_WD)
/* push pull with medium pull up */
#define GPIO_PAD_PP_MU		(GPIO_PAD_PP | GPIO_PAD_MU)
/* push pull with medium pull down */
#define GPIO_PAD_PP_MD		(GPIO_PAD_PP | GPIO_PAD_MD)
/* push pull with strong pull up */
#define GPIO_PAD_PP_SU		(GPIO_PAD_PP | GPIO_PAD_SU)
/* push pull with medium pull down */
#define GPIO_PAD_PP_SD		(GPIO_PAD_PP | GPIO_PAD_SD)
/* open drain with weak pull up */
#define GPIO_PAD_OD_WU		(GPIO_PAD_OD | GPIO_PAD_WU)
/* open drain with weak pull down */
#define GPIO_PAD_OD_WD		(GPIO_PAD_OD | GPIO_PAD_WD)
/* open drain with medium pull up */
#define GPIO_PAD_OD_MU		(GPIO_PAD_OD | GPIO_PAD_MU)
/* open drain with medium pull down */
#define GPIO_PAD_OD_MD		(GPIO_PAD_OD | GPIO_PAD_MD)
/* open drain with strong pull up */
#define GPIO_PAD_OD_SU		(GPIO_PAD_OD | GPIO_PAD_SU)
/* open drain with strong pull down */
#define GPIO_PAD_OD_SD		(GPIO_PAD_OD | GPIO_PAD_SD)

/*=========================================================================
 * GPIO irq configuration
 *=======================================================================*/
#define GPIO_IRQ_HIGH		0x01
#define GPIO_IRQ_LOW		0x02
#define GPIO_IRQ_EDGE_TRIG	0x00
#define GPIO_IRQ_LEVEL_TRIG	0x04

#define GPIO_IRQ_HE		(GPIO_IRQ_EDGE_TRIG | GPIO_IRQ_HIGH)
#define GPIO_IRQ_LE		(GPIO_IRQ_EDGE_TRIG | GPIO_IRQ_LOW)
#define GPIO_IRQ_BE		(GPIO_IRQ_EDGE_TRIG | GPIO_IRQ_HIGH | GPIO_IRQ_LOW)
#define GPIO_IRQ_HL		(GPIO_IRQ_LEVEL_TRIG | GPIO_IRQ_HIGH)
#define GPIO_IRQ_LL		(GPIO_IRQ_LEVEL_TRIG | GPIO_IRQ_LOW)

#include <driver/gpio.h>

#define NR_GPIOS		GPIO_HW_MAX_PINS
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
#define gpio_config_irq(port, pin, mode)		\
	gpio_hw_config_irq(port, pin, mode)
#define gpio_enable_irq(port, pin)			\
	gpio_hw_enable_irq(port, pin)
#define gpio_disable_irq(port, pin)			\
	gpio_hw_disable_irq(port, pin)
#define gpio_irq_status(port, pin)			\
	gpio_hw_irq_status(port, pin)
#define gpio_clear_irq(port, pin)			\
	gpio_hw_clear_irq(port, pin)

#ifdef CONFIG_GPIO
void gpio_init(void);
#else
#define gpio_init()
#endif

#endif /* __GPIO_H_INCLUDE__ */
