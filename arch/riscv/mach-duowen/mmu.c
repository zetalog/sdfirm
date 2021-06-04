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
#ifdef CONFIG_DUOWEN_SBI_DUAL
caddr_t duowen_clk_reg_base[2];
caddr_t duowen_cfab_clk_reg_base[2];
caddr_t duowen_apc_clk_reg_base[2][4];
caddr_t duowen_eth_clk_reg_base[2];

static void __duowen_mmu_map_clk(uint8_t soc)
{
	int i;

	if (duowen_clk_reg_base[soc] == __DUOWEN_CLK_BASE(soc)) {
		set_fixmap_io(FIX_CRCNTL(soc),
			      __DUOWEN_CLK_BASE(soc) & PAGE_MASK);
		duowen_clk_reg_base[soc] = fix_to_virt(FIX_CRCNTL(soc));
	}
	if (duowen_cfab_clk_reg_base[soc] == __DUOWEN_CFAB_CLK_BASE(soc)) {
		set_fixmap_io(FIX_CFAB(soc),
			      __DUOWEN_CFAB_CLK_BASE(soc) & PAGE_MASK);
		duowen_cfab_clk_reg_base[soc] = fix_to_virt(FIX_CFAB(soc));
	}
	for (i = 0; i < 4; i++) {
		if (duowen_apc_clk_reg_base[soc][i] ==
		    __DUOWEN_APC_CLK_BASE(soc, i)) {
			set_fixmap_io(FIX_APC0(soc) + i,
				__DUOWEN_APC_CLK_BASE(soc, i) & PAGE_MASK);
			duowen_apc_clk_reg_base[soc][i] =
				fix_to_virt(FIX_APC0(soc) + i);
		}
	}
	if (duowen_eth_clk_reg_base[soc] == __DUOWEN_ETH_CLK_BASE(soc)) {
		set_fixmap_io(FIX_ETH(soc),
			      __DUOWEN_ETH_CLK_BASE(soc) & PAGE_MASK);
		duowen_eth_clk_reg_base[soc] = fix_to_virt(FIX_ETH(soc));
	}
	for (i = 0; i < 4; i++)
		duowen_pll_reg_base[socpll(i, soc)] =
			CRCNTL_PLL_REG(soc, i, 0);
	duowen_pll_reg_base[socpll(4, soc)] = COHFAB_PLL_REG(soc, 0);
	for (i = 0; i < 4; i++)
		duowen_pll_reg_base[socpll(5 + i, soc)] =
			CLUSTER_PLL_REG(soc, i, 0);
	duowen_pll_reg_base[socpll(9, soc)] = ETH_PLL_REG(soc, 0);
}

static void __duowen_mmu_dump_clk(uint8_t soc)
{
	int i;

	if (duowen_clk_reg_base[soc] != __DUOWEN_CLK_BASE(soc))
		con_log("FIXMAP: %016llx -> %016llx: CRCNTL\n",
			__DUOWEN_CLK_BASE(soc), fix_to_virt(FIX_CRCNTL(soc)));
	if (duowen_cfab_clk_reg_base[soc] != __DUOWEN_CFAB_CLK_BASE(soc))
		con_log("FIXMAP: %016llx -> %016llx: CFAB\n",
			__DUOWEN_CFAB_CLK_BASE(soc),
			fix_to_virt(FIX_CFAB(soc)));
	for (i = 0; i < 4; i++) {
		if (duowen_apc_clk_reg_base[soc][i] !=
		    __DUOWEN_APC_CLK_BASE(soc, i))
			con_log("FIXMAP: %016llx -> %016llx: APC%d\n",
				__DUOWEN_APC_CLK_BASE(soc, i),
				fix_to_virt(FIX_APC0(soc) + i), i);
	}
	if (duowen_eth_clk_reg_base[soc] != __DUOWEN_ETH_CLK_BASE(soc))
		con_log("FIXMAP: %016llx -> %016llx: ETH\n",
			__DUOWEN_ETH_CLK_BASE(soc), fix_to_virt(FIX_ETH(soc)));
}

static void __duowen_mmu_init_clk(uint8_t soc)
{
	duowen_clk_reg_base[soc] = __DUOWEN_CLK_BASE(soc);
	duowen_cfab_clk_reg_base[soc] = __DUOWEN_CFAB_CLK_BASE(soc);
	duowen_apc_clk_reg_base[soc][0] = __DUOWEN_APC_CLK_BASE(soc, 0);
	duowen_apc_clk_reg_base[soc][1] = __DUOWEN_APC_CLK_BASE(soc, 1);
	duowen_apc_clk_reg_base[soc][2] = __DUOWEN_APC_CLK_BASE(soc, 2);
	duowen_apc_clk_reg_base[soc][3] = __DUOWEN_APC_CLK_BASE(soc, 3);
	duowen_eth_clk_reg_base[soc] = __DUOWEN_ETH_CLK_BASE(soc);
}

