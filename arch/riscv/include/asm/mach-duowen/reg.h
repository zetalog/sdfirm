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
#define IMC_ROM_BASE		UL(0x60380000)
#define IMC_ROM_SIZE		0x80000

#define IMC_RAM_BASE		UL(0x60100000)
#ifdef CONFIG_DUOWEN_IMC_RAM_SIZE
#define IMC_RAM_SIZE		CONFIG_DUOWEN_IMC_RAM_SIZE
#else
#define IMC_RAM_SIZE		0x40000
#endif

#define IMC_PLIC_BASE		UL(0x60000000)
#define IMC_DEBUG_BASE		UL(0x602F0000)
#define IMC_DM_BASE		UL(0x602F8000)
#define IMC_CSR_BASE		UL(0x60300000)

#ifdef CONFIG_DUOWEN_IMC_RAM_BOOT
#define ROM_BASE		RAM_BASE
#define ROMEND			RAMEND
#else
#define ROM_BASE		IMC_ROM_BASE
#define ROMEND			(IMC_ROM_BASE + IMC_ROM_SIZE)
#endif /* CONFIG_DUOWEN_IMC_RAM_BOOT */
#define RAM_BASE		IMC_RAM_BASE
#define RAMEND			(IMC_RAM_BASE + IMC_RAM_SIZE)

/* LOVEC */
#define VEC_BASE		ROM_BASE
#endif /* CONFIG_DUOWEN_IMC */

#ifdef CONFIG_DUOWEN_APC
#define APC_ROM_BASE		UL(0x00000000)
#define APC_ROM_SIZE		0x80000

#define APC_RAM_BASE		UL(0x00000000)
#ifdef CONFIG_DUOWEN_APC_RAM_SIZE
#define APC_RAM_SIZE		CONFIG_DUOWEN_APC_RAM_SIZE
#else
#define APC_RAM_SIZE		0x40000
#endif

#ifdef CONFIG_DUOWEN_APC_RAM_BOOT
#define ROM_BASE		RAM_BASE
#define ROMEND			RAMEND
#else
#define ROM_BASE		APC_ROM_BASE
#define ROMEND			(APC_ROM_BASE + APC_ROM_SIZE)
#endif /* CONFIG_DUOWEN_IMC_RAM_BOOT */
#define RAM_BASE		APC_RAM_BASE
#define RAMEND			(APC_RAM_BASE + APC_RAM_SIZE)

/* LOVEC */
#define VEC_BASE		ROM_BASE
#endif /* CONFIG_DUOWEN_APC */

#endif /* __REG_DUOWEN_H_INCLUDE__ */
