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
 * @(#)gpio.c: RV32M1 (VEGA) specific GPIO implementation
 * $Id: gpio.c,v 1.1 2019-08-17 07:23:00 zhenglv Exp $
 */

#include <target/gpio.h>

void port_config_pad(uint8_t port, uint8_t pin, uint8_t pad, uint8_t drv)
{
	uint32_t cfg = 0;

	if (port >= GPIO_HW_MAX_PORTS ||
	    pin >= GPIO_HW_MAX_PINS)
		return;

	if (drv == GPIO_DRIVE_IN) {
		gpio_direct_input(port, pin);
		if (pad & GPIO_PAD_DIGITAL_IO) {
			if (pad & GPIO_PAD_OPEN_DRAIN)
				cfg |= PCR_ODE;
			if (pad & GPIO_PAD_SLEW_RATE)
				cfg |= PCR_SRE;
			if (drv > 2 || !(pad & GPIO_PAD_WEAK_PULL))
				cfg |= PCR_DSE;
		}
	} else {
		gpio_direct_output(port, pin);
		if (pad & GPIO_PAD_DIGITAL_IO) {
			if (!(pad & GPIO_PAD_NO_PULL)) {
				if (pad & GPIO_PAD_PULL_UP)
					cfg |= PCR_PS;
				cfg |= PCR_PE;
			}
		}
	}
	__raw_writel_mask(cfg, PCR_PAD_MASK, PCR(port, pin));
}

void port_config_irq(uint8_t port, uint8_t pin, uint32_t mode)
{
	uint32_t cfg;

	if (port >= GPIO_HW_MAX_PORTS ||
	    pin >= GPIO_HW_MAX_PINS)
		return;

	if ((mode & GPIO_IRQ_LEVEL_TRIG) == GPIO_IRQ_EDGE_TRIG) {
		if (mode & GPIO_IRQ_HIGH)
			cfg = PCR_IRQC_IRQ_EDGE_HIGH;
		else
			cfg = PCR_IRQC_IRQ_EDGE_LOW;
	} else {
		if (mode & GPIO_IRQ_HIGH)
			cfg = PCR_IRQC_IRQ_LEVEL_HIGH;
		else
			cfg = PCR_IRQC_IRQ_LEVEL_LOW;
	}
	port_set_irqc(port, pin, cfg);
}
