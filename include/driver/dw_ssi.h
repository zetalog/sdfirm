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
 * @(#)dw_ssi.h: Synopsys DesignWare SSI interface
 * $Id: dw_ssi.h,v 1.0 2020-02-11 10:58:00 zhenglv Exp $
 */

#ifndef __DW_SSI_H_INCLUDE__
#define __DW_SSI_H_INCLUDE__

#include <target/arch.h>

#ifndef DW_SSI_BASE
#define DW_SSI_BASE(n)		(DW_SSI##n##_BASE)
#endif
#ifndef DW_SSI_REG
#define DW_SSI_REG(n, offset)	(DW_SSI_BASE(n) + (offset))
#endif
#ifndef NR_DW_SSIS
#define NR_DW_SSIS		1
#endif

/* ssi_memory_map/ssi_address_block registers */
#define SSI_CTRLR0(n)		DW_SSI_REG(n, 0x00)
#define SSI_CTRLR1(n)		DW_SSI_REG(n, 0x04)
#define SSI_SSIENR(n)		DW_SSI_REG(n, 0x08)
#define SSI_MWCR(n)		DW_SSI_REG(n, 0x0C)
#define SSI_SER(n)		DW_SSI_REG(n, 0x10)
#define SSI_BAUDR(n)		DW_SSI_REG(n, 0x14)
#define SSI_TXFTLR(n)		DW_SSI_REG(n, 0x18)
#define SSI_RXFTLR(n)		DW_SSI_REG(n, 0x1C)
#define SSI_TXFLR(n)		DW_SSI_REG(n, 0x20)
#define SSI_RXFLR(n)		DW_SSI_REG(n, 0x24)
#define SSI_SR(n)		DW_SSI_REG(n, 0x28)
#define SSI_IMR(n)		DW_SSI_REG(n, 0x2C)
#define SSI_ISR(n)		DW_SSI_REG(n, 0x30)
#define SSI_RISR(n)		DW_SSI_REG(n, 0x34)
#define SSI_TXOICR(n)		DW_SSI_REG(n, 0x38)
#define SSI_RXOICR(n)		DW_SSI_REG(n, 0x3C)
#define SSI_RXUICR(n)		DW_SSI_REG(n, 0x40)
#define SSI_MSTICR(n)		DW_SSI_REG(n, 0x44)
#define SSI_ICR(n)		DW_SSI_REG(n, 0x48)
#define SSI_DMACR(n)		DW_SSI_REG(n, 0x4C)
#define SSI_DMATDLR(n)		DW_SSI_REG(n, 0x50)
#define SSI_DMARDLR(n)		DW_SSI_REG(n, 0x54)
#define SSI_IDR(n)		DW_SSI_REG(n, 0x58)
#define SSI_VERSION_ID(n)	DW_SSI_REG(n, 0x5C)
#define SSI_DR(n, x)		DW_SSI_REG(n, 0x60 + (x) * 0x04)
#ifdef CONFIG_DW_SSI_MAX_XFER_SIZE_32
#define dw_ssi_read_dr(n)		__raw_readl(SSI_DR(n, 0))
#define dw_ssi_write_dr(n, v)		__raw_writel(v, SSI_DR(n, 0))
#endif
#ifdef CONFIG_DW_SSI_MAX_XFER_SIZE_16
#define dw_ssi_read_dr(n)		__raw_readw(SSI_DR(n, 0))
#define dw_ssi_write_dr(n, v)		__raw_writew(v, SSI_DR(n, 0))
#endif
#define SSI_RX_SAMPLE_DLY(n)	DW_SSI_REG(n, 0xF0)
#define SSI_SPI_CTRLR0(n)	DW_SSI_REG(n, 0xF4)
#define SSI_TXD_DRIVE_EDGE(n)	DW_SSI_REG(n, 0xF8)

/* 5.1.1 CTRLR0 */
#define SSI_SSTE		_BV(24)
#define SSI_SPI_FRF_OFFSET	21
#define SSI_SPI_FRF_MASK	REG_2BIT_MASK
#define SSI_SPI_FRF(value)	_SET_FV(SSI_SPI_FRF, value)
#define SSI_SPI_FRF_STD		0x0
#define SSI_SPI_FRF_DUAL	0x1
#define SSI_SPI_FRF_QUAD	0x2
#define SSI_SPI_FRF_OCTAL	0x3
#ifdef CONFIG_DW_SSI_MAX_XFER_SIZE_32
#define DW_SSI_MAX_XFER_SIZE	32
#define SSI_DFS_OFFSET		16
#define SSI_DFS_MASK		REG_5BIT_MASK
#define SSI_DFS(value)		_SET_FV(SSI_DFS, value)
#endif
#define SSI_CFS_OFFSET		12
#define SSI_CFS_MASK		REG_4BIT_MASK
#define SSI_CFS(value)		_SET_FV(SSI_CFS, value)
#define SSI_SRL			_BV(11)
#define SSI_SRL_NORMAL		0
#define SSI_SRL_TESTING		1 /* Tx & Rx shift reg connected */
#define SSI_SLV_OE		_BV(10) /* slave only */
#define SSI_TMOD_OFFSET		8
#define SSI_TMOD_MASK		REG_2BIT_MASK
#define SSI_TMOD(value)		_SET_FV(SSI_TMOD, value)
#define SSI_TMOD_TX_AND_RX	0
#define SSI_TMOD_TX_ONLY	1
#define SSI_TMOD_RX_ONLY	2
#define SSI_TMOD_EEPROM_READ	3
#define SSI_SPI_MODE_OFFSET	6
#define SSI_SPI_MODE_MASK	REG_2BIT_MASK
#define SSI_SPI_MODE(value)	_SET_FV(SSI_SPI_MODE, value)
#define SSI_FRF_OFFSET		4
#define SSI_FRF_MASK		REG_2BIT_MASK
#define SSI_FRF(value)		_SET_FV(SSI_FRF, value)
#define SSI_FRF_SPI		0
#define SSI_FRF_SSP		1
#define SSI_FRF_MICROWIRE	2
#ifdef CONFIG_DW_SSI_MAX_XFER_SIZE_16
#define DW_SSI_MAX_XFER_SIZE	16
#define SSI_DFS_OFFSET		0
#define SSI_DFS_MASK		REG_4BIT_MASK
#define SSI_DFS(value)		_SET_FV(SSI_DFS, value)
#endif

/* 5.1.2 CTRLR1 */
#define SSI_NDF_OFFSET		0
#define SSI_NDF_MASK		REG_16BIT_MASK
#define SSI_NDF(value)		_SET_FV(SSI_NDF, value)

/* 5.1.3 SSIENR */
#define SSI_EN			_BV(0)

/* 5.1.4 MWCR */
#define SSI_MHS			_BV(2)
#define SSI_MDD			_BV(1)
#define SSI_MWMOD		_BV(0)

/* 5.1.6 BAUDR */
#define SSI_SCKDV_OFFSET	0
#define SSI_SCKDV_MASK		REG_16BIT_MASK
#define SSI_SCKDV(value)	_SET_FV(SSI_SCKDV, value)

/* 5.1.11 SR */
#define SSI_DCOL		_BV(6)
#define SSI_TXE			_BV(5)
#define SSI_RFF			_BV(4)
#define SSI_RFNE		_BV(3)
#define SSI_TFE			_BV(2)
#define SSI_TFNF		_BV(1)
#define SSI_BUSY		_BV(0)

/* 5.1.12 IMR
 * 5.1.13 ISR
 * 5.1.14 RISR
 */
#define SSI_MSTI		_BV(5)
#define SSI_RXFI		_BV(4)
#define SSI_RXOI		_BV(3)
#define SSI_RXUI		_BV(2)
#define SSI_TXOI		_BV(1)
#define SSI_TXEI		_BV(0)

#define SSI_ALL_IRQS		(SSI_MSTI | SSI_RXFI | SSI_RXOI | \
				 SSI_RXUI | SSI_TXOI | SSI_TXEI)

