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
 * @(#)cbf.c: CBF power clamp reset implementation
 * $Id: cbf.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <target/delay.h>
#include <target/cpus.h>

#define CBF_CLAMP_ASSERT_DELAY_US		1
#define CBF_CLAMP_DEASSERT_DELAY_US		1
#define CBF_CLAMP_CBFPD_ASSERT_DELAY_US		2
#define CBF_CLAMP_CBFPD_DEASSERT_DELAY_US	1
#define CBF_POR_RST_DEASSERT_DELAY_US		1

#define CBF_LOGIC_MASK			(1 << (L3_CACHE_BLOCKS + 1))

static uint32_t cbf_initialized = 0;

void cbf_initialization(void)
{
	/* CBF power on sequence. */
	/* Assert CLAMP_CBFPD */
	__raw_writel(PWR_CTL_CLAMP_CBFPD, APCS_PM_CBF_PWR_CTL);
	udelay(CBF_CLAMP_CBFPD_ASSERT_DELAY_US);

	/* Deassert CLAMP_CBFPD, assert APC_ON */
	__raw_writel(PWR_CTL_PMIC_APC_ON, APCS_PM_CBF_PWR_CTL);
	udelay(CBF_CLAMP_CBFPD_DEASSERT_DELAY_US);
}

void cbf_l3_block_initialization(uint32_t block_number)
{
	if (cbf_initialized & (1 << block_number))
		return;

	/* Ensure that L3 block passed is valid. */
	/* BL_VERIFY(block_number < L3_CACHE_BLOCKS,
		     BL_ERROR_INVALID_L3_BLOCK);
	 */

	/* Assert all CLAMPs */
	__raw_writel(PWR_CTL_L3U_CLAMP_APMPD,
		     apcs_pm_cbf_l3u_pwr_ctl[block_number]);
	udelay(CBF_CLAMP_ASSERT_DELAY_US);

	/* Assert POR_RST and all CLAMPs */
	__raw_writel(PWR_CTL_L3U_POR_RST | PWR_CTL_L3U_CLAMP_APMPD,
		     apcs_pm_cbf_l3u_pwr_ctl[block_number]);
	udelay(CBF_CLAMP_ASSERT_DELAY_US);

	/* Unclamp All Clamps */
	/* Assert POR_RST and all CLAMPs */
	__raw_writel(PWR_CTL_L3U_POR_RST,
		     apcs_pm_cbf_l3u_pwr_ctl[block_number]);
	udelay(CBF_CLAMP_DEASSERT_DELAY_US);

	/* Deassert POR_RST */
	__raw_writel(0, apcs_pm_cbf_l3u_pwr_ctl[block_number]);
	udelay(CBF_CLAMP_DEASSERT_DELAY_US);

	cbf_initialized |= (1 << block_number);
}

uint64_t get_l3_block_mask(void)
{
	uint64_t l3_fuse_row = get_llc_mask();
	uint64_t l3_partial_goods = 0;
	unsigned char current_l3_block;

	/* There are 8 L3 blocks. Each block contains two interleave fuses.
	 * If either of the interleaves are not blown then the L3 block is
	 * functional. If both interleave fuses are blown then it is not. A
	 * "1" in the l3 partial goods value means the block is functional.
	 * Block 0 = LSB.
	 */
	/* Loop through all L3 blocks evaluating each interleave and
	 * setting l3_partial_goods appropriately.
	 */
	for (current_l3_block = 0;
	     current_l3_block < L3_CACHE_BLOCKS;
	     current_l3_block++) {
		/* Check both interleaves for a 0. If either exists write a
		 * 1 to the proper L3 bit.  Shift fuse data when done with
		 * current block.
		 */
		if (l3_fuse_row & 0x3)
			l3_partial_goods |= (1 << current_l3_block);
		/* Shift fuse row to the next block for evaluation. */
		l3_fuse_row = l3_fuse_row >> 2;
	}
	return l3_partial_goods;
}


void cbf_init(void)
{
	uint64_t l3_block_status = get_l3_block_mask();
	uint32_t current_l3_block = 0;

	cbf_initialization();
	for (current_l3_block = 0;
	     current_l3_block < L3_CACHE_BLOCKS;
	     current_l3_block++) {
		if (l3_block_status & (1 << current_l3_block))
			cbf_l3_block_initialization(current_l3_block);
	}
}
