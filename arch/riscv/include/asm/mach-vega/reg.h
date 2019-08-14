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
 * @(#)reg.h: RV32M1 (VEGA) space and register definitions
 * $Id: reg.h,v 1.1 2019-08-14 10:19:18 zhenglv Exp $
 */

#ifndef __REG_VEGA_H_INCLUDE__
#define __REG_VEGA_H_INCLUDE__

#include <target/config.h>
#include <target/sizes.h>

#define M4_FLASH_BASE		0x00000000
#define M4_FLASH_SIZE		0x00100000 /* 1MB */

#define M0_FLASH_BASE		0x01000000
#define M0_FLASH_SIZE		0x00040000 /* 256KB */

#define M4_ITCM_SRAM_BASE	0x08000000
#define M4_ITCM_SRAM_SIZE	0x00010000 /* 64KB */

#define BOOT_ROM_BASE		0x08800000
#define BOOT_ROM_SIZE		0x0000C000

#define M0_TCM_SRAM_BASE	0x09000000
#define M0_TCM_SRAM_SIZE	0x00020000 /* 128KB */

#define M4_DTCM_SRAM_BASE	0x20000000
#define M4_DTCM_SRAM_SIZE	0x00030000 /* 192KB */

#ifdef CONFIG_VEGA_RI5CY
#define ROM_BASE		M4_FLASH_BASE
#define ROMEND			(M4_FLASH_BASE + M4_FLASH_SIZE)
#define RAM_BASE		M4_DTCM_SRAM_BASE
#define RAMEND			(M4_DTCM_SRAM_BASE + M4_DTCM_SRAM_SIZE)
/* HIVEC */
#define VEC_BASE		(ROMEND - 0x100)
#endif

#ifdef CONFIG_VEGA_0RISCY
#define ROM_BASE		M0_FLASH_BASE
#define ROMEND			(M0_FLASH_BASE + M0_FLASH_SIZE)
#define RAM_BASE		M0_TCM_SRAM_BASE
#define RAMEND			(M0_TCM_SRAM_BASE + M0_TCM_SRAM_SIZE)
#endif

#endif /* __REG_VEGA_H_INCLUDE__ */
