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
 * @(#)tcsr.h: DPU-LP top control/status register definitions
 * $Id: tcsr.h,v 1.1 2021-11-01 14:39:00 zhenglv Exp $
 */

#ifndef __TCSR_DPULP_H_INCLUDE__
#define __TCSR_DPULP_H_INCLUDE__

#define AXI_AXSIZE_BYTES		32

#include <target/amba.h>

#define TCSR_REG(offset)		(TCSR_BASE + (offset))
#define TCSR_MSG_REG(offset)		TCSR_REG(0xFFF00 + (offset))
#define TCSR_SOC_VERSION		TCSR_REG(0x00)
#define TCSR_BOOT_MODE			TCSR_REG(0x04)
#define TCSR_BOOT_ADDR_LO		TCSR_REG(0x40)
#define TCSR_BOOT_ADDR_HI		TCSR_REG(0x44)
#define TCSR_ROM_ADDR_LO		TCSR_REG(0x48)
#define TCSR_ROM_ADDR_HI		TCSR_REG(0x4C)
#define TCSR_VPU_HI_ADDR		TCSR_REG(0x50)
#define TCSR_SIM_FINISH			TCSR_MSG_REG(0x0C)
#define TCSR_SD_STABLE			TCSR_REG(0x60)
#define TCSR_SD_STATUS			TCSR_REG(0x64)

/* SOC_VERSION */
#define TCSR_MAJOR_OFFSET		8
#define TCSR_MAJOR_MASK			REG_8BIT_MASK
#define TCSR_MAJOR(value)		_GET_FV(TCSR_MAJOR, value)
#define TCSR_MINOR_OFFSET		0
#define TCSR_MINOR_MASK			REG_8BIT_MASK
#define TCSR_MINOR(value)		_GET_FV(TCSR_MINOR, value)
/* BOOT_MODE */
#define TCSR_BOOT_PAD_OFFSET		0
#define TCSR_BOOT_PAD_MASK		REG_3BIT_MASK
#define TCSR_BOOT_PAD(value)		_GET_FV(TCSR_BOOT_PAD, value)
/* BOOT FROM */
#define TCSR_BOOT_ROM			0x00
#define TCSR_BOOT_SPI			0x02
#define TCSR_BOOT_RAM			0x04
#define TCSR_BOOT_DDR			0x06
#define TCSR_BOOT_MASK			0x06
/* LOAD TO */
#define TCSR_LOAD_SSI			0x00
#define TCSR_LOAD_SD			0x01
#define TCSR_LOAD_MASK			0x01
/* SIM MSG SRC */
#ifdef CONFIG_DPULP_TCSR_SIM_FINISH
#define TCSR_SIM_PASS			_BV(31)
#define TCSR_SIM_FAIL			_BV(30)
#endif /* CONFIG_DPULP_TCSR_SIM_FINISH */

/* SD_STABLE */
#define TCSR_SD_HOST_REG_VOL_STABLE	_BV(0)

/* SD_STATUS */
#define TCSR_SD_DATXFER_WIDTH_OFFSET	7
#define TCSR_SD_DATXFER_WIDTH_MASK	REG_2BIT_MASK
#define TCSR_SD_DATXFER_WIDTH(value)	_GET_FV(TCSR_SD_DATXFER_WIDTH, value)
#define TCSR_SD_VDD1_SEL_OFFSET		4
#define TCSR_SD_VDD1_SEL_MASK		REG_3BIT_MASK
#define TCSR_SD_VDD1_SEL(value)		_GET_FV(TCSR_SD_VDD1_SEL, value)
#define TCSR_SD_UHSI_DRV_STH_OFFSET	2
#define TCSR_SD_UHSI_DRV_STH_MASK	REG_2BIT_MASK
#define TCSR_SD_UHSI_DRV_STH(value)	_GET_FV(TCSR_SD_UHSI_DRV_STH, value)
#define TCSR_SD_VDD1_ON			_BV(1)
#define TCSR_SD_UHSI_SWVOLT_EN		_BV(0)

#define tcsr_soc_major()			\
	((uint8_t)TCSR_MAJOR(__raw_readl(TCSR_SOC_VERSION)))
#define tcsr_soc_minor()			\
	((uint8_t)TCSR_MINOR(__raw_readl(TCSR_SOC_VERSION)))
#define tcsr_boot_mode()			\
	((uint8_t)TCSR_BOOT_PAD(__raw_readl(TCSR_BOOT_MODE)))
#define tcsr_boot_from()			\
	(tcsr_boot_mode() & TCSR_BOOT_MASK)
#define tcsr_load_to()			\
	(tcsr_boot_mode() & TCSR_LOAD_MASK)
#ifdef CONFIG_DPULP_TCSR_SIM_FINISH
#define tcsr_sim_finish(pass)		\
	__raw_setl((pass) ? TCSR_SIM_PASS : TCSR_SIM_FAIL, TCSR_SIM_FINISH)
#else
#define tcsr_sim_finish(pass)		do { } while (0)
#endif

#endif /* __TCSR_DPULP_H_INCLUDE__ */
