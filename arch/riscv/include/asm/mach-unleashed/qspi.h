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
 * @(#)qspi.h: FU540 (unleashed) specific QSPI defintions
 * $Id: qspi.h,v 1.1 2019-10-22 13:29:00 zhenglv Exp $
 */

#ifndef __QSPI_UNLEASHED_H_INCLUDE__
#define __QSPI_UNLEASHED_H_INCLUDE__

/*===========================================================================
 * SiFive QSPI driver
 *===========================================================================*/

#define QSPI_REG(n, offset)	(QSPI_BASE(n) + (offset))

#define QSPI_SCKDIV(n)		QSPI_REG(n, 0x00)
#define QSPI_SCKMODE(n)		QSPI_REG(n, 0x04)
#define QSPI_CSID(n)		QSPI_REG(n, 0x10)
#define QSPI_CSDEF(n)		QSPI_REG(n, 0x14)
#define QSPI_CSMODE(n)		QSPI_REG(n, 0x18)
#define QSPI_DELAY0(n)		QSPI_REG(n, 0x28)
#define QSPI_DELAY1(n)		QSPI_REG(n, 0x2C)
#define QSPI_FMT(n)		QSPI_REG(n, 0x40)
#ifdef CONFIG_SIFIVE_QSPI_STATUS
#define QSPI_TXSTAT(n)		QSPI_REG(n, 0x4A)
#define QSPI_RXSTAT(n)		QSPI_REG(n, 0x4E)
/* TXSTAT */
#define QSPI_FULL		_BV(15)
/* RXSTAT */
#define QSPI_EMPTY		_BV(15)
#else
/* TXDATA */
#define QSPI_FULL		_BV(31)
/* RXDATA */
#define QSPI_EMPTY		_BV(31)
#endif
#define QSPI_TXDATA(n)		QSPI_REG(n, 0x48)
#define QSPI_RXDATA(n)		QSPI_REG(n, 0x4C)
#define QSPI_TXMARK(n)		QSPI_REG(n, 0x50)
#define QSPI_RXMARK(n)		QSPI_REG(n, 0x54)
#define QSPI_FCTRL(n)		QSPI_REG(n, 0x60)
#define QSPI_FFMT(n)		QSPI_REG(n, 0x64)
#define QSPI_IE(n)		QSPI_REG(n, 0x70)
#define QSPI_IP(n)		QSPI_REG(n, 0x74)

/* SCKMODE, exactly as spi_mode() */
#define QSPI_PHA		_BV(0)
#define QSPI_POL		_BV(1)
/* CSMODE */
#define QSPI_MODE_OFFSET	0
#define QSPI_MODE_MASK		REG_2BIT_MASK
#define QSPI_MODE(value)	_SET_FV(QSPI_MODE, value)
#define QSPI_MODE_AUTO		0
#define QSPI_MODE_HOLD		2
#define QSPI_MODE_OFF		3
/* FMT */
#define QSPI_PROTO_OFFSET	0
#define QSPI_PROTO_MASK		REG_2BIT_MASK
#define QSPI_PROTO(value)	_SET_FV(QSPI_PROTO, value)
/* exactly as spi_flash() */
#define QSPI_PROTO_SINGLE	SPI_FLASH_3WIRE
#define QSPI_PROTO_DUAL		SPI_FLASH_4WIRE
#define QSPI_PROTO_QUAD		SPI_FLASH_6WIRE
#define QSPI_ENDIAN_OFFSET	2
#define QSPI_ENDIAN_MASK	REG_1BIT_MASK
#define QSPI_ENDIAN(value)	_SET_FV(QSPI_ENDIAN, value)
#define QSPI_DIR		_BV(3) /* TX */
#define QSPI_LEN_OFFSET		16
#define QSPI_LEN_MASK		REG_4BIT_MASK
#define QSPI_LEN(value)		_SET_FV(QSPI_LEN, value)
/* FCTRL */
#define SPINOR_EN		_BV(0)
/* FFMT */
#define SPINOR_CMD_EN			_BV(0)
#define SPINOR_ADDR_LEN_OFFSET		1
#define SPINOR_ADDR_LEN_MASK		REG_3BIT_MASK
#define SPINOR_ADDR_LEN(value)		_SET_FV(SPINOR_ADDR_LEN, value)
#define SPINOR_PAD_LEN_OFFSET		4
#define SPINOR_PAD_LEN_MASK		REG_4BIT_MASK
#define SPINOR_PAD_LEN(value)		_SET_FV(SPINOR_PAD_LEN, value)
#define SPINOR_CMD_PROTO_OFFSET		8
#define SPINOR_CMD_PROTO_MASK		REG_2BIT_MASK
#define SPINOR_CMD_PROTO(value)		_SET_FV(SPINOR_CMD_PROTO, value)
#define SPINOR_ADDR_PROTO_OFFSET	10
#define SPINOR_ADDR_PROTO_MASK		REG_2BIT_MASK
#define SPINOR_ADDR_PROTO(value)	_SET_FV(SPINOR_ADDR_PROTO, value)
#define SPINOR_DATA_PROTO_OFFSET	12
#define SPINOR_DATA_PROTO_MASK		REG_2BIT_MASK
#define SPINOR_DATA_PROTO(value)	_SET_FV(SPINOR_DATA_PROTO, value)
#define SPINOR_CMD_CODE_OFFSET		16
#define SPINOR_CMD_CODE_MASK		REG_8BIT_MASK
#define SPINOR_CMD_CODE(value)		_SET_FV(SPINOR_CMD_CODE, value)
#define SPINOR_PAD_CODE_OFFSET		24
#define SPINOR_PAD_CODE_MASK		REG_8BIT_MASK
#define SPINOR_PAD_CODE(value)		_SET_FV(SPINOR_PAD_CODE, value)

/* SPINOR wrapper */
#define sifive_qspi_enable_spinor(n)	\
	__raw_setl(SPINOR_EN, QSPI_FCTRL(n))
#define sifive_qspi_disable_spinor(n)	\
	__raw_clearl(SPINOR_EN, QSPI_FCTRL(n))

/* SPI controller */
#define sifive_qspi_chip_mode(n, mode)	\
	__raw_writel(QSPI_MODE(mode), QSPI_CSMODE(n))
#ifdef CONFIG_SIFIVE_QSPI_STATUS
#define sifive_qspi_write_poll(n)				\
	(!(__raw_readw(QSPI_TXSTAT(n)) & QSPI_FULL))
#define sifive_qspi_read_poll(n)				\
	(!(__raw_readw(QSPI_RXSTAT(n)) & QSPI_EMPTY))
#define sifive_qspi_write_byte(n, byte)				\
	__raw_writeb(byte, QSPI_TXDATA(n))
#define sifive_qspi_read_byte(n)				\
	__raw_readb(QSPI_RXDATA(n))
#else
extern uint32_t sifive_qspi_rx;
#define sifive_qspi_write_poll(n)				\
	(!(__raw_readl(QSPI_TXDATA(n)) & QSPI_FULL))
#define sifive_qspi_write_byte(n, byte)				\
	__raw_writel((uint32_t)(byte), QSPI_TXDATA(n));
#define sifive_qspi_read_poll(n)				\
	(sifive_qspi_rx = __raw_readl(QSPI_RXDATA(n)),		\
	 !(sifive_qspi_rx & QSPI_EMPTY))
#define sifive_qspi_read_byte(n)				\
	(LOBYTE(sifive_qspi_rx))
#endif

#define sifive_qspi_chip_select(n, chip)			\
	do {							\
		__raw_setl(0x1 << (chip), QSPI_CSDEF(n));	\
		__raw_writel((chip), QSPI_CSID(n));		\
	} while (0)
#define sifive_qspi_config_freq(n, input_freq, max_output_freq)	\
	do {							\
		uint32_t div = sifive_qspi_min_div(input_freq,	\
			max_output_freq);			\
		__raw_writel(div, QSPI_SCKDIV(n));		\
	} while (0)
/* NOTE:
 * sifive_qspi_config_mode() configures read mode (QSPI_DIR=0),
 * sifive_qspi_config_read/write() switches the read/write mode.
 */
#define sifive_qspi_config_mode(n, mode)			\
	do {							\
		__raw_writel(spi_mode(mode), QSPI_SCKMODE(n));	\
		__raw_writel(QSPI_PROTO(spi_flash(mode)) |	\
			     QSPI_LEN(8) |			\
			     QSPI_ENDIAN(spi_order(mode)),	\
			     QSPI_FMT(n));			\
	} while (0)
#define sifive_qspi_config_write(n)				\
	__raw_setl(QSPI_DIR, QSPI_FMT(n))
#define sifive_qspi_config_read(n)				\
	__raw_clearl(QSPI_DIR, QSPI_FMT(n))

/*            Fin
 * Fsck = -------------
 *        2 * (div + 1)
 */
uint32_t sifive_qspi_min_div(uint32_t input_freq, uint32_t max_output_freq);
void sifive_qspi_tx(uint8_t spi, uint8_t byte);
uint8_t sifive_qspi_rx(uint8_t spi);

#endif /* __QSPI_UNLEASHED_H_INCLUDE__ */
