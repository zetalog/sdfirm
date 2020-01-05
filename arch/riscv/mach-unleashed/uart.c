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
 * @(#)uart.c: FU540 (unleashed) specific UART implementation
 * $Id: uart.c,v 1.1 2019-10-16 15:43:00 zhenglv Exp $
 */

#include <target/uart.h>

#ifndef CONFIG_SIFIVE_UART_STATUS
uint32_t sifive_uart_rx;
#endif

static bool sifive_uart_con_init;

void sifive_con_init(uint8_t params, uint32_t freq, uint32_t baudrate)
{
	if (sifive_uart_con_init)
		return;

	sifive_uart_ctrl_init(UART_CON_ID, params, freq, baudrate);
	sifive_uart_con_init = true;
}

/*          Fin
 * Fbaud = -------
 *         div + 1
 * Thus UART_DIV = (Fin / Fbaud) - 1
 */
static uint32_t sifive_uart_min_div(uint32_t freq, uint32_t baudrate)
{
	uint32_t quotient;

	/* The nearest integer for UART_DIV requires rounding up as to not
	 * exceed baudrate.
	 *  div = ceil(Fin / Fbaud) - 1
	 *      = floor((Fin - 1 + Fbaud) / Fbaud) - 1
	 * This should not overflow as long as (Fin - 1 + Fbaud) does not
	 * exceed 2^32 - 1.
	 */
	quotient = div32u(freq + baudrate - 1, baudrate);
	/* Avoid underflow */
	return quotient ? quotient - 1 : 0;
}

void sifive_uart_ctrl_init(int n, uint8_t params,
			   uint32_t freq, uint32_t baudrate)
{
	uint32_t div;

	div = sifive_uart_min_div(freq, baudrate);
	if (uart_stopb(params))
		__raw_setl(UART_NSTOP, UART_TXCTRL(n));
	__raw_writel(div, UART_DIV(n));
	__sifive_uart_enable_fifo(n, 0, 0);
}
