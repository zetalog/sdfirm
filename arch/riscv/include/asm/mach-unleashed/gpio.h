/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
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
 * @(#)gpio.h: FU540 (unleashed) specific GPIO pin defintions
 * $Id: gpio.h,v 1.1 2019-10-16 14:32:00 zhenglv Exp $
 */

#ifndef __GPIO_UNLEASHED_H_INCLUDE__
#define __GPIO_UNLEASHED_H_INCLUDE__

#include <target/arch.h>

#ifdef CONFIG_GPIO
#ifndef ARCH_HAVE_GPIO
#define ARCH_HAVE_GPIO		1
#else
#error "Multiple GPIO controller defined"
#endif
#endif

#define GPIO_REG(offset)	(GPIO_BASE + (offset))

#define GPIO_INPUT_VAL		GPIO_REG(0x00)
#define GPIO_INPUT_EN		GPIO_REG(0x04)
#define GPIO_OUTPUT_EN		GPIO_REG(0x08)
#define GPIO_OUTPUT_VAL		GPIO_REG(0x0C)
#define GPIO_PUE		GPIO_REG(0x10)
#define GPIO_DS			GPIO_REG(0x14)
#define GPIO_RISE_IE		GPIO_REG(0x18)
#define GPIO_RISE_IP		GPIO_REG(0x1C)
#define GPIO_FALL_IE		GPIO_REG(0x20)
#define GPIO_FALL_IP		GPIO_REG(0x24)
#define GPIO_HIGH_IE		GPIO_REG(0x28)
#define GPIO_HIGH_IP		GPIO_REG(0x2C)
#define GPIO_LOW_IE		GPIO_REG(0x30)
#define GPIO_LOW_IP		GPIO_REG(0x34)
#define GPIO_OUT_XOR		GPIO_REG(0x40)

#define sifive_gpio_input_enable(gpio)		\
	__raw_setl(_BV(gpio), GPIO_INPUT_EN)
#define sifive_gpio_input_disable(gpio)		\
	__raw_clearl(_BV(gpio), GPIO_INPUT_EN)
#define sifive_gpio_output_enable(gpio)		\
	__raw_setl(_BV(gpio), GPIO_OUTPUT_EN)
#define sifive_gpio_output_disable(gpio)	\
	__raw_clearl(_BV(gpio), GPIO_OUTPUT_EN)
#define sifive_gpio_pullup_enable(gpio)		\
	__raw_setl(_BV(gpio), GPIO_PUE)
#define sifive_gpio_pullup_disable(gpio)	\
	__raw_clearl(_BV(gpio), GPIO_PUE)
#define sifive_gpio_input_read(gpio)					\
	(!!(__raw_readl(GPIO_INPUT_VAL) & _BV(gpio)))
#define sifive_gpio_output_write(gpio, val)				\
	do {								\
		if (val)						\
			__raw_setl(_BV(gpio), GPIO_OUTPUT_VAL);		\
		else							\
			__raw_clearl(_BV(gpio), GPIO_OUTPUT_VAL);	\
	} while (0)
#define sifive_gpio_rise_enable(gpio)		\
	__raw_setl(_BV(gpio), GPIO_RISE_IE)
#define sifive_gpio_rise_disable(gpio)		\
	__raw_clearl(_BV(gpio), GPIO_RISE_IE)
#define sifive_gpio_rise_present(gpio)		\
	(!!(__raw_readl(GPIO_RISE_IP) & _BV(gpio)))
#define sifive_gpio_rise_clear(gpio)		\
	__raw_setl(_BV(gpio), GPIO_RISE_IP)
#define sifive_gpio_fall_enable(gpio)		\
	__raw_setl(_BV(gpio), GPIO_FALL_IE)
#define sifive_gpio_fall_disable(gpio)		\
	__raw_clearl(_BV(gpio), GPIO_FALL_IE)
#define sifive_gpio_fall_present(gpio)		\
	(!!(__raw_readl(GPIO_FALL_IP) & _BV(gpio)))
#define sifive_gpio_fall_clear(gpio)		\
	__raw_setl(_BV(gpio), GPIO_FALL_IP)
#define sifive_gpio_high_enable(gpio)		\
	__raw_setl(_BV(gpio), GPIO_HIGH_IE)
#define sifive_gpio_high_disable(gpio)		\
	__raw_clearl(_BV(gpio), GPIO_HIGH_IE)
#define sifive_gpio_high_present(gpio)		\
	(!!(__raw_readl(GPIO_HIGH_IP) & _BV(gpio)))
#define sifive_gpio_high_clear(gpio)		\
	__raw_setl(_BV(gpio), GPIO_HIGH_IP)
#define sifive_gpio_low_enable(gpio)		\
	__raw_setl(_BV(gpio), GPIO_LOW_IE)
#define sifive_gpio_low_disable(gpio)		\
	__raw_clearl(_BV(gpio), GPIO_LOW_IE)
#define sifive_gpio_low_present(gpio)		\
	(!!(__raw_readl(GPIO_LOW_IP) & _BV(gpio)))
#define sifive_gpio_low_clear(gpio)		\
	__raw_setl(_BV(gpio), GPIO_LOW_IP)

void sifive_gpio_config_pad(uint8_t gpio, uint8_t pad, uint8_t drv);

/* GPIO abstraction */
#define GPIO_HW_MAX_PORTS	1
#define GPIO_HW_MAX_PINS	16

#define gpio_hw_read_pin(port, pin)		\
	sifive_gpio_input_read(0, pin)
#define gpio_hw_write_pin(port, pin, val)	\
	sifive_gpio_output_write(0, pin, val)
/* TODO: PIN Controller */
#define sifive_gpio_high_enable(gpio)		\
	__raw_setl(_BV(gpio), GPIO_HIGH_IE)
#define sifive_gpio_high_disable(gpio)		\
	__raw_clearl(_BV(gpio), GPIO_HIGH_IE)
#define sifive_gpio_low_enable(gpio)		\
	__raw_setl(_BV(gpio), GPIO_LOW_IE)
#define sifive_gpio_low_disable(gpio)		\
	__raw_clearl(_BV(gpio), GPIO_LOW_IE)

void sifive_gpio_config_pad(uint8_t gpio, uint8_t pad, uint8_t drv);
void sifive_gpio_config_irq(uint8_t gpio, uint32_t mode);

/* GPIO abstraction */
#define GPIO_HW_MAX_PORTS	1
#define GPIO_HW_MAX_PINS	16

#define gpio_hw_read_pin(port, pin)		\
	sifive_gpio_input_read(0, pin)
#define gpio_hw_write_pin(port, pin, val)	\
	sifive_gpio_output_write(0, pin, val)
/* TODO: PIN Controller */
#define gpio_hw_config_mux(port, pin, mux)	do { } while (0)
#define gpio_hw_config_pad(port, pin, pad, drv)	\
	sifive_gpio_config_pad(gpio, pad, drv)
/* TODO: GPIO IRQ Controller */
#define gpio_hw_ctrl_init()			do { } while (0)

#endif /* __GPIO_UNLEASHED_H_INCLUDE__ */
