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

struct tlmm_group {
	uint8_t port;
	uint16_t pin;
	uint8_t group;
};

struct tlmm_group tlmm_group_map[] = {
	{ GPIO0B, pad_gpio_8, 0 },
	{ GPIO0B, pad_gpio_9, 0 },
	{ GPIO0B, pad_gpio_10, 1 },
	{ GPIO0B, pad_gpio_11, 1 },
	{ GPIO0B, pad_gpio_12, 2 },
	{ GPIO0B, pad_gpio_13, 2 },
	{ GPIO0B, pad_gpio_14, 3 },
	{ GPIO0B, pad_gpio_15, 3 },
	{ GPIO0C, pad_gpio_16, 4 },
	{ GPIO0C, pad_gpio_17, 4 },
	{ GPIO0C, pad_gpio_18, 5 },
	{ GPIO0C, pad_gpio_19, 5 },
	{ GPIO0C, pad_gpio_20, 6 },
	{ GPIO0C, pad_gpio_21, 6 },
	{ GPIO0C, pad_gpio_22, 7 },
	{ GPIO0C, pad_gpio_23, 7 },
	{ GPIO0D, pad_gpio_24, 8 },
	{ GPIO0D, pad_gpio_25, 8 },
	{ GPIO0D, pad_gpio_26, 9 },
	{ GPIO0D, pad_gpio_27, 9 },
	{ GPIO0D, pad_gpio_28, 10 },
	{ GPIO0D, pad_gpio_29, 10 },
	{ GPIO0D, pad_gpio_30, 11 },
	{ GPIO0D, pad_gpio_31, 11 },
	{ GPIO1A, pad_gpio_32, 25 },
	{ GPIO1A, pad_gpio_33, 25 },
	{ GPIO1A, pad_gpio_34, 25 },
	{ GPIO1A, pad_gpio_35, 25 },
	{ GPIO1A, pad_gpio_36, 25 },
	{ GPIO1A, pad_gpio_37, 25 },
	{ GPIO1A, pad_gpio_38, 25 },
	{ GPIO1A, pad_gpio_39, 25 },
	{ GPIO1A, pad_gpio_40, 25 },
	{ GPIO1B, pad_gpio_48, 12 },
	{ GPIO1B, pad_gpio_49, 12 },
	{ GPIO1B, pad_gpio_50, 12 },
	{ GPIO1B, pad_gpio_51, 12 },
	{ GPIO1B, pad_gpio_52, 12 },
	{ GPIO1B, pad_gpio_54, 16 },
	{ GPIO1B, pad_gpio_55, 16 },
	{ GPIO1B, pad_gpio_56, 13 },
	{ GPIO1B, pad_gpio_57, 13 },
	{ GPIO1B, pad_gpio_58, 13 },
	{ GPIO1B, pad_gpio_59, 13 },
	{ GPIO1B, pad_gpio_60, 13 },
	{ GPIO1B, pad_gpio_62, 17 },
	{ GPIO1B, pad_gpio_63, 17 },
	{ GPIO1C, pad_gpio_64, 14 },
	{ GPIO1C, pad_gpio_65, 14 },
	{ GPIO1C, pad_gpio_66, 14 },
	{ GPIO1C, pad_gpio_67, 14 },
	{ GPIO1C, pad_gpio_68, 14 },
	{ GPIO1C, pad_gpio_70, 18 },
	{ GPIO1C, pad_gpio_71, 18 },
	{ GPIO1D, pad_gpio_72, 15 },
	{ GPIO1D, pad_gpio_73, 15 },
	{ GPIO1D, pad_gpio_74, 15 },
	{ GPIO1D, pad_gpio_75, 15 },
	{ GPIO1D, pad_gpio_76, 15 },
	{ GPIO1D, pad_gpio_78, 19 },
	{ GPIO1D, pad_gpio_79, 19 },
	{ GPIO2A, pad_gpio_80, 20 },
	{ GPIO2A, pad_gpio_81, 20 },
	{ GPIO2A, pad_gpio_82, 20 },
	{ GPIO2A, pad_gpio_83, 20 },
	{ GPIO2A, pad_gpio_84, 20 },
	{ GPIO2A, pad_gpio_85, 20 },
	{ GPIO2A, pad_gpio_88, 21 },
	{ GPIO2A, pad_gpio_89, 21 },
	{ GPIO2A, pad_gpio_90, 21 },
	{ GPIO2A, pad_gpio_91, 21 },
	{ GPIO2A, pad_gpio_92, 21 },
	{ GPIO2A, pad_gpio_93, 21 },
	{ GPIO2A, pad_gpio_96, 22 },
	{ GPIO2A, pad_gpio_97, 22 },
	{ GPIO2A, pad_gpio_98, 22 },
	{ GPIO2A, pad_gpio_99, 22 },
	{ GPIO2A, pad_gpio_100, 22 },
	{ GPIO2A, pad_gpio_101, 22 },
	{ GPIO2A, pad_gpio_104, 23 },
	{ GPIO2A, pad_gpio_105, 23 },
	{ GPIO2A, pad_gpio_106, 23 },
	{ GPIO2A, pad_gpio_107, 23 },
	{ GPIO2A, pad_gpio_108, 23 },
	{ GPIO2A, pad_gpio_109, 23 },
	{ GPIO2B, pad_gpio_112, 24 },
	{ GPIO2B, pad_gpio_113, 24 },
	{ GPIO2B, pad_gpio_114, 24 },
	{ GPIO2B, pad_gpio_115, 24 },
	{ GPIO2B, pad_gpio_116, 24 },
	{ GPIO2B, pad_gpio_117, 24 },
	{ GPIO2B, pad_gpio_118, 24 },
	{ GPIO2B, pad_gpio_119, 24 },
	{ GPIO2B, pad_gpio_120, 24 },
	{ GPIO2B, pad_gpio_121, 24 },
	{ GPIO2B, pad_gpio_122, 24 },
	{ GPIO2B, pad_gpio_123, 24 },
	{ GPIO2B, pad_gpio_124, 24 },
	{ GPIO2B, pad_gpio_125, 24 },
	{ GPIO2B, pad_gpio_126, 24 },
	{ GPIO2B, pad_gpio_127, 24 },
	{ GPIO2B, pad_gpio_128, 24 },
	{ GPIO2B, pad_gpio_129, 24 },
	{ GPIO2B, pad_gpio_130, 24 },
	{ GPIO2B, pad_gpio_131, 24 },
	{ GPIO2B, pad_gpio_132, 24 },
	{ GPIO2B, pad_gpio_133, 24 },
	{ GPIO2B, pad_gpio_134, 24 },
	{ GPIO2B, pad_gpio_135, 24 },
	{ GPIO2B, pad_gpio_136, 24 },
	{ GPIO2B, pad_gpio_137, 24 },
	{ GPIO2B, pad_gpio_138, 24 },
	{ GPIO2B, pad_gpio_139, 24 },
	{ GPIO2B, pad_gpio_140, 24 },
	{ GPIO2B, pad_gpio_141, 24 },
	{ GPIO2B, pad_gpio_142, 24 },
	{ GPIO2B, pad_gpio_143, 24 },
	{ GPIO2C, pad_gpio_144, 29 },
	{ GPIO2C, pad_gpio_145, 29 },
	{ GPIO2C, pad_gpio_146, 29 },
	{ GPIO2C, pad_gpio_147, 29 },
	{ GPIO2C, pad_gpio_148, 29 },
	{ GPIO2D, pad_gpio_152, 26 },
	{ GPIO2D, pad_gpio_153, 26 },
	{ GPIO2D, pad_gpio_154, 26 },
	{ GPIO2D, pad_gpio_155, 26 },
	{ GPIO2D, pad_gpio_156, 27 },
	{ GPIO2D, pad_gpio_157, 27 },
	{ GPIO2D, pad_gpio_158, 27 },
	{ GPIO2D, pad_gpio_159, 27 },
};

