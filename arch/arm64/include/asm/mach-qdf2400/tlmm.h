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

#define TLMM_REG(offset)		(TLMM_BASE+(offset))
#define TLMM_PIN_REG(offset, n)		TLMM_REG((offset) + 0x10000 * (n))
#define TLMM_DIR_REG(offset, n)		\
	TLMM_REG((offset) * 0x10000 + (NR_GPIOS + (n)) * 0x04)

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
#define TLMM_DIR_CONN_INTR_CFG_IMC(n)	TLMM_DIR_REG(1, n)
#define TLMM_DIR_CONN_INTR_CFG_HMSS(n)	TLMM_DIR_REG(2, n)
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

/* 2.46.2.3.3. TLMM_GPIO_INTR_CFG%n%
 * (0xFF02010000 + 0x00008 + 0x10000 * n (n=[0..(150-1)]))
 */
#define TLMM_INTR_ENABLE		_BV(0)
#define TLMM_INTR_POL_CTL		_BV(1)
#define TLMM_INTR_DECT_CTL_OFFSET	2
#define TLMM_INTR_DECT_CTL_MASK		0x03
#define TLMM_INTR_DECT_CTL(value)	TLMM_SET_FV(INTR_DECT_CTL, value)
#define TLMM_INTR_DECT_LEVEL		0
#define TLMM_INTR_POS_EDGE		1
#define TLMM_INTR_NEG_EDGE		2
#define TLMM_INTR_DUAL_EDGE		3
#define TLMM_INTR_RAW_STATUS_EN		_BV(4)
#define TLMM_INTR_TARGET_PROC_OFFSET	5
#define TLMM_INTR_TARGET_PROC_MASK	0x07
#define TLMM_INTR_TARGET_PROC(value)	TLMM_SET_FV(INTR_TARGET_PROC, value)
#define TLMM_INTR_TARGET_IMC		0
#define TLMM_INTR_TARGET_HMSS		1
#define TLMM_INTR_TARGET_TZ		2
#define TLMM_INTR_TARGET_NONE		7
#define TLMM_INTR_DIR_CONN_EN		_BV(8)

/* 2.46.2.3.4. TLMM_GPIO_INTR_STATUS%n%
 * (0xFF02010000 + 0x0000C + 0x10000 * n (n=[0..(150-1)]))
 */
#define TLMM_INTR_STATUS_OFFSET		0
#define TLMM_INTR_STATUS_MASK		0x01
#define TLMM_SET_INTR_STATUS(value)	TLMM_SET_FV(INTR_STATUS, value)
#define TLMM_GET_INTR_STATUS(value)	TLMM_GET_FV(INTR_STATUS, value)

/* 2.46.2.3.13. TLMM_DIR_CONN_INTR%n%_CFG_IMC
 * (0xFF02010000 + 0x10000 * 151 + 0x4 * n (n=[0..(16-1)]))
 * 2.46.2.3.14. TLMM_DIR_CONN_INTR%n%_CFG_HMSS
 * (0xFF02010000 + 0x10000 * 152 + 0x4 * n (n=[0..(16-1)]))
 */
#define TLMM_INTR_GPIO_SEL_OFFSET	0
#define TLMM_INTR_GPIO_SEL_MASK		0xFF
#define TLMM_INTR_GPIO_SEL(value)	TLMM_SET_FV(INTR_GPIO_SEL, value)
#define TLMM_INTR_POLARITY_OFFSET	8
#define TLMM_INTR_POLARITY_MASK		0x01
#define TLMM_INTR_POLARITY(value)	TLMM_SET_FV(INTR_POLARITY, value)
#define TLMM_INTR_POLARITY_ACTIVE_HIGH	1
#define TLMM_INTR_POLARITY_ACTIVE_LOW	0

/* 2.46.2.3.28. TLMM_HW_REVISION_NUMBER (0x0FF 02A4 0004) */
#define TLMM_START_BIT			_BV(0)
#define TLMM_QUALCOMM_MFG_ID_OFFSET	1
#define TLMM_QUALCOMM_MFG_ID_MASK	0x7FF
#define TLMM_QUALCOMM_MFG_ID(value)	TLMM_GET_FV(QUALCOMM_MFG_ID, value)
#define TLMM_PARTNUM_OFFSET		12
#define TLMM_PARTNUM_MASK		0xFFFF
#define TLMM_PARTNUM(value)		TLMM_GET_FV(PARTNUM, value)
#define TLMM_VERSION_ID_OFFSET		28
#define TLMM_VERSION_ID_MASK		0xF
#define TLMM_VERSION_ID(value)		TLMM_GET_FV(VERSION_ID, value)

/* 2.46.2.3.16. TLMM_PMIC_CTL0 (0x0FF 029B 0000) */
#define PSHOLD_HDRV_OFFSET	22
#define PSHOLD_HDRV_MASK	0x07
#define PSHOLD_HDRV(value)	_SET_FV(PSHOLD_HDRV, value)

#define TLMM_GPIO_CFG_PAD_MASK				\
	(TLMM_GPIO_PULL(TLMM_GPIO_PULL_MASK) |		\
	 TLMM_DRV_STRENGTH(TLMM_DRV_STRENGTH_MASK) |	\
	 TLMM_GPIO_OE)
