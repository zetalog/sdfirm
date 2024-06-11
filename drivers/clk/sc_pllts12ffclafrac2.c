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

/* Calculate postdiv1/postdiv2 using predefined Fvco/Fout */
static void __sc_pllts12ffclafrac2_divide(int n, uint32_t Fvco, uint32_t Fout,
					  uint8_t *p_postdiv1,
					  uint8_t *p_postdiv2)
{
	uint8_t div = Fvco / Fout;
	uint8_t postdiv1, postdiv2;

	if (Fvco == Fout) {
		postdiv1 = 1;
		postdiv2 = 1;
		goto bst_exit;
	}
#ifdef CONFIG_SC_PLLTS12FFCLAFRAC2_PRESET
	postdiv1 = pll_hw_postdiv1[n];
	postdiv2 = pll_hw_postdiv2[n];
	if ((postdiv1 * postdiv2) == div)
		goto bst_exit;
#endif
	for (postdiv1 = 1; postdiv1 < 8; postdiv1++) {
		for (postdiv2 = 1; postdiv2 < postdiv1; postdiv2++) {
			if ((postdiv1 * postdiv2) == div)
				goto bst_exit;
		}
	}
	BUG();
bst_exit:
	if (p_postdiv1)
		*p_postdiv1 = postdiv1;
	if (p_postdiv2)
		*p_postdiv2 = postdiv2;
}

/* Calculate refdiv/fbdiv/frac using predefined Fvco */
static void __sc_pllts12ffclafrac2_feedback(int n, bool fractional, bool out_4phase,
					    uint16_t fbdiv_min, uint16_t fbdiv_max,
					    uint32_t Fpdf_min,
					    uint32_t Fref, uint32_t Fvco,
					    uint8_t *p_refdiv, uint16_t *p_fbdiv,
					    uint32_t *p_frac)
{
	uint32_t Fpfd;
	uint8_t refdiv;
	uint32_t fbdiv;
	uint32_t frac;

	uint8_t bst_refdiv = 1;
	uint32_t bst_fbdiv = 1;
	uint32_t bst_frac = Fvco;
	bool bst_saved;

#ifdef CONFIG_SC_PLLTS12FFCLAFRAC2_PRESET
	if (pll_hw_fvco[n] == Fvco) {
		bst_fbdiv = pll_hw_fbdiv[n];
		bst_refdiv = pll_hw_refdiv[n];
		bst_frac = pll_hw_frac[n];
		goto bst_exit;
	}
#endif
	bst_saved = false;
	refdiv = 0;
	do {
		do {
			refdiv++;
			if (refdiv == 64) {
				BUG_ON(!bst_saved);
				goto bst_exit;
			}
			Fpfd = Fref / refdiv;
			if (Fpfd < Fpdf_min) {
				BUG_ON(!bst_saved);
				goto bst_exit;
			}
		} while (Fpfd > (Fvco / fbdiv_min));
		fbdiv = Fvco / Fpfd;
		if (fbdiv < fbdiv_min) {
			refdiv++;
			continue;
		}
		if (fbdiv > fbdiv_max)
			continue;
		frac = Fvco - (Fpfd * fbdiv);
		if (frac < bst_frac) {
			bst_saved = true;
			bst_refdiv = refdiv;
			bst_fbdiv = fbdiv;
			bst_frac = frac;
		}
		if (frac == 0)
			goto bst_exit;
	} while (true);
bst_exit:
	BUG_ON(fractional && (bst_frac >= (UL(1) << 24)));
	refdiv = bst_refdiv;
	fbdiv = bst_fbdiv;
	frac = bst_frac;
	con_log("pll(%d): Fref = %d, Fvco=%d, REFDIV[5:0]=%d, FBDIV[11:0]=%d, FRAC[23:0]=%d\n",
	        n, Fref, Fvco, refdiv, fbdiv, fractional ? frac : 0);
	if (p_refdiv)
		*p_refdiv = refdiv;
	if (p_fbdiv)
		*p_fbdiv = (uint16_t)fbdiv;
	if (p_frac)
		*p_frac = frac;
}

void sc_pllts12ffclafrac2_disable(int n)
{
}

#ifdef CONFIG_SC_PLLTS12FFCLAFRAC2_POSTDIV
static bool mulof(uint32_t a, uint32_t b)
{
	return (UINT32_MAX / a) < b;
}

