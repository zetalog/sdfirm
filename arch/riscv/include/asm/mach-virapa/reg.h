/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2022
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
 * @(#)reg.h: VIRAPA space and register definitions
 * $Id: reg.h,v 1.1 2020-06-08 17:12:00 zhenglv Exp $
 */

#ifndef __REG_VIRAPA_H_INCLUDE__
#define __REG_VIRAPA_H_INCLUDE__

#define ROM_BASE		ULL(0xFFFFFFF000)
#define ROM_SIZE		UL(0xF00)
#define RAM_BASE		CONFIG_VIRAPA_MEM_BASE
#define RAM_SIZE		CONFIG_VIRAPA_MEM_SIZE
#define RAMEND			(RAM_BASE + RAM_SIZE)

#define DEV_BASE		ULL(0x8002000000)
#define DEV_SIZE		UL(0x40000)

#define CLINT_BASE		ULL(0x8002000000)
#define UART_BASE		ULL(0x8002010000)
#ifdef CONFIG_VIRAPA_TBOX_TIDY
#define TBOX_BASE		ULL(0x8002020000)
#endif
#include <asm/tbox.h>

#endif /* __REG_VIRAPA_H_INCLUDE__ */
