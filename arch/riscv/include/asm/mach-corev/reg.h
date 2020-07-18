/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2020
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
 * @(#)reg.h: core-v testbench space and register definitions
 * $Id: reg.h,v 1.1 2020-04-06 23:37:00 zhenglv Exp $
 */

#ifndef __REG_COREV_H_INCLUDE__
#define __REG_COREV_H_INCLUDE__

#define MEM_BASE		0x00000000
#ifdef CONFIG_COREV_MEM_WIDTH
#define MEM_WIDTH		CONFIG_COREV_MEM_WIDTH
#define MEM_SIZE		_BV(MEM_WIDTH)
#else
#define MEM_SIZE		0x400000
#endif

#define DEBUG_BASE		0x1A110800

#define ROM_BASE		RAM_BASE
#define ROM_SIZE		RAM_SIZE
#define RAM_BASE		MEM_BASE
#define RAMEND			(MEM_BASE + MEM_SIZE)

#define PRINT_BASE		0x10000000
#define TIMER_BASE		0x15000000
#define TESTS_BASE		0x20000000

#define TESTS_REG(offset)	(TESTS_BASE + (offset))
/* write test result of program to this reg */
#define TESTS_RESULT		TESTS_REG(0x00)
/* write exit value of program to this reg */
#define TESTS_EXIT		TESTS_REG(0x04)

#endif /* __REG_COREV_H_INCLUDE__ */
