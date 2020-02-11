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
 * @(#)dw_spi.c: Synopsys DesignWare SSI interface
 * $Id: dw_spi.c,v 1.0 2020-2-10 10:58:00 syl Exp $
 */
#include <driver/dw_spi.h>
#include <target/clk.h>
#include <target/bh.h>
#define DW_SPI_DEBUG
#ifdef DW_SPI_DEBUG
#include <target/console.h>
#endif

dw_spi_t dw_spi_str_ver;
static inline uint32_t spi_reg_get32(dw_spi_t *dw_spi_str, uint32_t offset)
{
	return __raw_readl(dw_spi_str->regs_base + offset);
}

static inline void spi_reg_set32(dw_spi_t *dw_spi_str, uint32_t offset, uint32_t val)
{
	__raw_writel(val, dw_spi_str->regs_base + offset);
}

static inline uint16_t spi_reg_get16(dw_spi_t *dw_spi_str, uint32_t offset)
{
	return __raw_readw(dw_spi_str->regs_base + offset);
}

static inline void spi_reg_set16(dw_spi_t *dw_spi_str, uint32_t offset, uint16_t val)
{
	__raw_writew(val, dw_spi_str->regs_base + offset);
}

static inline void spi_enable_chip(dw_spi_t *dw_spi_str, int32_t enable)
{
	spi_reg_set32(dw_spi_str, DW_SPI_SSIENR, (enable ? 1 : 0));
}

static void spi_hw_init(dw_spi_t *dw_spi_str)
{
	spi_enable_chip(dw_spi_str, 0);
	spi_reg_set32(dw_spi_str, DW_SPI_IMR, 0xff);
	spi_enable_chip(dw_spi_str, 1);

	if (!dw_spi_str->fifo_len) {
		uint32_t fifo;
		for (fifo = 1; fifo < 256; fifo++) {
			spi_reg_set16(dw_spi_str, DW_SPI_TXFLTR, fifo);
			if (fifo != spi_reg_get16(dw_spi_str, DW_SPI_TXFLTR))
				break;
		}

		dw_spi_str->fifo_len = (fifo == 1) ? 0 : fifo;
		spi_reg_set16(dw_spi_str, DW_SPI_TXFLTR, 0);
	}
	con_printf("%s: fifo_len=%d\n", __func__, dw_spi_str->fifo_len);
}
int32_t dw_spi_init(void)
{
       dw_spi_t *dw_spi_str = &dw_spi_str_ver;
	dw_spi_str->regs_base = DW_SPI_REG_BASE;
	dw_spi_str->freq = DW_SPI_FREQ_SPEED;
       dw_spi_str->bits_per_word = 8;
	dw_spi_str->tmode = 0; /* Tx & Rx */
       spi_hw_init(dw_spi_str);

	return SPI_OK;
}

static inline uint32_t tx_max(dw_spi_t *dw_spi_str)
{
	uint32_t tx_left, tx_room, rxtx_gap;

	tx_left = (dw_spi_str->tx_end - dw_spi_str->tx) / (dw_spi_str->bits_per_word >> 3);
	tx_room = dw_spi_str->fifo_len - spi_reg_get16(dw_spi_str, DW_SPI_TXFLR);

	rxtx_gap = ((dw_spi_str->rx_end - dw_spi_str->rx) - (dw_spi_str->tx_end - dw_spi_str->tx)) /
		(dw_spi_str->bits_per_word >> 3);

	return min3(tx_left, tx_room, (uint32_t)(dw_spi_str->fifo_len - rxtx_gap));
}
static inline uint32_t rx_max(dw_spi_t *dw_spi_str)
{
	uint32_t rx_left = (dw_spi_str->rx_end - dw_spi_str->rx) / (dw_spi_str->bits_per_word >> 3);
	return min_t(uint32_t, rx_left, spi_reg_get16(dw_spi_str, DW_SPI_RXFLR));
}

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

int32_t dw_spi_tx(const void *txdata, uint32_t txbytes,
		       void *rxdata)
{
       dw_spi_t *dw_spi_str = &dw_spi_str_ver;
	const uint8_t *tx = txdata;
	uint8_t *rx = rxdata;
	int32_t ret = 0;
	uint32_t cr0 = 0;
	uint32_t cs;
       int32_t bitlen = txbytes*8;

	cr0 = (dw_spi_str->bits_per_word - 1) | (dw_spi_str->type << SPI_FRF_OFFSET) |
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

	spi_enable_chip(dw_spi_str, 0);

	con_printf("%s: cr0=%08x\n", __func__, cr0);
	if (spi_reg_get16(dw_spi_str, DW_SPI_CTRL0) != cr0)
		spi_reg_set16(dw_spi_str, DW_SPI_CTRL0, cr0);

	cs = DW_SPI_CS;
	spi_reg_set32(dw_spi_str, DW_SPI_SER, 1 << cs);

	spi_enable_chip(dw_spi_str, 1);

	ret = poll_transfer(dw_spi_str);

	return ret;
}

static int32_t dw_spi_set_speed(uint32_t speed)
{
	dw_spi_t *dw_spi_str = &dw_spi_str_ver;
	uint16_t clk_div;

	spi_enable_chip(dw_spi_str, 0);

	clk_div = clk_get_frequency() / speed;
	clk_div = (clk_div + 1) & 0xfffe;
	spi_reg_set32(dw_spi_str, DW_SPI_BAUDR, clk_div);

	spi_enable_chip(dw_spi_str, 1);

	dw_spi_str->freq = speed;
	con_printf("%s: regs=%p speed=%d clk_div=%d\n", __func__, dw_spi_str->regs_base,
	      dw_spi_str->freq, clk_div);

	return SPI_OK;
}

static int32_t dw_spi_set_mode(uint32_t mode)
{
	dw_spi_t *dw_spi_str = &dw_spi_str_ver;

	dw_spi_str->mode = mode;
	con_printf("%s: regs=%p, mode=%d\n", __func__, dw_spi_str->regs_base, dw_spi_str->mode);

	return SPI_OK;
}

