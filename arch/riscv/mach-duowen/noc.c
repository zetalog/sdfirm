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
 * @(#)noc.c: DUOWEN specific NoC initialization
 * $Id: noc.c,v 1.1 2020-08-25 18:03:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/noc.h>
#include <target/clk.h>
#include <target/console.h>

#ifdef CONFIG_DUOWEN_NOC_DEBUG
static void duowen_noc_debug(void)
{
	console_init();
	con_dbg("noc: configuring socket%d %s%d clusters: %02x\n",
		imc_socket_id(), imc_chip_link() ? "chiplink " : "",
		rom_get_cluster_num(), rom_get_cluster_map());
}
#else /* CONFIG_DUOWEN_NOC_DEBUG */
#define duowen_noc_debug()		do { } while (0)
#endif /* CONFIG_DUOWEN_NOC_DEBUG */

#ifdef CONFIG_DUOWEN_NOC_INIT_CLOCKS
static void duowen_noc_init_clocks(void)
{
	/* NoC connects to fabrics and DDR/PCIes. So their clocks must
	 * be enabled before configuring NoC.
	 */
	board_init_clock();
	/* Ensured required clocks */
	clk_enable(ddr_aclk);
	clk_enable(pcie_aclk);
}
#else /* CONFIG_DUOWEN_NOC_INIT_CLOCKS */
#define duowen_noc_init_clocks()	do { } while (0)
#endif /* CONFIG_DUOWEN_NOC_INIT_CLOCKS */

void duowen_noc_init(void)
{
	if (!rom_get_noc_configured()) {
		duowen_noc_init_clocks();
		duowen_noc_debug();
		crcntl_pcie_ramp(1);
		ncore_init(rom_get_cluster_num(), rom_get_cluster_map(),
			   0, MAX_DDR_SEGMENTS, MAX_DDR_SEGMENTS);
		crcntl_pcie_ramp(15);
		rom_set_noc_configured();
	}
}
