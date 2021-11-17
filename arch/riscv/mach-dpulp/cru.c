/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2021
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
 * @(#)cru.c: DPU-LP clock/reset unit implementation
 * $Id: cru.c,v 1.1 2021-11-16 11:09:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/delay.h>
#include <target/cmdline.h>

void dpulp_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val)
{
	__raw_writel(PLL_REG_WRITE | PLL_REG_SEL(reg) | PLL_REG_WDATA(val),
		     PLL_REG_ACCESS(pll));
	while (!(__raw_readl(PLL_REG_ACCESS(pll)) & PLL_REG_IDLE));
}

uint8_t dpulp_pll_reg_read(uint8_t pll, uint8_t reg)
{
	uint32_t val;

	__raw_writel(PLL_REG_READ | PLL_REG_SEL(reg),
		     PLL_REG_ACCESS(pll));
	do {
		val = __raw_readl(PLL_REG_ACCESS(pll));
	} while (!(val & PLL_REG_IDLE));
	return PLL_REG_RDATA(val);
}

/*===========================================================================
 * CRU TRACE APIs
 *===========================================================================*/
#ifdef CONFIG_CRU_TRACE
void cru_trace(bool enabling, const char *name)
{
	con_dbg("crcntl: %c %s\n", enabling ? 'E' : 'D', name);
}
#endif /* CONFIG_CRU_TRACE */
