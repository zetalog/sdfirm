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
 * @(#)uart.h: FU540 (unleashed) specific UART defintions
 * $Id: uart.h,v 1.1 2019-10-16 15:25:00 zhenglv Exp $
 */

#ifndef __UART_UNLEASHED_H_INCLUDE__
#define __UART_UNLEASHED_H_INCLUDE__

#include <target/gpio.h>
#include <target/clk.h>

#define SIFIVE_UART_BASE(n)		(UART0_BASE + 0x1000 * (n))

#ifdef CONFIG_UNLEASHED_CON_UART0
#define UART_CON_ID			0
#endif
#ifdef CONFIG_UNLEASHED_CON_UART1
#define UART_CON_ID			1
#endif

/*===========================================================================
 * SiFive UART driver
 *===========================================================================*/

#ifndef ARCH_HAVE_UART
#define ARCH_HAVE_UART		1
#else
#error "Multiple UART controller defined"
#endif

#define UART_REG(n, offset)	(SIFIVE_UART_BASE(n) + (offset))

#define UART_TXDATA(n)		UART_REG(n, 0x00)
#define UART_RXDATA(n)		UART_REG(n, 0x04)
#define UART_TXCTRL(n)		UART_REG(n, 0x08)
#define UART_RXCTRL(n)		UART_REG(n, 0x0C)
#define UART_IE(n)		UART_REG(n, 0x10)
#define UART_IP(n)		UART_REG(n, 0x14)
#define UART_DIV(n)		UART_REG(n, 0x18)
#ifdef CONFIG_SIFIVE_UART_STATUS
#define UART_TXSTAT(n)		UART_REG(n, 0x02)
#define UART_RXSTAT(n)		UART_REG(n, 0x06)
/* TXSTAT */
#define UART_FULL		_BV(15)
/* RXSTAT */
#define UART_EMPTY		_BV(15)
#else
/* TXDATA */
#define UART_FULL		_BV(31)
/* RXDATA */
#define UART_EMPTY		_BV(31)
#endif

/* TXCTRL */
#define UART_TXEN		_BV(0)
#define UART_NSTOP		_BV(1)
#define UART_TXCNT_OFFSET	16
#define UART_TXCNT_MASK		REG_3BIT_MASK
#define UART_TXCNT(value)	_SET_FV(UART_TXCNT, value)
/* RXCTRL */
#define UART_RXEN		_BV(0)
#define UART_RXCNT_OFFSET	16
#define UART_RXCNT_MASK		REG_3BIT_MASK
#define UART_RXCNT(value)	_SET_FV(UART_RXCNT, value)
/* IE, IP */
#define UART_TXWM		_BV(0)
#define UART_RXWM		_BV(1)

#define sifive_uart_write_config(n, txcnt)			\
	__raw_writel_mask(UART_TXEN | UART_TXCNT(txcnt),	\
			  UART_TXEN |				\
			  UART_TXCNT(UART_TXCNT_MASK),		\
			  UART_TXCTRL(n))
#define sifive_uart_read_config(n, rxcnt)			\
	__raw_writel_mask(UART_RXEN | UART_RXCNT(rxcnt),	\
			  UART_RXEN |				\
			  UART_RXCNT(UART_RXCNT_MASK),		\
			  UART_RXCTRL(n))
#define __sifive_uart_enable_fifo(n, txcnt, rxcnt)		\
	do {							\
		sifive_uart_write_config(n, txcnt);		\
		sifive_uart_read_config(n, txcnt);		\
	} while (0)
#define sifive_uart_enable_fifo(n)				\
	do {							\
		__raw_setl(UART_TXEN, UART_TXCTRL(n));		\
		__raw_setl(UART_RXEN, UART_RXCTRL(n));		\
	} while (0)
#define sifive_uart_disable_fifo(n)				\
	do {							\
		__raw_clearl(UART_TXEN, UART_TXCTRL(n));	\
		__raw_clearl(UART_RXEN, UART_RXCTRL(n));	\
	} while (0)
#define sifive_uart_disable_irqs(n)				\
	__raw_writel(0, UART_IE(n))
#define sifive_uart_enable_irqs(n)				\
	__raw_writel(UART_TXWM | UART_RXWM, UART_IE(n))
#ifdef CONFIG_SIFIVE_UART_STATUS
#define sifive_uart_write_poll(n)				\
	(!(__raw_readw(UART_TXSTAT(n)) & UART_FULL))
#define sifive_uart_read_poll(n)				\
	(!(__raw_readw(UART_RXSTAT(n)) & UART_EMPTY))
#define sifive_uart_write_byte(n, byte)				\
	__raw_writeb(byte, UART_TXDATA(n))
#define sifive_uart_read_byte(n)				\
	__raw_readb(UART_RXDATA(n))
#else
#define sifive_uart_write_poll(n)				\
	(!(__raw_readl(UART_TXDATA(n)) & UART_FULL))
#define sifive_uart_read_poll(n)				\
	(sifive_uart_rx = __raw_readl(UART_RXDATA(n)),		\
	 !(sifive_uart_rx & UART_EMPTY))
#define sifive_uart_read_byte(n)				\
	(LOBYTE(sifive_uart_rx))
#define sifive_uart_write_byte(n, byte)				\
	__raw_writel((uint32_t)(byte), UART_TXDATA(n));
#endif

void sifive_uart_ctrl_init(int n, uint8_t params,
			   uint32_t freq, uint32_t baudrate);
void sifive_con_init(uint8_t params, uint32_t freq, uint32_t baudrate);

extern uint32_t sifive_uart_rx;

#ifdef CONFIG_DEBUG_PRINT
void uart_hw_dbg_init(void);
void uart_hw_dbg_start(void);
void uart_hw_dbg_stop(void);
void uart_hw_dbg_write(uint8_t byte);
void uart_hw_dbg_config(uint8_t params, uint32_t baudrate);
#endif

#ifdef CONFIG_CONSOLE
#define uart_hw_con_init()					\
	do {							\
		board_init_clock();				\
		sifive_con_init(UART_DEF_PARAMS,		\
				clk_get_frequency(tlclk),	\
				UART_CON_BAUDRATE);		\
	} while (0)
#endif
#ifdef CONFIG_CONSOLE_OUTPUT
#define uart_hw_con_write(byte)					\
	do {							\
		while (!sifive_uart_write_poll(UART_CON_ID));	\
		sifive_uart_write_byte(UART_CON_ID, byte);	\
	} while (0)
#endif
#ifdef CONFIG_CONSOLE_INPUT
#define uart_hw_con_read()	sifive_uart_read_byte(UART_CON_ID)
#define uart_hw_con_poll()	sifive_uart_read_poll(UART_CON_ID)
void uart_hw_irq_ack(void);
void uart_hw_irq_init(void);
#else
#define uart_hw_con_read()	-1
#define uart_hw_con_poll()	false
#endif
#ifdef CONFIG_MMU
#define uart_hw_mmu_init()	do { } while (0)
#endif

#endif /* __UART_UNLEASHED_H_INCLUDE__ */
