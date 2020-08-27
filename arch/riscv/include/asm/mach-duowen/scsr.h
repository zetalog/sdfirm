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
 * @(#)scsr.h: DUOWEN SysCSR register definitions
 * $Id: scsr.h,v 1.1 2019-09-02 11:12:00 zhenglv Exp $
 */

#ifndef __SCSR_DUOWEN_H_INCLUDE__
#define __SCSR_DUOWEN_H_INCLUDE__

#define AXI_AXSIZE_BYTES		32

#include <target/amba.h>

#define SCSR_REG(offset)		(SCSR_BASE + (offset))
#define SCSR_HW_VERSION			SCSR_REG(0x00)
#define SCSR_BOOT_MODE			SCSR_REG(0x04)
#define SCSR_BOOT_ADDR_LO		SCSR_REG(0x08)
#define SCSR_BOOT_ADDR_HI		SCSR_REG(0x0C)
#define SCSR_BOOT_ADDR_CFG_LO		SCSR_REG(0x10)
#define SCSR_BOOT_ADDR_CFG_HI		SCSR_REG(0x14)
#define SCSR_HART_ID_LO			SCSR_REG(0x18)
#define SCSR_HART_ID_HI			SCSR_REG(0x1C)

#define SCSR_SHUTDN_REQ			SCSR_REG(0x80)
#define SCSR_BRINGUP_REQ		SCSR_REG(0x84)
#define SCSR_SHUTDN_ACK			SCSR_REG(0x88)
#define SCSR_BRINGUP_ACK		SCSR_REG(0x8C)
#define SCSR_UART_STATUS		SCSR_REG(0x90)
#define SCSR_TIMER_PAUSE		SCSR_REG(0x94)
#define SCSR_TIMER_STATUS		SCSR_REG(0x98)
#define SCSR_SD_STABLE			SCSR_REG(0x9C)
#define SCSR_SD_STATUS			SCSR_REG(0xA0)
#define SCSR_SPI_STATUS			SCSR_REG(0xA4)
#define SCSR_I2C_STATUS			SCSR_REG(0xA8)

#define SCSR_CLINT_CFG			SCSR_REG(0xC0)
#define SCSR_CHIP_LINK_CFG		SCSR_REG(0xC4)

#define PMA_CFG_LO(n)			SCSR_REG(0x100 + (n) << 3)
#define PMA_CFG_HI(n)			SCSR_REG(0x104 + (n) << 3)
#define PMA_ADDR_LO(n)			SCSR_REG(0x140 + (n) << 3)
#define PMA_ADDR_HI(n)			SCSR_REG(0x144 + (n) << 3)

/* SOC_HW_VERSION */
#define SCSR_MINOR_OFFSET		0
#define SCSR_MINOR_MASK			REG_8BIT_MASK
#define SCSR_MINOR(value)		_GET_FV(SCSR_MINOR, value)
#define SCSR_MAJOR_OFFSET		8
#define SCSR_MAJOR_MASK			REG_8BIT_MASK
#define SCSR_MAJOR(value)		_GET_FV(SCSR_MAJOR, value)
/* BOOT_MODE */
#define IMC_BOOT_FLASH_OFFSET		0
#define IMC_BOOT_FLASH_MASK		REG_2BIT_MASK
#define IMC_BOOT_FLASH(value)		_GET_FV(IMC_BOOT_FLASH, value)
#define IMC_FLASH_SD_LOAD		0x00
#define IMC_FLASH_SSI_LOAD		0x01
#define IMC_FLASH_SPI_LOAD		0x02
#define IMC_FLASH_SPI_BOOT		0x03
#define IMC_BOOT_IMC			0x0
#define IMC_BOOT_APC			0x4
#define IMC_BOOT_SIM			0x8
#define IMC_BOOT_SIM_IMC		IMC_BOOT_SIM
#define IMC_BOOT_SIM_APC		(IMC_BOOT_SIM | 0x2)

#define imc_get_boot_addr()				\
	MAKELLONG(__raw_readl(SCSR_BOOT_ADDR_LO),	\
		  __raw_readl(SCSR_BOOT_ADDR_HI))
#define imc_set_boot_addr(addr)				\
	do {						\
		__raw_writel(LODWORD(addr),		\
			     SCSR_BOOT_ADDR_CFG_LO);	\
		__raw_writel(HIDWORD(addr),		\
			     SCSR_BOOT_ADDR_CFG_HI);	\
	} while (0)
#define imc_get_hart_id()				\
	MAKELLONG(__raw_readl(SCSR_HART_ID_LO),		\
		  __raw_readl(SCSR_HART_ID_HI))
#define imc_set_hart_id(hart)				\
	do {						\
		__raw_writel(LODWORD(hart),		\
			     SCSR_HART_ID_LO);		\
		__raw_writel(HIDWORD(hart),		\
			     SCSR_HART_ID_HI);		\
	} while (0)

#ifndef __ASSEMBLY__
uint8_t imc_boot_cpu(void);
uint8_t imc_boot_flash(void);
#endif

#endif /* __SCSR_DUOWEN_H_INCLUDE__ */
