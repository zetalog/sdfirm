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
 * @(#)dw_uart.c: Synopsys DesignWare UART implementation
 * $Id: dw_uart.c,v 1.1 2019-09-26 11:00:00 zhenglv Exp $
 */
#include <target/console.h>
#include <target/uart.h>
#include <target/barrier.h>
#include <target/irq.h>

#ifdef CONFIG_DW_UART_DLF
#define dw_uart_convert_baudrate(freq, baud, div, frac)	\
	do {						\
		div = 0;				\
		frac = 0;				\
	} while (0)
#define dw_uart_config_frac(n, frac)			\
	__raw_writel(frac, UART_DLF(n))
#else
#define dw_uart_convert_baudrate(freq, baud, div, frac)	\
	do {						\
		div = (uint16_t)div32u(freq,		\
				       (baud) << 4);	\
		frac = 0;				\
	} while (0)
#define dw_uart_config_frac(n, frac)			\
	do { __unused uint8_t __once = READ_ONCE(frac); } while (0)
#endif

uint8_t dw_uart_convert_params(uint8_t params)
{
	uint8_t cfg;

	cfg = LCR_DLS(LCR_DLS_CHAR_BITS(uart_bits(params)));
	switch (uart_parity(params)) {
	case UART_PARITY_EVEN:
		cfg |= LCR_EPS;
	case UART_PARITY_ODD:
		cfg |= LCR_PEN;
	break;
	}
	if (uart_stopb(params))
		cfg |= LCR_STOP;
	return cfg;
}

#ifdef CONFIG_CONSOLE
void dw_uart_con_init(uint32_t freq)
{
	uint16_t divisor;
	uint8_t fraction;
	bool latched = false;

	/* Configure UART mode */
	dw_uart_modem_disable(UART_CON_ID);
	dw_uart_loopback_disable(UART_CON_ID);
	dw_uart_16750_disable(UART_CON_ID);
	dw_uart_irda_disable(UART_CON_ID);

	while (!latched) {
		__raw_setl(LCR_DLAB, UART_LCR(UART_CON_ID));
		latched = !!dw_uart_is_baud(UART_CON_ID);
	}
	/* Configure baudrate */
	dw_uart_convert_baudrate(freq, UART_CON_BAUDRATE,
				 divisor, fraction);
	__raw_writel(LOBYTE(divisor), UART_DLL(UART_CON_ID));
	__raw_writel(HIBYTE(divisor), UART_DLH(UART_CON_ID));
	dw_uart_config_frac(UART_CON_ID, fraction);
	while (latched) {
		__raw_clearl(LCR_DLAB, UART_LCR(UART_CON_ID));
		latched = !!dw_uart_is_baud(UART_CON_ID);
	}
	/* Configure parameters */
	__raw_writel(dw_uart_convert_params(UART_DEF_PARAMS),
		     UART_LCR(UART_CON_ID));
	/* Configure FIFO */
	dw_uart_config_fifo(UART_CON_ID);
}
#endif

#ifdef CONFIG_CONSOLE_OUTPUT
void dw_uart_con_write(uint8_t byte)
{
	while (!dw_uart_write_poll(UART_CON_ID));
	dw_uart_write_byte(UART_CON_ID, byte);
}
#endif

#ifdef CONFIG_CONSOLE_INPUT
uint8_t dw_uart_con_read(void)
{
	return dw_uart_read_byte(UART_CON_ID);
}

bool dw_uart_con_poll(void)
{
	return dw_uart_read_poll(UART_CON_ID);
}

#ifndef CONFIG_SYS_NOIRQ
#define dw_uart_con_irq_id()	IIR_IID(__raw_readl(UART_IIR(UART_CON_ID)))

void dw_uart_handle_irq(irq_t irq)
{
	uint8_t uirq;

	uirq = dw_uart_con_irq_id();
	if (uirq != UART_IRQ_RBFI)
		return;

	irqc_mask_irq(UART_CON_IRQ);
	dw_uart_disable_irq(UART_CON_ID, IER_ERBFI);
	console_handle_irq();
}

void dw_uart_irq_init(void)
{
	dw_uart_disable_all_irqs(UART_CON_ID);
	irqc_configure_irq(UART_CON_IRQ, 0, IRQ_LEVEL_TRIGGERED);
	irq_register_vector(UART_CON_IRQ, dw_uart_handle_irq);
	irqc_enable_irq(UART_CON_IRQ);
	dw_uart_enable_irq(UART_CON_ID, IER_ERBFI);
}

void dw_uart_irq_ack(void)
{
	dw_uart_enable_irq(UART_CON_ID, IER_ERBFI);
	irqc_ack_irq(UART_CON_IRQ);
	irqc_unmask_irq(UART_CON_IRQ);
}
#endif
#endif
