/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)tlmm.c: DUOWEN top level mode multiplexer (TLMM) implementation
 * $Id: tlmm.c,v 1.1 2021-01-21 13:59:00 zhenglv Exp $
 */

#include <target/gpio.h>

void tlmm_config_mux(uint16_t gpio, uint8_t mux)
{
	if (gpio >= NR_TLMM_GPIOS)
		return;

	__raw_writel_mask(TLMM_PAD_SEL(mux), TLMM_PAD_SEL(TLMM_PAD_SEL_MASK),
			  TLMM_PAD_GPIO_CFG(duowen_gpio_soc(gpio), gpio));
}

void tlmm_config_pad(uint16_t gpio, pad_cfg_t pad, uint8_t drv)
{
	uint32_t cfg = 0;

	if (gpio >= NR_TLMM_GPIOS)
		return;

	switch (pad & GPIO_PAD_PULL_MASK) {
	case GPIO_PAD_PULL_DOWN:
		cfg |= TLMM_PAD_PULL(TLMM_PAD_PULL_DOWN);
		break;
	case GPIO_PAD_PULL_UP:
		if (pad & GPIO_PAD_STRONG_PULL)
			cfg |= TLMM_PAD_PULL(TLMM_PAD_STRONG_PULL_UP);
		else
			cfg |= TLMM_PAD_PULL(TLMM_PAD_PULL_UP);
		break;
	default:
		cfg |= TLMM_PAD_PULL(TLMM_PAD_NO_PULL);
		break;
	}
	if (drv == GPIO_DRIVE_IN) {
		/* Enable Schmitt Trigger */
		if (pad & GPIO_PAD_SCHMITT_TRIG)
			cfg |= TLMM_PAD_ST;
	} else {
		/* Set 12mA if required drive strength > 8mA */
		if (drv >= 11)
			cfg |= TLMM_PAD_DRIVE(TLMM_PAD_DRIVE_11MA);
		else if (drv >= 8)
			cfg |= TLMM_PAD_DRIVE(TLMM_PAD_DRIVE_8MA);
		else if (drv >= 6)
			cfg |= TLMM_PAD_DRIVE(TLMM_PAD_DRIVE_6MA);
	}
	__raw_writel_mask(cfg, TLMM_PAD_MASK,
			  TLMM_PAD_GPIO_CFG(duowen_gpio_soc(gpio), gpio));
}
