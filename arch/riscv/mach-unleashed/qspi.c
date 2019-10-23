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
 * @(#)qspi.c: FU540 (unleashed) specific QSPI implementation
 * $Id: qspi.c,v 1.1 2019-10-22 19:25:00 zhenglv Exp $
 */

#include <target/spi.h>

#ifndef CONFIG_SIFIVE_QSPI_STATUS
uint32_t sifive_qspi_rx;
#endif

uint32_t sifive_qspi_min_div(uint32_t input_freq, uint32_t max_output_freq)
{
	/* The nearest integer solution for div requires rounding up as to
	 * not exceed max_output_freq.
	 *  div = ceil(Fin / (2 * Fsck)) - 1
	 *  i    = floor((Fin - 1 + 2 * Fsck) / (2 * Fsck)) - 1
	 * This should not overflow as long as (Fin - 1 + 2 * Fsck) does
	 * not exceed 2^32 - 1.
	 */
	uint32_t quotient = div32u(input_freq + (max_output_freq << 1) - 1,
				   max_output_freq << 1);
	/* Avoid underflow */
	return quotient ? quotient - 1 : 0;
}

void sifive_qspi_tx(uint8_t spi, uint8_t byte)
{
	while (!sifive_qspi_write_poll(spi));
	sifive_qspi_write_byte(spi, byte);
}

uint8_t sifive_qspi_rx(uint8_t spi)
{
	while (!sifive_qspi_read_poll(spi));
	return sifive_qspi_read_byte(spi);
}
