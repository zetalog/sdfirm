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
#include <target/bh.h>
#include <target/irq.h>
#include <target/console.h>
#include <target/jiffies.h>

#ifdef CONFIG_DW_SSI_DEBUG
#define dw_ssi_dbg(...)		con_dbg(__VA_ARGS__)
#else
#define dw_ssi_dbg(...)		do { } while (0)
#endif

#if NR_DW_SSIS > 1
#define dw_ssi			dw_ssis[dw_ssid]
static spi_t dw_ssid = 0;

void dw_ssi_master_select(spi_t spi)
{
	BUG_ON(spi >= NR_DW_SSIS);
	dw_ssid = spi;
}
#else
#define dw_ssid				0
#define dw_ssi_master_select(spi)	do { } while (0)
static struct dw_ssi_ctx dw_ssi;
#endif

/* Only 8 Bits transfers are allowed */
#define DW_SSI_XFER_SIZE		8
typedef uint8_t dw_ssi_data;

struct dw_ssi_ctx dw_ssis[NR_DW_SSIS] = {0};

uint32_t dw_ssi_readl(caddr_t reg)
{
	uint32_t val;

	val = __raw_readl(reg);
	dw_ssi_dbg("dw_ssi: R %016lx=%08x\n", reg, val);
	return val;
}

void dw_ssi_writel(uint32_t val, caddr_t reg)
{
	dw_ssi_dbg("dw_ssi: W %016lx=%08x\n", reg, val);
	__raw_writel(val, reg);
}

#ifdef CONFIG_CLK
uint32_t dw_ssi_get_clk_freq(void)
{
	return div32u(clk_get_frequency(DW_SSI_CLK(dw_ssid)), 1000);
}
#else
#define dw_ssi_get_clk_freq()		(APB_CLK_FREQ / 1000)
#endif

void dw_ssi_config_mode(uint8_t mode)
{
	dw_ssis[dw_ssid].spi_mode = mode;
}

void dw_ssi_config_freq(uint32_t freq)
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
	dw_ssis[dw_ssid].sckdv = sckdv;
}

void dw_ssi_select_chip(int chip)
{
	dw_ssi_select_chips(dw_ssid, _BV(chip));
}

void dw_ssi_deselect_chips()
{
	dw_ssi_select_chips(dw_ssid, 0);
}

void dw_ssi_switch_xfer(int n, uint8_t tmod)
{
	if (tmod != dw_ssis[n].tmod) {
		dw_ssi_config_xfer(n, tmod);
		dw_ssis[n].tmod = tmod;
	}
}

void dw_ssi_write_byte(uint8_t byte)
{
	spi_sync_status();
	dw_ssi.status = SPI_STATUS_IDLE;
	dw_ssi_enable_irqs(dw_ssid, SSI_TXEI);
	dw_ssi_write_dr(dw_ssid, byte);
	while (dw_ssi.status == SPI_STATUS_IDLE)
		bh_sync();
	dw_ssi.status = SPI_STATUS_IDLE;
	dw_ssi_disable_irqs(dw_ssid, SSI_TXEI);
}

uint8_t dw_ssi_read_byte()
{
	uint8_t byte;
	spi_sync_status();
	dw_ssi.status = SPI_STATUS_IDLE;
	dw_ssi_enable_irqs(dw_ssid, SSI_RXFI);
	while (dw_ssi.status == SPI_STATUS_IDLE)
		bh_sync();
	byte = (uint8_t)dw_ssi_read_dr(dw_ssid);
	dw_ssi.status = SPI_STATUS_IDLE;
	dw_ssi_disable_irqs(dw_ssid, SSI_RXFI);
	return byte;
}

