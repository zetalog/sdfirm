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

#ifdef CONFIG_DUOWEN_SOCv2
#include <asm/mach/reg_socv2.h>
#endif
#ifdef CONFIG_DUOWEN_SOCv3
#include <asm/mach/reg_socv3.h>
#endif

/* Reserved APC self-bootloader RAM */
#define ASBL_RAM_SIZE		0x8000 /* APC ZSBL */
#define ZSBL_RAM_SIZE		SFAB_RAM_SIZE
#define FSBL_RAM_SIZE		(SFAB_RAM_SIZE - ASBL_RAM_SIZE)
#ifdef CONFIG_DUOWEN_IMC
#define IMC_ROM_BASE		SFAB_ROM_BASE
#define IMC_ROM_SIZE		0xE0000
#define IMC_RAM_BASE		SFAB_RAM_BASE
#ifdef CONFIG_DUOWEN_FSBL
#define IMC_RAM_SIZE		FSBL_RAM_SIZE
#else
#define IMC_RAM_SIZE		ZSBL_RAM_SIZE
#endif
#endif /* CONFIG_DUOWEN_IMC */

/* DDR memory region */
#ifdef CONFIG_DUOWEN_DDR_32M
#define DDR_SIZE		SZ_32M
#endif
#ifdef CONFIG_DUOWEN_DDR_64M
#define DDR_SIZE		SZ_64M
#endif
#ifdef CONFIG_DUOWEN_DDR_128M
#define DDR_SIZE		SZ_128M
#endif
#ifdef CONFIG_DUOWEN_DDR_256M
#define DDR_SIZE		SZ_256M
#endif
#ifdef CONFIG_DUOWEN_DDR_512M
#define DDR_SIZE		SZ_512M
#endif
#ifdef CONFIG_DUOWEN_DDR_1G
#define DDR_SIZE		SZ_1G
#endif
#ifdef CONFIG_DUOWEN_DDR_2G
#define DDR_SIZE		SZ_2G
#endif
#ifdef CONFIG_DUOWEN_DDR_4G
#define DDR_SIZE		SZ_4G
#endif
#ifdef CONFIG_DUOWEN_DDR_8G
#define DDR_SIZE		SZ_8G
#endif
#ifdef CONFIG_DUOWEN_DDR_16G
#define DDR_SIZE		SZ_16G
#endif
#ifdef CONFIG_DUOWEN_DDR_32G
#define DDR_SIZE		SZ_32G
#endif
#ifdef CONFIG_DUOWEN_DDR_64G
#define DDR_SIZE		SZ_64G
#endif
#ifdef CONFIG_DUOWEN_DDR_128G
#define DDR_SIZE		SZ_128G
#endif
#ifdef CONFIG_DUOWEN_DDR_256G
#define DDR_SIZE		SZ_256G
#endif
#ifdef CONFIG_DUOWEN_DDR_512G
#define DDR_SIZE		SZ_512G
#endif

#define APC_ROM_BASE		(SFAB_ROM_BASE + 0xE0000)
#define APC_ROM_SIZE		0x20000
#define APC_RAM_SIZE		ASBL_RAM_SIZE
#define APC_RAM_BASE		(SFAB_RAM_BASE + FSBL_RAM_SIZE)

#ifdef CONFIG_DUOWEN_ASBL
#define ROM_BASE		APC_ROM_BASE
#define ROMEND			(APC_ROM_BASE + APC_ROM_SIZE)
#endif
#ifdef CONFIG_DUOWEN_ZSBL
#ifdef CONFIG_DUOWEN_BOOT_APC
#define ROM_BASE		APC_ROM_BASE
#define ROMEND			(APC_ROM_BASE + APC_ROM_SIZE)
#else /* CONFIG_DUOWEN_BOOT_APC */
#define ROM_BASE		IMC_ROM_BASE
#define ROMEND			(IMC_ROM_BASE + IMC_ROM_SIZE)
#endif /* CONFIG_DUOWEN_BOOT_APC */
#endif /* CONFIG_DUOWEN_ZSBL */

#ifdef CONFIG_DUOWEN_FSBL
#ifdef CONFIG_DUOWEN_BOOT_APC
#define ROM_BASE		APC_RAM_BASE
#define ROMEND			(APC_RAM_BASE + APC_RAM_SIZE)
#else /* CONFIG_DUOWEN_BOOT_APC */
#define ROM_BASE		IMC_RAM_BASE
#define ROMEND			(IMC_RAM_BASE + IMC_RAM_SIZE)
#endif /* CONFIG_DUOWEN_BOOT_APC */
#endif /* CONFIG_DUOWEN_FSBL */

#ifdef CONFIG_DUOWEN_BOOT_APC
#define RAM_BASE		APC_RAM_BASE
#define RAM_SIZE		APC_RAM_SIZE
#else /* CONFIG_DUOWEN_BOOT_APC */
#ifdef CONFIG_DUOWEN_APC
#define RAM_BASE		DDR_BASE
#define RAM_SIZE		DDR_SIZE
#else
#define RAM_BASE		IMC_RAM_BASE
#define RAM_SIZE		IMC_RAM_SIZE
#endif
#endif /* CONFIG_DUOWEN_BOOT_APC */
#define RAMEND			(RAM_BASE + RAM_SIZE)

#endif /* __REG_DUOWEN_H_INCLUDE__ */
