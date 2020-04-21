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
 * @(#)dw_ssi.c: Synopsys DesignWare SSI interface
 * $Id: dw_ssi.c,v 1.0 2020-2-10 10:58:00 syl Exp $
 */

#include <target/spi.h>
#include <target/console.h>
#include <target/jiffies.h>

/* Only 8 Bits transfers are allowed */
#define DW_SSI_XFER_SIZE		8
typedef uint8_t dw_ssi_data;

struct dw_ssi_ctx dw_ssis[NR_DW_SSIS];

#ifdef CONFIG_CLK
uint32_t dw_ssi_get_clk_freq(void)
{
	return div32u(clk_get_frequency(DW_SSI_CLK), 1000);
}
#else
#define dw_ssi_get_clk_freq()		(APB_CLK_FREQ / 1000)
#endif

void dw_ssi_config_freq(int n, uint32_t freq)
{
	uint16_t sckdv;
	uint32_t f_ssi_clk = dw_ssi_get_clk_freq();

	/* 2.2 Clock Ratios
	 * The frequency of sclk_out can be derived from the following
	 * equation:
	 * Fsclk_out = Fssi_clk / SCKDV
	 * SCKDV is a bit field in the programmable register BAUDR,
	 * holding any even value in the range 0 to 65,534. If SCKDV is 0,
	 * then sclk_out is disabled.
	 */
	if ((freq << 1) > f_ssi_clk)
		freq = (f_ssi_clk >> 1);
	sckdv = (uint16_t)div32u(f_ssi_clk, freq);
	/* Ensure Fsclk_out is less than requested, and even value. */
	sckdv = (sckdv + 1) & 0xFFFE;
	__raw_writel(sckdv, SSI_BAUDR(n));
}

uint8_t dw_ssi_read_byte(int n)
{
	while (!(__raw_readl(SSI_RISR(n)) & SSI_RXFI));

	return dw_ssi_read_dr(n);
}

#define dw_ssi_probe_fifo(n, reg, depth)				\
	do {								\
		if (!depth) {						\
			uint32_t fifo;					\
			for (fifo = 0; fifo < 256; fifo++) {		\
				__raw_writel(fifo, reg(n));		\
				if (fifo != __raw_readl(reg(n)))	\
					break;				\
			}						\
			depth = fifo;					\
		}							\
	} while (0)

void dw_ssi_init_master(int n, uint8_t frf, uint8_t tmod,
			uint16_t txfifo, uint16_t rxfifo)
{
	if (n >= NR_DW_SSIS)
		return;

	dw_ssi_disable_ctrl(n);
	__raw_writel_mask(SSI_FRF(frf) | SSI_TMOD(tmod) |
			  SSI_DFS(DW_SSI_XFER_SIZE - 1),
			  SSI_FRF_MASK | SSI_TMOD_MASK | SSI_DFS_MASK,
			  SSI_CTRLR0(n));
	dw_ssi_probe_fifo(n, SSI_TXFTLR, txfifo);
	dw_ssis[n].tx_fifo_depth = (uint8_t)(txfifo - 1);
	dw_ssi_probe_fifo(n, SSI_RXFTLR, txfifo);
	dw_ssis[n].rx_fifo_depth = (uint8_t)(rxfifo - 1);
	__raw_writel(dw_ssis[n].tx_fifo_depth, SSI_TXFTLR(n));
	__raw_writel(1, SSI_RXFTLR(n));
	__raw_writel(0xFF, SSI_IMR(n));
	dw_ssi_enable_ctrl(n);
}

#ifdef CONFIG_DW_SSI_XFER
static inline uint32_t tx_max(int n)
{
	uint32_t tx_left, rxtx_gap, tx_room;

	tx_left = div32u(dw_ssis[n].tx_end - dw_ssis[n].tx,
			 DW_SSI_XFER_SIZE >> 3);
	tx_room = ((uint32_t)dw_ssis[n].tx_fifo_depth + 1) -
		  __raw_readl(SSI_TXFLR(n));
	rxtx_gap = div32u((dw_ssis[n].rx_end - dw_ssis[n].rx) -
			  (dw_ssis[n].tx_end - dw_ssis[n].tx),
			  DW_SSI_XFER_SIZE >> 3);

	return min3(tx_left, tx_room,
		    (uint32_t)(dw_ssis[n].tx_fifo_depth) + 1 - rxtx_gap);
}

static inline uint32_t rx_max(int n)
{
	uint32_t rx_left;

	rx_left = div32u(dw_ssis[n].rx_end - dw_ssis[n].rx,
			 DW_SSI_XFER_SIZE >> 3);
	return min(rx_left, __raw_readl(SSI_RXFLR(n)));
}

static void dw_writer(int n)
{
	uint32_t max = tx_max(n);
	dw_ssi_data txw = 0;

	while (max--) {
		if (dw_ssis[n].tx_end - dw_ssis[n].len)
			txw = *(dw_ssi_data *)(dw_ssis[n].tx);
		dw_ssi_write_dr(n, txw);
		con_printf("%s: tx=0x%02x\n", __func__, txw);
		dw_ssis[n].tx += DW_SSI_XFER_SIZE >> 3;
	}
}

static bool dw_reader(int n)
{
	uint32_t max;
	dw_ssi_data rxw;
	tick_t start = tick_get_counter();

	while (rx_max(n) == 0) {
		if (time_after(tick_get_counter(), start + RX_TIMEOUT))
			return false;
	}
	max = rx_max(n);

	while (max--) {
		rxw = dw_ssi_read_dr(n);
		con_printf("%s: rx=0x%02x\n", __func__, rxw);
		if (dw_ssis[n].rx_end - dw_ssis[n].len)
			*(dw_ssi_data *)(dw_ssis[n].rx) = rxw;
		dw_ssis[n].rx += DW_SSI_XFER_SIZE >> 3;
	}
	return true;
}

static int poll_transfer(int n)
{
	do {
		dw_writer(n);
		if (!dw_reader(n))
			return -EAGAIN;
	} while (dw_ssis[n].rx_end > dw_ssis[n].rx);
	return 0;
}

int dw_ssi_xfer(int n, const void *txdata, size_t txbytes, void *rxdata)
{
	const uint8_t *tx = txdata;
	uint8_t *rx = rxdata;
	uint32_t cr0 = 0;
	int32_t bitlen = txbytes * 8;

	cr0 = (DW_SSI_XFER_SIZE - 1) |
	      SSI_FRF(dw_ssis[n].frf) | SSI_TMOD(dw_ssis[n].tmod) |
	      SSI_SPI_FRF(dw_ssis[n].spi_type) |
	      SSI_SPI_MODE(dw_ssis[n].spi_mode);

	if (rx && tx)
		dw_ssis[n].tmod = SSI_TMOD_TX_AND_RX;
	else if (rx)
		dw_ssis[n].tmod = SSI_TMOD_RX_ONLY;
	else
		dw_ssis[n].tmod = SSI_TMOD_TX_ONLY;

	cr0 &= ~SSI_TMOD_MASK;
	cr0 |= SSI_TMOD(dw_ssis[n].tmod);

	dw_ssis[n].len = bitlen >> 3;
	con_printf("%s: rx=%p tx=%p len=%d [bytes]\n",
		   __func__, rx, tx, dw_ssis[n].len);

	dw_ssis[n].tx = (void *)tx;
	dw_ssis[n].tx_end = dw_ssis[n].tx + dw_ssis[n].len;
	dw_ssis[n].rx = rx;
	dw_ssis[n].rx_end = dw_ssis[n].rx + dw_ssis[n].len;

	dw_ssi_disable_ctrl(n);
	con_printf("%s: cr0=%08x\n", __func__, cr0);
	if (__raw_readl(SSI_CTRLR0(n)) != cr0)
		__raw_writel(cr0, SSI_CTRLR0(n));
	dw_ssi_enable_ctrl(n);
	return poll_transfer(n);
}
#endif
