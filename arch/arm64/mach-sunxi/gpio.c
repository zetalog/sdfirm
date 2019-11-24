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
 * @(#)gpio.c: SUNXI GPIO controller (GPIO) implementation
 * $Id: gpio.c,v 1.1 2019-11-24 10:37:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/gpio.h>

void sunxi_gpio_set_cfgbank(struct sunxi_gpio *pio, int bank_offset, uint32_t val)
{
	uint32_t index = GPIO_CFG_INDEX(bank_offset);
	uint32_t offset = GPIO_CFG_OFFSET(bank_offset);

	__raw_writel_mask(val << offset, 0xf << offset,
			  &pio->cfg[0] + index);
}

void sunxi_gpio_set_cfgpin(uint32_t pin, uint32_t val)
{
	uint32_t bank = GPIO_BANK(pin);
	struct sunxi_gpio *pio = BANK_TO_GPIO(bank);

	sunxi_gpio_set_cfgbank(pio, pin, val);
}

int sunxi_gpio_get_cfgbank(struct sunxi_gpio *pio, int bank_offset)
{
	uint32_t index = GPIO_CFG_INDEX(bank_offset);
	uint32_t offset = GPIO_CFG_OFFSET(bank_offset);
	uint32_t cfg;

	cfg = __raw_readl(&pio->cfg[0] + index);
	cfg >>= offset;

	return cfg & 0xf;
}

int sunxi_gpio_get_cfgpin(uint32_t pin)
{
	uint32_t bank = GPIO_BANK(pin);
	struct sunxi_gpio *pio = BANK_TO_GPIO(bank);

	return sunxi_gpio_get_cfgbank(pio, pin);
}

int sunxi_gpio_set_drv(uint32_t pin, uint32_t val)
{
	uint32_t bank = GPIO_BANK(pin);
	uint32_t index = GPIO_DRV_INDEX(pin);
	uint32_t offset = GPIO_DRV_OFFSET(pin);
	struct sunxi_gpio *pio = BANK_TO_GPIO(bank);

	__raw_writel_mask(val << offset, 0x3 << offset,
			  &pio->drv[0] + index);
	return 0;
}

int sunxi_gpio_set_pull(uint32_t pin, uint32_t val)
{
	uint32_t bank = GPIO_BANK(pin);
	uint32_t index = GPIO_PULL_INDEX(pin);
	uint32_t offset = GPIO_PULL_OFFSET(pin);
	struct sunxi_gpio *pio = BANK_TO_GPIO(bank);

	__raw_writel_mask(val << offset, 0x3 << offset,
			  &pio->pull[0] + index);
	return 0;
}
