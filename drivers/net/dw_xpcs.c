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
 * @(#)dw_xpcs.c: Synopsys DesignWare ethernet PCS implementation
 * $Id: dw_xpcs.c,v 1.0 2020-12-20 17:20:00 zhenglv Exp $
 */

#include <asm/mach/eth.h>

#ifdef CONFIG_ARCH_IS_DW_XPCS_APB
#ifdef CONFIG_ARCH_IS_DW_XPCS_APB_INDIRECT
#define DWCXS_APBI_MMD			0xFF
#define DWCXS_APBI_BASE(addr)		((addr) >> 8)
#define DWCXS_APBI_ADDR(addr)		((addr) & 0xff)

uint16_t xpcs_base;

uint16_t dw_xpcs_read(unsigned long addr)
{
	uint16_t base = DWCXS_APBI_BASE(addr);

	if (xpcs_base != base) {
		xpcs_base = base;
		__raw_writew(base, DWCXS_APBI_MMD << DW_XPCS_APBI_OFFSET);
	}
	return __raw_readw(DWCXS_APBI_ADDR(addr) << DW_XPCS_APBI_OFFSET);
}

void dw_xpcs_write(unsigned long addr, uint16_t val)
{
	uint16_t base = DWCXS_APBI_BASE(addr);

	if (xpcs_base != base) {
		xpcs_base = base;
		__raw_writew(base, DWCXS_APBI_MMD << DW_XPCS_APBI_OFFSET);
	}
	__raw_writew(val, DWCXS_APBI_ADDR(addr) << DW_XPCS_APBI_OFFSET);
}
#else /* CONFIG_ARCH_IS_DW_XPCS_APB_INDIRECT */
uint16_t dw_xpcs_read(unsigned long addr)
{
	return (uint16_t)__raw_readl(addr);
}

void dw_xpcs_write(unsigned long addr, uint16_t val)
{
	__raw_writel((uint32_t)val, addr);
}
#endif /* CONFIG_ARCH_IS_DW_XPCS_APB_INDIRECT */
#endif /* CONFIG_ARCH_IS_DW_XPCS_APB */

void dw_xpcs_init_10g(void)
{
	dw_xpcs_hw_clock_init();

	dw_xpcs_write(SR_PMA_CTRL1, SR_PMA_RST | SR_PMA_SS_10G);
	dw_xpcs_hw_sram_init();
	dw_xpcs_power_init();
	dw_xpcs_an_init();

	dw_xpcs_write(SR_PMA_CTRL2, SR_PMA_TYPE(DW_XPCS_PMA_TYPE));
	dw_xpcs_write(SR_XS_PCS_CTRL2, SR_PCS_TYPE(DW_XPCS_PCS_TYPE));

	while (dw_xpcs_read(SR_XS_PCS_STS1) & SR_PCS_RLU);
}
