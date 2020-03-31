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
 * @(#)tmr.c: DPU specific timer controller (TMR) driver
 * $Id: tmr.c,v 1.1 2020-03-04 15:21:00 zhenglv Exp $
 */

#include <target/delay.h>

uint64_t tmr_read_counter(void)
{
	uint32_t hi1, hi2;
	uint32_t lo;

	do {
		hi1 = __raw_readl(TMR_CNT_HI);
		lo = __raw_readl(TMR_CNT_LO);
		hi2 = __raw_readl(TMR_CNT_HI);
	} while (hi1 != hi2);
	return MAKELLONG(lo, hi1);
}

void tmr_write_compare(uint8_t id, uint64_t count)
{
	__raw_clearl(_BV(id), TMR_CMP_CTRL(id));
	__raw_writel(LODWORD(count), TMR_CMP_LO(id));
	__raw_writel(HIDWORD(count), TMR_CMP_HI(id));
	__raw_setl(_BV(id), TMR_CMP_CTRL(id));
}

void tmr_ctrl_init(void)
{
	__raw_setl(TMR_EN, TMR_CNT_CTRL);
}
