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
 * @(#)tlmm.h: qualcomm top level module multiplexer definitions
 * $Id: tlmm.h,v 1.279 2018-09-20 10:19:18 zhenglv Exp $
 */

#ifndef __TLMM_QDF2400_H_INCLUDE__
#define __TLMM_QDF2400_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>
#include <target/generic.h>
#include <asm/reg.h>

#ifdef CONFIG_GPIO_TLMM
#ifndef ARCH_HAVE_GPIO
#define ARCH_HAVE_GPIO		1
#else
#error "Multiple GPIO controller defined"
#endif
#endif

#define TLMM_BASE			ULL(0xFF02010000)
#define TLMM_REG(offset)		(TLMM_BASE+(offset))
#define TLMM_PIN_REG(offset, n)		TLMM_REG((offset) + 0x10000 * (n))
#define TLMM_DIR_REG(offset, n)		\
	TLMM_REG((offset) * 0x10000 + (n) * 0x04)

#define TLMM_GET_FV(name, value)	_GET_FV(TLMM_##name, value)
#define TLMM_SET_FV(name, value)	_SET_FV(TLMM_##name, value)

#define TLMM_GPIO_CFG(n)		TLMM_PIN_REG(0x000000, n)
#define TLMM_GPIO_IN_OUT(n)		TLMM_PIN_REG(0x000004, n)
#define TLMM_GPIO_INTR_CFG(n)		TLMM_PIN_REG(0x000008, n)
#define TLMM_GPIO_INTR_STATUS(n)	TLMM_PIN_REG(0x00000C, n)
#define TLMM_GPIO_SDC_CTL(n)		TLMM_PIN_REG(0x000010, n)
#define TLMM_GPIO_SDC_CLK_LOOP51	TLMM_REG(0x330014)
#define TLMM_GPIO_SDC_CLK_LOOP61	TLMM_REG(0x3D0014)
#define TLMM_GPIO_SDC_CLK_LOOP63	TLMM_REG(0x3F0014)
#define TLMM_GPIO_SDC_CLK_LOOP73	TLMM_REG(0x490014)
#define TLMM_CLK_GATE_EN		TLMM_REG(0x960000)
#define TLMM_IE_CTRL_DISABLE		TLMM_REG(0x960004)
#define TLMM_MPM_WAKEUP_INT_EN_0	TLMM_REG(0x960008)
#define TLMM_DIR_CONN_INTR_CFG_IMC(n)	TLMM_DIR_REG(151, n)
#define TLMM_DIR_CONN_INTR_CFG_HMSS(n)	TLMM_DIR_REG(152, n)
#define TLMM_MODE_PULL_CTL		TLMM_REG(0x990000)
#define TLMM_PMIC_CTL0			TLMM_REG(0x9A0000)
#define TLMM_PMIC_CTL1			TLMM_REG(0x9A0004)
#define TLMM_PMIC_CTL2			TLMM_REG(0x9A0008)
#define TLMM_THERM_HDRV_CTL		TLMM_REG(0x9B0000)
#define TLMM_CHIP_MODE			TLMM_REG(0xA30000)
#define TLMM_HW_VERSION_NUMBER		TLMM_REG(0xA30004)

/* 2.46.2.3.1. TLMM_GPIO_CFG%n%
 * (0xFF02010000 + 0x00000 + 0x10000 * n (n=[0..(150-1)]))
 */
#define TLMM_GPIO_HIHYS_EN		_BV(10)
#define TLMM_GPIO_OE			_BV(9)
#define TLMM_DRV_STRENGTH_OFFSET	6
#define TLMM_DRV_STRENGTH_MASK		0x07
#define TLMM_DRV_STRENGTH(value)	TLMM_SET_FV(DRV_STRENGTH, value)
#define TLMM_MA_TO_DRV_STRENGTH(ma)	((ALIGN((ma), 2) - 2) / 2)

#define TLMM_FUNC_SEL_OFFSET		2
#define TLMM_FUNC_SEL_MASK		0x0F
#define TLMM_FUNC_SEL(value)		TLMM_SET_FV(FUNC_SEL, value)

/* PIN multiplexer functions, from tlmm_gpio_pinmux_table.xlsx, associated
 * PIN direction is recorded as comments.
 */
#define GPIO0_FUNC_BLSP1_I2C0_SDA			1 /* L */
#define GPIO1_FUNC_BLSP1_I2C0_SCL			1 /* L */
#define GPIO2_FUNC_BLSP1_I2C1_SDA			1 /* L */
#define GPIO3_FUNC_BLSP1_I2C1_SCL			1 /* L */
#define GPIO4_FUNC_BLSP1_I2C2_SDA			1 /* L */
#define GPIO5_FUNC_BLSP1_I2C2_SCL			1 /* L */
#define GPIO6_FUNC_BLSP1_I2C3_SDA			1 /* L */
#define GPIO7_FUNC_BLSP1_I2C3_SCL			1 /* L */
#define GPIO8_FUNC_BLSP2_I2C0_SDA			1 /* L */
#define GPIO9_FUNC_BLSP2_I2C0_SCL			1 /* L */
#define GPIO10_FUNC_BLSP2_I2C1_SDA			1 /* L */
#define GPIO11_FUNC_BLSP2_I2C1_SCL			1 /* L */
#define GPIO12_FUNC_BLSP2_I2C2_SDA			1 /* L */
#define GPIO13_FUNC_BLSP2_I2C2_SCL			1 /* L */
#define GPIO14_FUNC_BLSP2_I2C3_SDA			1 /* L */
#define GPIO15_FUNC_BLSP2_I2C3_SCL			1 /* L */

#define GPIO4_FUNC_I2C_SLV3_SDA				2 /* L */
#define GPIO5_FUNC_I2C_SLV3_SCL				2 /* L */
#define GPIO6_FUNC_I2C_SLV2_SDA				2 /* L */
#define GPIO7_FUNC_I2C_SLV2_SCL				2 /* L */
#define GPIO12_FUNC_I2C_SLV1_SDA			2 /* L */
#define GPIO13_FUNC_I2C_SLV1_SCL			2 /* L */
#define GPIO14_FUNC_I2C_SLV0_SDA			2 /* L */
#define GPIO14_FUNC_I2C_SLV0_SCL			2 /* L */

#define GPIO16_FUNC_BLSP3_AC_UART0_TX			1 /* O */
#define GPIO17_FUNC_BLSP3_AC_UART0_RX			1 /* I */
#define GPIO18_FUNC_BLSP3_AC_UART0_RFR_N		1 /* O */
#define GPIO19_FUNC_BLSP3_AC_UART0_CTS_N		1 /* I */
#define GPIO24_FUNC_BLSP3_AC_UART1_TX			1 /* O */
#define GPIO25_FUNC_BLSP3_AC_UART1_RX			1 /* I */
#define GPIO26_FUNC_BLSP3_AC_UART1_RFR_N		1 /* O */
#define GPIO27_FUNC_BLSP3_AC_UART1_CTS_N		1 /* I */
#define GPIO28_FUNC_BLSP3_AC_UART2_TX			1 /* O */
#define GPIO29_FUNC_BLSP3_AC_UART2_RX			1 /* I */
#define GPIO30_FUNC_BLSP3_AC_UART2_RFR_N		1 /* O */
#define GPIO31_FUNC_BLSP3_AC_UART2_CTS_N		1 /* I */
#define GPIO124_FUNC_BLSP3_AC_UART3_TX			1 /* O */
#define GPIO125_FUNC_BLSP3_AC_UART3_RX			1 /* I */
#define GPIO126_FUNC_BLSP3_AC_UART3_RFR_N		1 /* O */
#define GPIO127_FUNC_BLSP3_AC_UART3_CTS_N		1 /* I */

#define GPIO16_FUNC_BLSP0_Q_UART2_TX			2 /* O */
#define GPIO17_FUNC_BLSP0_Q_UART2_RX			2 /* I */
#define GPIO18_FUNC_BLSP0_Q_UART2_RFR_N			2 /* O */
#define GPIO19_FUNC_BLSP0_Q_UART2_CTS_N			2 /* I */
#define GPIO20_FUNC_BLSP0_Q_UART3_TX			1 /* O */
#define GPIO21_FUNC_BLSP0_Q_UART3_RX			1 /* I */
#define GPIO22_FUNC_BLSP0_Q_UART3_RFR_N			1 /* O */
#define GPIO23_FUNC_BLSP0_Q_UART3_CTS_N			1 /* I */
#define GPIO24_FUNC_BLSP0_Q_UART0_TX			1 /* O */
#define GPIO25_FUNC_BLSP0_Q_UART0_RX			1 /* I */
#define GPIO26_FUNC_BLSP0_Q_UART0_RFR_N			1 /* O */
#define GPIO27_FUNC_BLSP0_Q_UART0_CTS_N			1 /* I */
#define GPIO28_FUNC_BLSP0_Q_UART1_TX			1 /* O */
#define GPIO29_FUNC_BLSP0_Q_UART1_RX			1 /* I */
#define GPIO30_FUNC_BLSP0_Q_UART1_RFR_N			1 /* O */
#define GPIO31_FUNC_BLSP0_Q_UART1_CTS_N			1 /* I */

#define GPIO32_FUNC_BLSP0_SPI0_CS1_N_B			1 /* Z */
#define GPIO33_FUNC_BLSP0_SPI0_CS2_N_B			1 /* Z */
#define GPIO34_FUNC_BLSP0_SPI0_CS3_N_B			1 /* Z */
#define GPIO40_FUNC_BLSP0_SPI0_MISO			1 /* B */
#define GPIO41_FUNC_BLSP0_SPI0_MOSI			1 /* L */
#define GPIO42_FUNC_BLSP0_SPI0_CLK			1 /* L */
#define GPIO43_FUNC_BLSP0_SPI0_CS0_N			1 /* L */
#define GPIO48_FUNC_BLSP0_SPI0_CS1_N_A			2 /* Z */
#define GPIO49_FUNC_BLSP0_SPI0_CS2_N_A			2 /* Z */
#define GPIO50_FUNC_BLSP0_SPI0_CS3_N_A			2 /* Z */
#define GPIO90_FUNC_BLSP0_SPI0_CS1_N_C			1 /* Z */
#define GPIO97_FUNC_BLSP0_SPI0_CS2_N_C			1 /* Z */
#define GPIO98_FUNC_BLSP0_SPI0_CS3_N_C			1 /* Z */

#define GPIO32_FUNC_BLSP3_SPI0_MISO			1 /* B */
#define GPIO33_FUNC_BLSP3_SPI0_MOSI			1 /* L */
#define GPIO34_FUNC_BLSP3_SPI0_CLK			1 /* L */
#define GPIO89_FUNC_BLSP3_SPI0_CS0_N			1 /* L */

#define GPIO36_FUNC_BLSP3_SPI1_MISO			1 /* B */
#define GPIO37_FUNC_BLSP3_SPI1_MOSI			1 /* L */
#define GPIO38_FUNC_BLSP3_SPI1_CLK			1 /* L */
#define GPIO39_FUNC_BLSP3_SPI1_CS0_N			1 /* L */

#define GPIO44_FUNC_BLSP0_SPI1_MISO			1 /* B */
#define GPIO45_FUNC_BLSP0_SPI1_MOSI			1 /* L */
#define GPIO46_FUNC_BLSP0_SPI1_CLK			1 /* L */
#define GPIO47_FUNC_BLSP0_SPI1_CS0_N			1 /* L */
#define GPIO48_FUNC_BLSP0_SPI1_CS1_N			1 /* Z */
#define GPIO49_FUNC_BLSP0_SPI1_CS2_N			1 /* Z */
#define GPIO50_FUNC_BLSP0_SPI1_CS3_N			1 /* Z */

#define GPIO103_FUNC_SPI_SLV_SCLK			1 /* I */
#define GPIO104_FUNC_SPI_SLV_MOSI			1 /* I */
#define GPIO105_FUNC_SPI_SLV_MISO			1 /* O */
#define GPIO106_FUNC_SPI_SLV_CS_N			1 /* I */

#define GPIO51_FUNC_SDC0_CLK				1 /* L */
#define GPIO52_FUNC_SDC0_CMD				1 /* B */
#define GPIO53_FUNC_SDC0_DATA_0				1 /* B */
#define GPIO54_FUNC_SDC0_DATA_1				1 /* B */
#define GPIO55_FUNC_SDC0_DATA_2				1 /* B */
#define GPIO56_FUNC_SDC0_DATA_3				1 /* B */
#define GPIO57_FUNC_SDC0_DATA_4				1 /* B */
#define GPIO58_FUNC_SDC0_DATA_5				1 /* B */
#define GPIO59_FUNC_SDC0_DATA_6				1 /* B */
#define GPIO60_FUNC_SDC0_DATA_7				1 /* B */

#define GPIO57_FUNC_SDC2_DATA_0				2 /* B */
#define GPIO58_FUNC_SDC2_DATA_1				2 /* B */
#define GPIO59_FUNC_SDC2_DATA_2				2 /* B */
#define GPIO60_FUNC_SDC2_DATA_3				2 /* B */
#define GPIO61_FUNC_SDC2_CLK				1 /* L */
#define GPIO62_FUNC_SDC2_CMD				1 /* B */

#define GPIO63_FUNC_SDC1_CLK				1 /* L */
#define GPIO64_FUNC_SDC1_CMD				1 /* B */
#define GPIO65_FUNC_SDC1_DATA_0				1 /* B */
#define GPIO66_FUNC_SDC1_DATA_1				1 /* B */
#define GPIO67_FUNC_SDC1_DATA_2				1 /* B */
#define GPIO68_FUNC_SDC1_DATA_3				1 /* B */
#define GPIO69_FUNC_SDC1_DATA_4				1 /* B */
#define GPIO70_FUNC_SDC1_DATA_5				1 /* B */
#define GPIO71_FUNC_SDC1_DATA_6				1 /* B */
#define GPIO72_FUNC_SDC1_DATA_7				1 /* B */

#define GPIO69_FUNC_SDC3_DATA_0				2 /* B */
#define GPIO70_FUNC_SDC3_DATA_1				2 /* B */
#define GPIO71_FUNC_SDC3_DATA_2				2 /* B */
#define GPIO72_FUNC_SDC3_DATA_3				2 /* B */
#define GPIO73_FUNC_SDC3_CLK				1 /* L */
#define GPIO74_FUNC_SDC3_CMD				1 /* B */

#define GPIO52_FUNC_MDDRX_MNC_GPIO_OUT_0		2 /* O */
#define GPIO53_FUNC_MDDRX_MNC_GPIO_OUT_1		2 /* O */
#define GPIO54_FUNC_MDDRX_MNC_GPIO_OUT_2		2 /* O */
#define GPIO55_FUNC_MDDRX_MNC_GPIO_OUT_3		2 /* O */
#define GPIO56_FUNC_MDDRX_MNC_GPIO_OUT_4		2 /* O */
#define GPIO57_FUNC_MDDRX_MNC_GPIO_OUT_5		3 /* O */
#define GPIO58_FUNC_MDDRX_MNC_GPIO_OUT_6		3 /* O */
#define GPIO59_FUNC_MDDRX_MNC_GPIO_OUT_7		3 /* O */
#define GPIO60_FUNC_MDDRX_MNC_GPIO_OUT_8		3 /* O */
#define GPIO61_FUNC_MDDRX_MNC_GPIO_OUT_9		2 /* O */
#define GPIO62_FUNC_MDDRX_MNC_GPIO_OUT_10		2 /* O */
#define GPIO63_FUNC_MDDRX_MNC_GPIO_OUT_11		2 /* O */
#define GPIO64_FUNC_MDDRX_MNC_GPIO_IN_0			2 /* I */
#define GPIO65_FUNC_MDDRX_MNC_GPIO_IN_1			2 /* I */

#define GPIO75_FUNC_SGMII_MDC_0				1 /* O */
#define GPIO76_FUNC_SGMII_MDC_1				1 /* O */
#define GPIO77_FUNC_SGMII_MDIO_0			1 /* B */
#define GPIO78_FUNC_SGMII_MDIO_1			1 /* B */

#define GPIO35_FUNC_IEEE_1588_PPS_IN			1 /* I */
#define GPIO109_FUNC_IEEE_1588_PPS_OUT_0		1 /* O */
#define GPIO110_FUNC_IEEE_1588_PPS_OUT_1		1 /* O */

#define GPIO91_FUNC_FAN_PWM_0				1 /* O */
#define GPIO92_FUNC_FAN_TACH_0				1 /* I */
#define GPIO93_FUNC_FAN_PWM_1				1 /* O */
#define GPIO94_FUNC_FAN_TACH_1				1 /* I */
#define GPIO95_FUNC_FAN_PWM_2				1 /* O */
#define GPIO96_FUNC_FAN_TACH_2				1 /* I */
#define GPIO97_FUNC_FAN_PWM_3				2 /* O */
#define GPIO98_FUNC_FAN_TACH_3				2 /* I */
#define GPIO99_FUNC_FAN_PWM_4				1 /* O */
#define GPIO100_FUNC_FAN_TACH_4				1 /* I */
#define GPIO101_FUNC_FAN_PWM_5				1 /* O */
#define GPIO102_FUNC_FAN_TACH_5				1 /* I */
#define GPIO103_FUNC_FAN_PWM_6				2 /* O */
#define GPIO104_FUNC_FAN_TACH_6				2 /* I */
#define GPIO105_FUNC_FAN_PWM_7				2 /* O */
#define GPIO106_FUNC_FAN_TACH_7				2 /* I */

#define GPIO113_FUNC_OBFF_WAKE				1 /* O */

/* GPIO132-GPIO147: Software Reserved */
/* GPIO148: Reserved for PCI hot plug interrupt */
/* GPIO149: Reserved for PCI link up/down interrupt */

#define TLMM_GPIO_PULL_OFFSET		0
#define TLMM_GPIO_PULL_MASK		0x03
#define TLMM_GPIO_PULL(value)		TLMM_SET_FV(GPIO_PULL, value)
#define TLMM_NO_PULL			0
#define TLMM_PULL_DOWN			1
#define TLMM_KEEPER			2
#define TLMM_PULL_UP			3

/* 2.46.2.3.2. TLMM_GPIO_IN_OUT%n%
 * (0xFF02010000 + 0x00004 + 0x10000 * n (n=[0..(150-1)]))
 */
#define TLMM_GPIO_OUT_OFFSET		1
#define TLMM_GPIO_OUT_MASK		0x01
#define TLMM_GET_GPIO_OUT(value)	TLMM_GET_FV(GPIO_OUT, value)
#define TLMM_SET_GPIO_OUT(value)	TLMM_SET_FV(GPIO_OUT, value)
#define TLMM_GPIO_IN_OFFSET		0
#define TLMM_GPIO_IN_MASK		0x01
#define TLMM_GPIO_IN(value)		TLMM_SET_FV(GPIO_IN, value)

#define TLMM_NR_GPIOS		150

#define tlmm_write_config(gpio, cfg)	\
	__raw_writel(cfg, TLMM_GPIO_CFG(gpio))
#define tlmm_read_config(gpio)		\
	__raw_readl(TLMM_GPIO_CFG(gpio))
#define tlmm_config_pull(gpio, pull)	\
	__raw_setl(TLMM_GPIO_PULL(pull), TLMM_GPIO_CFG(gpio))
#define tlmm_config_func(gpio, func)	\
	__raw_setl(TLMM_FUNC_SEL(func), TLMM_GPIO_CFG(gpio))
#define tlmm_config_drive(gpio, drive)	\
	__raw_setl(TLMM_DRV_STRENGTH(drive), TLMM_GPIO_CFG(gpio))
#define tlmm_config_dir(gpio, dir)	\
	__raw_setl(TLMM_GPIO_OE, dir == GPIO_DRIVE_IN ? 0 : TLMM_GPIO_CFG(gpio))
#define tlmm_read_input(gpio)		\
	TLMM_GPIO_IN(__raw_readl(TLMM_GPIO_IN_OUT(n)))
#define tlmm_read_output(gpio, val)	\
	TLMM_GPIO_GET_OUT(__raw_readl(TLMM_GPIO_IN_OUT(n)))
#define tlmm_write_output(gpio, val)	\
	__raw_writel(TLMM_GPIO_SET_OUT(val), TLMM_GPIO_IN_OUT(n))

#define GPIO_HW_PORT			0x00
#define GPIO_HW_MAX_PINS		TLMM_NR_GPIOS

#define gpio_hw_read_pin(port, pin)		\
	tlmm_read_input(pin)
#define gpio_hw_write_pin(port, pin, val)	\
	tlmm_write_output(pin, val)
#define gpio_hw_config_mux(port, pin, mux)	\
	tlmm_config_func(pin, mux)
#define gpio_hw_config_pad(port, pin, pad, drv)	\
	tlmm_config_pad(pin, pad, drv)
#define gpio_hw_ctrl_init()

void tlmm_config_pad(uint8_t gpio, uint8_t pad, uint8_t drv);

#endif /* __TLMM_QDF2400_H_INCLUDE__ */
