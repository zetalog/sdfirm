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
 * @(#)uart_pl01x.c: prime cell uart (pl01x) implementation
 * $Id: uart_pl01x.c,v 1.279 2011-10-19 10:19:18 zhenglv Exp $
 */

#include <target/irq.h>
#include <target/uart.h>

#ifndef CONFIG_PL01X_SBSA
/* compute the baud rate divisor
 * BRDDIV = BRDi + BRDf = CLK_UART / (16 * baudrate)
 *
 * we will compute 64*BRDDIV+0.5 to get DIVINT & DIVFRAC value
 * DIVINT  = (64*BRDi+0.5) / 64
 * DIVFRAC = (64*BRDf+0.5) % 64
 *
 * DIVCFG = 64*BRDDIV+0.5
 *        = ((64 * CLK_UART * 1000) / (16 * baudrate)) + 0.5
 *        = ((CLK_UART * 64000) / (16 * baudrate)) + 0.5
 *        = ((CLK_UART * 4000) / (baudrate)) + 0.5
 *        = (((CLK_UART * 20) / (baudrate/400)) + 1) / 2
 */
/* Since fractional part is based on the 64 (2^6) times of the BRD,
 * 6 is define as FBRD offset here.
 */
#define PL01X_BRDDIV_OFFSET	6
#define PL01X_BRDDIV_MASK	((1<<PL01X_BRDDIV_OFFSET)-1)

void pl01x_config_baudrate(uint8_t n, uint32_t src_clk, uint32_t baudrate)
{
	uint32_t cfg;

	cfg = (1 + (((src_clk / 1000) * 20) / (baudrate / 400))) / 2;
	__raw_writew(cfg >> PL01X_BRDDIV_OFFSET, UARTIBRD(n));
	__raw_writeb(cfg  & PL01X_BRDDIV_MASK, UARTFBRD(n));
}

void pl01x_config_params(uint8_t n, uint8_t params)
{
	uint8_t cfg;

	cfg = UART_WLEN(uart_bits(params)-5);
	switch (uart_parity(params)) {
	case UART_PARITY_EVEN:
		cfg |= UART_EPS;
	case UART_PARITY_ODD:
		cfg |= UART_PEN;
		break;
	}
	if (uart_stopb(params))
		cfg |= UART_STP2;
	__raw_writeb_mask(cfg, 0xEE, UARTLCR_H(n));
}
#endif

void pl01x_write_byte(uint8_t byte)
{
	uint8_t n = UART_CON_ID;

	while (pl01x_write_full(n));
	pl01x_write_data(n, byte);
	while (!pl01x_write_empty(n));
}

boolean pl01x_read_poll(void)
{
	return !pl01x_read_empty(UART_CON_ID);
}

uint8_t pl01x_read_byte(void)
{
	return pl01x_read_data(UART_CON_ID);
}

static void pl01x_handle_irq(void)
{
	irq_t uart_irq = UART_CON_IRQ;

	irqc_disable_irq(uart_irq);
	pl01x_mask_irq(UART_CON_ID, UART_RXI);
	/* TODO: handle RX IRQ:
	 * debug_handle_irq();
	 */
}

void pl01x_irq_ack(void)
{
	irq_t uart_irq = UART_CON_IRQ;

	pl01x_unmask_irq(UART_CON_ID, UART_RXI);
	irqc_enable_irq(uart_irq);
}

void pl01x_irq_init(void)
{
	irq_t uart_irq = UART_CON_IRQ;

	pl01x_disable_all_irqs(UART_CON_ID);
	irqc_configure_irq(uart_irq, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(uart_irq, pl01x_handle_irq);
	irqc_enable_irq(uart_irq);
	pl01x_enable_irq(UART_CON_ID, UART_RXI);
}

void pl01x_irq_exit(void)
{
	irq_t uart_irq = UART_CON_IRQ;

	pl01x_disable_all_irqs(UART_CON_ID);
	irqc_disable_irq(uart_irq);
	irqc_configure_irq(uart_irq, GIC_PRIORITY_IDLE, IRQ_LEVEL_TRIGGERED);
}
