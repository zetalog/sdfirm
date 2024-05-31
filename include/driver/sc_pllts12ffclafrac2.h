
/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2024
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
 * @(#)sc_pllts12ffclafrac2.h: Silicon creation PLL TSMS12FFC definition
 * $Id: sc_pllts12ffclafrac2.h,v 1.1 2024-05-31 17:37:00 zhenglv Exp $
 */

#ifndef __SC_PLLTS12FFCLAFRAC2_H_INCLUDE__
#define __SC_PLLTS12FFCLAFRAC2_H_INCLUDE__

#include <target/arch.h>
#include <target/generic.h>

/* Configure a PLL in Integer Mode
 *              FREF*FBDIV
 * FOUT* = ------------------------
 *         REFDIV*POSTDIV1*POSTDIV2
 * Programming restrictions:
 * 1. FBDIV should be valid when PLL is enabled.
 * 2. Maximize POSTDIV1 prior than enabling POSTDIV2.
 *
 * Configure a PLL in Fractional Mode
 *          FREF    FBDIV + (FRAC/(2^24))
 * FOUT* = ------ x ---------------------
 *         REFDIV     POSTDIV1*POSTDIV2
 * Programming restrictions:
 * 1. Maximize POSTDIV1 prior than enabling POSTDIV2.
 *
 * There are three programming conditions:
 * 1. static divide in integer or fractional mode
 *    No re-timing, divide is programmed while (PLLEN=0, EN-0 or PD=1)
 * 2. dynamic integer divide in integer mode
 * 3. dynamic integer/fractional divide in fractional mode
 */
#define SC_PLLTS12FFC_REG(n, offset)	(SC_PLLTS12FFC_BASE(n) + (offset))

#define PLL_CTL(n)			SC_PLLTS12FFC_REG(n, 0x00)
#define PLL_CFG1(n)			SC_PLLTS12FFC_REG(n, 0x04)
#define PLL_CFG2(n)			SC_PLLTS12FFC_REG(n, 0x08)

/* PLL_CTL */
#define PLL_LOCK			_BV(31) /* Lock signal */
#define PLL_DACEN			_BV(5)  /* Factional noise canceling DAC is active */
#define PLL_FOUTPHASEEN			_BV(4)  /* Enable of 4 phase clock generator */
#define PLL_FOUTPOSTDIVEN		_BV(3)  /* Post divide power down */
#define PLL_BYPASS			_BV(2)
#define PLL_DSMEN			_BV(1)  /* Delta-Sigma modulator */
#define PLL_PLLEN			_BV(0)  /* Global enable signal for PLL */

/* PLL_CFG1 */
#define PLL_POSTDIV2_OFFSET		27
#define PLL_POSTDIV2_MASK		REG_3BIT_MASK
#define PLL_POSTDIV2(value)		_SET_FV(PLL_POSTDIV2, value)
#define PLL_POSTDIV1_OFFSET		24
#define PLL_POSTDIV1_MASK		REG_3BIT_MASK
#define PLL_POSTDIV1(value)		_SET_FV(PLL_POSTDIV1, value)
#define PLL_FBDIV_OFFSET		8
#define PLL_FBDIV_MASK			REG_12BIT_MASK
#define PLL_FBDIV(value)		_SET_FV(PLL_FBDIV, value)
#define PLL_REFDIV_OFFSET		0
#define PLL_REFDIV_MASK			REG_6BIT_MASK
#define PLL_REFDIV(value)		_SET_FV(PLL_REFDIV, value)

/* PLL_CFG2 */
#define PLL_FRAC_OFFSET			0
#define PLL_FRAC_MASK			REG_24BIT_MASK
#define PLL_FRACE(value)		_SET_FV(PLL_FRAC, value)

void sc_pllts12ffclafrac2_int_enable(int n,
				     uint32_t fref_freq,
				     uint32_t fout_freq);
void sc_pllts12ffclafrac2_frac_enable(int n,
				      uint32_t fref_freq,
				      uint32_t fout_freq);
void sc_pllts12ffclafrace_disable(int n);

#endif /* __SC_PLLTS12FFCLAFRAC2_H_INCLUDE__ */
