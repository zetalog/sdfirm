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
 * @(#)ns16550.c: National semi 16550 UART implementation
 * $Id: ns16550.c,v 1.1 2020-06-20 11:47:00 zhenglv Exp $
 */

#include <target/console.h>

/*
 * Divide positive or negative dividend by positive divisor and round
 * to closest integer. Result is undefined for negative divisors and
 * for negative dividends if the divisor variable type is unsigned.
 */
#define DIV_ROUND_CLOSEST(x, divisor)(			\
{							\
	typeof(x) __x = x;				\
	typeof(divisor) __d = divisor;			\
	(((typeof(x))-1) > 0 ||				\
	 ((typeof(divisor))-1) > 0 || (__x) > 0) ?	\
		(((__x) + ((__d) / 2)) / (__d)) :	\
		(((__x) - ((__d) / 2)) / (__d));	\
}							\
)

static uint32_t ns16550_getfcr(int port)
{
	return UART_FCR_DEFVAL;
}

int ns16550_calc_divisor(int port, int clock, int baudrate)
{
	const unsigned int mode_x_div = 16;

	return DIV_ROUND_CLOSEST(clock, mode_x_div * baudrate);
}

static void NS16550_setbrg(int com_port, int baud_divisor)
{
	uart_reg_write(UART_LCR_BKSE | UART_LCRVAL, UART_LCR(com_port));
	uart_reg_write(baud_divisor & 0xff, UART_DLL(com_port));
	uart_reg_write((baud_divisor >> 8) & 0xff, UART_DLM(com_port));
	uart_reg_write(UART_LCRVAL, UART_LCR(com_port));
}

void NS16550_init(int com_port, int baud_divisor)
{
	while (!(uart_reg_read(UART_LSR(com_port)) & UART_LSR_TEMT))
		;

	uart_reg_write(NS16550_IER, UART_IER(com_port));
	uart_reg_write(UART_MCRVAL, UART_MCR(com_port));
	uart_reg_write(ns16550_getfcr(com_port), UART_FCR(com_port));
	if (baud_divisor != -1)
		NS16550_setbrg(com_port, baud_divisor);
}

void NS16550_reinit(int com_port, int baud_divisor)
{
	uart_reg_write(NS16550_IER, UART_IER(com_port));
	NS16550_setbrg(com_port, 0);
	uart_reg_write(UART_MCRVAL, UART_MCR(com_port));
	uart_reg_write(ns16550_getfcr(com_port), UART_FCR(com_port));
	NS16550_setbrg(com_port, baud_divisor);
}

void NS16550_putc(int com_port, char c)
{
	while ((uart_reg_read(UART_LSR(com_port)) & UART_LSR_THRE) == 0)
		;
	uart_reg_write(c, UART_THR(com_port));
}

char NS16550_getc(int com_port)
{
	while ((uart_reg_read(UART_LSR(com_port)) & UART_LSR_DR) == 0) {
	}
	return uart_reg_read(UART_RBR(com_port));
}

int NS16550_tstc(int com_port)
{
	return (uart_reg_read(UART_LSR(com_port)) & UART_LSR_DR) != 0;
}

void ns16550_con_init(void)
{
	int clock_divisor;

	clock_divisor = ns16550_calc_divisor(UART_CON_ID,
		NS16550_CLK, UART_CON_BAUDRATE);
	NS16550_init(UART_CON_ID, clock_divisor);
}

void ns16550_con_write(uint8_t byte)
{
	NS16550_putc(UART_CON_ID, byte);
}

uint8_t ns16550_con_read(void)
{
	return NS16550_getc(UART_CON_ID);
}

bool ns16550_con_poll(void)
{
	return !!NS16550_tstc(UART_CON_ID);
}