/* 5.1.27 SPI_CTRLR0 */
#define SSI_SPI_RXDS_EN		_BV(18)
#define SSI_INST_DDR_EN		_BV(17)
#define SSI_SPI_DDR_EN		_BV(16)
#define SSI_WAIT_CYCLES_OFFSET	11
#define SSI_WAIT_CYCLES_MASK	REG_5BIT_MASK
#define SSI_WAIT_CYCLES(value)	_SET_FV(SSI_WAIT_CYCLES, value)
#define SSI_INST_L_OFFSET	8
#define SSI_INST_L_MASK		REG_2BIT_MASK
#define SSI_INST_L(value)	_SET_FV(SSI_INST_L, value)
#define SSI_ADDR_L_OFFSET	2
#define SSI_ADDR_L_MASK		REG_4BIT_MASK
#define SSI_ADDR_L(value)	_SET_FV(SSI_ADDR_L, value)
#define SSI_TRANS_TYPE_OFFSET	0
#define SSI_TRANS_TYPE_MASK	REG_2BIT_MASK
#define SSI_TRANS_TYPE(value)	_SET_FV(SSI_TRANS_TYPE, value)

/* Bit fields in SR, 7 bits */
#define SR_MASK				GENMASK(6, 0)	/* cover 7 bits */
#define SR_BUSY				BIT(0)
#define SR_TF_NOT_FULL			BIT(1)
#define SR_TF_EMPT			BIT(2)
#define SR_RF_NOT_EMPT			BIT(3)
#define SR_RF_FULL			BIT(4)
#define SR_TX_ERR			BIT(5)
#define SR_DCOL				BIT(6)

