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
 * @(#)mmu.c: DPU machine specific MMU mappings
 * $Id: mmu.c,v 1.1 2020-07-01 16:39:00 zhenglv Exp $
 */

#include <target/paging.h>
#include <target/console.h>

caddr_t dpu_uart_reg_base = __DPU_UART_BASE;
caddr_t dpu_gpio_reg_base = GPIO_BASE;
caddr_t dpu_pll_reg_base = PLL_REG_BASE;
caddr_t dpu_tcsr_reg_base = TCSR_BASE;

caddr_t pe_dma0_reg_base = PE_DMA0_BASE;
caddr_t ddr0_ctrl_reg_base = DDR0_CTRL_BASE;
caddr_t pciex_subsys_cust_reg_base = PCIEx_SUBSYS_CUST_BASE;

void dpu_mmu_dump_maps(void)
{
	if (dpu_pll_reg_base != PLL_REG_BASE)
		printf("FIXMAP: %016llx -> %016llx: PLL\n",
		       (uint64_t)PLL_REG_BASE, fix_to_virt(FIX_PLL));
	if (dpu_gpio_reg_base != GPIO_BASE)
		printf("FIXMAP: %016llx -> %016llx: GPIO\n",
		       (uint64_t)GPIO_BASE, fix_to_virt(FIX_GPIO));
	if (dpu_uart_reg_base != __DPU_UART_BASE)
		printf("FIXMAP: %016llx -> %016llx: UART\n",
		       (uint64_t)__DPU_UART_BASE, fix_to_virt(FIX_UART));
	if (dpu_tcsr_reg_base != TCSR_BASE)
		printf("FIXMAP: %016llx -> %016llx: TCSR\n",
		       (uint64_t)TCSR_BASE, fix_to_virt(FIX_TCSR));
}

void dpu_mmu_map_periph(void)
{
	/* TODO: The driver side should utilize the mapped register
	 *       bases.
	 */
	if (ddr0_ctrl_reg_base == DDR0_CTRL_BASE) {
		set_fixmap_io(FIX_DDR0_CTRL, DDR0_CTRL_BASE & PAGE_MASK);
		ddr0_ctrl_reg_base = fix_to_virt(FIX_DDR0_CTRL);
	}
	if (ddr0_ctrl_reg_base != DDR0_CTRL_BASE)
		printf("FIXMAP: %016llx -> %016llx: DDR0_CTRL\n",
		       (uint64_t)DDR0_CTRL_BASE,
		       fix_to_virt(FIX_DDR0_CTRL));

	if (pe_dma0_reg_base == PE_DMA0_BASE) {
		set_fixmap_io(FIX_PE_DMA0, PE_DMA0_BASE & PAGE_MASK);
		pe_dma0_reg_base = fix_to_virt(FIX_PE_DMA0);
	}
	if (pe_dma0_reg_base != PE_DMA0_BASE)
		printf("FIXMAP: %016llx -> %016llx: PE_DMA0\n",
		       (uint64_t)PE_DMA0_BASE, fix_to_virt(FIX_PE_DMA0));

	if (pciex_subsys_cust_reg_base == PCIEx_SUBSYS_CUST_BASE) {
		set_fixmap_io(FIX_PCIEx_SUBSYS_CUST,
			      PCIEx_SUBSYS_CUST_BASE & PAGE_MASK);
		pciex_subsys_cust_reg_base =
			fix_to_virt(FIX_PCIEx_SUBSYS_CUST);
	}
	if (pciex_subsys_cust_reg_base != PCIEx_SUBSYS_CUST_BASE)
		printf("FIXMAP: %016llx -> %016llx: PCIEx_SUBSYS_CUST\n",
		       (uint64_t)PCIEx_SUBSYS_CUST_BASE,
		       fix_to_virt(FIX_PCIEx_SUBSYS_CUST));
}

void dpu_mmu_map_pll(void)
{
	if (dpu_pll_reg_base == PLL_REG_BASE) {
		set_fixmap_io(FIX_PLL, PLL_REG_BASE & PAGE_MASK);
		dpu_pll_reg_base = fix_to_virt(FIX_PLL);
	}
	if (dpu_tcsr_reg_base == TCSR_BASE) {
		set_fixmap_io(FIX_TCSR, TCSR_BASE & PAGE_MASK);
		dpu_tcsr_reg_base = fix_to_virt(FIX_TCSR);
	}
}

void dpu_mmu_map_gpio(void)
{
	if (dpu_gpio_reg_base == GPIO_BASE) {
		set_fixmap_io(FIX_GPIO, GPIO_BASE & PAGE_MASK);
		dpu_gpio_reg_base = fix_to_virt(FIX_GPIO);
	}
}

void dpu_mmu_map_uart(int n)
{
	if (dpu_uart_reg_base == __DPU_UART_BASE) {
		set_fixmap_io(FIX_UART, __DPU_UART_BASE & PAGE_MASK);
		dpu_uart_reg_base = fix_to_virt(FIX_UART);
	}
	dpu_mmu_map_periph();
}
