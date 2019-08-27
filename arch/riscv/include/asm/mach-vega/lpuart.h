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
 * @(#)lpuart.h: RV32M1 (VEGA) low power uart (LPUART) interfaces
 * $Id: lpuart.h,v 1.1 2019-08-16 09:35:00 zhenglv Exp $
 */

#ifndef __LPUART_VEGA_H_INCLUDE__
#define __LPUART_VEGA_H_INCLUDE__

#define LPUART_REG(n, offset)	(LPUART_BASE(n) + (offset))

#define LPUART_VERID(n)		LPUART_REG(n, 0x000)
#define LPUART_PARAM(n)		LPUART_REG(n, 0x004)
#define LPUART_GLOBAL(n)	LPUART_REG(n, 0x008)
#define LPUART_PINCFG(n)	LPUART_REG(n, 0x00C)
#define LPUART_BAUD(n)		LPUART_REG(n, 0x010)
#define LPUART_STAT(n)		LPUART_REG(n, 0x014)
#define LPUART_CTRL(n)		LPUART_REG(n, 0x018)
#define LPUART_DATA(n)		LPUART_REG(n, 0x01C)
#define LPUART_MATCH(n)		LPUART_REG(n, 0x020)
#define LPUART_MODIR(n)		LPUART_REG(n, 0x024)
#define LPUART_FIFO(n)		LPUART_REG(n, 0x028)
#define LPUART_WATER(n)		LPUART_REG(n, 0x02C)

/* 56.3.1.2 Version ID Register (VERID) */
#define LPUART_FEATURE_OFFSET	0
#define LPUART_FEATURE_MASK	REG_16BIT_MASK
#define LPUART_FEATURE(value)	_GET_FV(LPUART_FEATURE, value)
#define LPUART_MINOR_OFFSET	16
#define LPUART_MINOR_MASK	REG_8BIT_MASK
#define LPUART_MINOR(value)	_GET_FV(LPUART_MINOR, value)
#define LPUART_MAJOR_OFFSET	24
#define LPUART_MAJOR_MASK	REG_8BOT_MASK
#define LPUART_MAJOR(value)	_GET_FV(LPUART_MAJOR, value)

/* 56.3.1.3 Parameter Register (PARAM) */
#define LPUART_TXFIFO_OFFSET	0
#define LPUART_TXFIFO_MASK	REG_8BIT_MASK
#define LPUART_TXFIFO(value)	_GET_FV(LPUART_TXFIFO, value)
#define LPUART_RXFIFO_OFFSET	8
#define LPUART_RXFIFO_MASK	REG_8BIT_MASK
#define LPUART_RXFIFO(value)	_GET_FV(LPUART_RXFIFO, value)

/* 56.3.1.4 LPUART Global Register (GLOBAL) */
#define LPUART_RST		_BV(1)

/* 56.3.1.5 LPUART Pin Configuration Register (PINCFG) */
#define LPUART_TRGSEL_OFFSET	0
#define LPUART_TRGSEL_MASK	REG_2BIT_MASK
#define LPUART_TRGSEL(value)	_SET_FV(LPUART_TRGSEL, value)
/*  0b00..Input trigger is disabled.
 *  0b01..Input trigger is used instead of RXD pin input.
 *  0b10..Input trigger is used instead of CTS_B pin input.
 *  0b11..Input trigger is used to modulate the TXD pin output. The TXD pin
 *        output (after TXINV configuration) is ANDed with the input
 *        trigger.
 */
#define LPUART_TRGSEL_DISABLED	0
#define LPUART_TRGSEL_RXD	1
#define LPUART_TRGSEL_CTS_B	2
#define LPUART_TRGSEL_TXD	3

/* 56.3.1.6 LPUART Baud Rate Register (BAUD) */
#define LPUART_SBR_OFFSET	0
#define LPUART_SBR_MASK		REG_13BIT_MASK
#define LPUART_SBR(value)	_SET_FV(LPUART_SBR, value)
#define LPUART_SBNS		_BV(13)
#define LPUART_RXEDGIE		_BV(14)
#define LPUART_LBKDIE		_BV(15)
#define LPUART_BAUDIE_OFFSET	15
#define LPUART_BAUDIE_MASK	REG_2BIT_MASK
#define LPUART_BAUDIE(value)	_SET_FV(LPUART_BAUDIE, value)
#define LPUART_RESYNCDIS	_BV(16)
#define LPUART_BOTHEDGE		_BV(17)
#define LPUART_MATCFG_OFFSET	18
#define LPUART_MATCFG_MASK	REG_2BIT_MASK
#define LPUART_MATCFG(value)	_SET_FV(LPUART_MATCFG, value)
#define LPUART_RIDMAE		_BV(20)
#define LPUART_RDMAE		_BV(21)
#define LPUART_TDMAE		_BV(23)
#define LPUART_OSR_OFFSET	24
#define LPUART_OSR_MASK		REG_5BIT_MASK
#define LPUART_OSR(value)	_SET_FV(LPUART_OSR, value)
#define LPUART_M10		_BV(29)
#define LPUART_MAEN2		_BV(30)
#define LPUART_MAEN1		_BV(31)

/* 56.3.1.7 LPUART Status Register (STAT) */
#define LPUART_MA2F		_BV(14)
#define LPUART_MA1F		_BV(15)
#define LPUART_CTRLIF1_OFFSET	14
#define LPUART_CTRLIF1_MASK	REG_2BIT_MASK
#define LPUART_CTRLIF1(value)	_GET_FV(LPUART_CTRLIF1, value)

#define LPUART_PF		_BV(16)
#define LPUART_FE		_BV(17)
#define LPUART_NF		_BV(18)
#define LPUART_OR		_BV(19)
#define LPUART_CTRLIF3_OFFSET	16
#define LPUART_CTRLIF3_MASK	REG_4BIT_MASK
#define LPUART_CTRLIF3(value)	_GET_FV(LPUART_CTRLIF3, value)

#define LPUART_IDLE		_BV(20)
#define LPUART_RDRF		_BV(21)
#define LPUART_TC		_BV(22)
#define LPUART_TDRE		_BV(23)
#define LPUART_CTRLIF2_OFFSET	20
#define LPUART_CTRLIF2_MASK	REG_4BIT_MASK
#define LPUART_CTRLIF2(value)	_GET_FV(LPUART_CTRLIF2, value)

#define LPUART_RAF		_BV(24)
#define LPUART_LBKDE		_BV(25)
#define LPUART_BRK13		_BV(26)
#define LPUART_RWUID		_BV(27)
#define LPUART_RXINV		_BV(28)
#define LPUART_MSBF		_BV(29)

#define LPUART_RXEDGIF		_BV(30)
#define LPUART_LBKDIF		_BV(31)
#define LPUART_BAUDIF_OFFSET	30
#define LPUART_BAUDIF_MASK	REG_2BIT_MASK
#define LPUART_BAUDIF(value)	_GET_FV(LPUART_BAUDIF, value)

/* 56.3.1.8 LPUART Control Register (CTRL) */
#define LPUART_PT		_BV(0)
#define LPUART_PE		_BV(1)
#define LPUART_ILT		_BV(2)
#define LPUART_WAKE		_BV(3)
#define LPUART_M		_BV(4)
#define LPUART_RSRC		_BV(5)
#define LPUART_DOZEEN		_BV(6)
#define LPUART_LOOPS		_BV(7)
#define LPUART_IDLECFG_OFFSET	8
#define LPUART_IDLECFG_MASK	REG_3BIT_MASK
#define LPUART_IDLECFG(value)	_SET_FV(LPUART_IDLECFG, value)
#define LPUART_M7		_BV(11)

#define LPUART_MA2IE		_BV(14)
#define LPUART_MA1IE		_BV(15)
#define LPUART_CTRLIE1_OFFSET	14
#define LPUART_CTRLIE1_MASK	REG_2BIT_MASK
#define LPUART_CTRLIE1(value)	_SET_FV(LPUART_CTRLIE1, value)

#define LPUART_SBK		_BV(16)
#define LPUART_RWU		_BV(17)
#define LPUART_RE		_BV(18)
#define LPUART_TE		_BV(19)

#define LPUART_ILIE		_BV(20)
#define LPUART_RIE		_BV(21)
#define LPUART_TCIE		_BV(22)
#define LPUART_TIE		_BV(23)
#define LPUART_CTRLIE2_OFFSET	20
#define LPUART_CTRLIE2_MASK	REG_4BIT_MASK
#define LPUART_CTRLIE2(value)	_SET_FV(LPUART_CTRLIE2, value)

#define LPUART_PEIE		_BV(24)
#define LPUART_FEIE		_BV(25)
#define LPUART_NEIE		_BV(26)
#define LPUART_ORIE		_BV(27)
#define LPUART_CTRLIE3_OFFSET	24
#define LPUART_CTRLIE3_MASK	REG_4BIT_MASK
#define LPUART_CTRLIE3(value)	_SET_FV(LPUART_CTRLIE3, value)

#define LPUART_TXINV		_BV(28)
#define LPUART_TXDIR		_BV(29)
#define LPUART_R9T8		_BV(30)
#define LPUART_R8T9		_BV(31)

/* 56.3.1.9 LPUART Data Register (DATA) */
#define LPUART_R0T0		_BV(0)
#define LPUART_R1T1		_BV(1)
#define LPUART_R2T2		_BV(2)
#define LPUART_R3T3		_BV(3)
#define LPUART_R4T4		_BV(4)
#define LPUART_R5T5		_BV(5)
#define LPUART_R5T6		_BV(6)
#define LPUART_R5T7		_BV(7)
#define LPUART_R5T8		_BV(8)
#define LPUART_R5T9		_BV(9)
#define LPUART_IDLINE		_BV(11)
#define LPUART_RXEMPT		_BV(12)
#define LPUART_PRETSC		_BV(13)
#define LPUART_PARITYE		_BV(14)
#define LPUART_NOISY		_BV(15)

/* 56.3.1.10 LPUART Match Address Register (MATCH) */
#define LPUART_MA1_OFFSET	0
#define LPUART_MA1_MASK		REG_10BIT_MASK
#define LPUART_MA1(value)	_SET_FV(LPUART_MA1, value)
#define LPUART_MA2_OFFSET	16
#define LPUART_MA2_MASK		REG_10BIT_MASK
#define LPUART_MA2(value)	_SET_FV(LPUART_MA2, value)

/* 56.3.1.11 LPUART Modem IrDA Register (MODIR) */
#define LPUART_TXCTSE		_BV(0)
#define LPUART_TXRTSE		_BV(1)
#define LPUART_TXRTSPOL		_BV(2)
#define LPUART_RXRTSE		_BV(3)
#define LPUART_TXCTSC		_BV(4)
#define LPUART_TXCTSSRC		_BV(5)
#define LPUART_RTSWATER_OFFSET	8
#define LPUART_RTSWATER_MASK	REG_3BIT_MASK
#define LPUART_RTSWATER(value)	_SET_FV(LPUART_RTSWATER, value)
#define LPUART_TNP_OFFSET	16
#define LPUART_TNP_MASK		REG_2BIT_MASK
#define LPUART_TNP(value)	_SET_FV(LPUART_TNP, value)
#define LPUART_IREN		_BV(18)

/* 56.3.1.12 LPUART FIFO Register (FIFO) */
#define LPUART_RXFIFOSIZE_OFFSET	0
#define LPUART_RXFIFOSIZE_MASK		REG_3BIT_MASK
#define LPUART_RXFIFOSIZE(value)	_GET_FV(LPUART_RXFIFOSIZE, value)
#define LPUART_RXFE			_BV(3)
#define LPUART_TXFIFOSIZE_OFFSET	4
#define LPUART_TXFIFOSIZE_MASK		REG_3BIT_MASK
#define LPUART_TXFIFOSIZE(value)	_GET_FV(LPUART_TXFIFOSIZE, value)
#define LPUART_TXFE			_BV(7)

#define LPUART_RXUFE			_BV(8)
#define LPUART_TXOFE			_BV(9)
#define LPUART_FIFOIE1_OFFSET		8
#define LPUART_FIFOIE1_MASK		REG_2BIT_MASK
#define LPUART_FIFOIE1(value)		_SET_FV(LPUART_FIFOIE1, value)

#define LPUART_RXIDEN_OFFSET		10
#define LPUART_RXIDEN_MASK		REG_3BIT_MASK
#define LPUART_RXIDEN(value)		_SET_FV(LPUART_RXIDEN, value)
#define LPUART_RXFLUSH			_BV(14)
#define LPUART_TXFLUSH			_BV(15)

#define LPUART_RXUF			_BV(16)
#define LPUART_TXOF			_BV(17)
#define LPUART_FIFOIF1_OFFSET		16
#define LPUART_FIFOIF1_MASK		REG_2BIT_MASK
#define LPUART_FIFOIF1(value)		_GET_FV(LPUART_FIFOIF1, (value))

#define LPUART_RXFIFOEMPT		_BV(22)
#define LPUART_TXFIFOEMPT		_BV(23)
#define LPUART_FIFOIF2_OFFSET		22
#define LPUART_FIFOIF2_MASK		REG_2BIT_MASK
#define LPUART_FIFOIF2(value)		_GET_FV(LPUART_FIFOIF2,	value)

/* 56.3.1.13 LPUART Watermark Register (WATER) */
#define LPUART_TXWATER_OFFSET		0
#define LPUART_TXWATER_MASK		REG_3BIT_MASK
#define LPUART_TXWATER(value)		_SET_FV(LPUART_TXWATER, value)
#define LPUART_TXCOUNT_OFFSET		8
#define LPUART_TXCOUNT_MASK		REG_4BIT_MASK
#define LPUART_TXCOUNT(value)		_GET_FV(LPUART_TXCOUNT, value)
#define LPUART_RXWATER_OFFSET		16
#define LPUART_RXWATER_MASK		REG_3BIT_MASK
#define LPUART_RXWATER(value)		_SET_FV(LPUART_RXWATER, value)
#define LPUART_RXCOUNT_OFFSET		24
#define LPUART_RXCOUNT_MASK		REG_4BIT_MASK
#define LPUART_RXCOUNT(value)		_GET_FV(LPUART_TXCOUNT, value)

#define lpuart_config_fifo(id, txw, rxw)			\
	__raw_writel_mask(LPUART_TXWATER(txw) |			\
			  LPUART_RXWATER(rxw),			\
			  LPUART_TXWATER(LPUART_TXWATER_MASK) |	\
			  LPUART_RXWATER(LPUART_RXWATER_MASK),	\
			  LPUART_WATER(id));
#define lpuart_rx_count(id)					\
	LPUART_RXCOUNT(__raw_readl(LPUART_WATER(id)))
#define lpuart_tx_count(id)					\
	LPUART_TXCOUNT(__raw_readl(LPUART_WATER(id)))
#define lpuart_enable_fifo(id)				\
	__raw_writel_mask(LPUART_TXFE | LPUART_RXFE,	\
			  LPUART_TXFE | LPUART_RXFE,	\
			  LPUART_FIFO(id))
#define lpuart_disable_fifo(id)				\
	__raw_writel_mask(0, LPUART_TXFE | LPUART_RXFE,	\
			  LPUART_FIFO(id))
#define lpuart_enable_uart(id)				\
	__raw_writel_mask(LPUART_TE | LPUART_RE,	\
			  LPUART_TE | LPUART_RE,	\
			  LPUART_CTRL(id))
#define lpuart_disable_uart(id)				\
	__raw_writel_mask(0, LPUART_TE | LPUART_RE,	\
			  LPUART_CTRL(id))
#define lpuart_reset_uart(id)				\
	__raw_setl(LPUART_RST, LPUART_GLOBAL(id))

#define lpuart_7bit_enabled(id)				\
	(__raw_readl(LPUART_CTRL(id)) & LPUART_M7)
#define lpuart_enable_7bit(id)	__raw_setl(LPUART_M7, LPUART_CTRL(id))
#define lpuart_disable_7bit(id)	__raw_clearl(LPUART_M7, LPUART_CTRL(id))

/* ======================================================================
 * LPUART IRQs
 * ====================================================================== */
/* XXX: LPUART IRQ flags are collected here.
 *
 * All LPUART_STAT STS flags use their original positions, all LPUART_CTRL
 * EN flags are re-positioned to where the corresponding LPUART_STAT STS
 * flags are.
 * All LPUART_FIFO EN/STS flags are re-positioned in order not to overlap
 * with LPUART_STAT STS flags.
 */
/* _BV(0):  F: LPUART_RXFIFOEMPTY, E: N/A */
#define LPUART_RXEI			_BV(0)
/* _BV(1):  F: LPUART_TXFIFOEMPTY, E: N/A */
#define LPUART_TXEI			_BV(1)
#define LPUART_FIFOI2_OFFSET		0
#define LPUART_FIFOI2_MASK		REG_2BIT_MASK
#define LPUART_GET_FIFOI2(value)	_GET_FV(LPUART_FIFOI2, value)
#define LPUART_SET_FIFOI2(value)	_SET_FV(LPUART_FIFOI2, value)
/* _BV(8):  F: LPUART_RXUF, E: LPUART_RXUFE
 * _BV(9):  F: LPUART_TXOF, E: LPUART_TXOFE
 */
#define LPUART_RXUFI			LPUART_RXUFE
#define LPUART_TXOFI			LPUART_TXOFE
#define LPUART_FIFOI1_OFFSET		8
#define LPUART_FIFOI1_MASK		REG_2BIT_MASK
#define LPUART_GET_FIFOI1(value)	_GET_FV(LPUART_FIFOI1, value)
#define LPUART_SET_FIFOI1(value)	_SET_FV(LPUART_FIFOI1, value)
/* _BV(14): F: LPUART_MA2F, E: LPUART_MA2E
 * _BV(15): F: LPUART_MA1F, E: LPUART_MA1E
 */
#define LPUART_MA2I			LPUART_MA2F
#define LPUART_MA1I			LPUART_MA1F
#define LPUART_CTRLI1_OFFSET		14
#define LPUART_CTRLI1_MASK		REG_2BIT_MASK
#define LPUART_GET_CTRLI1(value)	_GET_FV(LPUART_CTRLI1, value)
#define LPUART_SET_CTRLI1(value)	_SET_FV(LPUART_CTRLI1, value)
/* _BV(16): F: LPUART_PF, E: LPUART_PEIE
 * _BV(17): F: LPUART_FE, E: LPUART_FEIE
 * _BV(18): F: LPUART_NF, E: LPUART_NEIE
 * _BV(19): F: LPUART_OR, E: LPUART_ORIE
 */
#define LPUART_PEI			LPUART_PF
#define LPUART_FEI			LPUART_FE
#define LPUART_NEI			LPUART_NF
#define LPUART_ORI			LPUART_OR
#define LPUART_CTRLI3_OFFSET		16
#define LPUART_CTRLI3_MASK		REG_4BIT_MASK
#define LPUART_GET_CTRLI3(value)	_GET_FV(LPUART_CTRLI3, value)
#define LPUART_SET_CTRLI3(value)	_SET_FV(LPUART_CTRLI3, value)
/* _BV(20): F: LPUART_IDLE, E: LPUART_ILIE
 * _BV(21): F: LPUART_RDRF, E: LPUART_RIE
 * _BV(22): F: LPUART_TC, E: LPUART_TCIE
 * _BV(23): F: LPUART_TDRE, E: LPUART_TIE
 */
#define LPUART_ILI			LPUART_IDLE
/* receive data register full */
#define LPUART_RDRFI			LPUART_RDRF
#define LPUART_TCI			LPUART_TC
/* transmit data register empty */
#define LPUART_TDREI			LPUART_TDRE
#define LPUART_CTRLI2_OFFSET		20
#define LPUART_CTRLI2_MASK		REG_4BIT_MASK
#define LPUART_GET_CTRLI2(value)	_GET_FV(LPUART_CTRLI2, value)
#define LPUART_SET_CTRLI2(value)	_SET_FV(LPUART_CTRLI2, value)
/* _BV(30): F: LPUART_RXEDGIF, E: LPUART_RXEDGIE
 * _BV(31): F: LPUART_LBKDIF, E: LPUART_LBKDIE
 */
#define LPUART_RXEDGI			LPUART_RXEDGIF
#define LPUART_LBKDI			LPUART_LBKDIF
#define LPUART_BAUDI1_OFFSET		30
#define LPUART_BAUDI1_MASK		REG_2BIT_MASK
#define LPUART_GET_BAUDI1(value)	_GET_FV(LPUART_BAUDI1, value)
#define LPUART_SET_BAUDI1(value)	_SET_FV(LPUART_BAUDI1, value)

/* All clearable interrupt */
#define LPUART_IRQS_ALL						\
	(LPUART_RXEDGI | LPUART_LBKDI | LPUART_ILI |		\
	 LPUART_PEI | LPUART_FEI | LPUART_NEI | LPUART_ORI |	\
	 LPUART_MA1I | LPUART_MA2I)

typedef uint32_t lpuart_irq_t;

void lpuart_enable_irqs(uint8_t id, lpuart_irq_t irqs);
void lpuart_disable_irqs(uint8_t id);
lpuart_irq_t lpuart_irqs_status(uint8_t id);
void lpuart_clear_irqs(uint8_t id, lpuart_irq_t irqs);

void lpuart_write_byte(uint8_t id, uint8_t byte);
uint8_t lpuart_read_byte(uint8_t id);
bool lpuart_ctrl_poll(uint8_t id);
void lpuart_ctrl_init(uint8_t id, uint32_t src_freq,
		      uint8_t params, uint32_t baud);

#endif /* __LPUART_VEGA_H_INCLUDE__ */