#define dw_ssi_probe_fifo(n, reg, depth)				\
	do {								\
		if (!depth) {						\
			uint32_t fifo;					\
			for (fifo = 0; fifo < 256; fifo++) {		\
				dw_ssi_writel(fifo, reg(n));		\
				if (fifo != dw_ssi_readl(reg(n)))	\
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

	dw_ssis[n].frf = frf;
	dw_ssis[n].tmod = tmod;
	/* Default to standard SPI wires */
	dw_ssis[n].spi_frf = SSI_SPI_FRF_STD;
	/* Default to SPI_MODE_0 */
	dw_ssis[n].spi_mode = SPI_MODE_0;

	dw_ssi_disable_ctrl(n);
	/* TODO: Probe DFS
	 * Currently the DFS is configured as firmware to save ROM
	 * consumption, we may need to probe it.
	 */
	dw_ssi_probe_fifo(n, SSI_TXFTLR, txfifo);
	dw_ssis[n].tx_fifo_depth = (uint8_t)(txfifo - 1);
	dw_ssi_probe_fifo(n, SSI_RXFTLR, rxfifo);
	dw_ssis[n].rx_fifo_depth = (uint8_t)(rxfifo - 1);

	/* reset the controller */
	dw_ssi_disable_irqs(n, SSI_ALL_IRQS);
	dw_ssi_enable_ctrl(n);
}

void dw_ssi_init_slave(int n, uint8_t frf, uint8_t tmod,
		       uint16_t txfifo, uint16_t rxfifo)
{
	if (n >= NR_DW_SSIS)
		return;
	
	dw_ssis[n].frf = frf;
	dw_ssis[n].tmod = tmod;
	/* Default to standard SPI wires */
	dw_ssis[n].spi_frf = SSI_SPI_FRF_STD;
	/* Default to SPI_MODE_0 */
	dw_ssis[n].spi_mode = SPI_MODE_0;


}

void dw_ssi_init_spi(int n, uint8_t spi_frf,
		     uint8_t inst_l, uint8_t addr_l,
		     uint8_t wait_cycles)
{
	if (n >= NR_DW_SSIS)
		return;

	dw_ssis[n].spi_frf = spi_frf;
	dw_ssis[n].eeprom_inst_len = inst_l;
	dw_ssis[n].eeprom_addr_len = addr_l;
	dw_ssis[n].spi_wait = wait_cycles;
}

void dw_ssi_start_ctrl()
{
	uint32_t ctrl;

	if (dw_ssid >= NR_DW_SSIS)
		return;

	/* Configure CTRLR0 */
	dw_ssi_writel(SSI_SSTE |
		      SSI_FRF(dw_ssis[dw_ssid].frf) |
		      SSI_TMOD(dw_ssis[dw_ssid].tmod) |
		      SSI_SPI_FRF(dw_ssis[dw_ssid].spi_frf) |
		      SSI_SPI_MODE(dw_ssis[dw_ssid].spi_mode) |
		      SSI_DFS(DW_SSI_XFER_SIZE - 1),
		      SSI_CTRLR0(dw_ssid));

	/* Configure FIFO */
	dw_ssi_writel(dw_ssis[dw_ssid].tx_fifo_depth, SSI_TXFTLR(dw_ssid));
	dw_ssi_writel(0, SSI_RXFTLR(dw_ssid));

	/* Configure SPI_CTRLR0 */
	ctrl = SSI_TRANS_TYPE(0);
	if (dw_ssis[dw_ssid].spi_frf != SSI_SPI_FRF_STD)
		ctrl |= SSI_TRANS_TYPE(2);
	if (dw_ssis[dw_ssid].tmod == SSI_TMOD_EEPROM_READ) {
		ctrl |= SSI_INST_L(dw_ssis[dw_ssid].eeprom_inst_len >> 2) |
			SSI_ADDR_L(dw_ssis[dw_ssid].eeprom_addr_len >> 2);
	}
	ctrl |= SSI_WAIT_CYCLES(dw_ssis[dw_ssid].spi_wait);
	dw_ssi_writel(ctrl, SSI_SPI_CTRLR0(dw_ssid));

	/* Configure BAUDR */
	dw_ssi_writel(dw_ssis[dw_ssid].sckdv, SSI_BAUDR(dw_ssid));
	dw_ssi_enable_ctrl(dw_ssid);
}

void dw_ssi_stop_ctrl()
{
	if (dw_ssid >= NR_DW_SSIS)
		return;

	dw_ssi_disable_ctrl(dw_ssid);
}

#ifdef CONFIG_DW_SSI_XFER
static inline uint32_t tx_max(int n)
{
	uint32_t tx_left, rxtx_gap, tx_room;

	tx_left = div32u(dw_ssis[n].tx_end - dw_ssis[n].tx,
			 DW_SSI_XFER_SIZE >> 3);
	tx_room = ((uint32_t)dw_ssis[n].tx_fifo_depth + 1) -
		  dw_ssi_readl(SSI_TXFLR(n));
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
	return min(rx_left, dw_ssi_readl(SSI_RXFLR(n)));
}

static void dw_writer(int n)
{
	uint32_t max = tx_max(n);
	dw_ssi_data txw = 0;

	while (max--) {
		if (dw_ssis[n].tx_end - dw_ssis[n].len)
			txw = *(dw_ssi_data *)(dw_ssis[n].tx);
		dw_ssi_write_dr(n, txw);
		con_dbg("dw_ssi: %s: tx=0x%02x\n", __func__, txw);
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
		con_dbg("dw_ssi: %s: rx=0x%02x\n", __func__, rxw);
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
	con_dbg("dw_ssi: %s: rx=%p tx=%p len=%d [bytes]\n",
		__func__, rx, tx, dw_ssis[n].len);

	dw_ssis[n].tx = (void *)tx;
	dw_ssis[n].tx_end = dw_ssis[n].tx + dw_ssis[n].len;
	dw_ssis[n].rx = rx;
	dw_ssis[n].rx_end = dw_ssis[n].rx + dw_ssis[n].len;

	dw_ssi_disable_ctrl(n);
	con_dbg("dw_ssi: %s: cr0=%08x\n", __func__, cr0);
	if (dw_ssi_readl(SSI_CTRLR0(n)) != cr0)
		dw_ssi_writel(cr0, SSI_CTRLR0(n));
	dw_ssi_enable_ctrl(n);
	return poll_transfer(n);
}
#endif

void dw_ssi_handle_irq(void)
{
	/* dw_ssi_transfer_handler */
	__unused int n = dw_ssid;

	uint32_t status = __raw_readl(SSI_ISR(n));

	if (status & SSI_MSTI) {
		dw_ssi_dbg("dw_ssi: SSI_MSTI\n");
		dw_ssi_readl(SSI_MSTICR(n));
	}
	if (status & SSI_RXFI) {
		dw_ssi_dbg("dw_ssi: SSI_RXFI\n");
		dw_ssi.status = SPI_STATUS_READ;
		spi_set_status(SPI_STATUS_READ);
	}
	if (status & SSI_RXOI) {
		dw_ssi_dbg("dw_ssi: SSI_RXOI\n");
		dw_ssi_readl(SSI_RXOICR(n));
	}
	if (status & SSI_RXUI) {
		dw_ssi_dbg("dw_ssi: SSI_RXUI\n");
		dw_ssi_readl(SSI_RXUICR(n));
	}
	if (status & SSI_TXOI) {
		dw_ssi_dbg("dw_ssi: SSI_TXOI\n");
		dw_ssi_readl(SSI_TXOICR(n));
	}
	if (status & SSI_TXEI) {
		dw_ssi_dbg("dw_ssi: SSI_TXEI\n");
		dw_ssi.status = SPI_STATUS_WRITE;
		spi_set_status(SPI_STATUS_WRITE);
	}
}

#ifndef SYS_REALTIME
static void dw_ssi_irq_handler(irq_t irq)
{
	dw_ssi_handle_irq();
	irqc_ack_irq(irq);
}

void dw_ssi_irq_init(void)
{
	irqc_configure_irq(IRQ_SPI, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(IRQ_SPI, dw_ssi_irq_handler);
	irqc_enable_irq(IRQ_SPI);
	dw_ssi_dbg("dw_ssi: Enable IRQ %d\n", IRQ_SPI);
}

#endif
