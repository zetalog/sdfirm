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
#define IMC_MAJOR_OFFSET		8
#define IMC_MAJOR_MASK			REG_8BIT_MASK
#define IMC_MAJOR(value)		_GET_FV(IMC_MAJOR, value)
#define IMC_MINOR_OFFSET		0
#define IMC_MINOR_MASK			REG_8BIT_MASK
#define IMC_MINOR(value)		_GET_FV(IMC_MINOR, value)
/* BOOT_MODE */
#define IMC_BOOT_MODE_OFFSET		0
#define IMC_BOOT_MODE_MASK		REG_3BIT_MASK
#define IMC_BOOT_MODE(value)		_GET_FV(IMC_BOOT_MODE, value)
#define IMC_BOOT_ROM			0x00
#define IMC_BOOT_FLASH			0x01
#define IMC_BOOT_USE_BOOT_ADDR		0x02
/* FLASH_SEL */
#define IMC_FLASH_SEL_OFFSET		4
#define IMC_FLASH_SEL_MASK		REG_1BIT_MASK
#define IMC_FLASH_SEL(value)		_GET_FV(IMC_FLASH_SEL, value)
#define IMC_FLASH_SPI			0x00
#define IMC_FLASH_SSI			0x01
/* SIM MSG SRC */
#ifdef CONFIG_DPULP_TCSR_SIM_FINISH
#define IMC_SIM_PASS			_BV(31)
#define IMC_SIM_FAIL			_BV(30)
#endif

#define imc_soc_major()			\
	IMC_MAJOR(__raw_readl(TCSR_SOC_VERSION))
#define imc_soc_minor()			\
	IMC_MINOR(__raw_readl(TCSR_SOC_VERSION))
#define imc_boot_mode()			\
	IMC_BOOT_MODE(__raw_readl(TCSR_BOOT_MODE))
#define imc_boot_flash()			\
	IMC_FLASH_SEL(__raw_readl(TCSR_BOOT_MODE))
#define imc_boot_addr()					\
	 MAKELLONG(__raw_readl(TCSR_BOOT_ADDR_LO),	\
		   __raw_readl(TCSR_BOOT_ADDR_HI))
#ifdef CONFIG_DPULP_TCSR_SIM_FINISH
#define imc_sim_finish(pass)		\
	__raw_setl((pass) ? IMC_SIM_PASS : IMC_SIM_FAIL, TCSR_SIM_FINISH)
#else
#define imc_sim_finish(pass)		do { } while (0)
#endif

#endif /* __TCSR_DPULP_H_INCLUDE__ */
