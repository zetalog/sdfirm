/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2018
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
 * @(#)uart_dragonfly.h: Qualcomm dragonfly uart interfaces
 * $Id: uart_dragonfly.h,v 1.279 2018-07-26 10:19:18 zhenglv Exp $
 */

#ifndef __UART_DRAGONFLY_H_INCLUDE__
#define __UART_DRAGONFLY_H_INCLUDE__

#include <target/config.h>
#include <target/types.h>
#include <target/muldiv.h>
#include <asm/io.h>

#define UART_DM_REG(n, offset)		\
	(UART_DM_BASE(n)+		\
	 (UART_DM_ID(n)<<UART_DM_BLOCK_SHIFT)+(offset))
#define UART_DM_GET_FV(name, value)	_GET_FV(UART_DM_##name, value)
#define UART_DM_SET_FV(name, value)	_SET_FV(UART_DM_##name, value)

/* Register map */
#define UART_DM_MR1(n)			UART_DM_REG(n, 0x000)
#define UART_DM_MR2(n)			UART_DM_REG(n, 0x004)
#define UART_DM_IPR(n)			UART_DM_REG(n, 0x018)
#define UART_DM_TFWR(n)			UART_DM_REG(n, 0x01C)
#define UART_DM_RFWR(n)			UART_DM_REG(n, 0x020)
#define UART_DM_HCR(n)			UART_DM_REG(n, 0x024)
#define UART_DM_DMRX(n)			UART_DM_REG(n, 0x034)
#define UART_DM_DMEN(n)			UART_DM_REG(n, 0x03C)
#define UART_DM_NO_CHARS_FOR_TX(n)	UART_DM_REG(n, 0x040)
#define UART_DM_BADR(n)			UART_DM_REG(n, 0x044)
#define UART_DM_TXFS(n)			UART_DM_REG(n, 0x04C)
#define UART_DM_RXFS(n)			UART_DM_REG(n, 0x050)
#define UART_DM_SIM_CFG(n)		UART_DM_REG(n, 0x080)
#define UART_DM_CSR(n)			UART_DM_REG(n, 0x0A0)
#define UART_DM_SR(n)			UART_DM_REG(n, 0x0A4)
#define UART_DM_CR(n)			UART_DM_REG(n, 0x0A8)
#define UART_DM_MISR(n)			UART_DM_REG(n, 0x0AC)
#define UART_DM_IMR(n)			UART_DM_REG(n, 0x0B0)
#define UART_DM_ISR(n)			UART_DM_REG(n, 0x0B4)
#define UART_DM_IRDA(n)			UART_DM_REG(n, 0x0B8)
#define UART_DM_RX_TOTAL_SNAP(n)	UART_DM_REG(n, 0x0BC)
/* x = 1...16 */
#define UART_DM_TF(n, x)		\
	(UART_DM_REG(n, 0x100) + ((x)-1) * 0x004)
#define UART_DM_RF(n, x)		\
	(UART_DM_REG(n, 0x140) + ((x)-1) * 0x004)

#define UART_DM_MISR_MODE(n)		UART_DM_REG(n, 0x060)
#define UART_DM_MISR_RESET(n)		UART_DM_REG(n, 0x064)
#define UART_DM_MISR_EXPORT(n)		UART_DM_REG(n, 0x068)
#define UART_DM_MISR_VAL(n)		UART_DM_REG(n, 0x06C)
#define UART_DM_WWT_TIMEOUT(n)		UART_DM_REG(n, 0x0C0)
#define UART_DM_CLK_CTRL(n)		UART_DM_REG(n, 0x0C4)
#define UART_DM_BCR(n)			UART_DM_REG(n, 0x0C8)
#define UART_DM_RX_TRANS_CTRL(n)	UART_DM_REG(n, 0x0CC)
#define UART_DM_FSM_STATUS(n)		UART_DM_REG(n, 0x0D4)
#define UART_DM_HW_VERSION(n)		UART_DM_REG(n, 0x0D8)
#define UART_DM_GENERICS(n)		UART_DM_REG(n, 0x0DC)
#define UART_DM_UIM_CFG(n)		UART_DM_REG(n, 0x180)
#define UART_DM_UIM_CMD(n)		UART_DM_REG(n, 0x184)
#define UART_DM_UIM_IO_STATUS(n)	UART_DM_REG(n, 0x188)
#define UART_DM_UIM_IRQ_ISR(n)		UART_DM_REG(n, 0x18C)
#define UART_DM_UIM_IRQ_MISR(n)		UART_DM_REG(n, 0x190)
#define UART_DM_UIM_IRQ_CLR(n)		UART_DM_REG(n, 0x194)
#define UART_DM_UIM_IRQ_IMR(n)		UART_DM_REG(n, 0x198)
#define UART_DM_UIM_IRQ_IMR_SET(n)	UART_DM_REG(n, 0x19C)
#define UART_DM_UIM_IRQ_IMR_CLR(n)	UART_DM_REG(n, 0x1A0)
#define UART_DM_RX_END_ADDR(n)		UART_DM_REG(n, 0x1A4)

/* Register field values */
/* 2.25.b.x.3.1. UART_DM_MR1 (0x0FF 79XX 0000) */
#define UART_DM_AUTO_RFR_LEVEL1_OFFSET	8
#define UART_DM_AUTO_RFR_LEVEL1_MASK	0xFFF
#define UART_DM_AUTO_RFR_LEVEL1(value)	UART_DM_SET_FV(AUTO_RFR_LEVEL1, value)
#define UART_DM_RX_RDY_CTL		_BV(7)
#define UART_DM_CTS_CTL			_BV(6)
#define UART_DM_AUTO_RFR_LEVEL0_OFFSET	0
#define UART_DM_AUTO_RFR_LEVEL0_MASK	0x1F
#define UART_DM_AUTO_RFR_LEVEL0(value)	UART_DM_SET_FV(AUTO_RFR_LEVEL0, value)

/* 2.25.b.x.3.2. UART_DM_MR2 (0x0FF 79XX 0004) */
#define UART_DM_ARM_UART_BUSY_FIX	_BV(11)
#define UART_DM_RFR_CTS_LOOPBACK	_BV(10)
#define UART_DM_RX_ERROR_CHAR_OFF	_BV(9)
#define UART_DM_RX_BREAK_ZERO_CHAR_OFF	_BV(8)
#define UART_DM_LOOPBACK		_BV(7)
#define UART_DM_ERROR_MODE		_BV(6)
#define UART_DM_BITS_PER_CHAR_OFFSET	4
#define UART_DM_BITS_PER_CHAR_MASK	0x03
#define UART_DM_BITS_PER_CHAR(value)	UART_DM_SET_FV(BITS_PER_CHAR, value)
/* n = 5...8 */
#define UART_DM_ENUM_BITS(n)		((n)-5)
#define UART_DM_STOP_BIT_LEN_OFFSET	2
#define UART_DM_STOP_BIT_LEN_MASK	0x03
#define UART_DM_STOP_BIT_LEN(value)	UART_DM_SET_FV(STOP_BIT_LEN, value)
#define UART_DM_ENUM_0_563		0
#define UART_DM_ENUM_1_000		1
#define UART_DM_ENUM_1_563		2
#define UART_DM_ENUM_2_000		3
#define UART_DM_PARITY_MODE_OFFSET	0
#define UART_DM_PARITY_MODE_MASK	0x03
#define UART_DM_PARITY_MODE(value)	UART_DM_SET_FV(PARITY_MODE, value)
#define UART_DM_PARITY_NONE		0
#define UART_DM_PARITY_ODD		1
#define UART_DM_PARITY_EVEN		2
#define UART_DM_PARITY_SPACE		3

/* 2.25.b.x.3.6. UART_DM_IPR (0x0FF 79XX 0018) */
#define UART_DM_STALE_TIMEOUT_MSB_OFFSET	7
#define UART_DM_STALE_TIMEOUT_MSB_MASK		0x1FFFFFF
#define UART_DM_STALE_TIMEOUT_MSB(value)	\
	UART_DM_SET_FV(STALE_TIMEOUT_MSB, value)
#define UART_DM_SAMPLE_DATA			_BV(6)
#define UART_DM_MASK_BUSY_FIX			_BV(5)
#define UART_DM_STALE_TIMEOUT_LSB_OFFSET	0
#define UART_DM_STALE_TIMEOUT_LSB_MASK		0x1F
#define UART_DM_STALE_TIMEOUT_LSB(value)	\
	UART_DM_SET_FV(STALE_TIMEOUT_LSB, value)
#define UART_DM_STALE_TIMEOUT(value)		\
	((UART_DM_STALE_TIMEOUT_MSB((value) &	\
	  UART_DM_STALE_TIMEOUT_MSB_MASK) >>	\
	  UART_DM_STALE_TIMEOUT_MSB_OFFSET) |	\
	 (UART_DM_STALE_TIMEOUT_LSB((value) &	\
	  UART_DM_STALE_TIMEOUT_LSB_MASK)))

/* 2.25.b.x.3.12. UART_DM_DMEN (0x0FF 79XX 003C) */
#define UART_DM_RX_SC_ENABLE		_BV(5)
#define UART_DM_TX_SC_ENABLE		_BV(4)
#define UART_DM_RX_BAM_ENABLE		_BV(3)
#define UART_DM_TX_BAM_ENABLE		_BV(2)
#define UART_DM_NEW_BUSY_FIXES_OFFSET	0
#define UART_DM_NEW_BUSY_FIXES_MASK	0x03
#define UART_DM_NEW_BUSY_FIXES(value)	UART_DM_SET_FV(NEW_BUSY_FIXES, value)

/* 2.25.b.x.3.16. UART_DM_TXFS (0x0FF 79XX 004C)
 * 2.25.b.x.3.17. UART_DM_RXFS (0x0FF 79XX 0050)
 */
#define UART_DM_FIFO_STATE_MSB_OFFSET	14
#define UART_DM_FIFO_STATE_MSB_MASK	0x3FFFF
#define UART_DM_FIFO_STATE_MSB(value)	UART_DM_GET_FV(FIFO_STATE_MSB, value)
#define UART_DM_ASYNC_FIFO_STATE_OFFSET	10
#define UART_DM_ASYNC_FIFO_STATE_MASK	0x0F
#define UART_DM_ASYNC_FIFO_STATE(value)	UART_DM_GET_FV(ASYNC_FIFO_STATE, value)
#define UART_DM_BUFFER_STATE_OFFSET	7
#define UART_DM_BUFFER_STATE_MASK	0x07
#define UART_DM_BUFFER_STATE(value)	UART_DM_GET_FV(BUFFER_STATE, value)
#define UART_DM_FIFO_STATE_LSB_OFFSET	0
#define UART_DM_FIFO_STATE_LSB_MASK	0x7F
#define UART_DM_FIFO_STATE_LSB(value)	UART_DM_GET_FV(FIFO_STATE_LSB, value)

/* 12: not 6 to avoid drain FIFO, fixes HW bugs */
#define UART_DM_FIFO_STATE(value)		\
	(UART_DM_FIFO_STATE_MSB(value) << 12 |	\
	 UART_DM_FIFO_STATE_LSB(value))

/* 2.25.b.x.3.26. UART_DM_SIM_CFG (0x0FF 79XX 0080) */
#define UART_DM_UIM_TX_MODE		_BV(17)
#define UART_DM_UIM_RX_MODE		_BV(16)
#define UART_DM_SIM_STOP_BIT_LEN_OFFSET	8
#define UART_DM_SIM_STOP_BIT_LEN_MASK	0xFF
#define UART_DM_SIM_STOP_BIT_LEN(value)	UART_DM_SET_FV(SIM_STOP_BIT_LEN, value)
/* n = 1...254 */
#define UART_DM_ENUM_BITS_TIMES(n)	((n))
#define UART_DM_SIM_CLK_ON		_BV(7)
#define UART_DM_SIM_CLK_TD8_SEL		_BV(6)
#define UART_DM_SIM_CLK_STOP_HIGH	_BV(5)
#define UART_DM_MASK_RX			_BV(3)
#define UART_DM_SWAP_D			_BV(2)
#define UART_DM_INV_D			_BV(1)
#define UART_DM_SIM_SEL			_BV(0)

/* 2.25.b.x.3.31. UART_DM_CSR (0x0FF 79XX 00A0) */
#define UART_DM_RX_CLK_SEL_OFFSET	4
#define UART_DM_RX_CLK_SEL_MASK		0x0F
#define UART_DM_RX_CLK_SEL(value)	UART_DM_SET_FV(RX_CLK_SEL, value)
#define UART_DM_TX_CLK_SEL_OFFSET	0
#define UART_DM_TX_CLK_SEL_MASK		0x0F
#define UART_DM_TX_CLK_SEL(value)	UART_DM_SET_FV(TX_CLK_SEL, value)

/* 2.25.b.x.3.32. UART_DM_SR (0x0FF 79XX 00A4) */
#define UART_DM_TRANS_END_TRIGGER_OFFSET	10
#define UART_DM_TRANS_END_TRIGGER_MASK		0x03
#define UART_DM_TRANS_END_TRIGGER(value)	\
	UART_DM_SET_FV(TRANS_END_TRIGGER, value)
#define UART_DM_TRANS_ACTIVE		_BV(9)
#define UART_DM_RX_BREAK_START_LAST	_BV(8)
#define UART_DM_HUNT_CHAR		_BV(7)
#define UART_DM_RX_BREAK		_BV(6)
#define UART_DM_PAR_FRAME_ERR		_BV(5)
#define UART_DM_OVERRUN			_BV(4)
#define UART_DM_TXEMT			_BV(3)
#define UART_DM_TXRDY			_BV(2)
#define UART_DM_RXFULL			_BV(1)
#define UART_DM_RXRDY			_BV(0)

/* 2.25.b.x.3.33. UART_DM_CR (0x0FF 79XX 00A8) */
#define UART_DM_CHANNEL_COMMAND_MSB		_BV(11)
#define UART_DM_GENERAL_COMMAND_OFFSET		8
#define UART_DM_GENERAL_COMMAND_MASK		0x07
#define UART_DM_GENERAL_COMMAND(value)		\
	UART_DM_SET_FV(GENERAL_COMMAND, value)
#define UART_DM_CMD_GENERAL(cmd)		\
	UART_DM_GENERAL_COMMAND(UART_DM_CMD_##cmd)
#define UART_DM_CHANNEL_COMMAND_LSB_OFFSET	4
#define UART_DM_CHANNEL_COMMAND_LSB_MASK	0x0F
#define UART_DM_CHANNEL_COMMAND_LSB(value)	\
	UART_DM_SET_FV(CHANNEL_COMMAND_LSB, value)
#define UART_DM_CMD_CHANNEL(cmd)				\
	((UART_DM_CMD_##cmd & UART_DM_CHANNEL_COMMAND_MSB) |	\
	 UART_DM_CHANNEL_COMMAND_LSB(UART_DM_CMD_##cmd &	\
				     UART_DM_CHANNEL_COMMAND_LSB_MASK))
#define UART_DM_TX_DISABLE			_BV(3)
#define UART_DM_TX_EN				_BV(2)
#define UART_DM_RX_DISABLE			_BV(1)
#define UART_DM_RX_EN				_BV(0)

#define UART_DM_CMD_NULL			0x00
/* Channel commands */
#define UART_DM_CMD_RESET_RX			0x01
#define UART_DM_CMD_RESET_TX			0x02
#define UART_DM_CMD_RESET_ERROR_STATUS		0x03
#define UART_DM_CMD_CLEAR_RX_HUNT_IRQ		UART_DM_CMD_RESET_ERROR_STATUS
#define UART_DM_CMD_CLEAR_BREAK_CHANGE_IRQ	0x04
#define UART_DM_CMD_START_BREAK			0x05
#define UART_DM_CMD_STOP_BREAK			0x06
#define UART_DM_CMD_CLEAR_DELTA_CTS_IRQ		0x07
#define UART_DM_CMD_CLEAR_STALE_IRQ		0x08
#define UART_DM_CMD_PACKET_MODE			0x09
#define UART_DM_CMD_MODE_RESET			0x0C
#define UART_DM_CMD_SET_RFR			0x0D
#define UART_DM_CMD_RESET_RFR			0x0E
#define UART_DM_CMD_UART_RESET_INTERN		0x0F
#define UART_DM_CMD_CLEAR_TX_ERROR_IRQ		0x10
#define UART_DM_CMD_CLEAR_TX_DONE_IRQ		0x11
#define UART_DM_CMD_CLEAR_BREAK_START_IRQ	0x12
#define UART_DM_CMD_CLEAR_BREAK_END_IRQ		0x13
#define UART_DM_CMD_CLEAR_PAR_FRAME_ERR_IRQ	0x14
#define UART_DM_CMD_START_RX_BAM_IFC		0x15
#define UART_DM_CMD_START_TX_BAM_IFC		0x16
#define UART_DM_CMD_ENABLE_WWT_IRQ		0x17
#define UART_DM_CMD_DISABLE_WWT_IRQ		0x18
#define UART_DM_CMD_CLEAR_TX_WR_ERROR_IRQ	0x19
#define UART_DM_CMD_CLEAR_RX_RD_ERROR_IRQ	0x1A
#define UART_DM_CMD_CLEAR_TX_COMP_IRQ		0x1B
#define UART_DM_CMD_CLEAR_WWT_IRQ		0x1C
/* General commands */
#define UART_DM_CMD_CR_PROTECTION_ENABLE	0x01
#define UART_DM_CMD_CR_PROTECTION_DISABLE	0x02
#define UART_DM_CMD_CLEAR_TX_READY_IRQ		0x03
#define UART_DM_CMD_SW_FORCE_STALE		0x04
#define UART_DM_CMD_ENABLE_STALE_IRQ		0x05
#define UART_DM_CMD_DISABLE_STALE_IRQ		0x06

/* 2.25.b.x.3.35. UART_DM_IMR (0x0FF 79XX 00B0)
 * 2.25.b.x.3.36. UART_DM_ISR (0x0FF 79XX 00B4)
 */
#define UART_DM_WWT_IRQ				_BV(16)
#define UART_DM_TX_COMP_IRQ			_BV(15)
#define UART_DM_RX_RD_ERROR_IRQ			_BV(14)
#define UART_DM_TX_WR_ERROR_IRQ			_BV(13)
#define UART_DM_PAR_FRAME_ERR_IRQ		_BV(12)
#define UART_DM_RXBREAK_END_IRQ			_BV(11)
#define UART_DM_RXBREAK_START_IRQ		_BV(10)
#define UART_DM_TX_DONE_IRQ			_BV(9)
#define UART_DM_TX_ERROR_IRQ			_BV(8)
#define UART_DM_TX_READY_IRQ			_BV(7)
#define UART_DM_CURRENT_CTS_IRQ			_BV(6)
#define UART_DM_DELTA_CTS_IRQ			_BV(5)
#define UART_DM_RXLEV_IRQ			_BV(4)
#define UART_DM_RXSTALE_IRQ			_BV(3)
#define UART_DM_RXBREAK_CHANGE_IRQ		_BV(2)
#define UART_DM_RXHUNT_IRQ			_BV(1)
#define UART_DM_TXLEV_IRQ			_BV(0)

/* 2.25.b.x.3.37. UART_DM_IRDA (0x0FF 79XX 00B8) */
#define UART_DM_MEDIUM_RATE_EN			_BV(4)
#define UART_DM_IRDA_LOOPBACK			_BV(3)
#define UART_DM_INVERT_IRDA_TX			_BV(2)
#define UART_DM_INVERT_IRDA_RX			_BV(1)
#define UART_DM_IRDA_EN				_BV(0)

/* 2.25.b.x.3.46. UART_DM_GENERICS (0x0FF 78XX 00DC) */
#define UART_DM_GENERIC_BAM_IFC			_BV(7)
#define UART_DM_GENERIC_DM_IFC			_BV(6)
#define UART_DM_GENERIC_IRDA_IFC		_BV(5)
#define UART_DM_GENERIC_SIM_GLUE		_BV(4)
#define UART_DM_GENERIC_RAM_ADDR_WIDTH_OFFSET	0
#define UART_DM_GENERIC_RAM_ADDR_WIDTH_MASK	0x0F
#define UART_DM_GENERIC_RAM_ADDR_WIDTH(value)	\
	UART_DM_GET_FV(GENERIC_RAM_ADDR_WIDTH, value)

#define __uart_dm_begin_reset(n)					\
	do {								\
		__raw_writel(						\
			UART_DM_CMD_GENERAL(CR_PROTECTION_ENABLE),	\
			UART_DM_CR(n));					\
		__raw_writel(UART_DM_CMD_CHANNEL(RESET_RFR),		\
			     UART_DM_CR(n));				\
		__raw_writel(UART_DM_CMD_CHANNEL(RESET_RX),		\
			     UART_DM_CR(n));				\
		__raw_writel(UART_DM_CMD_CHANNEL(RESET_TX),		\
			     UART_DM_CR(n));				\
	} while (0)
#define __uart_dm_end_reset(n)						\
	do {								\
		__raw_writel(UART_DM_CMD_CHANNEL(SET_RFR),		\
			     UART_DM_CR(n));				\
	} while (0)
#define __uart_dm_uart_init(n)						\
	do {								\
		__raw_writel(0, UART_DM_SIM_CFG(n));			\
		__raw_writel(0, UART_DM_IRDA(n));			\
		__raw_writel(0, UART_DM_HCR(n));			\
	} while (0)
#define __uart_dm_uart_enable(n)				\
	do {							\
		__raw_writel(UART_DM_CMD_CHANNEL(RESET_RX),	\
			     UART_DM_CR(n));			\
		__raw_writel(UART_DM_RX_EN, UART_DM_CR(n));	\
		__raw_writel(UART_DM_TX_EN, UART_DM_CR(n));	\
	} while (0)
#define __uart_dm_uart_disable(n)				\
	do {							\
		__raw_setl(UART_DM_RX_DISABLE, UART_DM_CR(n));	\
		__raw_setl(UART_DM_RX_DISABLE, UART_DM_CR(n));	\
	} while (0)
#define __uart_dm_enable_all_irqs(n)				\
	__raw_writel(~0, UART_DM_IMR(n))
#define __uart_dm_disable_all_irqs(n)				\
	__raw_writel(0, UART_DM_IMR(n))
#define __uart_dm_disable_irqs(n, irqs)				\
	__raw_clearl(irqs, UART_DM_IMR(n))
#define __uart_dm_enable_irqs(n, irqs)				\
	__raw_setl(irqs, UART_DM_IMR(n))

#define uart_dm_read_data(n)	__raw_readb(UARTDR(n))
#define uart_dm_write_data(n, v)	__raw_writeb(v, UARTDR(n))
#define uart_dm_read_error(n)	__raw_readb(UARTRSR(n))
#define uart_dm_clear_error(n)	__raw_writeb(0, UARTECR(n))
#define uart_dm_read_dataw(n, error)		\
	(uint16_t w = __raw_readw(UARTDR(n)),	\
	  *error = (uint8_t)((w) >> 8),		\
	 (uint8_t)((w) & 0xFF))
#define uart_dm_ctrl_disable_all(n)	__raw_writew(0, UARTCR(n))
#define uart_dm_write_full(n)	(__raw_readw(UARTFR(n)) & UART_TXFF)
#define uart_dm_read_empty(n)	(__raw_readw(UARTFR(n)) & UART_RXFE)

void uart_dm_irq_init(uint8_t n);
void uart_dm_uart_init(uint8_t n);
void uart_dm_config_baudrate(uint8_t n, uint32_t baudrate);
void uart_dm_config_params(uint8_t n, uint8_t params);

#endif /* __UART_DRAGONFLY_H_INCLUDE__ */
