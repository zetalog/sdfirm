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

/* Only 8 Bits transfers are allowed */
#define DW_SSI_XFER_SIZE		8

struct dw_ssi_ctx dw_ssis[NR_DW_SSIS];

#if 0
static inline uint32_t tx_max(int n)
{
	uint32_t tx_left, tx_room, rxtx_gap;

	tx_left = (dw_ssis[n].tx_end - dw_ssis[n].tx) /
		  (DW_SSI_XFER_SIZE >> 3);
	tx_room = dw_ssis[n].tx_fifo_depth - __raw_readl(SSI_TXFLR(n));
	rxtx_gap = ((dw_ssis[n].rx_end - dw_ssis[n].rx) -
		    (dw_ssis[n].tx_end - dw_ssis[n].tx)) /
		   (DW_SSI_XFER_SIZE >> 3);

	return min3(tx_left, tx_room,
		    (uint32_t)(dw_ssis[n].tx_fifo_depth - rxtx_gap));
}

static inline uint32_t rx_max(int n)
{
	uint32_t rx_left = (dw_ssis[n].rx_end - dw_ssis[n].rx) /
			   (DW_SSI_XFER_SIZE >> 3);
	return min(rx_left, __raw_readl(SSI_RXFLR(n)));
}
#endif

void dw_ssi_config_freq(int n, uint32_t freq)
{
	uint16_t clk_div;

	clk_div = (uint16_t)div32u(clk_get_frequency(DW_SSI_CLK), freq);
	clk_div = (clk_div + 1) & 0xfffe;
	__raw_writel(clk_div, SSI_BAUDR(n));
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
	__raw_writel(0, SSI_TXFTLR(n));
	/* __raw_writel(dw_ssis[n].tx_fifo_depth, SSI_TXFTLR(n)); */
	__raw_writel(0, SSI_RXFTLR(n));
	__raw_writel(0xFF, SSI_IMR(n));
	dw_ssi_enable_ctrl(n);
}

#if 0
static void dw_writer(dw_spi_t *dw_spi_str)
{
	uint32_t max = tx_max(dw_spi_str);
	uint16_t txw = 0;

	while (max--) {
		if (dw_spi_str->tx_end - dw_spi_str->len) {
			if (dw_spi_str->bits_per_word == 8)
				txw = *(uint8_t *)(dw_spi_str->tx);
			else
				txw = *(uint16_t *)(dw_spi_str->tx);
		}
		spi_reg_set16(dw_spi_str, DW_SPI_DR, txw);
		con_printf("%s: tx=0x%02x\n", __func__, txw);
		dw_spi_str->tx += dw_spi_str->bits_per_word >> 3;
	}
}

static int32_t dw_reader(dw_spi_t *dw_spi_str)
{
       uint32_t max;
	uint16_t rxw;
       uint32_t cnt;

	while (rx_max(dw_spi_str) == 0) {
              cnt++;
		if (cnt  > RX_TIMEOUT)
			return SPI_ERROR;
	}
	max = rx_max(dw_spi_str);

	while (max--) {
		rxw = spi_reg_get16(dw_spi_str, DW_SPI_DR);
		con_printf("%s: rx=0x%02x\n", __func__, rxw);

		if (dw_spi_str->rx_end - dw_spi_str->len) {
			if (dw_spi_str->bits_per_word == 8)
				*(uint8_t *)(dw_spi_str->rx) = rxw;
			else
				*(uint16_t *)(dw_spi_str->rx) = rxw;
		}
		dw_spi_str->rx += dw_spi_str->bits_per_word >> 3;
	}

	return SPI_OK;
}

static int32_t poll_transfer(dw_spi_t *dw_spi_str)
{
	int32_t ret;

	do {
		dw_writer(dw_spi_str);
		ret = dw_reader(dw_spi_str);
		if (ret < 0)
			return ret;
	} while (dw_spi_str->rx_end > dw_spi_str->rx);
	return SPI_OK;
}

int32_t dw_spi_tx(const void *txdata, uint32_t txbytes, void *rxdata)
{
	dw_spi_t *dw_spi_str = &dw_spi_str_ver;
	const uint8_t *tx = txdata;
	uint8_t *rx = rxdata;
	int32_t ret = 0;
	uint32_t cr0 = 0;
	uint32_t cs;
	int32_t bitlen = txbytes*8;

	cr0 = (dw_spi_str->bits_per_word - 1) |
	      (dw_spi_str->type << SPI_FRF_OFFSET) |
	      (dw_spi_str->mode << SPI_MODE_OFFSET) |
	      (dw_spi_str->tmode << SPI_TMOD_OFFSET);

	if (rx && tx)
		dw_spi_str->tmode = SPI_TMOD_TR;
	else if (rx)
		dw_spi_str->tmode = SPI_TMOD_RO;
	else
		dw_spi_str->tmode = SPI_TMOD_TO;

	cr0 &= ~SPI_TMOD_MASK;
	cr0 |= (dw_spi_str->tmode << SPI_TMOD_OFFSET);

	dw_spi_str->len = bitlen >> 3;
	con_printf("%s: rx=%p tx=%p len=%d [bytes]\n", __func__, rx, tx, dw_spi_str->len);

	dw_spi_str->tx = (void *)tx;
	dw_spi_str->tx_end = dw_spi_str->tx + dw_spi_str->len;
	dw_spi_str->rx = rx;
	dw_spi_str->rx_end = dw_spi_str->rx + dw_spi_str->len;

	dw_ssi_disable_ctrl(n);

	con_printf("%s: cr0=%08x\n", __func__, cr0);
	if (spi_reg_get16(dw_spi_str, DW_SPI_CTRL0) != cr0)
		spi_reg_set16(dw_spi_str, DW_SPI_CTRL0, cr0);

	dw_ssi_enable_ctrl(n);
	ret = poll_transfer(dw_spi_str);
	return ret;
}
#endif
