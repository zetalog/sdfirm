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
 * @(#)ns16550.c: National semi 16550 UART interface
 * $Id: ns16550.c,v 1.1 2020-06-20 11:48:00 zhenglv Exp $
 */

#ifndef __NS16550_UART_H_INCLUDE__
#define __NS16550_UART_H_INCLUDE__

#include <target/arch.h>

#ifndef NS16550_IER
#define NS16550_IER  0x00
#endif /* NS16550_IER */

/*
 * Note that the following macro magic uses the fact that the compiler
 * will not allocate storage for arrays of size 0
 */
#ifndef NS16550_REG_SIZE
/*
 * For driver model we always use one byte per register, and sort out the
 * differences in the driver
 */
#define NS16550_REG_SIZE (-1)
#endif

#if !defined(NS16550_REG_SIZE) || (NS16550_REG_SIZE == 0)
#error "Please define NS16550 registers size."
#elif defined(CONFIG_ARCH_HAS_NS16550_MEM32)
#define UART_REG2(n, offset)		(UART_BASE(n) + (offset) << 2)
#define uart_reg_write(v, reg)		__raw_writel(v, reg)
#define uart_reg_read(reg)		__raw_readl(reg)
#define UART_REG(x)			uint32_t x
#elif (NS16550_REG_SIZE == -4)
#define UART_REG2(n, offset)		(UART_BASE(n) + (offset) << 2)
#define uart_reg_write(v, reg)		__raw_writeb(v, reg)
#define uart_reg_read(reg)		__raw_readb(reg)
#define UART_REG(x)			\
	uint8_t x;			\
	uint8_t postpad_##x[3];
#elif (NS16550_REG_SIZE == -2)
#define UART_REG2(n, offset)		(UART_BASE(n) + (offset) << 1)
#define uart_reg_write(v, reg)		__raw_writeb(v, reg)
#define uart_reg_read(reg)		__raw_readb(reg)
#define UART_REG(x)			\
	uint8_t x;			\
	uint8_t postpad_##x;
#elif (NS16550_REG_SIZE == 4)
#define UART_REG2(n, offset)		(UART_BASE(n) + (offset) << 2)
#define uart_reg_write(v, reg)		__raw_writeb(v, reg)
#define uart_reg_read(reg)		__raw_readb(reg)
#define UART_REG(x)			\
	uint8_t prepad_##x[3];		\
	uint8_t x;
#elif (NS16550_REG_SIZE == 2)
#define UART_REG2(n, offset)		(UART_BASE(n) + (offset) << 1)
#define uart_reg_write(v, reg)		__raw_writeb(v, reg)
#define uart_reg_read(reg)		__raw_readb(reg)
#define UART_REG(x)			\
	uint8_t prepad_##x;		\
	uint8_t x;
#else /* (NS16550_REG_SIZE == -1) || (NS16550_REG_SIZE == 1) */
#define UART_REG2(n, offset)		(UART_BASE(n) + (offset))
#define uart_reg_write(v, reg)		__raw_writeb(v, reg)
#define uart_reg_read(reg)		__raw_readb(reg)
#define UART_REG(x)			uint8_t x
#endif

#define UART_RBR(n)		UART_REG2(n, 0)
#define UART_IER(n)		UART_REG2(n, 1)
#define UART_FCR(n)		UART_REG2(n, 2)
#define UART_LCR(n)		UART_REG2(n, 3)
#define UART_MCR(n)		UART_REG2(n, 4)
#define UART_LSR(n)		UART_REG2(n, 5)
#define UART_MSR(n)		UART_REG2(n, 6)
#define UART_SPR(n)		UART_REG2(n, 7)
#define UART_MDR1(n)		UART_REG2(n, 8)
#define UART_UASR(n)		UART_REG2(n, 15)
#define UART_SCR(n)		UART_REG2(n, 16)
#define UART_SSR(n)		UART_REG2(n, 17)
#define UART_THR(n)		UART_RBR(n)
#define UART_IIR(n)		UART_FCR(n)
#define UART_DLL(n)		UART_RBR(n)
#define UART_DLM(n)		UART_IER(n)

/*
 * These are the definitions for the FIFO Control Register
 */
#define UART_FCR_FIFO_EN	0x01 /* Fifo enable */
#define UART_FCR_CLEAR_RCVR	0x02 /* Clear the RCVR FIFO */
#define UART_FCR_CLEAR_XMIT	0x04 /* Clear the XMIT FIFO */
#define UART_FCR_DMA_SELECT	0x08 /* For DMA applications */
#define UART_FCR_TRIGGER_MASK	0xC0 /* Mask for the FIFO trigger range */
#define UART_FCR_TRIGGER_1	0x00 /* Mask for trigger set at 1 */
#define UART_FCR_TRIGGER_4	0x40 /* Mask for trigger set at 4 */
#define UART_FCR_TRIGGER_8	0x80 /* Mask for trigger set at 8 */
#define UART_FCR_TRIGGER_14	0xC0 /* Mask for trigger set at 14 */

#define UART_FCR_RXSR		0x02 /* Receiver soft reset */
#define UART_FCR_TXSR		0x04 /* Transmitter soft reset */

/* Ingenic JZ47xx specific UART-enable bit. */
#define UART_FCR_UME		0x10

/* Clear & enable FIFOs */
#define UART_FCR_DEFVAL (UART_FCR_FIFO_EN | \
			UART_FCR_RXSR |	\
			UART_FCR_TXSR)