#define RX_TIMEOUT			100000	/* timeout in ms */
#define DW_SPI_CS			1
#define DW_SPI_FREQ_SPEED		50

struct dw_ssi_ctx {
	uint8_t frf;		/* SPI/SSP/MicroWire */
	uint8_t tmod;		/* TR/TO/RO/EEPROM */
	uint8_t tx_fifo_depth;	/* depth of the FIFO buffer */
	uint8_t rx_fifo_depth;	/* depth of the FIFO buffer */
	uint8_t spi_frf;	/* STD/DUAL/QUAD/OCTAL */
	uint8_t spi_mode;	/* SCPOL/SCPH */
	uint8_t eeprom_inst_len;
	uint8_t eeprom_addr_len;
	uint8_t spi_wait;	/* wait cycles */
	uint16_t sckdv;		/* sck divisor */
#ifdef CONFIG_DW_SSI_XFER
	void *tx;
	void *tx_end;
	void *rx;
	void *rx_end;
	int len;
#endif
};

/* The maximum frequency of the bit-rate clock (sclk_out) is one-half the
 * frequency of ssi_clk.
 */
#define SPI_HW_MAX_FREQ			(DW_SSI_CLK_FREQ / 2000) /* kHz */

#define dw_ssi_enable_ctrl(n)		__raw_setl(SSI_EN, SSI_SSIENR(n))
#define dw_ssi_disable_ctrl(n)		__raw_clearl(SSI_EN, SSI_SSIENR(n))
#define dw_ssi_reset_ctrl(n)				\
	do {						\
		dw_ssi_disable_ctrl(n);			\
		dw_ssi_disable_irqs(n, SSI_ALL_IRQS);	\
		dw_ssi_enable_ctrl(n);			\
	} while (0)
#define dw_ssi_config_xfer(n, tmod)			\
	__raw_writel_mask(SSI_TMOD(tmod),		\
			  SSI_TMOD_MASK, SSI_CTRLR0(n))
#define dw_ssi_select_chips(n, chips)			\
	__raw_writel((chips), SSI_SER(n))
#define dw_ssi_select_chip(n, chip)	dw_ssi_select_chips(n, _BV(chip))
#define dw_ssi_deselect_chips(n)	dw_ssi_select_chips(n, 0)
#define dw_ssi_enable_irqs(n, irqs)	__raw_setl(irqs, SSI_IMR(n))
#define dw_ssi_disable_irqs(n, irqs)	__raw_clearl(irqs, SSI_IMR(n))
#define dw_ssi_irqs_status(n)		__raw_readl(SSI_ISR(n))
#define dw_ssi_clear_irqs(n, irqs)	__raw_clearl(irqs, SSI_ISR(n))

uint8_t dw_ssi_read_byte(int n);
void dw_ssi_write_byte(int n, uint8_t byte);
void dw_ssi_config_mode(int n, uint8_t mode);
void dw_ssi_config_freq(int n, uint32_t freq);
void dw_ssi_init_master(int n, uint8_t frf, uint8_t tmod,
			uint16_t txfifo, uint16_t rxfifo);
void dw_ssi_init_spi(int n, uint8_t spi_frf,
		     uint8_t inst_l, uint8_t addr_l,
		     uint8_t wait_cycles);
void dw_ssi_start_ctrl(int n);
#define dw_ssi_stop_ctrl(n)		dw_ssi_disable_ctrl(n)

#ifdef CONFIG_DW_SSI_FIFO_DEPTH
#define DW_SSI_TX_FIFO_DEPTH		CONFIG_DW_SSI_TX_FIFO_DEPTH
#define DW_SSI_RX_FIFO_DEPTH		CONFIG_DW_SSI_RX_FIFO_DEPTH
#else
/* Allow probe */
#define DW_SSI_TX_FIFO_DEPTH		0
#define DW_SSI_RX_FIFO_DEPTH		0
#endif

#define __dw_ssi_init_master_fifo(n, frf, tmod)		\
	dw_ssi_init_master(n, frf, tmod,		\
			   DW_SSI_TX_FIFO_DEPTH,	\
			   DW_SSI_RX_FIFO_DEPTH)
#ifdef CONFIG_DW_SSI_XFER
int dw_ssi_xfer(int n, const void *txdata, size_t txbytes, void *rxdata);
#else
#define dw_ssi_xfer(n, txdata, txsize, rxdata)		-ENODEV
#endif

#endif /* __DW_SSI_H_INCLUDE__ */
