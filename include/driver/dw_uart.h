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
 * @(#)dw_uart.h: Synopsys DesignWare UART interface
 * $Id: dw_uart.h,v 1.1 2019-09-26 10:58:00 zhenglv Exp $
 */

#ifndef __DW_UART_H_INCLUDE__
#define __DW_UART_H_INCLUDE__

#include <target/arch.h>

/* Required implementation specific definitions:
 *
 * CONFIG_ARCH_HAS_DW_AMBA_ADVANCED:
 *   The architecture allows to configure DWC-APB-Advanced-Source
 * DW_UARTx_BASE: the base address of the uart register block
 *  or
 * DW_UART_REG: the algorithm used to obtain the base addresses
 * DW_UART_FREQ: the frequency of UART clock
 * UART_CON_ID: console UART ID
 * DW_UART_DBG_ID: debug print UART ID
 */

#ifdef CONFIG_DW_UART_FIFO_NONE
#define DW_UART_FIFO_MODE	0
#endif
#ifdef CONFIG_DW_UART_FIFO_16
#define DW_UART_FIFO_MODE	16
#endif
#ifdef CONFIG_DW_UART_FIFO_32
#define DW_UART_FIFO_MODE	32
#endif
#ifdef CONFIG_DW_UART_FIFO_64
#define DW_UART_FIFO_MODE	64
#endif
#ifdef CONFIG_DW_UART_FIFO_128
#define DW_UART_FIFO_MODE	128
#endif
#ifdef CONFIG_DW_UART_FIFO_256
#define DW_UART_FIFO_MODE	256
#endif
#ifdef CONFIG_DW_UART_FIFO_512
#define DW_UART_FIFO_MODE	512
#endif
#ifdef CONFIG_DW_UART_FIFO_1024
#define DW_UART_FIFO_MODE	1024
#endif
#ifdef CONFIG_DW_UART_FIFO_2048
#define DW_UART_FIFO_MODE	2048
#endif

#ifndef DW_UART_REG
#define DW_UART_REG(n, offset)	(DW_UART##n##_BASE + (offset))
#endif

#define UART_RBR(n)		DW_UART_REG(n, 0x00)
#define UART_DLL(n)		DW_UART_REG(n, 0x00)
#define UART_THR(n)		DW_UART_REG(n, 0x00)
#define UART_DLH(n)		DW_UART_REG(n, 0x04)
#define UART_IER(n)		DW_UART_REG(n, 0x04)
#define UART_IIR(n)		DW_UART_REG(n, 0x08)
#define UART_LCR(n)		DW_UART_REG(n, 0x0C)
#define UART_MCR(n)		DW_UART_REG(n, 0x10)
#define UART_LSR(n)		DW_UART_REG(n, 0x14)
#define UART_MSR(n)		DW_UART_REG(n, 0x18)
#define UART_FAR(n)		DW_UART_REG(n, 0x70)
#define UART_SCR(n)		DW_UART_REG(n, 0x1C)
#define UART_USR(n)		DW_UART_REG(n, 0x7C)
#define UART_HTX(n)		DW_UART_REG(n, 0xA4)
#define UART_DMASA(n)		DW_UART_REG(n, 0xA8)

#ifndef CONFIG_DW_UART_FIFO_NONE
#define UART_FCR(n)		DW_UART_REG(n, 0x08)
#define UART_SDMAM(n)		DW_UART_REG(n, 0x94)
#define UART_SFE(n)		DW_UART_REG(n, 0x98)
#define UART_SRT(n)		DW_UART_REG(n, 0x9C)
#define UART_STET(n)		DW_UART_REG(n, 0xA0)

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

#define dw_uart_config_fifo(n)				\
	do {						\
		__raw_writel(FCR_FIFOE |		\
			     FCR_RT(FCR_RT_CHAR_1) |	\
			     FCR_TET(FCR_TET_EMPTY) |	\
			     FCR_RESET_MASK,		\
			     UART_FCR(n));		\
	} while (0)
#else
#define dw_uart_config_fifo(n)
#endif
#ifdef CONFIG_DW_UART_SHADOW
#define UART_SRBRn(n, i)	DW_UART_REG(n, 0x30 + (i)*0x04)
#define UART_STHRn(n, i)	DW_UART_REG(n, 0x30 + (i)*0x04)
#define UART_SRR(n)		DW_UART_REG(n, 0x88)
#define UART_SRTS(n)		DW_UART_REG(n, 0x8C)
#define UART_SBCR(n)		DW_UART_REG(n, 0x90)
#endif
#ifdef CONFIG_DW_UART_FIFO_ACCESS
#define UART_TFR(n)		DW_UART_REG(n, 0x74)
#define UART_RFW(n)		DW_UART_REG(n, 0x78)
#endif
#ifdef CONFIG_DW_UART_FIFO_STAT
#define UART_TFL(n)		DW_UART_REG(n, 0x80)
#define UART_RFL(n)		DW_UART_REG(n, 0x84)
/* UART Status Register - USR */
#define USR_RFF			_BV(4) /* RX FIFO full */
#define USR_RFNE		_BV(3) /* RX FIFO not empty */
#define USR_TFE			_BV(2) /* TX FIFO empty */
#define USR_TFNF		_BV(1) /* TX FIFO not full */
#endif
#ifdef CONFIG_DW_UART_DLF
#define UART_DLF(n)		DW_UART_REG(n, 0xC0)
#endif
#ifdef CONFIG_DW_UART_ADD_ENCODED_PARAMS
#define UART_CPR(n)		DW_UART_REG(n, 0xF4)
#endif
#ifdef CONFIG_DW_UART_ADDITIONAL_FEATURES
#define UART_UCV(n)		DW_UART_REG(n, 0xF8)
#define UART_CTR(n)		DW_UART_REG(n, 0xFC)
#endif

/* Interrupt Enable Register - IER */
#define IER_PTIME		_BV(7)
#define IER_ELCOLR		_BV(4)
#define IER_EDSSI		_BV(3)
#define IER_ELSI		_BV(2)
#define IER_ETBEI		_BV(1)
#define IER_ERBFI		_BV(0)

/* Interrupt Identification Register - IIR */
#define IIR_FIFOSE_OFFSET	6
#define IIR_FIFOSE_MASK		REG_2BIT_MASK
#define IIR_FIFOSE(value)	_GET_FV(IIR_FIFOSE, value)
#define IIR_IID_OFFSET		0
#define IIR_IID_MASK		REG_4BIT_MASK
#define IIR_IID(value)		_GET_FV(IIR_IID, value)
#define UART_IRQ_DSSI		0  /* Modem status */
#define UART_IRQ_NONE		1  /* No IRQ pending */
#define UART_IRQ_TBEI		2  /* THR empty */
#define UART_IRQ_RBFI		4  /* RBR full */
#define UART_IRQ_LSI		6  /* Receiver line status */
#define UART_IRQ_BUSY		7  /* Busy detet */
#define UART_IRQ_TIMEOUT	12 /* Character timeout */

/* Modem Control Register - MCR */
#define MCR_SIRE		_BV(6)
#define MCR_AFCE		_BV(5)
#define MCR_LOOPBACK		_BV(4)
#define MCR_OUT2		_BV(3)
#define MCR_OUT1		_BV(2)
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

#ifdef CONFIG_DW_UART_SIR_MODE
#define dw_uart_irda_enable(n)		\
	__raw_setl(MCR_SIRE, UART_MCR(n))
#define dw_uart_irda_disable(n)		\
	__raw_clearl(MCR_SIRE, UART_MCR(n))
#else
#define dw_uart_irda_enable(n)
#define dw_uart_irda_disable(n)
#endif
#ifdef CONFIG_DW_UART_AFCE_MODE
#define dw_uart_16750_enable(n)		\
	__raw_setl(MCR_AFCE, UART_MCR(n))
#define dw_uart_16750_disable(n)	\
	__raw_clearl(MCR_AFCE, UART_MCR(n))
#else
#define dw_uart_16750_enable(n)		do { } while (0)
#define dw_uart_16750_disable(n)	do { } while (0)
#endif
#define dw_uart_loopback_enable(n)	\
	__raw_setl(MCR_LOOPBACK, UART_MCR(n))
#define dw_uart_loopback_disable(n)	\
	__raw_clearl(MCR_LOOPBACK, UART_MCR(n))
#define dw_uart_modem_enable(n)		\
	__raw_writel_mask(MCR_MODEM_MASK, MCR_MODEM_MASK, UART_MCR(n))
#define dw_uart_modem_disable(n)		\
	__raw_writel_mask(0, MCR_MODEM_MASK, UART_MCR(n))

#ifdef CONFIG_DW_UART_16550_COMPATIBLE
#define dw_uart_wait_busy(n)
#define dw_uart_wait_idle(n)
#else
/* UART Status Register - USR */
#define USR_BUSY		_BV(0) /* UART busy */

#define dw_uart_wait_busy(n)		\
	while (__raw_readl(UART_USR(n)) & USR_BUSY)
#define dw_uart_wait_idle(n)		\
	while (!(__raw_readl(UART_USR(n)) & USR_BUSY))
#endif

#define dw_uart_write_poll(n)		\
	(!!(__raw_readl(UART_LSR(n)) & LSR_TEMT))
#define dw_uart_read_poll(n)		\
	(!!(__raw_readl(UART_LSR(n)) & LSR_DR))
#define dw_uart_read_byte(n)		__raw_readl(UART_RBR(n))
#define dw_uart_write_byte(n, byte)	__raw_writel((byte), UART_THR(n))

#define dw_uart_disable_all_irqs(n)	__raw_writel(0, UART_IER(n))
#define dw_uart_enable_irq(n, irq)	__raw_setl(irq, UART_IER(n))
#define dw_uart_disable_irq(n, irq)	__raw_clearl(irq, UART_IER(n))

#ifdef CONFIG_CONSOLE
void dw_uart_con_init(uint32_t freq);
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
void dw_uart_con_write(uint8_t byte);
#endif
#ifdef CONFIG_CONSOLE_INPUT
uint8_t dw_uart_con_read(void);
bool dw_uart_con_poll(void);
#ifndef CONFIG_SYS_NOIRQ
void dw_uart_irq_init(void);
void dw_uart_irq_ack(void);
#endif
#endif

#endif /* __DW_UART_H_INCLUDE__ */
