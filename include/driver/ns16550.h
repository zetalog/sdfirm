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
 * @(#)ns16550.h: National semiconductor 16550 UART interface
 * $Id: ns16550.h,v 1.1 2019-10-17 21:53:00 zhenglv Exp $
 */

#ifndef __NS16550_H_INCLUDE__
#define __NS16550_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

/* Required implementation specific definitions:
 *
 * UARTx_BASE: the base address of the uart register block
 *  or
 * UART_REG: the algorithm used to obtain the base addresses
 * UART_FREQ: the frequency of UART clock
 * UART_CON_ID: console UART ID
 * UART_DBG_ID: debug print UART ID
 */

#ifdef CONFIG_NS16550_FIFO_NONE
#define UART_FIFO_MODE	0
#endif
#ifdef CONFIG_NS16550_FIFO_16
#define UART_FIFO_MODE	16
#endif
#ifdef CONFIG_NS16550_FIFO_32
#define UART_FIFO_MODE	32
#endif
#ifdef CONFIG_NS16550_FIFO_64
#define UART_FIFO_MODE	64
#endif
#ifdef CONFIG_NS16550_FIFO_128
#define UART_FIFO_MODE	128
#endif
#ifdef CONFIG_NS16550_FIFO_256
#define UART_FIFO_MODE	256
#endif
#ifdef CONFIG_NS16550_FIFO_512
#define UART_FIFO_MODE	512
#endif
#ifdef CONFIG_NS16550_FIFO_1024
#define UART_FIFO_MODE	1024
#endif
#ifdef CONFIG_NS16550_FIFO_2048
#define UART_FIFO_MODE	2048
#endif

#ifndef UART_REG
#define UART_REG(n, offset)	(UART##n##_BASE + (offset))
#endif

#define UART_RBR(n)		UART_REG(n, 0x00)
#define UART_DLL(n)		UART_REG(n, 0x00)
#define UART_THR(n)		UART_REG(n, 0x00)
#define UART_DLH(n)		UART_REG(n, 0x04)
#define UART_IER(n)		UART_REG(n, 0x04)
#define UART_IIR(n)		UART_REG(n, 0x08)
#define UART_LCR(n)		UART_REG(n, 0x0C)
#define UART_MCR(n)		UART_REG(n, 0x10)
#define UART_LSR(n)		UART_REG(n, 0x14)
#define UART_MSR(n)		UART_REG(n, 0x18)
#define UART_SCR(n)		UART_REG(n, 0x1C)
#define UART_USR(n)		UART_REG(n, 0x7C)
#define UART_HTX(n)		UART_REG(n, 0xA4)

#ifndef CONFIG_UART_FIFO_NONE
#define UART_FCR(n)		UART_REG(n, 0x08)
#define UART_TFL(n)		UART_REG(n, 0x80)
#define UART_RFL(n)		UART_REG(n, 0x84)

/* FIFO Control Register - FCR */
#define FCR_RT_OFFSET		6
#define FCR_RT_MASK		REG_2BIT_MASK
#define FCR_RT(value)		_SET_FV(FCR_RT, value)
#define FCR_RT_CHAR_1		0
#define FCR_RT_QUARTER_FULL	1
#define FCR_RT_HAFL_FULL	2
#define FCR_RT_2		3
#define FCR_TET_OFFSET		4
#define FCR_TET_MASK		REG_2BIT_MASK
#define FCR_TET(value)		_SET_FV(FCR_TET, value)
#define FCR_TET_EMPTY		0
#define FCR_TET_CHAR_2		1
#define FCR_TET_QUARTER_FULL	2
#define FCR_TET_HALF_FULL	3
#define FCR_DMAM		_BV(3)
#define FCR_XFIFOR		_BV(2)
#define FCR_RFIFOR		_BV(1)
#define FCR_FIFOE		_BV(0)
#define FCR_RESET_MASK		(FCR_XFIFOR | FCR_RFIFOR)

/* UART Status Register - USR */
#define USR_RFF			_BV(4) /* RX FIFO full */
#define USR_RFNE		_BV(3) /* RX FIFO not empty */
#define USR_TFE			_BV(2) /* TX FIFO empty */
#define USR_TFNF		_BV(1) /* TX FIFO not full */

#define ns16550_config_fifo(n)				\
	do {						\
		__raw_writel(FCR_FIFOE |		\
			     FCR_RT(FCR_RT_CHAR_1) |	\
			     FCR_TET(FCR_TET_EMPTY),	\
			     UART_FCR(n));		\
	} while (0)
#define ns16550_reset_fifo(n)				\
	__raw_writel_mask(FCR_RESET_MASK, FCR_RESET_MASK, UART_FCR(n))
#else
#define ns16550_config_fifo(n)
#define ns16550_reset_fifo(n)
#endif

/* Modem Control Register - MCR */
#define MCR_SIRE		_BV(6)
#define MCR_AFCE		_BV(5)
#define MCR_LOOPBACK		_BV(4)
#define MCR_RTS			_BV(1)
#define MCR_DTR			_BV(0)
/* Line Control Register - LCR */
#define LCR_DLAB		_BV(7) /* Divisor access latch */
#define LCR_BC			_BV(6) /* Break control */
#define LCR_SP			_BV(5) /* Stick parity */
#define LCR_EPS			_BV(4) /* Even parity select */
#define LCR_PEN			_BV(3) /* Parity enable */
/* 0: 1 stop bit
 * 1: 1.5 stop bit if 5 data bits, otherwise 2 stop bit
 */
#define LCR_STOP		_BV(2) /* Stop bit */
#define LCR_DLS_OFFSET		0
#define LCR_DLS_MASK		REG_2BIT_MASK
#define LCR_DLS(value)		_SET_FV(LCR_DLS, value)
#define LCR_DLS_CHAR_BITS(bit)	((bit) - 5)
/* Line Status Register - LSR */
#define LSR_RFE			_BV(7)
#define LSR_TEMT		_BV(6)
#define LSR_THRE		_BV(5)
#define LSR_BI			_BV(4)
#define LSR_FE			_BV(3)
#define LSR_PE			_BV(2)
#define LSR_OE			_BV(1)
#define LSR_DR			_BV(0)

#define MCR_MODEM_MASK		(MCR_RTS | MCR_DTR)

#ifdef CONFIG_UART_SIR_MODE
#define ns16550_irda_enable(n)		\
	__raw_setl(MCR_SIRE, UART_MCR(n))
#define ns16550_irda_disable(n)		\
	__raw_clearl(MCR_SIRE, UART_MCR(n))
#else
#define ns16550_irda_enable(n)
#define ns16550_irda_disable(n)
#endif
#define ns16550_loopback_enable(n)	\
	__raw_setl(MCR_LOOPBACK, UART_MCR(n))
#define ns16550_loopback_disable(n)	\
	__raw_clearl(MCR_LOOPBACK, UART_MCR(n))
#define ns16550_modem_enable(n)		\
	__raw_writel_mask(MCR_MODEM_MASK, MCR_MODEM_MASK, UART_MCR(n))
#define ns16550_modem_disable(n)		\
	__raw_writel_mask(0, MCR_MODEM_MASK, UART_MCR(n))

#ifdef CONFIG_NS16540
/* UART Status Register - USR */
#define USR_BUSY		_BV(0) /* UART busy */

#define ns16550_wait_busy(n)		\
	while (__raw_readl(UART_USR(n)) & USR_BUSY)
#define ns16550_wait_idle(n)		\
	while (!(__raw_readl(UART_USR(n)) & USR_BUSY))
#else
#define ns16550_wait_busy(n)
#define ns16550_wait_idle(n)
#endif
#ifdef CONFIG_NS16750
#define ns16550_16750_enable(n)		\
	__raw_setl(MCR_AFCE, UART_MCR(n))
#define ns16550_16750_disable(n)	\
	__raw_clearl(MCR_AFCE, UART_MCR(n))
#else
#define ns16550_16750_enable(n)		do { } while (0)
#define ns16550_16750_disable(n)	do { } while (0)
#endif

#define ns16550_write_poll(n)		\
	(!!(__raw_readl(UART_LSR(n)) & LSR_TEMT))
#define ns16550_read_poll(n)		\
	(!!(__raw_readl(UART_LSR(n)) & LSR_DR))
#define ns16550_read_byte(n)		__raw_readl(UART_RBR(n))
#define ns16550_write_byte(n, byte)	__raw_writel((byte), UART_THR(n))

#ifdef CONFIG_CONSOLE
void ns16550_con_init(void);
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
void ns16550_con_write(uint8_t byte);
#endif
#ifdef CONFIG_CONSOLE_INPUT
uint8_t ns16550_con_read(void);
bool ns16550_con_poll(void);
#endif

#endif /* __UART_H_INCLUDE__ */
