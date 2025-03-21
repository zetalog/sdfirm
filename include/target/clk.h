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
 * @(#)clk.h: clock tree framework interface
 * $Id: clk.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __CLK_H_INCLUDE__
#define __CLK_H_INCLUDE__

#include <target/generic.h>

#ifdef CONFIG_CLK_MAX_DRIVERS
#define MAX_CLK_DRIVERS		CONFIG_CLK_MAX_DRIVERS
#endif

#ifndef __ASSEMBLY__
#include <driver/clk.h>

#ifndef ARCH_HAVE_CLK_TYPES
typedef uint16_t clk_t;
typedef uint8_t clk_cat_t;
typedef uint8_t clk_clk_t;

#ifndef clkid
#define clkid(cat, clk)		((clk_t)MAKEWORD(clk, cat))
#define invalid_clk		clkid(0xFF, 0xFF)
#endif
#ifndef clk_clk
#define clk_clk(clkid)		LOBYTE(clkid)
#endif
#ifdef clk_cat
#define clk_cat(clkid)		HIBYTE(clkid)
#endif
#endif

#ifndef clk_freq_t
#define clk_freq_t		uint32_t
#endif
#ifndef INVALID_FREQ
#define INVALID_FREQ		((clk_freq_t)0)
#endif

struct clk_driver {
	clk_clk_t max_clocks;
	int (*enable)(clk_clk_t clk);
	void (*disable)(clk_clk_t clk);
	clk_freq_t (*get_freq)(clk_clk_t clk);
	int (*set_freq)(clk_clk_t clk, clk_freq_t freq);
	void (*select)(clk_clk_t clk, clk_t src);
	const char *(*get_name)(clk_clk_t clk);
};

#ifdef CONFIG_CLK
clk_freq_t clk_get_frequency(clk_t clk);
int clk_set_frequency(clk_t clk, clk_freq_t freq);
int clk_enable(clk_t clk);
void clk_disable(clk_t clk);
void clk_select_source(clk_t clk, clk_t src);
const char *clk_get_mnemonic(clk_t clk);
int clk_register_driver(clk_cat_t category, const struct clk_driver *clkd);
void clk_init(void);
caddr_t sel_clk_reg(clk_clk_t sel);
caddr_t div_clk_reg(clk_clk_t clk);
caddr_t div_clk_rst_reg(clk_clk_t clk);
#ifndef CONFIG_MMU
#define clk_hw_mmu_init()		do { } while (0)
#endif /* CONFIG_MMU */
#else
#ifdef CONFIG_CLK_GENERIC
#define clk_get_frequency(clk)		clk_hw_get_frequency(clk)
#define clk_set_frequency(clk)		0
#define clk_enable(clk)			clk_hw_enable(clk)
#define clk_disable(clk)		do { } while (0)
#define clk_select_source(clk, src)	do { } while (0)
#define clk_get_mnemonic(clk)		NULL
#define clk_init()			do { } while (0)
#ifndef CONFIG_MMU
#define clk_hw_mmu_init()		do { } while (0)
#define clk_register_driver(cat, clkd)	(-ENODEV)
#endif /* CONFIG_MMU */
#else
#define clk_get_frequency(clk)		INVALID_FREQ
#define clk_set_frequency(clk)		(-ENODEV)
#define clk_enable(clk)			(-ENODEV)
#define clk_disable(clk)		do { } while (0)
#define clk_select_source(clk, src)	do { } while (0)
#define clk_get_mnemonic(clk)		NULL
#define clk_init()			do { } while (0)
#ifndef CONFIG_MMU
#define clk_hw_mmu_init()		do { } while (0)
#define clk_register_driver(cat, clkd)	(-ENODEV)
#endif /* CONFIG_MMU */
#endif
#endif
#endif /* __ASSEMBLY__ */

#endif /* __CLK_H_INCLUDE__ */
