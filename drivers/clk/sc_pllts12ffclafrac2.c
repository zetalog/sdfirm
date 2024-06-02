
/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2024
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
 * @(#)sc_pllts12ffclafrac2.c: Silicon creation PLL TSMS12FFC implementation
 * $Id: sc_pllts12ffclafrac2.c,v 1.1 2024-05-31 17:37:00 zhenglv Exp $
 */

#include <target/clk.h>
#include <target/panic.h>

void __sc_pllts12ffclafrac2_enable(int n, bool fractional, bool out_4phase,
				   uint16_t fbdiv_min, uint16_t fbdiv_max,
				   uint32_t Fpdf_min,
				   uint32_t Fref, uint32_t Fvco,
				   uint8_t *p_frefdiv, uint16_t *p_fbdiv,
				   uint32_t *p_frac)
{
	uint32_t Fpfd;
	uint8_t frefdiv;
	uint32_t fbdiv;
	uint32_t frac;

	uint8_t bst_frefdiv = 1;
	uint32_t bst_fbdiv = 1;
	uint32_t bst_frac = Fvco;
	bool bst_saved;

#ifdef CONFIG_SC_PLLTS12FFCLAFRAC2_PRESET
	if (pll_hw_fvco[n] == Fvco) {
		bst_fbdiv = pll_hw_fbdiv[n];
		bst_frefdiv = pll_hw_frefdiv[n];
		bst_frac = pll_hw_frac[n];
		goto bst_exit;
	}
#endif
	bst_saved = false;
	frefdiv = 0;
	do {
		do {
			frefdiv++;
			if (frefdiv == 64) {
				BUG_ON(!bst_saved);
				goto bst_exit;
			}
			Fpfd = Fref / frefdiv;
			if (Fpfd < Fpdf_min) {
				BUG_ON(!bst_saved);
				goto bst_exit;
			}
		} while (Fpfd > (Fvco / 16));
		fbdiv = Fvco / Fpfd;
		if (fbdiv < fbdiv_min) {
			frefdiv++;
			continue;
		}
		if (fbdiv > fbdiv_max)
			continue;
		frac = Fvco - (Fpfd * fbdiv);
		if (frac < bst_frac) {
			bst_saved = true;
			bst_frefdiv = frefdiv;
			bst_fbdiv = fbdiv;
			bst_frac = frac;
		}
		if (frac == 0)
			goto bst_exit;
	} while (true);
bst_exit:
	BUG_ON(fractional && (bst_frac >= (UL(1) << 24)));
	frefdiv = bst_frefdiv;
	fbdiv = bst_fbdiv;
	frac = bst_frac;
	con_log("pll(%d): Fref = %d, Fvco=%d, FREFDIV[5:0]=%d, FBDIV[11:0]=%d, FRAC[23:0]=%d\n",
	        n, Fref, Fvco, frefdiv, fbdiv, fractional ? frac : 0);
	if (p_frefdiv)
		*p_frefdiv = frefdiv;
	if (p_fbdiv)
		*p_fbdiv = (uint16_t)fbdiv;
	if (p_frac)
		*p_frac = frac;
}

#ifdef CONFIG_SC_PLLTS12FFCLAFRAC2_FRAC
/* Configure a PLL in Fractional Mode
 * ---------------------------------------------------------------------------
 *          FREF    FBDIV + (FRAC/(2^24))
 * FOUT* = ------ x ---------------------
 *         REFDIV     POSTDIV1*POSTDIV2
 * ---------------------------------------------------------------------------
 * Fpfd        = Fref / REFDIV[1~63] = 10MHz~Fvco/20
 * Fvco        = Fpfd * FBDIV[20~320]  = 800MHz ~ 3200MHz
 * Foutpostdiv = Fout / POSDIV12[1~49]
 * Fout4phase  = Foutpostdiv / 2
 * FRAC        = Fvco - (Fvco / FBDIV), lower 24-bits
 * ---------------------------------------------------------------------------
 * Programming restrictions:
 * 1. Maximize POSTDIV1 prior than enabling POSTDIV2.
 */
void sc_pllts12ffclafrac2_enable(int n, bool out_4phase,
				 uint32_t Fref, uint32_t Fvco)
{
	uint8_t frefdiv;
	uint16_t fbdiv;
	uint32_t frac;

	__sc_pllts12ffclafrac2_enable(n, true, out_4phase, 20, 320, 10000000,
				      Fref, Fvco, &frefdiv, &fbdiv, &frac);
}
#else
/* Configure a PLL in Integer Mode
 * ---------------------------------------------------------------------------
 *              FREF*FBDIV
 * FOUT* = ------------------------
 *         REFDIV*POSTDIV1*POSTDIV2
 * ---------------------------------------------------------------------------
 * Fpfd        = Fref / REFDIV[1~63] = 5MHz~Fvco/16
 * Fvco        = Fpfd * FBDIV[16~640] = 800MHz ~ 3200MHz (640 or 3200?)
 * Foutpostdiv = Fout / POSDIV12[1~49]
 * Fout4phase  = Foutpostdiv / 2
 * ---------------------------------------------------------------------------
 * Programming restrictions:
 * 1. FBDIV should be valid when PLL is enabled.
 * 2. Maximize POSTDIV1 prior than enabling POSTDIV2.
 */
void sc_pllts12ffclafrac2_enable(int n, bool out_4phase,
				 uint32_t Fref, uint32_t Fvco)
{
	uint8_t frefdiv;
	uint16_t fbdiv;

	__sc_pllts12ffclafrac2_enable(n, false, out_4phase, 16, 640, 5000000,
				      Fref, Fvco, &frefdiv, &fbdiv, NULL);
}
#endif

void sc_pllts12ffclafrac2_disable(int n)
{
}
