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
 * @(#)reg.h: SUNXI space and register definitions
 * $Id: reg.h,v 1.1 2019-11-17 11:52:00 zhenglv Exp $
 */

#ifndef __REG_SUNXI_H_INCLUDE__
#define __REG_SUNXI_H_INCLUDE__

#include <target/config.h>
#include <target/sizes.h>

/* Allwinner H5 Datasheet - Quad-Core OTT Box Processor revision 1.0 */
/* 4.1. Memory Mapping */
#define N_BROM_BASE		ULL(0x0000000000000000)
#define N_BROM_SIZE		ULL(0x8000)
#define S_BROM_BASE		ULL(0x0000000000000000)
#define S_BROM_SIZE		ULL(0x10000)
#define SRAM_A1_BASE		ULL(0x0000000000010000)
#define SRAM_A1_SIZE		ULL(0x8000)
#define SRAM_C_BASE		ULL(0x0000000000018000)
#define SRAM_C_SIZE		ULL(0x1C000)
#define SRAM_A2_BASE		ULL(0x0000000000044000)
#define SRAM_A2_SIZE		ULL(0x10000)

#ifdef CONFIG_SUNXI_BROM
#define ROM_BASE		N_BROM_BASE
#define ROMEND			N_BROM_SIZE
#define RAM_BASE		SRAM_A1_BASE
#define RAMEND			SRAM_A1_SIZE
#endif
#ifdef CONFIG_SUNXI_SPL
#define ROM_BASE		SRAM_A1_BASE
#define ROMEND			SRAM_A1_SIZE
#define RAM_BASE		SRAM_C_BASE
#define RAMEND			SRAM_C_SIZE
#endif

#define IDMAP_DEV_BASE		UART0_BASE
#define IDMAP_DEVEND		PAGE_SIZE

#endif /* __REG_SUNXI_H_INCLUDE__ */
