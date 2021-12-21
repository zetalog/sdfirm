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
 * @(#)dw_e16mp_tsmc12ffc.c: Synopsys 16G PHY TSMC12FFC implementation
 * $Id: dw_e16mp_tsmc12ffc.c,v 1.0 2021-12-17 16:00:00 zhenglv Exp $
 */

#include <target/rio.h>

#define dw_e16mp_lane_wait_rx_idle(n, l)		\
	while (dw_e16mp_lane_get_rx_ack(n, l))
#define dw_e16mp_lane_wait_rx_busy(n, l)		\
	while (!dw_e16mp_lane_get_rx_ack(n, l))
#define dw_e16mp_lane_wait_tx_idle(n, l)		\
	while (dw_e16mp_lane_get_tx_ack(n, l))
#define dw_e16mp_lane_wait_tx_busy(n, l)		\
	while (!dw_e16mp_lane_get_tx_ack(n, l))
#define dw_e16mp_lane_wait_idle(n, l)			\
	while (dw_e16mp_lane_get_tx_ack(n, l) ||	\
	       dw_e16mp_lane_get_rx_ack(n, l))

#define dw_e16mp_lane_cfg_rx_pstate(n, l, s)		\
	do {						\
		dw_e16mp_lane_set_rx_req(n, l);		\
		dw_e16mp_lane_set_rx_pstate(n, l, s);	\
		dw_e16mp_lane_wait_rx_busy(n, l);	\
		dw_e16mp_lane_clear_rx_req(n, l);	\
		dw_e16mp_lane_wait_rx_idle(n, l);	\
	} while (0)
#define dw_e16mp_lane_cfg_tx_pstate(n, l, s)		\
	do {						\
		dw_e16mp_lane_set_tx_req(n, l);		\
		dw_e16mp_lane_set_tx_pstate(n, l, s);	\
		dw_e16mp_lane_wait_tx_busy(n, l);	\
		dw_e16mp_lane_clear_tx_req(n, l);	\
		dw_e16mp_lane_wait_tx_idle(n, l);	\
	} while (0)

void dw_e16mp_tsmc12ffc_init(int n, int l)
{
	int i;

	for (i = 0; i < l; i++) {
		/* Step 6: complete phy init */
		dw_e16mp_lane_wait_idle(n, l);

		dw_e16mp_lane_set_rx_ovrd_in_0(n, l);
		dw_e16mp_lane_set_tx_ovrd_in_0(n, l);

		/* Step 7: update rxX_pstate */
		dw_e16mp_lane_cfg_rx_pstate(n, l, LANE_P0);

		/* Step 8: update txX_pstate */
		dw_e16mp_lane_cfg_tx_pstate(n, l, LANE_P1);
		while (!(__raw_readw(SUP_DIG_ASIC_IN(n)) & SUP_MPLLA_STATE));
		dw_e16mp_lane_set_tx_clk_rdy(n, l);
		dw_e16mp_lane_cfg_tx_pstate(n, l, LANE_P2);

		/* Step 9: rxX_data_en, txX_data_en */
		dw_e16mp_lane_set_rx_data_en(n, l);
		dw_e16mp_lane_set_tx_data_en(n, l);
	}
}