/*
 * These are the definitions for the Modem Control Register
 */
#define UART_MCR_DTR	0x01		/* DTR   */
#define UART_MCR_RTS	0x02		/* RTS   */
#define UART_MCR_OUT1	0x04		/* Out 1 */
#define UART_MCR_OUT2	0x08		/* Out 2 */
#define UART_MCR_LOOP	0x10		/* Enable loopback test mode */
#define UART_MCR_AFE	0x20		/* Enable auto-RTS/CTS */

#define UART_MCR_DMA_EN	0x04
#define UART_MCR_TX_DFR	0x08

/*
 * These are the definitions for the Line Control Register
 *
 * Note: if the word length is 5 bits (UART_LCR_WLEN5), then setting
 * UART_LCR_STOP will select 1.5 stop bits, not 2 stop bits.
 */
#define UART_LCR_WLS_MSK 0x03		/* character length select mask */
#define UART_LCR_WLS_5	0x00		/* 5 bit character length */
#define UART_LCR_WLS_6	0x01		/* 6 bit character length */
#define UART_LCR_WLS_7	0x02		/* 7 bit character length */
#define UART_LCR_WLS_8	0x03		/* 8 bit character length */
#define UART_LCR_STB	0x04		/* # stop Bits, off=1, on=1.5 or 2) */
#define UART_LCR_PEN	0x08		/* Parity eneble */
#define UART_LCR_EPS	0x10		/* Even Parity Select */
#define UART_LCR_STKP	0x20		/* Stick Parity */
#define UART_LCR_SBRK	0x40		/* Set Break */
#define UART_LCR_BKSE	0x80		/* Bank select enable */
#define UART_LCR_DLAB	0x80		/* Divisor latch access bit */

/*
 * These are the definitions for the Line Status Register
 */
#define UART_LSR_DR	0x01		/* Data ready */
#define UART_LSR_OE	0x02		/* Overrun */
#define UART_LSR_PE	0x04		/* Parity error */
#define UART_LSR_FE	0x08		/* Framing error */
#define UART_LSR_BI	0x10		/* Break */
#define UART_LSR_THRE	0x20		/* Xmit holding register empty */
#define UART_LSR_TEMT	0x40		/* Xmitter empty */
#define UART_LSR_ERR	0x80		/* Error */

#define UART_MSR_DCD	0x80		/* Data Carrier Detect */
#define UART_MSR_RI	0x40		/* Ring Indicator */
#define UART_MSR_DSR	0x20		/* Data Set Ready */
#define UART_MSR_CTS	0x10		/* Clear to Send */
#define UART_MSR_DDCD	0x08		/* Delta DCD */
#define UART_MSR_TERI	0x04		/* Trailing edge ring indicator */
#define UART_MSR_DDSR	0x02		/* Delta DSR */
#define UART_MSR_DCTS	0x01		/* Delta CTS */

/*
 * These are the definitions for the Interrupt Identification Register
 */
#define UART_IIR_NO_INT	0x01	/* No interrupts pending */
#define UART_IIR_ID	0x06	/* Mask for the interrupt ID */

#define UART_IIR_MSI	0x00	/* Modem status interrupt */
#define UART_IIR_THRI	0x02	/* Transmitter holding register empty */
#define UART_IIR_RDI	0x04	/* Receiver data interrupt */
#define UART_IIR_RLSI	0x06	/* Receiver line status interrupt */

/*
 * These are the definitions for the Interrupt Enable Register
 */
#define UART_IER_MSI	0x08	/* Enable Modem status interrupt */
#define UART_IER_RLSI	0x04	/* Enable receiver line status interrupt */
#define UART_IER_THRI	0x02	/* Enable Transmitter holding register int. */
#define UART_IER_RDI	0x01	/* Enable receiver data interrupt */

/* useful defaults for LCR */
#define UART_LCR_8N1	0x03

#define UART_LCRVAL UART_LCR_8N1		/* 8 data, 1 stop, no parity */
#define UART_MCRVAL (UART_MCR_DTR | \
		     UART_MCR_RTS)		/* RTS/DTR */

void NS16550_init(int com_port, int baud_divisor);
void NS16550_putc(int com_port, char c);
char NS16550_getc(int com_port);
int NS16550_tstc(int com_port);
void NS16550_reinit(int com_port, int baud_divisor);

/**
 * ns16550_calc_divisor() - calculate the divisor given clock and baud rate
 *
 * Given the UART input clock and required baudrate, calculate the divisor
 * that should be used.
 *
 * @port:	UART port
 * @clock:	UART input clock speed in Hz
 * @baudrate:	Required baud rate
 * @return baud rate divisor that should be used
 */
int ns16550_calc_divisor(int port, int clock, int baudrate);

void ns16550_con_init(void);
void ns16550_con_write(uint8_t byte);
uint8_t ns16550_con_read(void);
bool ns16550_con_poll(void);

#endif /* __NS16550_UART_H_INCLUDE__ */
