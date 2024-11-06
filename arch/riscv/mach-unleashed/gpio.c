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
 * @(#)gpio.c: FU540 (unleashed) specific GPIO implementation
 * $Id: gpio.c,v 1.1 2019-10-16 15:40:00 zhenglv Exp $
 */

#include <target/gpio.h>
#include <target/paging.h>

void sifive_gpio_config_pad(uint8_t gpio, pad_cfg_t pad, uint8_t drv)
{
	if (gpio >= GPIO_HW_MAX_PINS)
		return;

	if (drv == GPIO_DRIVE_IN) {
		sifive_gpio_input_enable(gpio);
		if (pad & GPIO_PAD_DIGITAL_IO) {
			if (pad & GPIO_PAD_OPEN_DRAIN)
				sifive_gpio_pullup_disable(gpio);
			else
				sifive_gpio_pullup_enable(gpio);
		}
	} else {
		sifive_gpio_output_enable(gpio);
	}
}

void sifive_gpio_config_irq(uint8_t gpio, uint32_t mode)
{
	if (gpio >= GPIO_HW_MAX_PINS)
		return;

	if ((mode & GPIO_IRQ_LEVEL_TRIG) == GPIO_IRQ_EDGE_TRIG) {
		sifive_gpio_high_disable(gpio);
		sifive_gpio_low_disable(gpio);
		if (mode & GPIO_IRQ_HIGH) {
			sifive_gpio_rise_enable(gpio);
			sifive_gpio_fall_disable(gpio);
		} else {
			sifive_gpio_rise_disable(gpio);
			sifive_gpio_fall_enable(gpio);
		}
	} else {
		sifive_gpio_rise_disable(gpio);
		sifive_gpio_fall_disable(gpio);
		if (mode & GPIO_IRQ_HIGH) {
			sifive_gpio_high_enable(gpio);
			sifive_gpio_low_disable(gpio);
		} else {
			sifive_gpio_high_disable(gpio);
			sifive_gpio_low_enable(gpio);
		}
	}
}