void duowen_mmu_map_clk(void)
{
	__duowen_mmu_map_clk(0);
	__duowen_mmu_map_clk(1);
}

void duowen_mmu_dump_clk(void)
{
	__duowen_mmu_dump_clk(0);
	__duowen_mmu_dump_clk(1);
}

void duowen_mmu_init_clk(void)
{
	__duowen_mmu_init_clk(0);
	__duowen_mmu_init_clk(1);
}
#else /* CONFIG_DUOWEN_SBI_DUAL */
caddr_t duowen_clk_reg_base;
caddr_t duowen_cfab_clk_reg_base;
caddr_t duowen_apc_clk_reg_base[4];
caddr_t duowen_eth_clk_reg_base;

void duowen_mmu_map_clk(void)
{
	__unused uint8_t soc = imc_socket_id();
	int i;

	if (duowen_clk_reg_base == __DUOWEN_CLK_BASE(soc)) {
		set_fixmap_io(FIX_CRCNTL(soc),
			      __DUOWEN_CLK_BASE(soc) & PAGE_MASK);
		duowen_clk_reg_base = fix_to_virt(FIX_CRCNTL(soc));
	}
	if (duowen_cfab_clk_reg_base == __DUOWEN_CFAB_CLK_BASE(soc)) {
		set_fixmap_io(FIX_CFAB(soc),
			      __DUOWEN_CFAB_CLK_BASE(soc) & PAGE_MASK);
		duowen_cfab_clk_reg_base = fix_to_virt(FIX_CFAB(soc));
	}
	for (i = 0; i < 4; i++) {
		if (duowen_apc_clk_reg_base[i] ==
		    __DUOWEN_APC_CLK_BASE(soc, i)) {
			set_fixmap_io(FIX_APC0(soc) + i,
				__DUOWEN_APC_CLK_BASE(soc, i) & PAGE_MASK);
			duowen_apc_clk_reg_base[i] =
				fix_to_virt(FIX_APC0(soc) + i);
		}
	}
	if (duowen_eth_clk_reg_base == __DUOWEN_ETH_CLK_BASE(soc)) {
		set_fixmap_io(FIX_ETH(soc),
			      __DUOWEN_ETH_CLK_BASE(soc) & PAGE_MASK);
		duowen_eth_clk_reg_base = fix_to_virt(FIX_ETH(soc));
	}
	for (i = 0; i < 4; i++)
		duowen_pll_reg_base[i] = CRCNTL_PLL_REG(soc, i, 0);
	duowen_pll_reg_base[4] = COHFAB_PLL_REG(soc, 0);
	for (i = 0; i < 4; i++)
		duowen_pll_reg_base[5 + i] = CLUSTER_PLL_REG(soc, i, 0);
	duowen_pll_reg_base[9] = ETH_PLL_REG(soc, 0);
}

void duowen_mmu_dump_clk(void)
{
	__unused uint8_t soc = imc_socket_id();
	int i;

	if (duowen_clk_reg_base != __DUOWEN_CLK_BASE(soc))
		con_log("FIXMAP: %016llx -> %016llx: CRCNTL\n",
			__DUOWEN_CLK_BASE(soc), fix_to_virt(FIX_CRCNTL(soc)));
	if (duowen_cfab_clk_reg_base != __DUOWEN_CFAB_CLK_BASE(soc))
		con_log("FIXMAP: %016llx -> %016llx: CFAB\n",
			__DUOWEN_CFAB_CLK_BASE(soc),
			fix_to_virt(FIX_CFAB(soc)));
	for (i = 0; i < 4; i++) {
		if (duowen_apc_clk_reg_base[i] !=
		    __DUOWEN_APC_CLK_BASE(soc, i))
			con_log("FIXMAP: %016llx -> %016llx: APC%d\n",
				__DUOWEN_APC_CLK_BASE(soc, i),
				fix_to_virt(FIX_APC0(soc) + i), i);
	}
	if (duowen_eth_clk_reg_base != __DUOWEN_ETH_CLK_BASE(soc))
		con_log("FIXMAP: %016llx -> %016llx: ETH\n",
			__DUOWEN_ETH_CLK_BASE(soc), fix_to_virt(FIX_ETH(soc)));
}

