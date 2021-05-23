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
 * @(#)crcntl.c: DUOWEN clock/reset controller (CRCNTL) implementation
 * $Id: crcntl.c,v 1.1 2019-11-06 10:39:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/panic.h>

/*===========================================================================
 * DUOWEN unitified PLL (crcntl, cohfab, cluster) APIs
 *===========================================================================*/
phys_addr_t duowen_pll_reg_base[DUOWEN_MAX_PLLS];

#ifdef CONFIG_DUOWEN_BBL_DUAL
void duowen_pll_init(void)
{
	duowen_pll_reg_base[0] = __CRCNTL_PLL_REG(0, 0, 0);
	duowen_pll_reg_base[1] = __CRCNTL_PLL_REG(0, 1, 0);
	duowen_pll_reg_base[2] = __CRCNTL_PLL_REG(0, 2, 0);
	duowen_pll_reg_base[3] = __CRCNTL_PLL_REG(0, 3, 0);
	duowen_pll_reg_base[4] = __DUOWEN_CFAB_CLK_BASE(0);
	duowen_pll_reg_base[5] = __DUOWEN_APC_CLK_BASE(0, 0);
	duowen_pll_reg_base[6] = __DUOWEN_APC_CLK_BASE(0, 1);
	duowen_pll_reg_base[7] = __DUOWEN_APC_CLK_BASE(0, 2);
	duowen_pll_reg_base[8] = __DUOWEN_APC_CLK_BASE(0, 3);
	duowen_pll_reg_base[9] = __DUOWEN_ETH_CLK_BASE(0);
	duowen_pll_reg_base[10] = __CRCNTL_PLL_REG(1, 0, 0);
	duowen_pll_reg_base[11] = __CRCNTL_PLL_REG(1, 1, 0);
	duowen_pll_reg_base[12] = __CRCNTL_PLL_REG(1, 2, 0);
	duowen_pll_reg_base[13] = __CRCNTL_PLL_REG(1, 3, 0);
	duowen_pll_reg_base[14] = __DUOWEN_CFAB_CLK_BASE(1);
	duowen_pll_reg_base[15] = __DUOWEN_APC_CLK_BASE(1, 0);
	duowen_pll_reg_base[16] = __DUOWEN_APC_CLK_BASE(1, 1);
	duowen_pll_reg_base[17] = __DUOWEN_APC_CLK_BASE(1, 2);
	duowen_pll_reg_base[18] = __DUOWEN_APC_CLK_BASE(1, 3);
	duowen_pll_reg_base[19] = __DUOWEN_ETH_CLK_BASE(1);
}
#else /* CONFIG_DUOWEN_BBL_DUAL */
void duowen_pll_init(void)
{
	/* Adaptive base addresses for both socket0 and socket1 */
	duowen_pll_reg_base[0] = __CRCNTL_PLL_REG(imc_socket_id(), 0, 0);
	duowen_pll_reg_base[1] = __CRCNTL_PLL_REG(imc_socket_id(), 1, 0);
	duowen_pll_reg_base[2] = __CRCNTL_PLL_REG(imc_socket_id(), 2, 0);
	duowen_pll_reg_base[3] = __CRCNTL_PLL_REG(imc_socket_id(), 3, 0);
	duowen_pll_reg_base[4] = __DUOWEN_CFAB_CLK_BASE(imc_socket_id());
	duowen_pll_reg_base[5] = __DUOWEN_APC_CLK_BASE(imc_socket_id(), 0);
	duowen_pll_reg_base[6] = __DUOWEN_APC_CLK_BASE(imc_socket_id(), 1);
	duowen_pll_reg_base[7] = __DUOWEN_APC_CLK_BASE(imc_socket_id(), 2);
	duowen_pll_reg_base[8] = __DUOWEN_APC_CLK_BASE(imc_socket_id(), 3);
	duowen_pll_reg_base[9] = __DUOWEN_ETH_CLK_BASE(imc_socket_id());
}
#endif /* CONFIG_DUOWEN_BBL_DUAL */

void duowen_pll_reg_write(uint8_t pll, uint8_t reg, uint8_t val)
{
	__raw_writel(PLL_REG_WRITE | PLL_REG_SEL(reg) | PLL_REG_WDATA(val),
		     DW_PLL_REG_ACCESS(pll));
	while (!(__raw_readl(DW_PLL_REG_ACCESS(pll)) & PLL_REG_IDLE));
}

uint8_t duowen_pll_reg_read(uint8_t pll, uint8_t reg)
{
	uint32_t val;

	__raw_writel(PLL_REG_READ | PLL_REG_SEL(reg),
		     DW_PLL_REG_ACCESS(pll));
	do {
		val = __raw_readl(DW_PLL_REG_ACCESS(pll));
	} while (!(val & PLL_REG_IDLE));
	return PLL_REG_RDATA(val);
}

#define pll_cnt_elapsed(o, n)	\
	((uint16_t)((n>=o) ? (n-o) : (PLL_CNT_COUNTER_MASK-o+n+1)))

void duowen_pll_wait_cmpclk(uint8_t pll, uint16_t cycles)
{
	uint16_t cnt;

	/* Also reset counter to 0 */
	__raw_writel(PLL_CNT_EN, DW_PLL_CNT(pll));
	do {
		cnt = pll_cnt_counter(__raw_readl(DW_PLL_CNT(pll)));
	} while (pll_cnt_elapsed(0, cnt) < cycles);
	__raw_writel(0, DW_PLL_CNT(pll));
}

bool duowen_pll_wait_timing(uint8_t pll, uint8_t timing)
{
	return !!(__raw_readl(DW_PLL_HW_CFG(pll)) & PLL_WAIT_T(timing));
}

/*===========================================================================
 * CRCNTL SYSFAB APIs
 *===========================================================================*/
bool __crcntl_clk_asserted(clk_clk_t clk, uint8_t soc)
{
	uint8_t n, bit;

	bit = clk & REG_5BIT_MASK;
	n = clk >> 5;
	return !!(__raw_readl(CRCNTL_SW_RST_CFG(soc, n)) & _BV(bit));
}

#ifdef CONFIG_CRCNTL_RESET
void __crcntl_clk_assert(clk_clk_t clk, uint8_t soc)
{
	uint8_t n, bit;

	if (!__crcntl_clk_asserted(clk, soc)) {
		bit = clk & REG_5BIT_MASK;
		n = clk >> 5;
		__raw_setl(_BV(bit), CRCNTL_SW_RST_CFG(soc, n));
	}
}
#else
void __crcntl_clk_assert(clk_clk_t clk, uint8_t soc)
{
}
#endif

void __crcntl_clk_deassert(clk_clk_t clk, uint8_t soc)
{
	uint8_t n, bit;

	if (__crcntl_clk_asserted(clk, soc)) {
		bit = clk & REG_5BIT_MASK;
		n = clk >> 5;
		__raw_clearl(_BV(bit), CRCNTL_SW_RST_CFG(soc, n));
	}
}

bool __crcntl_clk_enabled(clk_clk_t clk, uint8_t soc)
{
	uint8_t n, bit;

	bit = clk & REG_5BIT_MASK;
	n = clk >> 5;
	return !!(__raw_readl(CRCNTL_CLK_EN_CFG(soc, n)) & _BV(bit));
}

void __crcntl_clk_enable(clk_clk_t clk, uint8_t soc)
{
	uint8_t n, bit;

	if (!__crcntl_clk_enabled(clk, soc)) {
		bit = clk & REG_5BIT_MASK;
		n = clk >> 5;
		__raw_setl(_BV(bit), CRCNTL_CLK_EN_CFG(soc, n));
	}
}

void __crcntl_clk_disable(clk_clk_t clk, uint8_t soc)
{
	uint8_t n, bit;

	if (__crcntl_clk_enabled(clk, soc)) {
		bit = clk & REG_5BIT_MASK;
		n = clk >> 5;
		__raw_clearl(_BV(bit), CRCNTL_CLK_EN_CFG(soc, n));
	}
}

/*===========================================================================
 * CRCNTL COHFAB APIs
 *===========================================================================*/
bool __cohfab_clk_asserted(clk_clk_t clk, uint8_t soc)
{
	clk_clk_t pll = COHFAB_CLK_PLL(clk);

	if (pll == COHFAB_PLL)
		return !!(__raw_readl(COHFAB_RESET_COHFAB(soc, pll)) &
			  COHFAB_RESET);
	return !!(__raw_readl(COHFAB_RESET_CLUSTER(soc, pll)) &
		  CLUSTER_POR_RST);
}

void __cohfab_clk_assert(clk_clk_t clk, uint8_t soc)
{
	clk_clk_t pll = COHFAB_CLK_PLL(clk);

	if (__cohfab_clk_asserted(clk, soc))
		return;
	if (pll == COHFAB_PLL)
		__raw_setl(COHFAB_RESET, COHFAB_RESET_COHFAB(soc, pll));
	else
		__raw_setl(CLUSTER_POR_RST, COHFAB_RESET_CLUSTER(soc, pll));
}

void __cohfab_clk_deassert(clk_clk_t clk, uint8_t soc)
{
	clk_clk_t pll = COHFAB_CLK_PLL(clk);

	if (!__cohfab_clk_asserted(clk, soc))
		return;
	if (pll == COHFAB_PLL)
		__raw_clearl(COHFAB_RESET, COHFAB_RESET_COHFAB(soc, pll));
	else
		__raw_clearl(CLUSTER_POR_RST, COHFAB_RESET_CLUSTER(soc, pll));
}

bool __cohfab_clk_enabled(clk_clk_t clk, uint8_t soc)
{
	clk_clk_t pll = COHFAB_CLK_PLL(clk);

	return !!(__raw_readl(COHFAB_CLK_CFG(soc, pll)) & COHFAB_CLOCK_ON);
}

void __cohfab_clk_enable(clk_clk_t clk, uint8_t soc)
{
	clk_clk_t pll = COHFAB_CLK_PLL(clk);

	if (__cohfab_clk_enabled(clk, soc))
		return;
	__raw_setl(COHFAB_CLOCK_ON, COHFAB_CLK_CFG(soc, pll));
}

void __cohfab_clk_disable(clk_clk_t clk, uint8_t soc)
{
	clk_clk_t pll = COHFAB_CLK_PLL(clk);

	if (!__cohfab_clk_enabled(clk, soc))
		return;
	__raw_clearl(COHFAB_CLOCK_ON, COHFAB_CLK_CFG(soc, pll));
}

/*===========================================================================
 * CRCNTL CLUSTER APIs
 *===========================================================================*/
bool __cluster_clk_asserted(clk_clk_t clk, uint8_t soc)
{
	uint8_t apc = CLUSTER_CLK_APC(clk);
	uint8_t rst = CLUSTER_RESET(CLUSTER_CLK_CG(clk));

	return !!(__raw_readl(CLUSTER_RESET_CTRL(soc, apc)) & _BV(rst));
}

void __cluster_clk_assert(clk_clk_t clk, uint8_t soc)
{
	uint8_t apc = CLUSTER_CLK_APC(clk);
	uint8_t rst = CLUSTER_RESET(CLUSTER_CLK_CG(clk));

	if (__cluster_clk_asserted(clk, soc))
		return;
	__raw_setl(_BV(rst), CLUSTER_RESET_CTRL(soc, apc));
}

void __cluster_clk_deassert(clk_clk_t clk, uint8_t soc)
{
	uint8_t apc = CLUSTER_CLK_APC(clk);
	uint8_t rst = CLUSTER_RESET(CLUSTER_CLK_CG(clk));

	if (!__cluster_clk_asserted(clk, soc))
		return;
	__raw_clearl(_BV(rst), CLUSTER_RESET_CTRL(soc, apc));
}

bool __cluster_clk_enabled(clk_clk_t clk, uint8_t soc)
{
	uint8_t apc = CLUSTER_CLK_APC(clk);
	uint8_t cg = CLUSTER_CLK_CG(clk);

	return !!(__raw_readl(CLUSTER_CLK_CG_CFG(soc, apc)) & _BV(cg));
}

void __cluster_clk_enable(clk_clk_t clk, uint8_t soc)
{
	uint8_t apc = CLUSTER_CLK_APC(clk);
	uint8_t cg = CLUSTER_CLK_CG(clk);

	if (__cluster_clk_enabled(clk, soc))
		return;
	__raw_setl(_BV(cg), CLUSTER_CLK_CG_CFG(soc, apc));
}

void __cluster_clk_disable(clk_clk_t clk, uint8_t soc)
{
	uint8_t apc = CLUSTER_CLK_APC(clk);
	uint8_t cg = CLUSTER_CLK_CG(clk);

	if (!__cluster_clk_enabled(clk, soc))
		return;
	__raw_clearl(_BV(cg), CLUSTER_CLK_CG_CFG(soc, apc));
}

/*===========================================================================
 * SYSFAB/COHFAB MUX APIs
 *===========================================================================*/
#ifdef CONFIG_CRCNTL_MUX
bool __crcntl_clk_selected(clk_clk_t clk, uint8_t soc)
{
	return !(__raw_readl(CRCNTL_CLK_SEL_CFG(soc)) & _BV(clk));
}

void __crcntl_clk_select(clk_clk_t clk, uint8_t soc)
{
	if (!__crcntl_clk_selected(clk, soc))
		__raw_clearl(_BV(clk), CRCNTL_CLK_SEL_CFG(soc));
}

void __crcntl_clk_deselect(clk_clk_t clk, uint8_t soc)
{
	if (__crcntl_clk_selected(clk, soc))
		__raw_setl(_BV(clk), CRCNTL_CLK_SEL_CFG(soc));
}

bool __cohfab_clk_selected(clk_clk_t clk, uint8_t soc)
{
	clk_clk_t pll = COHFAB_CLK_SEL_PLL(clk);

	return !(__raw_readl(COHFAB_CLK_CFG(soc, pll)) & COHFAB_CLOCK_SEL);
}

void __cohfab_clk_select(clk_clk_t clk, uint8_t soc)
{
	clk_clk_t pll = COHFAB_CLK_SEL_PLL(clk);

	if (__cohfab_clk_selected(clk, soc))
		return;
	__raw_clearl(COHFAB_CLOCK_SEL, COHFAB_CLK_CFG(soc, pll));
}

void __cohfab_clk_deselect(clk_clk_t clk, uint8_t soc)
{
	clk_clk_t pll = COHFAB_CLK_SEL_PLL(clk);

	if (!__cohfab_clk_selected(clk, soc))
		return;
	__raw_setl(COHFAB_CLOCK_SEL, COHFAB_CLK_CFG(soc, pll));
}

uint32_t __crcntl_clk_sel_read(uint8_t soc)
{
	return __raw_readl(CRCNTL_CLK_SEL_CFG(soc));
}

void __crcntl_clk_sel_write(uint32_t sels, uint8_t soc)
{
	__raw_writel(sels, CRCNTL_CLK_SEL_CFG(soc));
}
#endif

/*===========================================================================
 * CRCNTL TRACE APIs
 *===========================================================================*/
#ifdef CONFIG_CRCNTL_TRACE
void crcntl_trace(bool enabling, const char *name)
{
	con_dbg("crcntl: %c %s\n", enabling ? 'E' : 'D', name);
}
#endif

/*===========================================================================
 * CRCNTL generic APIs
 *===========================================================================*/
#ifdef CONFIG_MMU
void clk_hw_mmu_init(void)
{
	duowen_mmu_map_scsr();
	duowen_mmu_map_clk();
}
#endif
