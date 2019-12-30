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
 * @(#)pcc.c: RV32M1 (VEGA) PCC (peripheral clock controller) driver
 * $Id: pcc.c,v 1.1 2019-08-19 16:43:00 zhenglv Exp $
 */

#include <target/clk.h>

void pcc_select_source(caddr_t reg, uint8_t src)
{
	if (!pcc_clk_present(reg) || pcc_clk_inuse(reg))
		return;

	pcc_disable_clk(reg);
	__raw_writel_mask(PCC_PCS(src), PCC_PCS(PCC_PCS_MASK), reg);
	pcc_enable_clk(reg);
}

void pcc_config_divider(caddr_t reg,
			uint32_t input_hz, uint32_t output_hz)
{
	uint8_t try_pcd, try_frac;
	uint8_t pcd, frac;
	uint32_t quo, rem, calc_hz;
	uint32_t last_err_quo, last_err_rem;

	if (!pcc_clk_present(reg) || pcc_clk_inuse(reg))
		return;
	/* When dividing by 1 (PCD = 000), do not set the FRAC bit;
	 * otherwise, the output clock is disabled.
	 * That means: PCC is not a clock multiplexer.
	 */
	BUG_ON(output_hz > input_hz);

	pcd = frac = 0;
	last_err_quo = input_hz - output_hz;
	last_err_rem = 0;
	try_frac = 0;
again:
	for (try_pcd = 0; try_pcd < PCC_PCD_DIV_MAX; try_pcd++) {
		calc_hz = div32u(input_hz, (try_pcd + 1));
		rem = mod32u(input_hz, (try_pcd + 1));
		quo = calc_hz < output_hz ?
		      output_hz - calc_hz :
		      calc_hz - output_hz;
		if (quo < last_err_quo ||
		    (quo == last_err_quo && rem < last_err_rem)) {
			frac = try_frac;
			pcd = try_pcd;
			last_err_quo = quo;
			last_err_rem = rem;
			if (last_err_quo == 0 && last_err_rem == 0)
				goto exit_calib;
		}
	}

	if (!try_frac) {
		input_hz <<= 1;
		try_frac++;
		if (output_hz <= input_hz)
			goto again;
	}

exit_calib:
	pcc_disable_clk(reg);
	if (frac)
		__raw_setl(PCC_FRAC, reg);
	else
		__raw_clearl(PCC_FRAC, reg);
	__raw_writel_mask(PCC_PCD(pcd),
			  PCC_PCD(PCC_PCD_MASK), reg);
	pcc_enable_clk(reg);
}