/* Recalculate Fvco with respect to postdiv */
uint32_t __sc_pllts12ffclafrac2_recalc(int n,
				       uint16_t fbdiv_min, uint16_t fbdiv_max,
				       uint32_t Fpdf_min,
				       uint32_t Fref, uint32_t Fout)
{
	uint32_t Fpfd;
	uint8_t refdiv;
	uint32_t fbdiv;
	uint32_t frac;
	uint8_t postdiv1;
	uint8_t postdiv2;
	uint8_t postdiv;
	uint32_t Fvco;

	bool bst_saved = false;
	uint32_t bst_frac = 3200000000;
	uint8_t bst_postdiv = 1;

	refdiv = 0;
	for (postdiv1 = 1; postdiv1 < 8; postdiv1++) {
		if (mulof(postdiv1, Fout))
			break;
		for (postdiv2 = 1; postdiv2 < postdiv1; postdiv2++) {
			postdiv = postdiv1 * postdiv2;
			if (mulof(postdiv, Fout))
				break;
			Fvco = Fout * postdiv;
			if (Fvco > 3200000000)
				break;
			if (Fvco < 800)
			    break;
			do {
				do {
					refdiv++;
					if (refdiv == 64)
						goto nxt_post;
					Fpfd = Fref / refdiv;
					if (Fpfd < Fpdf_min)
						goto nxt_post;
				} while (Fpfd > (Fvco / fbdiv_min));
				fbdiv = Fvco / Fpfd;
				if (fbdiv < fbdiv_min) {
					refdiv++;
					continue;
				}
				if (fbdiv > fbdiv_max)
					continue;
				frac = Fvco - (Fpfd * fbdiv);
				if (frac < bst_frac) {
					bst_saved = true;
					bst_frac = frac;
					bst_postdiv = postdiv;
				}
				if (frac == 0)
					goto bst_exit;
			} while (true);
nxt_post:
		}
	}
	con_err("pll(%d): Failed to recalculate Fref = %d Fout = %d\n", n, Fref, Fout);
	BUG();
bst_exit:
	BUG_ON(!bst_saved);
	return Fout * bst_postdiv;
}
#else
uint32_t __sc_pllts12ffclafrac2_recalc(int n,
				       uint16_t fbdiv_min, uint16_t fbdiv_max,
				       uint32_t Fpdf_min,
				       uint32_t Fref, uint32_t Fout)
{
	return Fout;
}
#endif

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
				 uint32_t Fref, uint32_t Fvco, uint32_t Fout)
{
	uint8_t refdiv;
	uint16_t fbdiv;
	uint32_t frac;
	uint8_t postdiv1;
	uint8_t postdiv2;

	__sc_pllts12ffclafrac2_divide(n, Fvco, Fout, &postdiv1, &postdiv2);
	__sc_pllts12ffclafrac2_feedback(n, true, out_4phase, 20, 320, 10000000,
					Fref, Fvco, &refdiv, &fbdiv, &frac);
	__raw_writel(PLL_REFDIV(refdiv) | PLL_FBDIV(fbdiv) |
		     PLL_POSTDIV1(postdiv1) | PLL_POSTDIV2(postdiv2),
		     PLL_CFG1(n));
	__raw_writel(PLL_FRAC(frac), PLL_CFG2(n));
	__raw_writel(PLL_DACEN | PLL_DSMEN | PLL_PLLEN | PLL_FOUTPOSTDIVEN |
		     out_4phase ? PLL_FOUTPHASEEN : 0, PLL_CTL(n));
	while (!(__raw_readl(PLL_CTL(n)) & PLL_LOCK));
}

uint32_t sc_pllts12ffclafrac2_recalc(int n, uint32_t Fref, uint32_t Fout)
{
	return __sc_pllts12ffclafrac2_recalc(n, 20, 320, 10000000, Fref, Fout);
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
				 uint32_t Fref, uint32_t Fvco, uint32_t Fout)
{
	uint8_t refdiv;
	uint16_t fbdiv;
	uint8_t postdiv1;
	uint8_t postdiv2;

	__sc_pllts12ffclafrac2_divide(n, Fvco, Fout, &postdiv1, &postdiv2);
	__sc_pllts12ffclafrac2_feedback(n, false, out_4phase, 16, 640, 5000000,
					Fref, Fvco, &refdiv, &fbdiv, NULL);
	__raw_writel(PLL_REFDIV(refdiv) | PLL_FBDIV(fbdiv) |
		     PLL_POSTDIV1(postdiv1) | PLL_POSTDIV2(postdiv2),
		     PLL_CFG1(n));
	__raw_writel(PLL_PLLEN | PLL_FOUTPOSTDIVEN |
		     out_4phase ? PLL_FOUTPHASEEN : 0, PLL_CTL(n));
	while (!(__raw_readl(PLL_CTL(n)) & PLL_LOCK));
}

uint32_t sc_pllts12ffclafrac2_recalc(int n, uint32_t Fref, uint32_t Fout)
{
	return __sc_pllts12ffclafrac2_recalc(n, 16, 640, 5000000, Fref, Fout);
}
#endif
