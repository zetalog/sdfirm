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
 * @(#)mmu.c: DUOWEN machine specific MMU mappings
 * $Id: mmu.c,v 1.1 2020-07-01 10:28:00 zhenglv Exp $
 */

#include <target/paging.h>
#include <target/console.h>

#ifdef CONFIG_CRCNTL
caddr_t duowen_clk_reg_base = __DUOWEN_CLK_BASE;
caddr_t duowen_cfab_clk_reg_base = __DUOWEN_CFAB_CLK_BASE;
caddr_t duowen_apc_clk_reg_base[4] = {
	__DUOWEN_APC_CLK_BASE(0),
	__DUOWEN_APC_CLK_BASE(1),
	__DUOWEN_APC_CLK_BASE(2),
	__DUOWEN_APC_CLK_BASE(3),
};

void duowen_mmu_map_clk(void)
{
	int i;

	if (duowen_clk_reg_base == __DUOWEN_CLK_BASE) {
		set_fixmap_io(FIX_CRCNTL, __DUOWEN_CLK_BASE & PAGE_MASK);
		duowen_clk_reg_base = fix_to_virt(FIX_CRCNTL);
	}
	if (duowen_cfab_clk_reg_base == __DUOWEN_CFAB_CLK_BASE) {
		set_fixmap_io(FIX_CFAB, __DUOWEN_CFAB_CLK_BASE & PAGE_MASK);
		duowen_cfab_clk_reg_base = fix_to_virt(FIX_CFAB);
	}
	for (i = 0; i < 4; i++) {
		if (duowen_apc_clk_reg_base[i] == __DUOWEN_APC_CLK_BASE(i)) {
			set_fixmap_io(FIX_APC0 + i, __DUOWEN_APC_CLK_BASE(i) & PAGE_MASK);
			duowen_apc_clk_reg_base[i] = fix_to_virt(FIX_APC0 + i);
		}
	}
	for (i = 0; i < 4; i++)
		duowen_pll_reg_base[i] = CRCNTL_PLL_REG(i, 0);
	duowen_pll_reg_base[4] = COHFAB_PLL_REG(0);
	for (i = 0; i < 4; i++)
		duowen_pll_reg_base[5 + i] = CLUSTER_PLL_REG(i, 0);
}

void duowen_mmu_dump_clk(void)
{
	int i;

	if (duowen_clk_reg_base != __DUOWEN_CLK_BASE)
		con_log("FIXMAP: %016llx -> %016llx: CRCNTL\n",
			__DUOWEN_CLK_BASE, fix_to_virt(FIX_CRCNTL));
	if (duowen_cfab_clk_reg_base != __DUOWEN_CFAB_CLK_BASE)
		con_log("FIXMAP: %016llx -> %016llx: CFAB\n",
			__DUOWEN_CFAB_CLK_BASE, fix_to_virt(FIX_CFAB));
	for (i = 0; i < 4; i++) {
		if (duowen_apc_clk_reg_base[i] != __DUOWEN_APC_CLK_BASE(i))
			con_log("FIXMAP: %016llx -> %016llx: APC%d\n",
				__DUOWEN_APC_CLK_BASE(i),
				fix_to_virt(FIX_APC0 + i), i);
	}
}
#endif

#ifdef CONFIG_DUOWEN_UART
caddr_t duowen_uart_reg_base = __DUOWEN_UART_BASE;

void duowen_mmu_map_uart(int n)
{
	if (duowen_uart_reg_base == __DUOWEN_UART_BASE) {
		set_fixmap_io(FIX_UART, __DUOWEN_UART_BASE & PAGE_MASK);
		duowen_uart_reg_base = fix_to_virt(FIX_UART);
	}
}

void duowen_mmu_dump_uart(void)
{
	if (duowen_uart_reg_base != __DUOWEN_UART_BASE)
		con_log("FIXMAP: %016llx -> %016llx: UART\n",
			__DUOWEN_UART_BASE, fix_to_virt(FIX_UART));
}
#endif

void duowen_mmu_dump_maps(void)
{
	duowen_mmu_dump_clk();
	duowen_mmu_dump_uart();
}
