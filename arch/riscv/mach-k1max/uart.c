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
 * @(#)uart.c: K1MAX machine specific UART implementation
 * $Id: uart.c,v 1.1 2022-10-15 14:41:00 zhenglv Exp $
 */

#include <target/console.h>
#include <target/paging.h>

/*#ifdef CONFIG_K1M_K1X_UART
void k1max_k1x_uart_init(void)
{
	uint32_t data;
	data = __raw_readl(0xD4090104); // pll
	data |= 0x1f << 3;
	__raw_writel(data, 0xD4090104);

	data = __raw_readl(REG_PMUM_ACGR);
	data |= (1<<1) | (1<<8);
	__raw_writel(data, REG_PMUM_ACGR);

	__raw_writel(1, 0xD4051050ul); // apb clock: 52MHz

	__raw_writel(7, APB_BUSCLK_BASE+0x8); // gpio clock rst
	__raw_writel(3, APB_BUSCLK_BASE+0x8);

	__raw_writel(7, APB_BUSCLK_BASE+0x3c); // aib clk rst
	__raw_writel(3, APB_BUSCLK_BASE+0x3c);

	data = __raw_readl(MFPR_BASE+0x290); // uart txd
	data &= ~7;
	data |= 1;
	__raw_writel(data, MFPR_BASE+0x290);

	data = __raw_readl(MFPR_BASE+0x294); // uart rxd
	data &= ~7;
	data |= 1;
	__raw_writel(data, MFPR_BASE+0x294);

}
#endif*/

#ifdef CONFIG_MMU
void uart_hw_mmu_init(void)
{
	k1max_mmu_map_uart(UART_CON_ID);
	uart_hw_con_init();
	k1max_mmu_dump_maps();
}
#endif
