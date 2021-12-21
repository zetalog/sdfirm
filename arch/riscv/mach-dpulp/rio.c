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
 * @(#)rio.c: DPU-LP specific RapidIO interconnect implementation
 * $Id: rio.c,v 1.1 2021-11-02 17:18:00 zhenglv Exp $
 */

#include <target/rio.h>
#include <target/delay.h>

/* Define DPULP_RAB_TEST to specify the original RAB test environment,
 * Undefine it to specify the DPU-LP RAB integration environment.
 */

#ifdef CONFIG_DPULP_RAB_TES
#define dpulp_rio_init_phy(n)		do { } while (0)
#else /* CONFIG_DPULP_RAB_TES */
void dpulp_rio_init_phy(int n)
{
	__raw_setw(RAB_ref_clk_en | \
		   RAB_rx_reset | RAB_tx_reset | RAB_phy_reset,
		   PHY_RESET(n));
	udelay(10);
	__raw_clearw(RAB_phy_reset, PHY_RESET(n));
	__raw_clearw(RAB_tx_reset | RAB_rx_reset, PHY_RESET(n));

	while (!(__raw_readw(SRAM_INIT_DONE(n)) & RAB_sram_init_done));
	__raw_writew(RAB_sram_ext_ld_done, SRAM_EXT_LD_DONE(n));
}
#endif /* CONFIG_DPULP_RAB_TES */

void dpulp_rio_init(int n)
{
	dpulp_rio_init_phy(n);
	dw_e16mp_tsmc12ffc_init(n, RAB_SRIO_LANES);

	/* TODO: Read lane asic_tx|rx_axic_out */
	rab_init_port();
}