#define TLMM_GPIO_INTR_CFG_IRQ_MASK				\
	(TLMM_INTR_DIR_CONN_EN |				\
	 TLMM_INTR_TARGET_PROC(TLMM_INTR_TARGET_PROC_MASK) |	\
	 TLMM_INTR_RAW_STATUS_EN |				\
	 TLMM_INTR_DECT_CTL(TLMM_INTR_DECT_CTL_MASK) |		\
	 TLMM_INTR_POL_CTL)

#define TLMM_NR_GPIOS		150

#define tlmm_config_pull(gpio, pull)				\
	__raw_writel_mask(TLMM_GPIO_PULL(pull),			\
			  TLMM_GPIO_PULL(TLMM_GPIO_PULL_MASK),	\
			  TLMM_GPIO_CFG(gpio))
#define tlmm_config_func(gpio, func)				\
	__raw_writel_mask(TLMM_FUNC_SEL(func),			\
			  TLMM_FUNC_SEL(TLMM_FUNC_SEL_MASK),	\
			  TLMM_GPIO_CFG(gpio))
#define tlmm_config_drive(gpio, drive)					\
	__raw_writel_mask(TLMM_DRV_STRENGTH(drive),			\
			  TLMM_DRV_STRENGTH(TLMM_DRV_STRENGTH_MASK),	\
			  TLMM_GPIO_CFG(gpio))
#define tlmm_config_dir(gpio, dir)				\
	((dir) == GPIO_DRIVE_IN ?				\
	 __raw_clearl(TLMM_GPIO_OE, TLMM_GPIO_CFG(gpio)) :	\
	 __raw_setl(TLMM_GPIO_OE, TLMM_GPIO_CFG(gpio)))
#define tlmm_read_input(gpio)		\
	TLMM_GPIO_IN(__raw_readl(TLMM_GPIO_IN_OUT(gpio)))
#define tlmm_read_output(gpio, val)	\
	TLMM_GET_GPIO_OUT(__raw_readl(TLMM_GPIO_IN_OUT(gpio)))
#define tlmm_write_output(gpio, val)				\
	__raw_writel_mask(TLMM_SET_GPIO_OUT(val),		\
			  TLMM_SET_GPIO_OUT(TLMM_GPIO_OUT_MASK),\
			  TLMM_GPIO_IN_OUT(gpio))
#define tlmm_enable_irq(gpio)		\
	__raw_setl(TLMM_INTRL_ENABLE, TLMM_GPIO_INTR_CFG(n))
#define tlmm_disable_irq(gpio)		\
	__raw_clearl(TLMM_INTRL_ENABLE, TLMM_GPIO_INTR_CFG(n))
#define tlmm_trigger_irq(gpio)						\
	__raw_writel_mask(TLMM_SET_INTR_STATUS(1),			\
			  TLMM_SET_INTR_STATUS(TLMM_INTR_STATUS_MASK),	\
			  TLMM_GPIO_INTR_STATUS(gpio))
#define tlmm_clear_irq(gpio)						\
	__raw_writel_mask(TLMM_SET_INTR_STATUS(0),			\
			  TLMM_SET_INTR_STATUS(TLMM_INTR_STATUS_MASK),	\
			  TLMM_GPIO_INTR_STATUS(gpio))
#define tlmm_irq_status(gpio)		\
	TLMM_GET_INTR_STATUS(__raw_readl(TLMM_GPIO_INTR_STATUS(gpio)))
#define tlmm_config_direct_irq_imc(dir, pol, irq)	\
	__raw_writel(TLMM_INTR_GPIO_SEL(irq) |		\
		     TLMM_INTR_POLARITY(pol),		\
		     TLMM_DIR_CONN_INTR_CFG_IMC(dir))
#define tlmm_config_direct_irq_hmss(dir, pol, irq)	\
	__raw_writel(TLMM_INTR_GPIO_SEL(irq) |		\
		     TLMM_INTR_POLARITY(pol),		\
		     TLMM_DIR_CONN_INTR_CFG_HMSS(dir))
#define tlmm_config_sdc(gpio, val)    	\
	__raw_writel(val, TLMM_GPIO_SDC_CTL(gpio))
#define tlmm_write_phase_flags(flags)	\
	__raw_writel(flags, TLMM_PHASE_FLAGS)

#define tlmm_config_pshold(drv)						\
	__raw_writel_mask(PSHOLD_HDRV(TLMM_MA_TO_DRV_STRENGTH(drv)),	\
			  PSHOLD_HDRV(PSHOLD_HDRV_MASK), TLMM_PMIC_CTL0)

#define tlmm_get_version()		\
	TLMM_VERSION_ID(__raw_readl(TLMM_HW_REVISION_NUMBER))
#define tlmm_get_partnum()		\
	TLMM_PARTNUM(__raw_readl(TLMM_HW_REVISION_NUMBER))

/* TLMM specific GPIO IRQ flags */
#define GPIO_TARGET_PROC(proc)	TLMM_INTR_TARGET_PROC(proc)
#define GPIO_DIRECT_CONN	TLMM_INTR_DIR_CONN_EN
#define GPIO_TLMM_MASK			\
	(TLMM_INTR_DIR_CONN_EN |	\
	 TLMM_INTR_TARGET_PROC(TLMM_INTR_TARGET_PROC_MASK))

void tlmm_config_pad(uint8_t gpio, uint8_t pad, uint8_t drv);
void tlmm_config_irq(uint8_t gpio, uint32_t mode);

#endif /* __TLMM_QDF2400_H_INCLUDE__ */
