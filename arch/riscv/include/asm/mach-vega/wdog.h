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
 * @(#)wdog.h: RV32M1 (VEGA) watch dog (WDOG) interfaces
 * $Id: wdog.h,v 1.1 2019-08-27 17:56:00 zhenglv Exp $
 */

#ifndef __WDOG_VEGA_H_INCLUDE__
#define __WDOG_VEGA_H_INCLUDE__

#include <target/types.h>
#include <asm/io.h>

#define WDOG0_BASE		UL(0x4002A000)
#define WDOG1_BASE		UL(0xT41026000)
#ifdef CONFIG_VEGA_RI5CY
#define WDOG_BASE		WDOG0_BASE
#endif
#ifdef CONFIG_VEGA_0RISCY
#define WDOG_BASE		WDOG1_BASE
#endif
#define WDOG_REG(offset)	(WDOG_BASE + (offset))

#define WDOG_CS			WDOG_REG(0x00)
#define WDOG_CNT		WDOG_REG(0x04)
#define WDOG_TOVAL		WDOG_REG(0x08)
#define WDOG_WIN		WDOG_REG(0x0C)

/* 41.3.2.2 Watchdog Control and Status Register (CS) */
#define WDOT_STOP		_BV(0)
#define WDOG_WAIT		_BV(1)
#define WDOG_DBG		_BV(2)
#define WDOG_TST_OFFSET		3
#define WDOG_TST_MASK		REG_2BIT_MASK
#define WDOG_TST(value)		_SET_FV(WDOG_TST, value)
#define WDOG_UPDATE		_BV(5)
#define WDOG_INT		_BV(6)
#define WDOG_EN			_BV(7)
#define WDOG_CLK_OFFSET		8
#define WDOG_CLK_MASK		REG_2BIT_MASK
#define WDOG_CLK(value)		_SET_FV(WDOG_CLK, value)
#define WDOG_RCS		_BV(10)
#define WDOG_ULK		_BV(11)
#define WDOG_PRES		_BV(12)
#define WDOG_CMD32EN		_BV(13)
#define WDOG_FLG		_BV(14)
#define WDOG_WIN_EN		_BV(15)

/* 41.3.2.3 Watchdog Counter Register (CNT) */
#define WDOG_CNTLOW_OFFSET	0
#define WDOG_CNTLOW_MASK	REG_8BIT_MASK
#define WDOG_CNTLOW(value)	_SET_FV(WDOG_CNTLOW, value)
#define WDOG_CNTHIGH_OFFSET	8
#define WDOG_CNTHIGH_MASK	REG_8BIT_MASK
#define WDOG_CNTHIGH(value)	_SET_FV(WDOG_CNTHIGH, value)

/* 41.3.2.4 Watchdog Timeout Value Register (TOVAL) */
#define WDOG_TOVALLOW_OFFSET	0
#define WDOG_TOVALLOW_MASK	REG_8BIT_MASK
#define WDOG_TOVALLOW(value)	_SET_FV(WDOG_TOVALLOW, value)
#define WDOG_TOVALHIGH_OFFSET	8
#define WDOG_TOVALHIGH_MASK	REG_8BIT_MASK
#define WDOG_TOVALHIGH(value)	_SET_FV(WDOG_TOVALHIGH, value)

/* 41.3.2.5 Watchdog Window Register (WIN) */
#define WDOG_WINLOW_OFFSET	0
#define WDOG_WINLOW_MASK	REG_8BIT_MASK
#define WDOG_WINLOW(value)	_SET_FV(WDOG_WINLOW, value)
#define WDOG_WINHIGH_OFFSET	8
#define WDOG_WINHIGH_MASK	REG_8BIT_MASK
#define WDOG_WINHIGH(value)	_SET_FV(WDOG_WINHIGH, value)

void wdog_disable(void);

#endif /* __WDOG_VEGA_H_INCLUDE__ */
