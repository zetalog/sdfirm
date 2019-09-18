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
 * @(#)arch.h: DUOWEN machine specific definitions
 * $Id: arch.h,v 1.1 2019-09-02 10:56:00 zhenglv Exp $
 */

#ifndef __ARCH_DUOWEN_H_INCLUDE__
#define __ARCH_DUOWEN_H_INCLUDE__

/* This file is intended to be used for implementing SoC specific
 * instructions, registers.
 */

#include <target/init.h>
#include <target/types.h>

#ifdef CONFIG_DUOWEN_IMC
#define __VEC			__LOVEC
#endif

#define CSR_BASE		UL(0x60300000)
#define CSR_REG(offset)		(CSR_BASE + (offset))

#define CSR_BOOT_ADDR		CSR_REG(0x00)
#define CSR_CORE_ID		CSR_REG(0x04)
#define CSR_CLUSTER_ID		CSR_REG(0x08)
#define CSR_CLOCK_EN		CSR_REG(0x0C)
/* n is 0..3 */
#define ADDR_TRANS_IN_(n)	CSR_REG(0x40 + (n) * 0x08)
#define ADDR_TRANS_OUT_(n)	CSR_REG(0x44 + (n) * 0x08)
#define ADDR_TRANS_IN_UPPER	CSR_REG(0x60)
#define ADDR_TRANS_OUT_UPPER	CSR_REG(0x64)
#define ADDR_TRANS_IN_LOWER	CSR_REG(0x68)
#define ADDR_TRANS_OUT_LOWER	CSR_REG(0x6C)

/* 4.2.2 CORE_ID (offset 0x04) */
#define CORE_ID_OFFSET		0
#define CORE_ID_MASK		REG_4BIT_MASK
#define CORE_ID(value)		_GET_FV(CORE_ID, value)
/* 4.2.3 CLUSTER_ID (offset 0x08) */
#define CLUSTER_ID_OFFSET	0
#define CLUSTER_ID_MASK		REG_6BIT_MASK
#define CLUSTER_ID(value)	_GET_FV(CLUSTER_ID, value)
/* 4.2.4 CLOCK_EN (offset 0x0C) */
#define CLOCK_EN		_BV(0)
/* 4.2.5 ADDR_TRANS_IN_0 (offset 0x40)
 * 4.2.7 ADDR_TRANS_IN_1 (offset 0x48)
 * 4.2.9 ADDR_TRANS_IN_2 (offset 0x50)
 * 4.2.11 ADDR_TRANS_IN_3 (offset 0x58)
 * 4.2.13 ADDR_TRANS_IN_UPPER (offset 0x60)
 */
#define ADDR_TRANS_VALID	_BV(31)
#define ADDR_TRANS_IN_OFFSET	0
#define ADDR_TRANS_IN_MASK	REG_12BIT_MASK
#define ADDR_TRANS_IN(value)	_SET_FV(ADDR_TRANS_IN, value)
/* 4.2.6 ADDR_TRANS_OUT_0 (offset 0x44)
 * 4.2.8 ADDR_TRANS_OUT_1 (offset 0x4C)
 * 4.2.10 ADDR_TRANS_OUT_2 (offset 0x54)
 * 4.2.12 ADDR_TRANS_OUT_3 (offset 0x5C)
 * 4.2.14 ADDR_TRANS_OUT_UPPER (offset 0x64)
 */
#define ADDR_TRANS_OUT_OFFSET	0
#define ADDR_TRANS_OUT_MASK	REG_24BIT_MASK
#define ADDR_TRANS_OUT(value)	_SET_FV(ADDR_TRANS_OUT, value)

#ifndef __ASSEMBLY__
void board_reset(void);
void board_suspend(void);
void board_hibernate(void);
#endif /* __ASSEMBLY__ */

#endif /* __ARCH_DUOWEN_H_INCLUDE__ */
