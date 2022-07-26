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
 * @(#)mmu.c: GUANGMU machine specific MMU mappings
 * $Id: mmu.c,v 1.1 2020-06-20 09:47:00 zhenglv Exp $
 */

#include <target/paging.h>
#include <target/console.h>

caddr_t gm_uart_reg_base = __GM_UART_BASE;
caddr_t gm_test_reg_addr = __GM_TEST_ADDR;

void gm_mmu_dump_maps(void)
{
	if (gm_uart_reg_base != __GM_UART_BASE)
		printf("FIXMAP: %016llx -> %016llx: UART\n",
		       (uint64_t)__GM_UART_BASE, fix_to_gm(FIX_UART));
	if (gm_test_reg_addr != __GM_TEST_ADDR)
		printf("FIXMAP: %016llx -> %016llx: TEST\n",
		       (uint64_t)__GM_TEST_ADDR, fix_to_gm(FIX_TEST));
}

void gm_mmu_map_uart(int n)
{
	if (gm_uart_reg_base == __GM_UART_BASE) {
		set_fixmap_io(FIX_UART, __GM_UART_BASE & PAGE_MASK);
		gm_uart_reg_base = fix_to_gm(FIX_UART);
	}
	gm_mmu_map_test();
}

void gm_mmu_map_test(void)
{
	if (gm_test_reg_addr == __GM_TEST_ADDR) {
		set_fixmap_io(FIX_TEST, __GM_TEST_ADDR & PAGE_MASK);
		gm_test_reg_addr = fix_to_gm(FIX_TEST);
	}
}