void duowen_mmu_init_clk(void)
{
	__unused uint8_t soc = imc_socket_id();

	duowen_clk_reg_base = __DUOWEN_CLK_BASE(soc);
	duowen_cfab_clk_reg_base = __DUOWEN_CFAB_CLK_BASE(soc);
	duowen_apc_clk_reg_base[0] = __DUOWEN_APC_CLK_BASE(soc, 0);
	duowen_apc_clk_reg_base[1] = __DUOWEN_APC_CLK_BASE(soc, 1);
	duowen_apc_clk_reg_base[2] = __DUOWEN_APC_CLK_BASE(soc, 2);
	duowen_apc_clk_reg_base[3] = __DUOWEN_APC_CLK_BASE(soc, 3);
	duowen_eth_clk_reg_base = __DUOWEN_ETH_CLK_BASE(soc);
}
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#endif /* CONFIG_CRCNTL */

#ifdef CONFIG_DUOWEN_UART
caddr_t duowen_uart_reg_base;

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

#ifdef CONFIG_DUOWEN_GPIO
caddr_t duowen_gpio_reg_base[3];
#ifdef CONFIG_DUOWEN_SBI_DUAL
caddr_t duowen_tlmm_reg_base[2];

static void __duowen_mmu_map_tlmm(uint8_t soc)
{
	if (duowen_tlmm_reg_base == __DUOWEN_TLMM_BASE(soc)) {
		set_fixmap_io(FIX_TLMM(soc),
			      __DUOWEN_TLMM_BASE(soc) & PAGE_MASK);
		duowen_tlmm_reg_base(soc) = fix_to_virt(FIX_TLMM(soc));
	}
}

static void __duowen_mmu_dump_tlmm(uint8_t soc)
{
	if (duowen_tlmm_reg_base(soc) != __DUOWEN_TLMM_BASE(soc))
		con_log("FIXMAP: %016llx -> %016llx: TLMM\n",
			__DUOWEN_TLMM_BASE(soc), fix_to_virt(FIX_TLMM(soc)));
}

static void __duowen_mmu_init_tlmm(uint8_t soc)
{
	duowen_tlmm_reg_base(soc) = __DUOWEN_TLMM_BASE(soc);
}

static void duowen_mmu_map_tlmm(void)
{
	__duowen_mmu_map_tlmm(0);
	__duowen_mmu_map_tlmm(1);
}

static void duowen_mmu_dump_tlmm(void)
{
	__duowen_mmu_dump_tlmm(0);
	__duowen_mmu_dump_tlmm(1);
}

static void duowen_mmu_init_tlmm(void)
{
	__duowen_mmu_init_tlmm(0);
	__duowen_mmu_init_tlmm(1);
}
#else /* CONFIG_DUOWEN_SBI_DUAL */
caddr_t duowen_tlmm_reg_base;

static void duowen_mmu_map_tlmm(void)
{
	__unused uint8_t soc = imc_socket_id();

	if (duowen_tlmm_reg_base == __DUOWEN_TLMM_BASE(soc)) {
		set_fixmap_io(FIX_TLMM(soc),
			      __DUOWEN_TLMM_BASE(soc) & PAGE_MASK);
		duowen_tlmm_reg_base = fix_to_virt(FIX_TLMM(soc));
	}
}

static void duowen_mmu_dump_tlmm(void)
{
	__unused uint8_t soc = imc_socket_id();

	if (duowen_tlmm_reg_base != __DUOWEN_TLMM_BASE(soc))
		con_log("FIXMAP: %016llx -> %016llx: TLMM\n",
			__DUOWEN_TLMM_BASE(soc), fix_to_virt(FIX_TLMM(soc)));
}

static void duowen_mmu_init_tlmm(void)
{
	__unused uint8_t soc = imc_socket_id();

	duowen_tlmm_reg_base = __DUOWEN_TLMM_BASE(soc);
}
#endif /* CONFIG_DUOWEN_SBI_DUAL */

void duowen_mmu_map_gpio(void)
{
	__unused uint8_t soc = imc_socket_id();
	int i;

	for (i = 0; i < 3; i++) {
		if (duowen_gpio_reg_base[i] == __DUOWEN_GPIO_BASE(i)) {
			set_fixmap_io(FIX_GPIO0(soc) + i,
				      __DUOWEN_GPIO_BASE(i) & PAGE_MASK);
			duowen_gpio_reg_base[i] =
				fix_to_virt(FIX_GPIO0(soc) + i);
		}
	}
	duowen_mmu_map_tlmm();
}

void duowen_mmu_dump_gpio(void)
{
	__unused uint8_t soc = imc_socket_id();
	int i;

	for (i = 0; i < 3; i++) {
		if (duowen_gpio_reg_base[i] != __DUOWEN_GPIO_BASE(i))
			con_log("FIXMAP: %016llx -> %016llx: GPIO%d\n",
				__DUOWEN_GPIO_BASE(i),
				fix_to_virt(FIX_GPIO0(soc) + i), i);
	}
	duowen_mmu_dump_tlmm();
}
#endif

