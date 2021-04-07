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
 * @(#)scsr.h: DUOWEN SysCSR register usage models
 * $Id: scsr.h,v 1.1 2020-11-09 22:46:00 zhenglv Exp $
 */

#include <target/arch.h>

void apc_set_jump_addr(caddr_t addr)
{
	cpu_t cpu;

	for (cpu = 0; cpu < MAX_APC_NUM; cpu++)
		__apc_set_jump_addr(CPU_TO_APC(cpu), addr);
}

static void __pma_cfg(int n, unsigned long attr)
{
	unsigned long cfgmask, pmacfg;
	int pmacfg_index, pmacfg_shift;
	bool tor = ((attr & PMA_A) == PMA_A_TOR);

	/* calculate PMA register and offset */
	pmacfg_index = REG64_16BIT_INDEX(tor ? n + 1 : n);
	pmacfg_shift = REG64_16BIT_OFFSET(tor ? n + 1 : n);

	cfgmask = ~(UL(0xffff) << pmacfg_shift);
	pmacfg	= imc_pma_read_cfg(pmacfg_index) & cfgmask;
	pmacfg |= ((attr << pmacfg_shift) & ~cfgmask);

	imc_pma_write_cfg(pmacfg_index, pmacfg);
}

int imc_pma_set(int n, unsigned long attr,
		phys_addr_t addr, unsigned long log2len)
{
	unsigned long addrmask, pmaaddr;
	bool tor = !IS_ALIGNED(addr, PMA_GRAIN_ALIGN) ||
		   log2len < PMA_GRAIN_SHIFT;

	/* check parameters */
	if (n >= PMA_COUNT || log2len > __riscv_xlen || log2len < PMA_SHIFT)
		return -EINVAL;

	/* encode PMA config */
	attr |= tor ? PMA_A_TOR :
		((log2len == PMA_SHIFT) ? PMA_A_NA4 : PMA_A_NAPOT);

	if (tor) {
		imc_pma_write_addr(n, addr);
		imc_pma_write_addr(n + 1, addr + (1 << log2len));
		__pma_cfg(n, attr);
		return 2;
	}

	/* encode PMA address */
	if (log2len == PMA_SHIFT) {
		pmaaddr = (addr >> PMA_SHIFT);
	} else {
		if (log2len == __riscv_xlen) {
			pmaaddr = -UL(1);
		} else {
			addrmask = (UL(1) << (log2len - PMA_SHIFT)) - 1;
			pmaaddr	 = ((addr >> PMA_SHIFT) & ~addrmask);
			pmaaddr |= (addrmask >> 1);
		}
	}

	/* write csrs */
	imc_pma_write_addr(n, pmaaddr);
	__pma_cfg(n, attr);
	return 1;
}

uint8_t apc_expand_l2_map(uint8_t map)
{
	uint8_t mask = map;

	mask = ((mask & 0x08) << 3) | ((mask & 0x04) << 2) |
	       ((mask & 0x02) << 1) | (mask & 0x01);
	mask = mask | (mask << 1);
	return mask;
}

uint16_t apc_expand_apc_map(uint8_t map)
{
	uint16_t mask = map;

	mask = ((mask & 0x80) << 7) | ((mask & 0x40) << 6) |
	       ((mask & 0x20) << 5) | ((mask & 0x10) << 4) |
	       ((mask & 0x08) << 3) | ((mask & 0x04) << 2) |
	       ((mask & 0x02) << 1) | (mask & 0x01);
	mask = mask | (mask << 1);
	return mask;
}

uint8_t apc_get_apc_map(void)
{
	uint32_t mask = apc_get_apc_mask();

	mask = ((mask & 0x08080808) >> 2) | (mask & 0x01010101);
	mask = ((mask & 0x03000300) >> 6) | (mask & 0x00030003);
	mask = ((mask & 0x000f0000) >> 12) | (mask & 0x0000000f);
	mask &= apc_expand_l2_map(apc_get_l2_map());
	return (uint8_t)mask;
}

uint16_t apc_get_cpu_map(void)
{
	uint32_t mask = apc_get_cpu_mask();

	mask = ((mask & 0x18181818) >> 1) | (mask & 0x03030303);
	mask = ((mask & 0x0f000f00) >> 4) | (mask & 0x000f000f);
	mask = ((mask & 0x00ff0000) >> 8) | (mask & 0x000000ff);
	mask &= apc_expand_apc_map(apc_get_apc_map());
	return (uint16_t)mask;
}

uint8_t apc_get_l2_map(void)
{
	uint32_t mask = apc_get_cluster_mask();

	mask = (mask & 0x00010001) | ((mask & 0x01000100) >> 7);
	mask = (mask & 0x00000003) | ((mask & 0x00030000) >> 14);
	mask &= apc_get_l2_mask();
	return (uint8_t)mask;
}

uint16_t rom_get_s0_apc_map(void)
{
	if (__raw_readl(ROM_STATUS) & ROM_S0_APC_VALID)
		return ROM_GET_S0_APC(__raw_readl(ROM_APC_MAP));
	return CPU_TO_MASK(GOOD_CPU_NUM>>1)-1;
}

uint16_t rom_get_s1_apc_map(void)
{
	if (__raw_readl(ROM_STATUS) & ROM_S1_APC_VALID)
		return ROM_GET_S1_APC(__raw_readl(ROM_APC_MAP));
	return CPU_TO_MASK(GOOD_CPU_NUM>>1)-1;
}
