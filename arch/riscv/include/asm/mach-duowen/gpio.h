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
 * @(#)gpio.h: DUOWEN general purpose input/output (GPIO) definitions
 * $Id: gpio.h,v 1.1 2021-01-21 09:55:00 zhenglv Exp $
 */

#ifndef __GPIO_DUOWEN_H_INCLUDE__
#define __GPIO_DUOWEN_H_INCLUDE__

#include <target/paging.h>
#include <target/arch.h>

#ifdef CONFIG_DUOWEN_GPIO
#define __GPIO_HW_MAX_CTRLS		3

#ifdef CONFIG_DUOWEN_SBI_DUAL
#define GPIO_HW_MAX_CTRLS		(2 * __GPIO_HW_MAX_CTRLS)
#define __DUOWEN_GPIO_BASE(n)				\
	(__SOC_BASE(!!((n) > __GPIO_HW_MAX_CTRLS)) +	\
	 GPIO0_BASE +					\
	 (((n) > __GPIO_HW_MAX_CTRLS ?			\
	   (n) - __GPIO_HW_MAX_CTRLS : (n)) << 20))
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define GPIO_HW_MAX_CTRLS		__GPIO_HW_MAX_CTRLS
#define __DUOWEN_GPIO_BASE(n)		(GPIO0_BASE + ((n) << 20))
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#ifdef CONFIG_MMU
#define __GPIO_BASE(n)			duowen_gpio_reg_base[n]
#else
#define __GPIO_BASE(n)			__DUOWEN_GPIO_BASE(n)
#endif
#define DW_GPIO_REG(n, offset)		(__GPIO_BASE(n) + (offset))
#ifndef ARCH_HAVE_GPIO
#include <driver/dw_gpio.h>
#include <asm/mach/tlmm.h>
#define ARCH_HAVE_GPIO			1
#else
#error "Multiple GPIO controller defined"
#endif
#endif

/* GPIO abstraction */
#ifdef CONFIG_GPIO
#ifdef CONFIG_DUOWEN_GPIO_PORT
#define __GPIO_HW_MAX_PORTS		12
/* APIs for multiple port GPIOs */
#define DUOWEN_GPIOP_PORT_OFFSET	0
#define DUOWEN_GPIOP_PORT_MASK		REG_2BIT_MASK
#define DUOWEN_GPIOP_PORT(value)	_SET_FV(DUOWEN_GPIOP_PORT, value)
#define duowen_gpiop_port(value)	_GET_FV(DUOWEN_GPIOP_PORT, value)
#define DUOWEN_GPIOP_CHIP_OFFSET	2
#define DUOWEN_GPIOP_CHIP_MASK		REG_2BIT_MASK
#define DUOWEN_GPIOP_CHIP(value)	_SET_FV(DUOWEN_GPIOP_CHIP, value)
#define duowen_gpiop_chip(value)	_GET_FV(DUOWEN_GPIOP_CHIP, value)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define GPIO_HW_MAX_PORTS		(2 * __GPIO_HW_MAX_PORTS)
#define DUOWEN_GPIOP_SOC_OFFSET		4
#define DUOWEN_GPIOP_SOC_MASK		REG_1BIT_MASK
#define DUOWEN_GPIOP_SOC(value)		_SET_FV(DUOWEN_GPIOP_SOC, value)
#define duowen_gpiop_soc(value)		_GET_FV(DUOWEN_GPIOP_SOC, value)
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define GPIO_HW_MAX_PORTS		__GPIO_HW_MAX_PORTS
#define DUOWEN_GPIOP_SOC(value)		0
#define duowen_gpiop_soc(value)		0
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#define DUOWEN_GPIOP(soc, chip, port)	\
	(DUOWEN_GPIOP_SOC(soc) |	\
	 DUOWEN_GPIOP_CHIP(chip) |	\
	 DUOWEN_GPIOP_PORT(port))
#define DUOWEN_GPIO_PIN_OFFSET		0
#define DUOWEN_GPIO_PIN_MASK		REG_5BIT_MASK
#define DUOWEN_GPIO_PIN(value)		_SET_FV(DUOWEN_GPIO_PIN, value)
#define duowen_gpio_pin(value)		_GET_FV(DUOWEN_GPIO_PIN, value)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define DUOWEN_GPIO_SOC_OFFSET		5
#define DUOWEN_GPIO_SOC_MASK		REG_1BIT_MASK
#define DUOWEN_GPIO_SOC(value)		_SET_FV(DUOWEN_GPIO_SOC, value)
#define duowen_gpio_soc(value)		_GET_FV(DUOWEN_GPIO_SOC, value)
#define __DUOWEN_GPIO(soc, chip, port, pin)	\
	(DUOWEN_GPIO_SOC(soc) | DUOWEN_GPIO_PIN(pin))
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define duowen_gpio_soc(value)			0
#define __DUOWEN_GPIO(soc, chip, port, pin)	DUOWEN_GPIO_PIN(pin)
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#define DUOWEN_GPIO(soc, chip, port, pin)	\
	__DUOWEN_GPIO(imc_socket_id(), chip, port, pin)
#else /* CONFIG_DUOWEN_GPIO_PORT */
/* APIs for single port GPIOs */
#define GPIO_HW_PORT			0
#define GPIO_HW_MAX_PORTS		1
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define GPIO_HW_MAX_PINS		320
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define GPIO_HW_MAX_PINS		160
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#define DUOWEN_GPIOP(soc, chip, pin)	GPIO_HW_PORT
#define DUOWEN_GPIO_PIN_OFFSET		0
#define DUOWEN_GPIO_PIN_MASK		REG_5BIT_MASK
#define DUOWEN_GPIO_PIN(value)		_SET_FV(DUOWEN_GPIO_PIN, value)
#define duowen_gpio_pin(value)		_GET_FV(DUOWEN_GPIO_PIN, value)
#define DUOWEN_GPIO_PORT_OFFSET		5
#define DUOWEN_GPIO_PORT_MASK		REG_2BIT_MASK
#define DUOWEN_GPIO_PORT(value)		_SET_FV(DUOWEN_GPIO_PORT, value)
#define duowen_gpio_port(value)		_GET_FV(DUOWEN_GPIO_PORT, value)
#define DUOWEN_GPIO_CHIP_OFFSET		7
#define DUOWEN_GPIO_CHIP_MASK		REG_2BIT_MASK
#define DUOWEN_GPIO_CHIP(value)		_SET_FV(DUOWEN_GPIO_CHIP, value)
#define duowen_gpio_chip(value)		_GET_FV(DUOWEN_GPIO_CHIP, value)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define DUOWEN_GPIO_SOC_OFFSET		9
#define DUOWEN_GPIO_SOC_MASK		REG_1BIT_MASK
#define DUOWEN_GPIO_SOC(value)		_SET_FV(DUOWEN_GPIO_SOC, value)
#define duowen_gpio_soc(value)		_GET_FV(DUOWEN_GPIO_SOC, value)
#define __DUOWEN_GPIO(soc, chip, port, pin)	\
	(DUOWEN_GPIO_SOC(soc) |			\
	 DUOWEN_GPIO_CHIP(chip) |		\
	 DUOWEN_GPIO_PORT(port) |		\
	 DUOWEN_GPIO_PIN(pin))
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define duowen_gpio_soc(value)			0
#define __DUOWEN_GPIO(soc, chip, port, pin)	\
	(DUOWEN_GPIO_CHIP(chip) |		\
	 DUOWEN_GPIO_PORT(port) |		\
	 DUOWEN_GPIO_PIN(pin))
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#define DUOWEN_GPIO(chip, port, pin)		\
	__DUOWEN_GPIO(imc_socket_id(), chip, port, pin)
#endif /* CONFIG_DUOWEN_GPIO_PORT */

#define GPIO0A				DUOWEN_GPIOP(0, 0, 0)
#define GPIO0B				DUOWEN_GPIOP(0, 0, 1)
#define GPIO0C				DUOWEN_GPIOP(0, 0, 2)
#define GPIO0D				DUOWEN_GPIOP(0, 0, 3)
#define GPIO1A				DUOWEN_GPIOP(0, 1, 0)
#define GPIO1B				DUOWEN_GPIOP(0, 1, 1)
#define GPIO1C				DUOWEN_GPIOP(0, 1, 2)
#define GPIO1D				DUOWEN_GPIOP(0, 1, 3)
#define GPIO2A				DUOWEN_GPIOP(0, 2, 0)
#define GPIO2B				DUOWEN_GPIOP(0, 2, 1)
#define GPIO2C				DUOWEN_GPIOP(0, 2, 2)
#define GPIO2D				DUOWEN_GPIOP(0, 2, 3)
#ifdef CONFIG_DUOWEN_SBI_DUAL
#define GPIO20A				DUOWEN_GPIOP(1, 0, 0)
#define GPIO20B				DUOWEN_GPIOP(1, 0, 1)
#define GPIO20C				DUOWEN_GPIOP(1, 0, 2)
#define GPIO20D				DUOWEN_GPIOP(1, 0, 3)
#define GPIO21A				DUOWEN_GPIOP(1, 1, 0)
#define GPIO21B				DUOWEN_GPIOP(1, 1, 1)
#define GPIO21C				DUOWEN_GPIOP(1, 1, 2)
#define GPIO21D				DUOWEN_GPIOP(1, 1, 3)
#define GPIO22A				DUOWEN_GPIOP(1, 2, 0)
#define GPIO22B				DUOWEN_GPIOP(1, 2, 1)
#define GPIO22C				DUOWEN_GPIOP(1, 2, 2)
#define GPIO22D				DUOWEN_GPIOP(1, 2, 3)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

#ifdef CONFIG_DUOWEN_SBI_DUAL
#define pad_gpio_0			__DUOWEN_GPIO(0, 0, 0, 0)
#define pad_gpio_1			__DUOWEN_GPIO(0, 0, 0, 1)
#define pad_gpio_2			__DUOWEN_GPIO(0, 0, 0, 2)
#define pad_gpio_3			__DUOWEN_GPIO(0, 0, 0, 3)
#define pad_gpio_4			__DUOWEN_GPIO(0, 0, 0, 4)
#define pad_gpio_5			__DUOWEN_GPIO(0, 0, 0, 5)
#define pad_gpio_6			__DUOWEN_GPIO(0, 0, 0, 6)
#define pad_gpio_7			__DUOWEN_GPIO(0, 0, 0, 7)
#define pad_gpio_8			__DUOWEN_GPIO(0, 0, 1, 0)
#define pad_gpio_9			__DUOWEN_GPIO(0, 0, 1, 1)
#define pad_gpio_10			__DUOWEN_GPIO(0, 0, 1, 2)
#define pad_gpio_11			__DUOWEN_GPIO(0, 0, 1, 3)
#define pad_gpio_12			__DUOWEN_GPIO(0, 0, 1, 4)
#define pad_gpio_13			__DUOWEN_GPIO(0, 0, 1, 5)
#define pad_gpio_14			__DUOWEN_GPIO(0, 0, 1, 6)
#define pad_gpio_15			__DUOWEN_GPIO(0, 0, 1, 7)
#define pad_gpio_16			__DUOWEN_GPIO(0, 0, 2, 0)
#define pad_gpio_17			__DUOWEN_GPIO(0, 0, 2, 1)
#define pad_gpio_18			__DUOWEN_GPIO(0, 0, 2, 2)
#define pad_gpio_19			__DUOWEN_GPIO(0, 0, 2, 3)
#define pad_gpio_20			__DUOWEN_GPIO(0, 0, 2, 4)
#define pad_gpio_21			__DUOWEN_GPIO(0, 0, 2, 5)
#define pad_gpio_22			__DUOWEN_GPIO(0, 0, 2, 6)
#define pad_gpio_23			__DUOWEN_GPIO(0, 0, 2, 7)
#define pad_gpio_24			__DUOWEN_GPIO(0, 0, 3, 0)
#define pad_gpio_25			__DUOWEN_GPIO(0, 0, 3, 1)
#define pad_gpio_26			__DUOWEN_GPIO(0, 0, 3, 2)
#define pad_gpio_27			__DUOWEN_GPIO(0, 0, 3, 3)
#define pad_gpio_28			__DUOWEN_GPIO(0, 0, 3, 4)
#define pad_gpio_29			__DUOWEN_GPIO(0, 0, 3, 5)
#define pad_gpio_30			__DUOWEN_GPIO(0, 0, 3, 6)
#define pad_gpio_31			__DUOWEN_GPIO(0, 0, 3, 7)
#define pad_gpio_32			__DUOWEN_GPIO(0, 1, 0, 0)
#define pad_gpio_33			__DUOWEN_GPIO(0, 1, 0, 1)
#define pad_gpio_34			__DUOWEN_GPIO(0, 1, 0, 2)
#define pad_gpio_35			__DUOWEN_GPIO(0, 1, 0, 3)
#define pad_gpio_36			__DUOWEN_GPIO(0, 1, 0, 4)
#define pad_gpio_37			__DUOWEN_GPIO(0, 1, 0, 5)
#define pad_gpio_38			__DUOWEN_GPIO(0, 1, 0, 6)
#define pad_gpio_39			__DUOWEN_GPIO(0, 1, 0, 7)
#define pad_gpio_40			__DUOWEN_GPIO(0, 1, 0, 8)
#define pad_gpio_41			__DUOWEN_GPIO(0, 1, 0, 9)
#define pad_gpio_42			__DUOWEN_GPIO(0, 1, 0, 10)
#define pad_gpio_43			__DUOWEN_GPIO(0, 1, 0, 11)
#define pad_gpio_44			__DUOWEN_GPIO(0, 1, 0, 12)
#define pad_gpio_45			__DUOWEN_GPIO(0, 1, 0, 13)
#define pad_gpio_46			__DUOWEN_GPIO(0, 1, 0, 14)
#define pad_gpio_47			__DUOWEN_GPIO(0, 1, 0, 15)
#define pad_gpio_48			__DUOWEN_GPIO(0, 1, 1, 0)
#define pad_gpio_49			__DUOWEN_GPIO(0, 1, 1, 1)
#define pad_gpio_50			__DUOWEN_GPIO(0, 1, 1, 2)
#define pad_gpio_51			__DUOWEN_GPIO(0, 1, 1, 3)
#define pad_gpio_52			__DUOWEN_GPIO(0, 1, 1, 4)
#define pad_gpio_53			__DUOWEN_GPIO(0, 1, 1, 5)
#define pad_gpio_54			__DUOWEN_GPIO(0, 1, 1, 6)
#define pad_gpio_55			__DUOWEN_GPIO(0, 1, 1, 7)
#define pad_gpio_56			__DUOWEN_GPIO(0, 1, 1, 8)
#define pad_gpio_57			__DUOWEN_GPIO(0, 1, 1, 9)
#define pad_gpio_58			__DUOWEN_GPIO(0, 1, 1, 10)
#define pad_gpio_59			__DUOWEN_GPIO(0, 1, 1, 11)
#define pad_gpio_60			__DUOWEN_GPIO(0, 1, 1, 12)
#define pad_gpio_61			__DUOWEN_GPIO(0, 1, 1, 13)
#define pad_gpio_62			__DUOWEN_GPIO(0, 1, 1, 14)
#define pad_gpio_63			__DUOWEN_GPIO(0, 1, 1, 15)
#define pad_gpio_64			__DUOWEN_GPIO(0, 1, 2, 0)
#define pad_gpio_65			__DUOWEN_GPIO(0, 1, 2, 1)
#define pad_gpio_66			__DUOWEN_GPIO(0, 1, 2, 2)
#define pad_gpio_67			__DUOWEN_GPIO(0, 1, 2, 3)
#define pad_gpio_68			__DUOWEN_GPIO(0, 1, 2, 4)
#define pad_gpio_69			__DUOWEN_GPIO(0, 1, 2, 5)
#define pad_gpio_70			__DUOWEN_GPIO(0, 1, 2, 6)
#define pad_gpio_71			__DUOWEN_GPIO(0, 1, 2, 7)
#define pad_gpio_72			__DUOWEN_GPIO(0, 1, 3, 0)
#define pad_gpio_73			__DUOWEN_GPIO(0, 1, 3, 1)
#define pad_gpio_74			__DUOWEN_GPIO(0, 1, 3, 2)
#define pad_gpio_75			__DUOWEN_GPIO(0, 1, 3, 3)
#define pad_gpio_76			__DUOWEN_GPIO(0, 1, 3, 4)
#define pad_gpio_77			__DUOWEN_GPIO(0, 1, 3, 5)
#define pad_gpio_78			__DUOWEN_GPIO(0, 1, 3, 6)
#define pad_gpio_79			__DUOWEN_GPIO(0, 1, 3, 7)
#define pad_gpio_80			__DUOWEN_GPIO(0, 2, 0, 0)
#define pad_gpio_81			__DUOWEN_GPIO(0, 2, 0, 1)
#define pad_gpio_82			__DUOWEN_GPIO(0, 2, 0, 2)
#define pad_gpio_83			__DUOWEN_GPIO(0, 2, 0, 3)
#define pad_gpio_84			__DUOWEN_GPIO(0, 2, 0, 4)
#define pad_gpio_85			__DUOWEN_GPIO(0, 2, 0, 5)
#define pad_gpio_86			__DUOWEN_GPIO(0, 2, 0, 6)
#define pad_gpio_87			__DUOWEN_GPIO(0, 2, 0, 7)
#define pad_gpio_88			__DUOWEN_GPIO(0, 2, 0, 8)
#define pad_gpio_89			__DUOWEN_GPIO(0, 2, 0, 9)
#define pad_gpio_90			__DUOWEN_GPIO(0, 2, 0, 10)
#define pad_gpio_91			__DUOWEN_GPIO(0, 2, 0, 11)
#define pad_gpio_92			__DUOWEN_GPIO(0, 2, 0, 12)
#define pad_gpio_93			__DUOWEN_GPIO(0, 2, 0, 13)
#define pad_gpio_94			__DUOWEN_GPIO(0, 2, 0, 14)
#define pad_gpio_95			__DUOWEN_GPIO(0, 2, 0, 15)
#define pad_gpio_96			__DUOWEN_GPIO(0, 2, 0, 16)
#define pad_gpio_97			__DUOWEN_GPIO(0, 2, 0, 17)
#define pad_gpio_98			__DUOWEN_GPIO(0, 2, 0, 18)
#define pad_gpio_99			__DUOWEN_GPIO(0, 2, 0, 19)
#define pad_gpio_100			__DUOWEN_GPIO(0, 2, 0, 20)
#define pad_gpio_101			__DUOWEN_GPIO(0, 2, 0, 21)
#define pad_gpio_102			__DUOWEN_GPIO(0, 2, 0, 22)
#define pad_gpio_103			__DUOWEN_GPIO(0, 2, 0, 23)
#define pad_gpio_104			__DUOWEN_GPIO(0, 2, 0, 24)
#define pad_gpio_105			__DUOWEN_GPIO(0, 2, 0, 25)
#define pad_gpio_106			__DUOWEN_GPIO(0, 2, 0, 26)
#define pad_gpio_107			__DUOWEN_GPIO(0, 2, 0, 27)
#define pad_gpio_108			__DUOWEN_GPIO(0, 2, 0, 28)
#define pad_gpio_109			__DUOWEN_GPIO(0, 2, 0, 29)
#define pad_gpio_110			__DUOWEN_GPIO(0, 2, 0, 30)
#define pad_gpio_111			__DUOWEN_GPIO(0, 2, 0, 31)
#define pad_gpio_112			__DUOWEN_GPIO(0, 2, 1, 0)
#define pad_gpio_113			__DUOWEN_GPIO(0, 2, 1, 1)
#define pad_gpio_114			__DUOWEN_GPIO(0, 2, 1, 2)
#define pad_gpio_115			__DUOWEN_GPIO(0, 2, 1, 3)
#define pad_gpio_116			__DUOWEN_GPIO(0, 2, 1, 4)
#define pad_gpio_117			__DUOWEN_GPIO(0, 2, 1, 5)
#define pad_gpio_118			__DUOWEN_GPIO(0, 2, 1, 6)
#define pad_gpio_119			__DUOWEN_GPIO(0, 2, 1, 7)
#define pad_gpio_120			__DUOWEN_GPIO(0, 2, 1, 8)
#define pad_gpio_121			__DUOWEN_GPIO(0, 2, 1, 9)
#define pad_gpio_122			__DUOWEN_GPIO(0, 2, 1, 10)
#define pad_gpio_123			__DUOWEN_GPIO(0, 2, 1, 11)
#define pad_gpio_124			__DUOWEN_GPIO(0, 2, 1, 12)
#define pad_gpio_125			__DUOWEN_GPIO(0, 2, 1, 13)
#define pad_gpio_126			__DUOWEN_GPIO(0, 2, 1, 14)
#define pad_gpio_127			__DUOWEN_GPIO(0, 2, 1, 15)
#define pad_gpio_128			__DUOWEN_GPIO(0, 2, 1, 16)
#define pad_gpio_129			__DUOWEN_GPIO(0, 2, 1, 17)
#define pad_gpio_130			__DUOWEN_GPIO(0, 2, 1, 18)
#define pad_gpio_131			__DUOWEN_GPIO(0, 2, 1, 19)
#define pad_gpio_132			__DUOWEN_GPIO(0, 2, 1, 20)
#define pad_gpio_133			__DUOWEN_GPIO(0, 2, 1, 21)
#define pad_gpio_134			__DUOWEN_GPIO(0, 2, 1, 22)
#define pad_gpio_135			__DUOWEN_GPIO(0, 2, 1, 23)
#define pad_gpio_136			__DUOWEN_GPIO(0, 2, 1, 24)
#define pad_gpio_137			__DUOWEN_GPIO(0, 2, 1, 25)
#define pad_gpio_138			__DUOWEN_GPIO(0, 2, 1, 26)
#define pad_gpio_139			__DUOWEN_GPIO(0, 2, 1, 27)
#define pad_gpio_140			__DUOWEN_GPIO(0, 2, 1, 28)
#define pad_gpio_141			__DUOWEN_GPIO(0, 2, 1, 29)
#define pad_gpio_142			__DUOWEN_GPIO(0, 2, 1, 30)
#define pad_gpio_143			__DUOWEN_GPIO(0, 2, 1, 31)
#define pad_gpio_144			__DUOWEN_GPIO(0, 2, 2, 0)
#define pad_gpio_145			__DUOWEN_GPIO(0, 2, 2, 1)
#define pad_gpio_146			__DUOWEN_GPIO(0, 2, 2, 2)
#define pad_gpio_147			__DUOWEN_GPIO(0, 2, 2, 3)
#define pad_gpio_148			__DUOWEN_GPIO(0, 2, 2, 4)
#define pad_gpio_149			__DUOWEN_GPIO(0, 2, 2, 5)
#define pad_gpio_150			__DUOWEN_GPIO(0, 2, 2, 6)
#define pad_gpio_151			__DUOWEN_GPIO(0, 2, 2, 7)
#define pad_gpio_152			__DUOWEN_GPIO(0, 2, 3, 0)
#define pad_gpio_153			__DUOWEN_GPIO(0, 2, 3, 1)
#define pad_gpio_154			__DUOWEN_GPIO(0, 2, 3, 2)
#define pad_gpio_155			__DUOWEN_GPIO(0, 2, 3, 3)
#define pad_gpio_156			__DUOWEN_GPIO(0, 2, 3, 4)
#define pad_gpio_157			__DUOWEN_GPIO(0, 2, 3, 5)
#define pad_gpio_158			__DUOWEN_GPIO(0, 2, 3, 6)
#define pad_gpio_159			__DUOWEN_GPIO(0, 2, 3, 7)

#define pad2_gpio_0			__DUOWEN_GPIO(1, 0, 0, 0)
#define pad2_gpio_1			__DUOWEN_GPIO(1, 0, 0, 1)
#define pad2_gpio_2			__DUOWEN_GPIO(1, 0, 0, 2)
#define pad2_gpio_3			__DUOWEN_GPIO(1, 0, 0, 3)
#define pad2_gpio_4			__DUOWEN_GPIO(1, 0, 0, 4)
#define pad2_gpio_5			__DUOWEN_GPIO(1, 0, 0, 5)
#define pad2_gpio_6			__DUOWEN_GPIO(1, 0, 0, 6)
#define pad2_gpio_7			__DUOWEN_GPIO(1, 0, 0, 7)
#define pad2_gpio_8			__DUOWEN_GPIO(1, 0, 1, 0)
#define pad2_gpio_9			__DUOWEN_GPIO(1, 0, 1, 1)
#define pad2_gpio_10			__DUOWEN_GPIO(1, 0, 1, 2)
#define pad2_gpio_11			__DUOWEN_GPIO(1, 0, 1, 3)
#define pad2_gpio_12			__DUOWEN_GPIO(1, 0, 1, 4)
#define pad2_gpio_13			__DUOWEN_GPIO(1, 0, 1, 5)
#define pad2_gpio_14			__DUOWEN_GPIO(1, 0, 1, 6)
#define pad2_gpio_15			__DUOWEN_GPIO(1, 0, 1, 7)
#define pad2_gpio_16			__DUOWEN_GPIO(1, 0, 2, 0)
#define pad2_gpio_17			__DUOWEN_GPIO(1, 0, 2, 1)
#define pad2_gpio_18			__DUOWEN_GPIO(1, 0, 2, 2)
#define pad2_gpio_19			__DUOWEN_GPIO(1, 0, 2, 3)
#define pad2_gpio_20			__DUOWEN_GPIO(1, 0, 2, 4)
#define pad2_gpio_21			__DUOWEN_GPIO(1, 0, 2, 5)
#define pad2_gpio_22			__DUOWEN_GPIO(1, 0, 2, 6)
#define pad2_gpio_23			__DUOWEN_GPIO(1, 0, 2, 7)
#define pad2_gpio_24			__DUOWEN_GPIO(1, 0, 3, 0)
#define pad2_gpio_25			__DUOWEN_GPIO(1, 0, 3, 1)
#define pad2_gpio_26			__DUOWEN_GPIO(1, 0, 3, 2)
#define pad2_gpio_27			__DUOWEN_GPIO(1, 0, 3, 3)
#define pad2_gpio_28			__DUOWEN_GPIO(1, 0, 3, 4)
#define pad2_gpio_29			__DUOWEN_GPIO(1, 0, 3, 5)
#define pad2_gpio_30			__DUOWEN_GPIO(1, 0, 3, 6)
#define pad2_gpio_31			__DUOWEN_GPIO(1, 0, 3, 7)
#define pad2_gpio_32			__DUOWEN_GPIO(1, 1, 0, 0)
#define pad2_gpio_33			__DUOWEN_GPIO(1, 1, 0, 1)
#define pad2_gpio_34			__DUOWEN_GPIO(1, 1, 0, 2)
#define pad2_gpio_35			__DUOWEN_GPIO(1, 1, 0, 3)
#define pad2_gpio_36			__DUOWEN_GPIO(1, 1, 0, 4)
#define pad2_gpio_37			__DUOWEN_GPIO(1, 1, 0, 5)
#define pad2_gpio_38			__DUOWEN_GPIO(1, 1, 0, 6)
#define pad2_gpio_39			__DUOWEN_GPIO(1, 1, 0, 7)
#define pad2_gpio_40			__DUOWEN_GPIO(1, 1, 0, 8)
#define pad2_gpio_41			__DUOWEN_GPIO(1, 1, 0, 9)
#define pad2_gpio_42			__DUOWEN_GPIO(1, 1, 0, 10)
#define pad2_gpio_43			__DUOWEN_GPIO(1, 1, 0, 11)
#define pad2_gpio_44			__DUOWEN_GPIO(1, 1, 0, 12)
#define pad2_gpio_45			__DUOWEN_GPIO(1, 1, 0, 13)
#define pad2_gpio_46			__DUOWEN_GPIO(1, 1, 0, 14)
#define pad2_gpio_47			__DUOWEN_GPIO(1, 1, 0, 15)
#define pad2_gpio_48			__DUOWEN_GPIO(1, 1, 1, 0)
#define pad2_gpio_49			__DUOWEN_GPIO(1, 1, 1, 1)
#define pad2_gpio_50			__DUOWEN_GPIO(1, 1, 1, 2)
#define pad2_gpio_51			__DUOWEN_GPIO(1, 1, 1, 3)
#define pad2_gpio_52			__DUOWEN_GPIO(1, 1, 1, 4)
#define pad2_gpio_53			__DUOWEN_GPIO(1, 1, 1, 5)
#define pad2_gpio_54			__DUOWEN_GPIO(1, 1, 1, 6)
#define pad2_gpio_55			__DUOWEN_GPIO(1, 1, 1, 7)
#define pad2_gpio_56			__DUOWEN_GPIO(1, 1, 1, 8)
#define pad2_gpio_57			__DUOWEN_GPIO(1, 1, 1, 9)
#define pad2_gpio_58			__DUOWEN_GPIO(1, 1, 1, 10)
#define pad2_gpio_59			__DUOWEN_GPIO(1, 1, 1, 11)
#define pad2_gpio_60			__DUOWEN_GPIO(1, 1, 1, 12)
#define pad2_gpio_61			__DUOWEN_GPIO(1, 1, 1, 13)
#define pad2_gpio_62			__DUOWEN_GPIO(1, 1, 1, 14)
#define pad2_gpio_63			__DUOWEN_GPIO(1, 1, 1, 15)
#define pad2_gpio_64			__DUOWEN_GPIO(1, 1, 2, 0)
#define pad2_gpio_65			__DUOWEN_GPIO(1, 1, 2, 1)
#define pad2_gpio_66			__DUOWEN_GPIO(1, 1, 2, 2)
#define pad2_gpio_67			__DUOWEN_GPIO(1, 1, 2, 3)
#define pad2_gpio_68			__DUOWEN_GPIO(1, 1, 2, 4)
#define pad2_gpio_69			__DUOWEN_GPIO(1, 1, 2, 5)
#define pad2_gpio_70			__DUOWEN_GPIO(1, 1, 2, 6)
#define pad2_gpio_71			__DUOWEN_GPIO(1, 1, 2, 7)
#define pad2_gpio_72			__DUOWEN_GPIO(1, 1, 3, 0)
#define pad2_gpio_73			__DUOWEN_GPIO(1, 1, 3, 1)
#define pad2_gpio_74			__DUOWEN_GPIO(1, 1, 3, 2)
#define pad2_gpio_75			__DUOWEN_GPIO(1, 1, 3, 3)
#define pad2_gpio_76			__DUOWEN_GPIO(1, 1, 3, 4)
#define pad2_gpio_77			__DUOWEN_GPIO(1, 1, 3, 5)
#define pad2_gpio_78			__DUOWEN_GPIO(1, 1, 3, 6)
#define pad2_gpio_79			__DUOWEN_GPIO(1, 1, 3, 7)
#define pad2_gpio_80			__DUOWEN_GPIO(1, 2, 0, 0)
#define pad2_gpio_81			__DUOWEN_GPIO(1, 2, 0, 1)
#define pad2_gpio_82			__DUOWEN_GPIO(1, 2, 0, 2)
#define pad2_gpio_83			__DUOWEN_GPIO(1, 2, 0, 3)
#define pad2_gpio_84			__DUOWEN_GPIO(1, 2, 0, 4)
#define pad2_gpio_85			__DUOWEN_GPIO(1, 2, 0, 5)
#define pad2_gpio_86			__DUOWEN_GPIO(1, 2, 0, 6)
#define pad2_gpio_87			__DUOWEN_GPIO(1, 2, 0, 7)
#define pad2_gpio_88			__DUOWEN_GPIO(1, 2, 0, 8)
#define pad2_gpio_89			__DUOWEN_GPIO(1, 2, 0, 9)
#define pad2_gpio_90			__DUOWEN_GPIO(1, 2, 0, 10)
#define pad2_gpio_91			__DUOWEN_GPIO(1, 2, 0, 11)
#define pad2_gpio_92			__DUOWEN_GPIO(1, 2, 0, 12)
#define pad2_gpio_93			__DUOWEN_GPIO(1, 2, 0, 13)
#define pad2_gpio_94			__DUOWEN_GPIO(1, 2, 0, 14)
#define pad2_gpio_95			__DUOWEN_GPIO(1, 2, 0, 15)
#define pad2_gpio_96			__DUOWEN_GPIO(1, 2, 0, 16)
#define pad2_gpio_97			__DUOWEN_GPIO(1, 2, 0, 17)
#define pad2_gpio_98			__DUOWEN_GPIO(1, 2, 0, 18)
#define pad2_gpio_99			__DUOWEN_GPIO(1, 2, 0, 19)
#define pad2_gpio_100			__DUOWEN_GPIO(1, 2, 0, 20)
#define pad2_gpio_101			__DUOWEN_GPIO(1, 2, 0, 21)
#define pad2_gpio_102			__DUOWEN_GPIO(1, 2, 0, 22)
#define pad2_gpio_103			__DUOWEN_GPIO(1, 2, 0, 23)
#define pad2_gpio_104			__DUOWEN_GPIO(1, 2, 0, 24)
#define pad2_gpio_105			__DUOWEN_GPIO(1, 2, 0, 25)
#define pad2_gpio_106			__DUOWEN_GPIO(1, 2, 0, 26)
#define pad2_gpio_107			__DUOWEN_GPIO(1, 2, 0, 27)
#define pad2_gpio_108			__DUOWEN_GPIO(1, 2, 0, 28)
#define pad2_gpio_109			__DUOWEN_GPIO(1, 2, 0, 29)
#define pad2_gpio_110			__DUOWEN_GPIO(1, 2, 0, 30)
#define pad2_gpio_111			__DUOWEN_GPIO(1, 2, 0, 31)
#define pad2_gpio_112			__DUOWEN_GPIO(1, 2, 1, 0)
#define pad2_gpio_113			__DUOWEN_GPIO(1, 2, 1, 1)
#define pad2_gpio_114			__DUOWEN_GPIO(1, 2, 1, 2)
#define pad2_gpio_115			__DUOWEN_GPIO(1, 2, 1, 3)
#define pad2_gpio_116			__DUOWEN_GPIO(1, 2, 1, 4)
#define pad2_gpio_117			__DUOWEN_GPIO(1, 2, 1, 5)
#define pad2_gpio_118			__DUOWEN_GPIO(1, 2, 1, 6)
#define pad2_gpio_119			__DUOWEN_GPIO(1, 2, 1, 7)
#define pad2_gpio_120			__DUOWEN_GPIO(1, 2, 1, 8)
#define pad2_gpio_121			__DUOWEN_GPIO(1, 2, 1, 9)
#define pad2_gpio_122			__DUOWEN_GPIO(1, 2, 1, 10)
#define pad2_gpio_123			__DUOWEN_GPIO(1, 2, 1, 11)
#define pad2_gpio_124			__DUOWEN_GPIO(1, 2, 1, 12)
#define pad2_gpio_125			__DUOWEN_GPIO(1, 2, 1, 13)
#define pad2_gpio_126			__DUOWEN_GPIO(1, 2, 1, 14)
#define pad2_gpio_127			__DUOWEN_GPIO(1, 2, 1, 15)
#define pad2_gpio_128			__DUOWEN_GPIO(1, 2, 1, 16)
#define pad2_gpio_129			__DUOWEN_GPIO(1, 2, 1, 17)
#define pad2_gpio_130			__DUOWEN_GPIO(1, 2, 1, 18)
#define pad2_gpio_131			__DUOWEN_GPIO(1, 2, 1, 19)
#define pad2_gpio_132			__DUOWEN_GPIO(1, 2, 1, 20)
#define pad2_gpio_133			__DUOWEN_GPIO(1, 2, 1, 21)
#define pad2_gpio_134			__DUOWEN_GPIO(1, 2, 1, 22)
#define pad2_gpio_135			__DUOWEN_GPIO(1, 2, 1, 23)
#define pad2_gpio_136			__DUOWEN_GPIO(1, 2, 1, 24)
#define pad2_gpio_137			__DUOWEN_GPIO(1, 2, 1, 25)
#define pad2_gpio_138			__DUOWEN_GPIO(1, 2, 1, 26)
#define pad2_gpio_139			__DUOWEN_GPIO(1, 2, 1, 27)
#define pad2_gpio_140			__DUOWEN_GPIO(1, 2, 1, 28)
#define pad2_gpio_141			__DUOWEN_GPIO(1, 2, 1, 29)
#define pad2_gpio_142			__DUOWEN_GPIO(1, 2, 1, 30)
#define pad2_gpio_143			__DUOWEN_GPIO(1, 2, 1, 31)
#define pad2_gpio_144			__DUOWEN_GPIO(1, 2, 2, 0)
#define pad2_gpio_145			__DUOWEN_GPIO(1, 2, 2, 1)
#define pad2_gpio_146			__DUOWEN_GPIO(1, 2, 2, 2)
#define pad2_gpio_147			__DUOWEN_GPIO(1, 2, 2, 3)
#define pad2_gpio_148			__DUOWEN_GPIO(1, 2, 2, 4)
#define pad2_gpio_149			__DUOWEN_GPIO(1, 2, 2, 5)
#define pad2_gpio_150			__DUOWEN_GPIO(1, 2, 2, 6)
#define pad2_gpio_151			__DUOWEN_GPIO(1, 2, 2, 7)
#define pad2_gpio_152			__DUOWEN_GPIO(1, 2, 3, 0)
#define pad2_gpio_153			__DUOWEN_GPIO(1, 2, 3, 1)
#define pad2_gpio_154			__DUOWEN_GPIO(1, 2, 3, 2)
#define pad2_gpio_155			__DUOWEN_GPIO(1, 2, 3, 3)
#define pad2_gpio_156			__DUOWEN_GPIO(1, 2, 3, 4)
#define pad2_gpio_157			__DUOWEN_GPIO(1, 2, 3, 5)
#define pad2_gpio_158			__DUOWEN_GPIO(1, 2, 3, 6)
#define pad2_gpio_159			__DUOWEN_GPIO(1, 2, 3, 7)
#else /* CONFIG_DUOWEN_SBI_DUAL */
#define pad_gpio_0			DUOWEN_GPIO(0, 0, 0)
#define pad_gpio_1			DUOWEN_GPIO(0, 0, 1)
#define pad_gpio_2			DUOWEN_GPIO(0, 0, 2)
#define pad_gpio_3			DUOWEN_GPIO(0, 0, 3)
#define pad_gpio_4			DUOWEN_GPIO(0, 0, 4)
#define pad_gpio_5			DUOWEN_GPIO(0, 0, 5)
#define pad_gpio_6			DUOWEN_GPIO(0, 0, 6)
#define pad_gpio_7			DUOWEN_GPIO(0, 0, 7)
#define pad_gpio_8			DUOWEN_GPIO(0, 1, 0)
#define pad_gpio_9			DUOWEN_GPIO(0, 1, 1)
#define pad_gpio_10			DUOWEN_GPIO(0, 1, 2)
#define pad_gpio_11			DUOWEN_GPIO(0, 1, 3)
#define pad_gpio_12			DUOWEN_GPIO(0, 1, 4)
#define pad_gpio_13			DUOWEN_GPIO(0, 1, 5)
#define pad_gpio_14			DUOWEN_GPIO(0, 1, 6)
#define pad_gpio_15			DUOWEN_GPIO(0, 1, 7)
#define pad_gpio_16			DUOWEN_GPIO(0, 2, 0)
#define pad_gpio_17			DUOWEN_GPIO(0, 2, 1)
#define pad_gpio_18			DUOWEN_GPIO(0, 2, 2)
#define pad_gpio_19			DUOWEN_GPIO(0, 2, 3)
#define pad_gpio_20			DUOWEN_GPIO(0, 2, 4)
#define pad_gpio_21			DUOWEN_GPIO(0, 2, 5)
#define pad_gpio_22			DUOWEN_GPIO(0, 2, 6)
#define pad_gpio_23			DUOWEN_GPIO(0, 2, 7)
#define pad_gpio_24			DUOWEN_GPIO(0, 3, 0)
#define pad_gpio_25			DUOWEN_GPIO(0, 3, 1)
#define pad_gpio_26			DUOWEN_GPIO(0, 3, 2)
#define pad_gpio_27			DUOWEN_GPIO(0, 3, 3)
#define pad_gpio_28			DUOWEN_GPIO(0, 3, 4)
#define pad_gpio_29			DUOWEN_GPIO(0, 3, 5)
#define pad_gpio_30			DUOWEN_GPIO(0, 3, 6)
#define pad_gpio_31			DUOWEN_GPIO(0, 3, 7)
#define pad_gpio_32			DUOWEN_GPIO(1, 0, 0)
#define pad_gpio_33			DUOWEN_GPIO(1, 0, 1)
#define pad_gpio_34			DUOWEN_GPIO(1, 0, 2)
#define pad_gpio_35			DUOWEN_GPIO(1, 0, 3)
#define pad_gpio_36			DUOWEN_GPIO(1, 0, 4)
#define pad_gpio_37			DUOWEN_GPIO(1, 0, 5)
#define pad_gpio_38			DUOWEN_GPIO(1, 0, 6)
#define pad_gpio_39			DUOWEN_GPIO(1, 0, 7)
#define pad_gpio_40			DUOWEN_GPIO(1, 0, 8)
#define pad_gpio_41			DUOWEN_GPIO(1, 0, 9)
#define pad_gpio_42			DUOWEN_GPIO(1, 0, 10)
#define pad_gpio_43			DUOWEN_GPIO(1, 0, 11)
#define pad_gpio_44			DUOWEN_GPIO(1, 0, 12)
#define pad_gpio_45			DUOWEN_GPIO(1, 0, 13)
#define pad_gpio_46			DUOWEN_GPIO(1, 0, 14)
#define pad_gpio_47			DUOWEN_GPIO(1, 0, 15)
#define pad_gpio_48			DUOWEN_GPIO(1, 1, 0)
#define pad_gpio_49			DUOWEN_GPIO(1, 1, 1)
#define pad_gpio_50			DUOWEN_GPIO(1, 1, 2)
#define pad_gpio_51			DUOWEN_GPIO(1, 1, 3)
#define pad_gpio_52			DUOWEN_GPIO(1, 1, 4)
#define pad_gpio_53			DUOWEN_GPIO(1, 1, 5)
#define pad_gpio_54			DUOWEN_GPIO(1, 1, 6)
#define pad_gpio_55			DUOWEN_GPIO(1, 1, 7)
#define pad_gpio_56			DUOWEN_GPIO(1, 1, 8)
#define pad_gpio_57			DUOWEN_GPIO(1, 1, 9)
#define pad_gpio_58			DUOWEN_GPIO(1, 1, 10)
#define pad_gpio_59			DUOWEN_GPIO(1, 1, 11)
#define pad_gpio_60			DUOWEN_GPIO(1, 1, 12)
#define pad_gpio_61			DUOWEN_GPIO(1, 1, 13)
#define pad_gpio_62			DUOWEN_GPIO(1, 1, 14)
#define pad_gpio_63			DUOWEN_GPIO(1, 1, 15)
#define pad_gpio_64			DUOWEN_GPIO(1, 2, 0)
#define pad_gpio_65			DUOWEN_GPIO(1, 2, 1)
#define pad_gpio_66			DUOWEN_GPIO(1, 2, 2)
#define pad_gpio_67			DUOWEN_GPIO(1, 2, 3)
#define pad_gpio_68			DUOWEN_GPIO(1, 2, 4)
#define pad_gpio_69			DUOWEN_GPIO(1, 2, 5)
#define pad_gpio_70			DUOWEN_GPIO(1, 2, 6)
#define pad_gpio_71			DUOWEN_GPIO(1, 2, 7)
#define pad_gpio_72			DUOWEN_GPIO(1, 3, 0)
#define pad_gpio_73			DUOWEN_GPIO(1, 3, 1)
#define pad_gpio_74			DUOWEN_GPIO(1, 3, 2)
#define pad_gpio_75			DUOWEN_GPIO(1, 3, 3)
#define pad_gpio_76			DUOWEN_GPIO(1, 3, 4)
#define pad_gpio_77			DUOWEN_GPIO(1, 3, 5)
#define pad_gpio_78			DUOWEN_GPIO(1, 3, 6)
#define pad_gpio_79			DUOWEN_GPIO(1, 3, 7)
#define pad_gpio_80			DUOWEN_GPIO(2, 0, 0)
#define pad_gpio_81			DUOWEN_GPIO(2, 0, 1)
#define pad_gpio_82			DUOWEN_GPIO(2, 0, 2)
#define pad_gpio_83			DUOWEN_GPIO(2, 0, 3)
#define pad_gpio_84			DUOWEN_GPIO(2, 0, 4)
#define pad_gpio_85			DUOWEN_GPIO(2, 0, 5)
#define pad_gpio_86			DUOWEN_GPIO(2, 0, 6)
#define pad_gpio_87			DUOWEN_GPIO(2, 0, 7)
#define pad_gpio_88			DUOWEN_GPIO(2, 0, 8)
#define pad_gpio_89			DUOWEN_GPIO(2, 0, 9)
#define pad_gpio_90			DUOWEN_GPIO(2, 0, 10)
#define pad_gpio_91			DUOWEN_GPIO(2, 0, 11)
#define pad_gpio_92			DUOWEN_GPIO(2, 0, 12)
#define pad_gpio_93			DUOWEN_GPIO(2, 0, 13)
#define pad_gpio_94			DUOWEN_GPIO(2, 0, 14)
#define pad_gpio_95			DUOWEN_GPIO(2, 0, 15)
#define pad_gpio_96			DUOWEN_GPIO(2, 0, 16)
#define pad_gpio_97			DUOWEN_GPIO(2, 0, 17)
#define pad_gpio_98			DUOWEN_GPIO(2, 0, 18)
#define pad_gpio_99			DUOWEN_GPIO(2, 0, 19)
#define pad_gpio_100			DUOWEN_GPIO(2, 0, 20)
#define pad_gpio_101			DUOWEN_GPIO(2, 0, 21)
#define pad_gpio_102			DUOWEN_GPIO(2, 0, 22)
#define pad_gpio_103			DUOWEN_GPIO(2, 0, 23)
#define pad_gpio_104			DUOWEN_GPIO(2, 0, 24)
#define pad_gpio_105			DUOWEN_GPIO(2, 0, 25)
#define pad_gpio_106			DUOWEN_GPIO(2, 0, 26)
#define pad_gpio_107			DUOWEN_GPIO(2, 0, 27)
#define pad_gpio_108			DUOWEN_GPIO(2, 0, 28)
#define pad_gpio_109			DUOWEN_GPIO(2, 0, 29)
#define pad_gpio_110			DUOWEN_GPIO(2, 0, 30)
#define pad_gpio_111			DUOWEN_GPIO(2, 0, 31)
#define pad_gpio_112			DUOWEN_GPIO(2, 1, 0)
#define pad_gpio_113			DUOWEN_GPIO(2, 1, 1)
#define pad_gpio_114			DUOWEN_GPIO(2, 1, 2)
#define pad_gpio_115			DUOWEN_GPIO(2, 1, 3)
#define pad_gpio_116			DUOWEN_GPIO(2, 1, 4)
#define pad_gpio_117			DUOWEN_GPIO(2, 1, 5)
#define pad_gpio_118			DUOWEN_GPIO(2, 1, 6)
#define pad_gpio_119			DUOWEN_GPIO(2, 1, 7)
#define pad_gpio_120			DUOWEN_GPIO(2, 1, 8)
#define pad_gpio_121			DUOWEN_GPIO(2, 1, 9)
#define pad_gpio_122			DUOWEN_GPIO(2, 1, 10)
#define pad_gpio_123			DUOWEN_GPIO(2, 1, 11)
#define pad_gpio_124			DUOWEN_GPIO(2, 1, 12)
#define pad_gpio_125			DUOWEN_GPIO(2, 1, 13)
#define pad_gpio_126			DUOWEN_GPIO(2, 1, 14)
#define pad_gpio_127			DUOWEN_GPIO(2, 1, 15)
#define pad_gpio_128			DUOWEN_GPIO(2, 1, 16)
#define pad_gpio_129			DUOWEN_GPIO(2, 1, 17)
#define pad_gpio_130			DUOWEN_GPIO(2, 1, 18)
#define pad_gpio_131			DUOWEN_GPIO(2, 1, 19)
#define pad_gpio_132			DUOWEN_GPIO(2, 1, 20)
#define pad_gpio_133			DUOWEN_GPIO(2, 1, 21)
#define pad_gpio_134			DUOWEN_GPIO(2, 1, 22)
#define pad_gpio_135			DUOWEN_GPIO(2, 1, 23)
#define pad_gpio_136			DUOWEN_GPIO(2, 1, 24)
#define pad_gpio_137			DUOWEN_GPIO(2, 1, 25)
#define pad_gpio_138			DUOWEN_GPIO(2, 1, 26)
#define pad_gpio_139			DUOWEN_GPIO(2, 1, 27)
#define pad_gpio_140			DUOWEN_GPIO(2, 1, 28)
#define pad_gpio_141			DUOWEN_GPIO(2, 1, 29)
#define pad_gpio_142			DUOWEN_GPIO(2, 1, 30)
#define pad_gpio_143			DUOWEN_GPIO(2, 1, 31)
#define pad_gpio_144			DUOWEN_GPIO(2, 2, 0)
#define pad_gpio_145			DUOWEN_GPIO(2, 2, 1)
#define pad_gpio_146			DUOWEN_GPIO(2, 2, 2)
#define pad_gpio_147			DUOWEN_GPIO(2, 2, 3)
#define pad_gpio_148			DUOWEN_GPIO(2, 2, 4)
#define pad_gpio_149			DUOWEN_GPIO(2, 2, 5)
#define pad_gpio_150			DUOWEN_GPIO(2, 2, 6)
#define pad_gpio_151			DUOWEN_GPIO(2, 2, 7)
#define pad_gpio_152			DUOWEN_GPIO(2, 3, 0)
#define pad_gpio_153			DUOWEN_GPIO(2, 3, 1)
#define pad_gpio_154			DUOWEN_GPIO(2, 3, 2)
#define pad_gpio_155			DUOWEN_GPIO(2, 3, 3)
#define pad_gpio_156			DUOWEN_GPIO(2, 3, 4)
#define pad_gpio_157			DUOWEN_GPIO(2, 3, 5)
#define pad_gpio_158			DUOWEN_GPIO(2, 3, 6)
#define pad_gpio_159			DUOWEN_GPIO(2, 3, 7)
#endif /* CONFIG_DUOWEN_SBI_DUAL */

#ifndef CONFIG_SYS_NOIRQ
void gpio_hw_enable_irq(uint8_t port, uint16_t pin);
void gpio_hw_disable_irq(uint8_t port, uint16_t pin);
#endif /* CONFIG_SYS_NOIRQ */
void gpio_hw_config_irq(uint8_t port, uint16_t pin, uint8_t trig);
uint8_t gpio_hw_irq_status(uint8_t port, uint16_t pin);
void gpio_hw_clear_irq(uint8_t port, uint16_t pin);
uint8_t gpio_hw_read_pin(uint8_t port, uint16_t pin);
void gpio_hw_write_pin(uint8_t port, uint16_t pin, uint8_t val);
void gpio_hw_config_pad(uint8_t port, uint16_t pin,
			pad_cfg_t pad, uint8_t drv);
void gpio_hw_config_mux(uint8_t port, uint16_t pin, uint8_t mux);
#ifdef CONFIG_DUOWEN_GPIO_PORT
uint32_t gpio_hw_read_port(uint8_t port, uint32_t mask);
void gpio_hw_write_port(uint8_t port, uint32_t value);
#endif /* CONFIG_DUOWEN_GPIO_PORT */
void gpio_hw_ctrl_init(void);

#ifdef CONFIG_DUOWEN_SIM_GPIO_IRQ
void duowen_gpio_irq_init(void);
#else /* CONFIG_DUOWEN_SIM_GPIO_IRQ */
#define duowen_gpio_irq_init()			do { } while (0)
#endif /* CONFIG_DUOWEN_SIM_GPIO_IRQ */
#ifdef CONFIG_MMU
#define gpio_hw_mmu_init()			duowen_mmu_map_gpio()
#endif /* CONFIG_MMU */
void duowen_gpio_init(void);
#else
#define duowen_gpio_init()			do { } while (0)
#endif /* CONFIG_GPIO */

#endif /* __GPIO_DUOWEN_H_INCLUDE__ */