uint8_t tlmm_group_cache[NR_TLMM_GROUPS];

static uint8_t tlmm_gpio2group(uint8_t port, uint16_t pin)
{
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(tlmm_group_map); i++) {
		if (port == tlmm_group_map[i].port &&
		    pin == tlmm_group_map[i].pin)
			return tlmm_group_map[i].group;
	}
	return INVALID_TLMM_GROUP;
}

void tlmm_config_mux(uint8_t port, uint16_t pin, uint8_t mux)
{
	uint8_t group = tlmm_gpio2group(port, pin);

	if (group == INVALID_TLMM_GROUP)
		return;

	if (tlmm_group_cache[group] != mux) {
		tlmm_set_function(group, mux);
		tlmm_group_cache[group] = mux;
	}
}

void tlmm_config_pad(uint16_t gpio, uint8_t pad, uint8_t drv)
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
		cfg |= TLMM_PAD_IE;
		/* Enable Schmitt Trigger */
		if (pad & GPIO_PAD_SCHMITT_TRIG)
			cfg |= TLMM_PAD_ST;
	} else {
		/* TODO: drive strength */
	}
	__raw_writel_mask(cfg, TLMM_PAD_MASK, TLMM_PAD_GPIO_CFG(gpio));
}

void tlmm_config_irq(uint16_t gpio, uint8_t trig)
{
	uint32_t mode = 0;

	if (gpio >= NR_TLMM_GPIOS)
		return;

	if (!(trig & GPIO_IRQ_HIGH))
		mode |= TLMM_INTR_TRIGGER_LOW;
	if ((trig & GPIO_IRQ_LEVEL_TRIG) == GPIO_IRQ_EDGE_TRIG)
		mode |= TLMM_INTR_TRIGGER_EDGE;
	tlmm_set_trigger_mode(gpio, mode);
}

void tlmm_init(void)
{
	uint8_t i;

	for (i = 0; i < NR_TLMM_GROUPS; i++) {
		tlmm_group_cache[i] = tlmm_get_function(i);
	}
}
