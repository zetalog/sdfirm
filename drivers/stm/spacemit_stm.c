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
 * @(#)spacemit_stm.c: SpacemiT STM implementation
 * $Id: spacemit_stm.c,v 1.1 2023-11-27 16:50:00 zhenglv Exp $
 */
#include <target/console.h>
#include <target/barrier.h>
#include <target/irq.h>
#include <target/stream.h>
#include <driver/spacemit_stm.h>

static uint64_t stm_read64(uint64_t reg)
{
	return (uint64_t)__raw_readl(reg)
               | ((uint64_t)__raw_readl(reg + 4) << 32);
}

static void stm_write64(uint64_t reg, uint64_t val)
{
	__raw_writel((uint32_t)val, reg);
	__raw_writel((uint32_t)(val >> 32), reg + 4);
}

static uint32_t stm_read32(uint64_t reg)
{
	return __raw_readl(reg);
}

static void stm_write32(uint64_t reg, uint32_t val)
{
	__raw_writel(val, reg);
}

void stm_sync_2dies(void)
{
	uint64_t die0_tsp_ss, die1_tsp_ss;

	if (sysreg_die_id() != 0)
		return;

	stm_write32(STM_SS_CTRL, STM_SS_CTRL_BIT1_EN);

	while (!stm_read32(STM_INT_FLAG + DIE1_BASE) & STM_INT_FLAG_BIT0_EN);

	stm_write32(STM_INT_FLAG + DIE1_BASE, STM_INT_FLAG_BIT0_EN); //clear int flag

	die0_tsp_ss = stm_read64(STM_TSP_SS_L);
	die1_tsp_ss = stm_read64(STM_TSP_SS_L + DIE1_BASE);

	if (die1_tsp_ss > die0_tsp_ss) {
		stm_write64(STM_TSP_TRIM_L, (die1_tsp_ss - die0_tsp_ss)
				| ((uint64_t)STM_TSP_TRIM_H_BIT31_EN << 32));
	} else {
		stm_write64(STM_TSP_TRIM_L + DIE1_BASE, (die0_tsp_ss - die1_tsp_ss)
				| ((uint64_t)STM_TSP_TRIM_H_BIT31_EN << 32));
	}
}

void stm_hw_ctrl_init(void)
{
	if (sysreg_die_id() == 0) {
		stm_write32(STM_SS_CFG, STM_SS_CFG_BIT1_EN | STM_SS_CFG_BIT2_EN);
	} else if (sysreg_die_id() == 1) {
		stm_write32(STM_SS_CFG + DIE1_BASE, STM_SS_CFG_BIT2_EN);
		stm_write32(STM_INT_EN + DIE1_BASE, STM_INT_EN_BIT0_EN);
	}
}

