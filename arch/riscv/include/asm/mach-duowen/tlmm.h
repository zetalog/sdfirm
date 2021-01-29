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
 * @(#)tlmm.h: DUOWEN top level mode multiplexer (TLMM) definitions
 * $Id: tlmm.h,v 1.1 2021-01-21 12:03:00 zhenglv Exp $
 */

#ifndef __TLMM_DUOWEN_H_INCLUDE__
#define __TLMM_DUOWEN_H_INCLUDE__

#include <target/arch.h>

#define __DUOWEN_TLMM_BASE		TLMM_BASE
#ifdef CONFIG_MMU
#define __TLMM_BASE			duowen_tlmm_reg_base
#else
#define __TLMM_BASE			__DUOWEN_TLMM_BASE
#endif
#define TLMM_REG(offset)		(__TLMM_BASE + (offset))

#define TLMM_PAD_GPIO_CFG(n)		(TLMM_REG((n) << 2))
#define TLMM_PAD_IE_CTRL		TLMM_REG(0x300)
#define TLMM_PAD_BOOT_MODE_0		TLMM_REG(0xC00)
#define TLMM_PAD_BOOT_MODE_1		TLMM_REG(0xC04)
#define TLMM_PAD_BOOT_MODE_2		TLMM_REG(0xC08)
#define TLMM_PAD_BOOT_MODE_3		TLMM_REG(0xC0C)
#define TLMM_PAD_TEST_MODE_0		TLMM_REG(0xC10)
#define TLMM_PAD_TEST_MODE_1		TLMM_REG(0xC14)
#define TLMM_PAD_POR_N			TLMM_REG(0xC18)
#define TLMM_PAD_SOCKET_ID		TLMM_REG(0xC1C)
#define TLMM_PAD_CHIP_LINK		TLMM_REG(0xC20)
#define TLMM_PAD_POWER_GOOD		TLMM_REG(0xC34)
#define TLMM_PAD_O_RST_N		TLMM_REG(0xC38)
#define TLMM_PAD_PS_HOLD		TLMM_REG(0xC3C)
#define TLMM_PAD_SHUT_DOWN		TLMM_REG(0xC40)
#define TLMM_PAD_PWM_OUT		TLMM_REG(0xC44)
#define TLMM_PAD_JTAG_TCK		TLMM_REG(0xC48)
#define TLMM_PAD_JTAG_TDI		TLMM_REG(0xC4C)
#define TLMM_PAD_JTAG_TMS		TLMM_REG(0xC50)
#define TLMM_PAD_JTAG_TRST_N		TLMM_REG(0xC54)
#define TLMM_PAD_JTAG_TDO		TLMM_REG(0xC58)
#define TLMM_PAD_XO_CLK			TLMM_REG(0xC5C)
#define TLMM_PAD_RTE_CONTROL		TLMM_REG(0xE00)

/* PAD_GPIO_CFG */
#define TLMM_PAD_PULL_OFFSET		0
#define TLMM_PAD_PULL_MASK		REG_2BIT_MASK
#define TLMM_PAD_PULL(value)		_SET_FV(TLMM_PAD_PULL, value)
#define TLMM_PAD_NO_PULL		0
#define TLMM_PAD_PULL_UP		1
#define TLMM_PAD_PULL_DOWN		2
#define TLMM_PAD_STRONG_PULL_UP		3
#define TLMM_PAD_ST			_BV(2)
#define TLMM_PAD_SEL_OFFSET		3
#define TLMM_PAD_SEL_MASK		REG_2BIT_MASK
#define TLMM_PAD_SEL(value)		_SET_FV(TLMM_PAD_SEL, value)
#define TLMM_PAD_GPIO			0
#define TLMM_PAD_FUNCTION		1
#define TLMM_PAD_DEBUG			2
#define TLMM_PAD_DRIVE_12MA		_BV(5)
#define TLMM_PAD_MASK			\
	(TLMM_PAD_PULL(TLMM_PAD_PULL_MASK) | TLMM_PAD_ST | TLMM_PAD_DRIVE_12MA)

/* TLMM_IE_CTRL */
#define TLMM_PAD_IE			_BV(0)

/* Specific values */
#define TLMM_GROUP_0_GPIO0B_8_9		0
#define TLMM_GROUP_0_I2C0		1
#define TLMM_GROUP_1_GPIO0B_10_11	0
#define TLMM_GROUP_1_I2C1		1
#define TLMM_GROUP_2_GPIO0B_12_13	0
#define TLMM_GROUP_2_I2C2		1
#define TLMM_GROUP_3_GPIO0B_14_15	0
#define TLMM_GROUP_3_I2C3		1
#define TLMM_GROUP_4_GPIO0C_16_17	0
#define TLMM_GROUP_4_I2C4		1
#define TLMM_GROUP_5_GPIO0C_18_19	0
#define TLMM_GROUP_5_I2C5		1
#define TLMM_GROUP_6_GPIO0C_20_21	0
#define TLMM_GROUP_6_I2C6		1
#define TLMM_GROUP_7_GPIO0C_22_23	0
#define TLMM_GROUP_7_I2C6		1
#define TLMM_GROUP_8_GPIO0D_24_25	0
#define TLMM_GROUP_8_I2CS0		1
#define TLMM_GROUP_9_GPIO0D_26_27	0
#define TLMM_GROUP_9_I2CS1		1
#define TLMM_GROUP_10_GPIO0D_28_29	0
#define TLMM_GROUP_10_I2CS2		1
#define TLMM_GROUP_11_GPIO0D_30_31	0
#define TLMM_GROUP_11_I2CS3		1
#define TLMM_GROUP_12_GPIO1B_48_52	0
#define TLMM_GROUP_12_SPI0		1
#define TLMM_GROUP_13_GPIO1B_56_60	0
#define TLMM_GROUP_13_SPI1		1
#define TLMM_GROUP_14_GPIO1C_64_68	0
#define TLMM_GROUP_14_SPI2		1
#define TLMM_GROUP_15_GPIO1D_72_76	0
#define TLMM_GROUP_15_SPI3		1
#define TLMM_GROUP_16_GPIO1B_54_55	0
#define TLMM_GROUP_16_UART0		1
#define TLMM_GROUP_17_GPIO1B_62_63	0
#define TLMM_GROUP_17_UART1		1
#define TLMM_GROUP_18_GPIO1C_70_71	0
#define TLMM_GROUP_18_UART2		1
#define TLMM_GROUP_19_GPIO1D_78_79	0
#define TLMM_GROUP_19_UART3		1
#define TLMM_GROUP_20_GPIO2A_80_85	0
#define TLMM_GROUP_20_UART0		1
#define TLMM_GROUP_21_GPIO2A_88_93	0
#define TLMM_GROUP_21_UART1		1
#define TLMM_GROUP_22_GPIO2A_96_101	0
#define TLMM_GROUP_22_UART2		1
#define TLMM_GROUP_23_GPIO2A_104_109	0
#define TLMM_GROUP_23_UART3		1
#define TLMM_GROUP_24_GPIO2B_112_143	0
#define TLMM_GROUP_24_PHASE_FLAG	1
#define TLMM_GROUP_25_GPIO1A_32_40	0
#define TLMM_GROUP_25_SD		1
#define TLMM_GROUP_26_GPIO2D_152_155	0
#define TLMM_GROUP_26_FLASH		1
#define TLMM_GROUP_27_GPIO2D_156_159	0
#define TLMM_GROUP_27_SPIS		1
#define TLMM_GROUP_29_GPIO2C_144_148	0
#define TLMM_GROUP_29_ETH		1

#define NR_TLMM_GPIOS			160
#define INVALID_TLMM_GPIO		NR_TLMM_GPIOS

#define tlmm_input_enable()	__raw_setl(TLMM_PAD_IE, TLMM_PAD_IE_CTRL)

#ifdef CONFIG_DUOWEN_TLMM
void tlmm_config_mux(uint16_t gpio, uint8_t mux);
void tlmm_config_pad(uint16_t gpio, uint8_t pad, uint8_t drv);
#else /* CONFIG_DUOWEN_TLMM */
#define tlmm_config_mux(gpio, mux)		do { } while (0)
#define tlmm_config_pad(gpio, pad, drv)		do { } while (0)
#endif /* CONFIG_DUOWEN_TLMM */

#endif /* __TLMM_DUOWEN_H_INCLUDE__ */