#ifdef CONFIG_DUOWEN_SCSR
caddr_t duowen_lcsr_reg_base;

static void duowen_mmu_map_lcsr(void)
{
	if (duowen_lcsr_reg_base == __DUOWEN_LCSR_BASE) {
		set_fixmap_io(FIX_LCSR, __DUOWEN_LCSR_BASE & PAGE_MASK);
		duowen_lcsr_reg_base = fix_to_virt(FIX_LCSR);
	}
}

static void duowen_mmu_dump_lcsr(void)
{
	if (duowen_lcsr_reg_base != __DUOWEN_LCSR_BASE)
		con_log("FIXMAP: %016llx -> %016llx: LCSR\n",
			__DUOWEN_LCSR_BASE, fix_to_virt(FIX_LCSR));
}

static void duowen_mmu_init_lcsr(void)
{
	duowen_lcsr_reg_base = __DUOWEN_LCSR_BASE;
}
#ifdef CONFIG_DUOWEN_SBI_DUAL
caddr_t duowen_scsr_reg_base[2];

static void __duowen_mmu_map_scsr(uint8_t soc)
{
	if (duowen_scsr_reg_base == __DUOWEN_SCSR_BASE(soc)) {
		set_fixmap_io(FIX_SCSR(soc),
			      __DUOWEN_SCSR_BASE(soc) & PAGE_MASK);
		duowen_scsr_reg_base(soc) = fix_to_virt(FIX_SCSR(soc));
	}
}

static void __duowen_mmu_dump_scsr(uint8_t soc)
{
	if (duowen_scsr_reg_base(soc) != __DUOWEN_SCSR_BASE(soc))
		con_log("FIXMAP: %016llx -> %016llx: SCSR\n",
			__DUOWEN_SCSR_BASE(soc), fix_to_virt(FIX_SCSR(soc)));
}

void duowen_mmu_map_scsr(void)
{
	__duowen_mmu_map_scsr(0);
	__duowen_mmu_map_scsr(1);
	duowen_mmu_map_lcsr();
}

void duowen_mmu_dump_scsr(void)
{
	__duowen_mmu_dump_scsr(0);
	__duowen_mmu_dump_scsr(1);
	duowen_mmu_dump_lcsr();
}

void duowen_mmu_init_scsr(void)
{
	duowen_scsr_reg_base[0] = __DUOWEN_SCSR_BASE[0];
	duowen_scsr_reg_base[1] = __DUOWEN_SCSR_BASE[1];
	duowen_mmu_init_lcsr();
}
#else /* CONFIG_DUOWEN_SBI_DUAL */
caddr_t duowen_scsr_reg_base;

void duowen_mmu_map_scsr(void)
{
	__unused uint8_t soc = imc_socket_id();

	if (duowen_scsr_reg_base == __DUOWEN_SCSR_BASE(soc)) {
		set_fixmap_io(FIX_SCSR(soc),
			      __DUOWEN_SCSR_BASE(soc) & PAGE_MASK);
		duowen_scsr_reg_base = fix_to_virt(FIX_SCSR(soc));
	}
	duowen_mmu_map_lcsr();
}

void duowen_mmu_dump_scsr(void)
{
	__unused uint8_t soc = imc_socket_id();

	if (duowen_scsr_reg_base != __DUOWEN_SCSR_BASE(soc))
		con_log("FIXMAP: %016llx -> %016llx: SCSR\n",
			__DUOWEN_SCSR_BASE(soc), fix_to_virt(FIX_SCSR(soc)));
	duowen_mmu_dump_lcsr();
}

void duowen_mmu_init_scsr(void)
{
	__unused uint8_t soc = imc_socket_id();

	duowen_scsr_reg_base = __DUOWEN_SCSR_BASE(soc);
	duowen_mmu_init_lcsr();
}
#endif /* CONFIG_DUOWEN_SBI_DUAL */
#endif /* CONFIG_DUOWEN_SCSR */

void duowen_mmu_dump_maps(void)
{
	duowen_mmu_dump_scsr();
	duowen_mmu_dump_gpio();
	duowen_mmu_dump_clk();
	duowen_mmu_dump_uart();
}

void duowen_mmu_init(void)
{
	duowen_mmu_init_clk();
#ifdef CONFIG_DUOWEN_UART
	duowen_uart_reg_base = __DUOWEN_UART_BASE;
#endif
#ifdef CONFIG_DUOWEN_GPIO
	duowen_gpio_reg_base[0] = __DUOWEN_GPIO_BASE(0);
	duowen_gpio_reg_base[1] = __DUOWEN_GPIO_BASE(1);
	duowen_gpio_reg_base[2] = __DUOWEN_GPIO_BASE(2);
	duowen_mmu_init_tlmm();
#endif
	duowen_mmu_init_scsr();
}
