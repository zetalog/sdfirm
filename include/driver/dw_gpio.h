/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)dw_gpio.h: Synopsys DesignWare GPIO interface
 * $Id: dw_gpio.h,v 1.0 2020-5-15 08:26:00 syl Exp $
 */

#ifndef __DW_GPIO_H_INCLUDE__
#define __DW_GPIO_H_INCLUDE__

#ifndef DW_GPIO_BASE
#define DW_GPIO_BASE(n)		(DW_GPIO##n##_BASE)
#endif
#ifndef DW_GPIO_REG
#define DW_GPIO_REG(n, offset)	(DW_GPIO_BASE(n) + (offset))
#endif
#ifndef NR_DW_GPIOS
#define NR_DW_GPIOS		1
#endif

#define GPIOA			0
#define GPIOB			1
#define GPIOC			2
#define GPIOD			3

#define GPIO_SWPORT_DR(n, p)	DW_GPIO_REG(n, (p) * 0x0C)
#define GPIO_SWPORT_DDR(n, p)	DW_GPIO_REG(n, ((p) * 0x0C) + 0x04)
#define GPIO_SWPORT_CTL(n, p)	DW_GPIO_REG(n, ((p) * 0x0C) + 0x08)
#define GPIO_INTEN(n)		DW_GPIO_REG(n, 0x30)
#define GPIO_INTMASK(n)		DW_GPIO_REG(n, 0x34)
#define GPIO_INTTYPE_LEVEL(n)	DW_GPIO_REG(n, 0x38)
#define GPIO_INT_POLARITY(n)	DW_GPIO_REG(n, 0x3C)
#define GPIO_INTSTATUS(n)	DW_GPIO_REG(n, 0x40)
#define GPIO_RAW_INTSTATUS(n)	DW_GPIO_REG(n, 0x44)
#define GPIO_DEBOUNCE(n)	DW_GPIO_REG(n, 0x48)
#define GPIO_PORTA_EOI(n)	DW_GPIO_REG(n, 0x4C)
#define GPIO_EXT_PORT(n, p)	DW_GPIO_REG(n, (p) * 0x04 + 0x50)
#define GPIO_LS_SYNC(n)		DW_GPIO_REG(n, 0x60)
#define GPIO_ID_CODE(n)		DW_GPIO_REG(n, 0x64)
#define GPIO_INT_BOTHEDGE(n)	DW_GPIO_REG(n, 0x68)
#define GPIO_VER_ID_CODE(n)	DW_GPIO_REG(n, 0x6C)
#define GPIO_CONFIG_REG2(n)	DW_GPIO_REG(n, 0x70)
#define GPIO_CONFIG_REG1(n)	DW_GPIO_REG(n, 0x74)

#define dw_gpio_write_pin(n, p, pin, v)				\
	__raw_writel_mask(v ? _BV(pin) : 0, _BV(pin), GPIO_SWPORT_DR(n, p))
#define dw_gpio_read_pin(n, p, pin)				\
	(!!(__raw_readl(GPIO_SWPORT_DR(n, p)) & _BV(pin)))

#define dw_gpio_enable_irq(n, p, pin)				\
	__raw_setl(_BV(pin), GPIO_INTEN(n, p))
#define dw_gpio_disable_irq(n, p, pin)				\
	__raw_clearl(_BV(pin), GPIO_INTEN(n, p))
#define dw_gpio_mask_irq(n, p, pin)				\
	__raw_clearl(_BV(pin), GPIO_INTMASK(n, p))
#define dw_gpio_unmask_irq(n, p, pin)				\
	__raw_setl(_BV(pin), GPIO_INTMASK(n, p))
#define dw_gpio_set_irq_level(n, p, pin)			\
	__raw_clearl(_BV(pin), GPIO_INTTYPE_LEVEL(n))
#define dw_gpio_set_irq_edge(n, p, pin)				\
	__raw_setl(_BV(pin), GPIO_INTTYPE_LEVEL(n))
#define dw_gpio_set_irq_high(n, p, pin)				\
	__raw_setl(_BV(pin), GPIO_INT_POLARITY(n))
#define dw_gpio_set_irq_low(n, p, pin)				\
	__raw_clearl(_BV(pin), GPIO_INT_POLARITY(n))
#define dw_gpio_irq_status(n, p, pin)				\
	(!!(__raw_readl(GPIO_RAW_INTSTATUS(n)) & _BV(pin)))
#define dw_gpio_enable_irq_debounce(n, p, pin)			\
	__raw_setl(_BV(pin), GPIO_DEBOUNCE(n))
#define dw_gpio_disable_irq_debounce(n, p, pin)			\
	__raw_clearl(_BV(pin), GPIO_DEBOUNCE(n))
#define dw_gpio_clear_irq(n, p, pin)				\
	__raw_setl(_BV(pin), GPIO_PORTA_EOI(n))

void dw_gpio_config_pad(uint8_t n, uint8_t p, uint8_t pin,
			uint8_t pad, uint8_t drv);
void dw_gpio_config_irq(uint8_t n, uint8_t p, uint8_t pin, uint8_t mode);

#endif /* __DW_GPIO_H_INCLUDE__ */
