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
 * @(#)reg.h: DUOWEN space and register definitions
 * $Id: reg.h,v 1.1 2019-09-02 11:12:00 zhenglv Exp $
 */

#ifndef __REG_DUOWEN_H_INCLUDE__
#define __REG_DUOWEN_H_INCLUDE__

#include <target/config.h>
#include <target/sizes.h>

#ifdef CONFIG_DUOWEN_IMC
#define SFAB_ROM_BASE		UL(0x7FF00000)
#define SFAB_ROM_SIZE		0x100000
#define SFAB_RAM_BASE		UL(0x60400000)
#define SFAB_RAM_SIZE		0x200000
#ifdef CONFIG_DUOWEN_SFAB
#define IMC_ROM_BASE		SFAB_ROM_BASE
#define IMC_ROM_SIZE		SFAB_ROM_SIZE
#define IMC_RAM_BASE		SFAB_RAM_BASE
#define IMC_RAM_SIZE		SFAB_RAM_SIZE

#define IMC_CSR_BASE		UL(0x60100000)
#define IMC_TLMM_BASE		UL(0x60200000)
#define IMC_DEBUG_BASE		UL(0x60300000)
#define IMC_UART_BASE		UART0_BASE
#define IMC_TIMER_BASE		TIMER3_BASE
#define IMC_PLIC_BASE		PLIC_BASE
#else /* CONFIG_DUOWEN_SFAB */
#define IMC_ROM_BASE		UL(0x00010000)
#define IMC_ROM_SIZE		0x10000
#define IMC_RAM_BASE		UL(0x00100000)
#define IMC_RAM_SIZE		0x100000

#define IMC_DEBUG_BASE		UL(0x00000000)
#define IMC_DM_BASE		UL(0x00008000)
#define IMC_CSR_BASE		UL(0x0000C000)
#define IMC_UART_BASE		UL(0x00020000)
#define IMC_TIMER_BASE		UL(0x00024000)
#define IMC_PLIC_BASE		UL(0x00200000)
#endif /* CONFIG_DUOWEN_SFAB */

#define CRCNTL_BASE		UL(0x60000000)
#define GPIO0_BASE		UL(0x61000000)
#define GPIO1_BASE		UL(0x61100000)
#define GPIO2_BASE		UL(0x61200000)
#define SPI0_BASE		UL(0x62000000)
#define SPI1_BASE		UL(0x62100000)
#define SPI2_BASE		UL(0x62200000)
#define SPI3_BASE		UL(0x62300000)
#define SPISLV_BASE		UL(0x62400000)
#define UART0_BASE		UL(0x63000000)
#define UART1_BASE		UL(0x63100000)
#define UART2_BASE		UL(0x63200000)
#define UART3_BASE		UL(0x63300000)
#define I2C0_BASE		UL(0x64000000)
#define I2C1_BASE		UL(0x64100000)
#define I2C2_BASE		UL(0x64200000)
#define I2C3_BASE		UL(0x64300000)
#define I2C4_BASE		UL(0x64400000)
#define I2C5_BASE		UL(0x64500000)
#define I2C6_BASE		UL(0x64600000)
#define I2C7_BASE		UL(0x64700000)
#define I2CSLV0_BASE		UL(0x64800000)
#define I2CSLV1_BASE		UL(0x64900000)
#define I2CSLV2_BASE		UL(0x64A00000)
#define I2CSLV3_BASE		UL(0x64B00000)
#define WDT0_BASE		UL(0x65000000)
#define WDT1_BASE		UL(0x65100000)
#define TIMER0_BASE		UL(0x66000000)
#define TIMER1_BASE		UL(0x66100000)
#define TIMER2_BASE		UL(0x66200000)
#define TIMER3_BASE		UL(0x66300000)
#define SD_BASE			UL(0x67000000)
#define PLIC_BASE		UL(0x70000000)
#endif /* CONFIG_DUOWEN_IMC */

#ifdef CONFIG_DUOWEN_APC
#define SFAB_ROM_BASE		ULL(0xFF7FF00000)
#define SFAB_ROM_SIZE		0x100000
#define SFAB_RAM_BASE		ULL(0xFF60400000)
#define SFAB_RAM_SIZE		0x200000
#define APC_ROM_BASE		SFAB_ROM_BASE
#define APC_ROM_SIZE		SFAB_ROM_SIZE
#define APC_RAM_BASE		SFAB_RAM_BASE
#define APC_RAM_SIZE		SFAB_RAM_SIZE

#define IMC_CSR_BASE		UL(0xFF60100000)
#define IMC_TLMM_BASE		UL(0xFF60200000)
#define IMC_DEBUG_BASE		UL(0xFF60300000)
#define IMC_UART_BASE		UART0_BASE
#define IMC_TIMER_BASE		TIMER3_BASE
#define IMC_PLIC_BASE		PLIC_BASE

#define CRCNTL_BASE		UL(0xFF60000000)
#define GPIO0_BASE		UL(0xFF61000000)
#define GPIO1_BASE		UL(0xFF61100000)
#define GPIO2_BASE		UL(0xFF61200000)
#define SPI0_BASE		UL(0xFF62000000)
#define SPI1_BASE		UL(0xFF62100000)
#define SPI2_BASE		UL(0xFF62200000)
#define SPI3_BASE		UL(0xFF62300000)
#define SPISLV_BASE		UL(0xFF62400000)
#define UART0_BASE		UL(0xFF63000000)
#define UART1_BASE		UL(0xFF63100000)
#define UART2_BASE		UL(0xFF63200000)
#define UART3_BASE		UL(0xFF63300000)
#define I2C0_BASE		UL(0xFF64000000)
#define I2C1_BASE		UL(0xFF64100000)
#define I2C2_BASE		UL(0xFF64200000)
#define I2C3_BASE		UL(0xFF64300000)
#define I2C4_BASE		UL(0xFF64400000)
#define I2C5_BASE		UL(0xFF64500000)
#define I2C6_BASE		UL(0xFF64600000)
#define I2C7_BASE		UL(0xFF64700000)
#define I2CSLV0_BASE		UL(0xFF64800000)
#define I2CSLV1_BASE		UL(0xFF64900000)
#define I2CSLV2_BASE		UL(0xFF64A00000)
#define I2CSLV3_BASE		UL(0xFF64B00000)
#define WDT0_BASE		UL(0xFF65000000)
#define WDT1_BASE		UL(0xFF65100000)
#define TIMER0_BASE		UL(0xFF66000000)
#define TIMER1_BASE		UL(0xFF66100000)
#define TIMER2_BASE		UL(0xFF66200000)
#define TIMER3_BASE		UL(0xFF66300000)
#define SD_BASE			UL(0xFF67000000)
#define PLIC_BASE		UL(0xFF70000000)
#endif /* CONFIG_DUOWEN_APC */

#ifdef CONFIG_DUOWEN_FSBL
#ifdef CONFIG_DUOWEN_BOOT_APC
#define ROM_BASE		APC_RAM_BASE
#define ROMEND			(APC_RAM_BASE + APC_RAM_SIZE)
#else /* CONFIG_DUOWEN_BOOT_APC */
#define ROM_BASE		IMC_RAM_BASE
#define ROMEND			(IMC_RAM_BASE + IMC_RAM_SIZE)
#endif /* CONFIG_DUOWEN_BOOT_APC */
#endif /* CONFIG_DUOWEN_FSBL */

#ifdef CONFIG_DUOWEN_ZSBL
#ifdef CONFIG_DUOWEN_BOOT_APC
#define ROM_BASE		APC_ROM_BASE
#define ROMEND			(APC_ROM_BASE + APC_ROM_SIZE)
#else /* CONFIG_DUOWEN_BOOT_APC */
#define ROM_BASE		IMC_ROM_BASE
#define ROMEND			(IMC_ROM_BASE + IMC_ROM_SIZE)
#endif /* CONFIG_DUOWEN_BOOT_APC */
/* LOVEC */
#ifdef CONFIG_ARCH_HAS_LOVEC
#define VEC_BASE		ROM_BASE
#endif
#endif /* CONFIG_DUOWEN_ZSBL */

#ifdef CONFIG_DUOWEN_BOOT_APC
#define RAM_BASE		APC_RAM_BASE
#define RAMEND			(APC_RAM_BASE + APC_RAM_SIZE)
#else /* CONFIG_DUOWEN_BOOT_APC */
#define RAM_BASE		IMC_RAM_BASE
#define RAMEND			(IMC_RAM_BASE + IMC_RAM_SIZE)
#endif /* CONFIG_DUOWEN_BOOT_APC */

#endif /* __REG_DUOWEN_H_INCLUDE__ */
