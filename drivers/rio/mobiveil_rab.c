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
	rab_page = page;
}

void rab_writel(uint32_t value, caddr_t addr)
{
	uint16_t pfn;
	uint16_t reg;

	if (addr < RAB_PAGE_SIZE) {
		__raw_writel(value, addr);
		return;
	}
	pfn = addr >> RAB_PAGE_SHIFT;
	reg = (addr & RAB_PAGE_MASK) + RAB_PAGE_SIZE;
	rab_page_select(pfn);
	__raw_writel(value, reg);
}

uint32_t rab_readl(caddr_t addr)
{
	uint16_t pfn;
	uint16_t pad;

	if (addr < RAB_PAGE_SIZE)
		return __raw_readl(addr);
	pfn = addr >> RAB_PAGE_SHIFT;
	pad = (addr & RAB_PAGE_MASK) + RAB_PAGE_SIZE;
	rab_page_select(pfn);
	return __raw_readl(pad);
}

/* ======================================================================
 * RAB Enumeration
 * ====================================================================== */
void rab_enum(void)
{
	rab_setl(RAB_AMBA_PIO_Enable | RAB_RIO_PIO_Enable |
		 RAB_Write_DMA_Enable | RAB_Read_DMA_Enable,
		 RAB_CTRL);
}

void rab_init_port(void)
{
	rab_enum();
	srio_init_port();
}
