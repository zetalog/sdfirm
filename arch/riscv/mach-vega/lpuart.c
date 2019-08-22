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
 * @(#)lpuart.c: RV32M1 (VEGA) low power UART implementation
 * $Id: lpuart.c,v 1.1 2019-08-16 23:02:00 zhenglv Exp $
 */

#include <target/uart.h>
#include <target/gpio.h>
#include <target/clk.h>

#ifdef CONFIG_LPUART_FIFO
static void lpuart_enable_fifo_irqs(lpuart_irq_t irqs)
{
	uint32_t fifo1;

	fifo1 = LPUART_GET_FIFOI1(irqs);
	/* unconditionally enabled: LPUART_GET_FIFOI2(irqs) */
	__raw_writel_mask(LPUART_FIFOIE1(fifo1),
			  LPUART_FIFOIE1(LPUART_FIFOIE1_MASK),
			  LPUART_FIFO(UART_CON_ID));
}

static void lpuart_disable_fifo_irqs(void)
{
	__raw_writel_mask(LPUART_FIFOIE1(0),
			  LPUART_FIFOIE1(LPUART_FIFOIE1_MASK),
			  LPUART_FIFO(UART_CON_ID));
}

static lpuart_irq_t lpuart_fifo_irqs_status(void)
{
	uint32_t fifo1, fifo2;
	lpuart_irq_t irqs = __raw_readl(LPUART_FIFO(UART_CON_ID));

	fifo1 = LPUART_FIFOIF1(irqs);
	fifo2 = LPUART_FIFOIF2(irqs);
	return LPUART_SET_FIFOI1(fifo1) | LPUART_SET_FIFOI2(fifo2);
}

static void lpuart_clear_fifo_irqs(lpuart_irq_t irqs)
{
	caddr_t fifo = LPUART_FIFO(UART_CON_ID);

	if (irqs & LPUART_RXUFI)
		__raw_clearl(LPUART_RXUFE, fifo);
	if (irqs & LPUART_TXOFI)
		__raw_clearl(LPUART_TXOFE, fifo);
}

static void lpuart_fifo_init(void)
{
	__raw_setl(LPUART_TXFE, LPUART_FIFO(UART_CON_ID));
	__raw_setl(LPUART_RXFE, LPUART_FIFO(UART_CON_ID));
}
#else
#define lpuart_enable_fifo_irqs(irqs)	do { } while (0)
#define lpuart_disable_fifo_irqs()	do { } while (0)
#define lpuart_fifo_irqs_status()	0
#define lpuart_clear_fifo_irqs(irqs)	do { } while (0)

static void lpuart_fifo_init(void)
{
	__raw_clearl(LPUART_TXFE, LPUART_FIFO(UART_CON_ID));
	__raw_clearl(LPUART_RXFE, LPUART_FIFO(UART_CON_ID));
}
#endif

void lpuart_enable_irqs(lpuart_irq_t irqs)
{
	uint32_t baud;
	uint32_t ctrl1, ctrl2, ctrl3;

	baud = LPUART_GET_BAUDI1(irqs);
	__raw_writel_mask(LPUART_BAUDIE(baud),
			  LPUART_BAUDIE(LPUART_BAUDIE_MASK),
			  LPUART_BAUD(UART_CON_ID));
	ctrl1 = LPUART_GET_CTRLI1(irqs);
	ctrl2 = LPUART_GET_CTRLI2(irqs);
	ctrl3 = LPUART_GET_CTRLI3(irqs);
	__raw_writel_mask(LPUART_CTRLIE1(ctrl1) |
			  LPUART_CTRLIE2(ctrl2) |
			  LPUART_CTRLIE3(ctrl3),
			  LPUART_CTRLIE1(LPUART_CTRLIE1_MASK) |
			  LPUART_CTRLIE2(LPUART_CTRLIE2_MASK) |
			  LPUART_CTRLIE3(LPUART_CTRLIE3_MASK),
			  LPUART_CTRL(UART_CON_ID));
	lpuart_enable_fifo_irqs(irqs);
}

void lpuart_disable_irqs(void)
{
	__raw_writel_mask(LPUART_BAUDIE(0),
			  LPUART_BAUDIE(LPUART_BAUDIE_MASK),
			  LPUART_BAUD(UART_CON_ID));
	__raw_writel_mask(LPUART_CTRLIE1(0) |
			  LPUART_CTRLIE2(0) |
			  LPUART_CTRLIE3(0),
			  LPUART_CTRLIE1(LPUART_CTRLIE1_MASK) |
			  LPUART_CTRLIE2(LPUART_CTRLIE2_MASK) |
			  LPUART_CTRLIE3(LPUART_CTRLIE3_MASK),
			  LPUART_CTRL(UART_CON_ID));
	lpuart_disable_fifo_irqs();
}

lpuart_irq_t lpuart_irqs_status(void)
{
	uint32_t baud;
	uint32_t ctrl1, ctrl2, ctrl3;
	lpuart_irq_t irqs = __raw_readl(LPUART_STAT(UART_CON_ID));

	baud = LPUART_BAUDIF(irqs);
	ctrl1 = LPUART_CTRLIF1(irqs);
	ctrl2 = LPUART_CTRLIF2(irqs);
	ctrl3 = LPUART_CTRLIF3(irqs);
	return LPUART_SET_BAUDI1(baud) | LPUART_SET_CTRLI1(ctrl1) |
	       LPUART_SET_CTRLI2(ctrl2) | LPUART_SET_CTRLI3(ctrl3) |
	       lpuart_fifo_irqs_status();
}

void lpuart_clear_irqs(lpuart_irq_t irqs)
{
	caddr_t stat = LPUART_STAT(UART_CON_ID);

	if (irqs & LPUART_MA2I)
		__raw_clearl(LPUART_MA2F, stat);
	if (irqs & LPUART_MA1I)
		__raw_clearl(LPUART_MA1F, stat);
	if (irqs & LPUART_FEI)
		__raw_clearl(LPUART_FE, stat);
	if (irqs & LPUART_ORI)
		__raw_clearl(LPUART_OR, stat);
	if (irqs & LPUART_ILI)
		__raw_clearl(LPUART_IDLE, stat);
	if (irqs & LPUART_RXEDGI)
		__raw_clearl(LPUART_RXEDGIF, stat);
	if (irqs & LPUART_LBKDI)
		__raw_clearl(LPUART_LBKDIF, stat);
	lpuart_clear_fifo_irqs(irqs);
}

void lpuart_write_byte(uint8_t byte)
{
	while (!(lpuart_irqs_status() & LPUART_TDREI));
	__raw_writel(byte, LPUART_DATA(UART_CON_ID));
}

#ifdef CONFIG_LPUART_7BIT
uint8_t lpuart_read_byte(void)
{
	uint32_t ctrl = __raw_readl(LPUART_CTRL(UART_CON_ID));

	if ((ctrl & LPUART_M7) ||
	    ((!(ctrl & LPUART_M7)) && (!(ctrl & LPUART_M)) &&
	     (ctrl & LPUART_PE)))
		return __raw_readl(LPUART_DATA(UART_CON_ID)) & 0x7F;
	else
		return __raw_readl(LPUART_DATA(UART_CON_ID));
}
#else
uint8_t lpuart_read_byte(void)
{
	return __raw_readl(LPUART_DATA(UART_CON_ID));
}
#endif

bool lpuart_ctrl_poll(void)
{
	return !!(lpuart_irqs_status() & LPUART_RDRF);
}

void lpuart_ctrl_init(void)
{
	clk_enable(portc_clk);
	gpio_config_mux(3, 7, PTC7_MUX_LPUART0_RX);
	gpio_config_mux(3, 8, PTC8_MUX_LPUART0_TX);
	lpuart_fifo_init();
}
