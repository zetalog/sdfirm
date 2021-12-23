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
 * @(#)mobiveil_rab.c: MobiVeil RapidIO to AXI Bridge (RAB) implementation
 * $Id: mobiveil_rab.c,v 1.0 2021-11-02 17:05:00 zhenglv Exp $
 */

#include <target/rio.h>
#include <target/barrier.h>

#define RIO_PTR2INT(addr)	((uintptr_t)(addr))

/* ======================================================================
 * RAB Config Access
 * ====================================================================== */
static uint16_t rab_page = 0;

void rab_page_select(uint16_t page)
{
	if (rab_page == page)
		return;

	__raw_writel_mask(RAB_APBPageSelect(page),
			  RAB_APBPageSelect(RAB_APBPageSelect_MASK),
			  RAB_APB_CSR);
	wmb();
	rab_page = page;
}

void rab_writel(uint32_t value, caddr_t addr)
{
	uint16_t pfn;
	caddr_t reg;

	if (addr < RAB_PAGE_SIZE) {
		__raw_writel(value, addr);
		return;
	}
	pfn = RIO_PTR2INT(addr) >> RAB_PAGE_SHIFT;
	reg = RAB_ACCESS(addr);
	rab_page_select(pfn);
	__raw_writel(value, reg);
}

uint32_t rab_readl(caddr_t addr)
{
	uint16_t pfn;
	caddr_t reg;

	if (addr < RAB_PAGE_SIZE)
		return __raw_readl(addr);
	pfn = RIO_PTR2INT(addr) >> RAB_PAGE_SHIFT;
	reg = RAB_ACCESS(addr);
	rab_page_select(pfn);
	return __raw_readl(reg);
}

/* ======================================================================
 * RAB Enumeration
 * ====================================================================== */
static void rab_config_axi_amap(int win, uint8_t type, uint8_t prio,
				caddr_t axi_base, uint64_t axi_size,
				uint64_t rio_base)
{
	rab_writel(RAB_AXI_AMAP_Enable |
		   RAB_AXI_AMAP_Type(type) | RAB_AXI_AMAP_Priority(prio),
		   RAB_AXI_AMAP_CTRL(win));
	rab_writel(RAB_AXI_AMAP_MASK(axi_size), RAB_AXI_AMAP_SIZE(win));
	rab_writel(RAB_AXI_AMAP_BASE(axi_base), RAB_AXI_AMAP_ABAR(win));
	rab_writel(RAB_AXI_AMAP_BASE(rio_base), RAB_AXI_AMAP_RBAR(win));
}

void rab_enable_axi_amap(int apio, int win, uint8_t type, uint8_t prio,
			 caddr_t axi_base, uint64_t axi_size,
			 uint64_t rio_base)
{
	rab_writel(RAB_APIO_Enable | RAB_APIO_MemoryMappingEnable |
		   RAB_APIO_CCPEnable, RAB_APIO_CTRL(apio));

	while (axi_size > RAB_AXI_AMAP_SIZE_MAX) {
		rab_config_axi_amap(win, type, prio, axi_base,
				    RAB_AXI_AMAP_SIZE_MAX, rio_base);
		axi_base += RAB_AXI_AMAP_SIZE_MAX;
		rio_base += RAB_AXI_AMAP_SIZE_MAX;
		axi_size -= RAB_AXI_AMAP_SIZE_MAX;
		win++;
	}
	if (axi_size)
		rab_config_axi_amap(win, type, prio, axi_base,
				    axi_size, rio_base);

#ifdef CONFIG_RAB_RIO_50BIT
	rab_writel(RAB_AXI_AMAP_UPPER(rio_base),
		   RAB_APIO_RIO_UPPER16_ADDR(apio));
#endif /* CONFIG_RAB_RIO_50BIT */
}

/* apio_maint_read_trans */
void rab_axi_testcase(void)
{
}

void rab_init_port(void)
{
	rab_setl(RAB_AXIPIOEnable | RAB_RIOPIOEnable |
		 RAB_WriteDMAEnable | RAB_ReadDMAEnable, RAB_CTRL);
	srio_init_port();
}
