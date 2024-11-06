/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2018
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
 * @(#)tlmm.c: qualcomm top level module multiplexer implementation
 * $Id: tlmm.c,v 1.279 2018-09-20 10:19:18 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/gpio.h>

void tlmm_config_pad(uint8_t gpio, pad_cfg_t pad, uint8_t drv)
{
	uint32_t cfg = 0;

	if (gpio >= TLMM_NR_GPIOS)
		return;

	if (pad & GPIO_PAD_KEEPER)
		cfg |= TLMM_GPIO_PULL(TLMM_KEEPER);
	else {
		switch (pad & GPIO_PAD_PULL_MASK) {
		case GPIO_PAD_PULL_DOWN:
			cfg |= TLMM_GPIO_PULL(TLMM_PULL_DOWN);
			break;
		case GPIO_PAD_PULL_UP:
			cfg |= TLMM_GPIO_PULL(TLMM_PULL_UP);
			break;
		default:
			cfg |= TLMM_GPIO_PULL(TLMM_NO_PULL);
			break;
		}
	}
	if (drv != GPIO_DRIVE_IN) {
		cfg |= TLMM_GPIO_OE;
		cfg |= TLMM_DRV_STRENGTH(TLMM_MA_TO_DRV_STRENGTH(drv));
	}
	__raw_writel_mask(cfg, TLMM_GPIO_CFG_PAD_MASK,
			  TLMM_GPIO_CFG(gpio));
}

void tlmm_config_irq(uint8_t gpio, uint32_t mode)
{
	uint32_t cfg;

	if (gpio >= NR_GPIOS)
		return;

	cfg = (mode & GPIO_TLMM_MASK) | TLMM_INTR_RAW_STATUS_EN;
	if (mode & GPIO_IRQ_HIGH)
		cfg |= TLMM_INTR_POL_CTL;
	if ((mode & GPIO_IRQ_LEVEL_TRIG) == GPIO_IRQ_EDGE_TRIG)
		cfg |= TLMM_INTR_DECT_CTL(TLMM_INTR_POS_EDGE);
	__raw_writel_mask(cfg, TLMM_GPIO_INTR_CFG_IRQ_MASK,
			  TLMM_GPIO_INTR_CFG(gpio));
}
