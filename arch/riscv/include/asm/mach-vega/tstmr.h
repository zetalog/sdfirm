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
 * @(#)tstmr.h: RV32M1 (VEGA) TSTMR (time stamp timer) driver
 * $Id: tstmr.h,v 1.1 2019-08-14 16:09:00 zhenglv Exp $
 */

#ifndef __TSTMR_VEGA_H_INCLUDE__
#define __TSTMR_VEGA_H_INCLUDE__

#include <target/types.h>
#include <asm/io.h>

#define TSTMRA_BASE		UL(0x40034000)
#define TSTMRB_BASE		UL(0x4102C000)
#define TSTMR_REG(id, offset)	((caddr_t)(TSTMR##id##_BASE+(offset)))

#define TSTMR_FREQ		1000000

/* ----------------------------------------------------------------------------
   Time Stamp Timer (TSTMR)
   ---------------------------------------------------------------------------- */

/* L - Time Stamp Timer Register Low */
#define TSTMR_L(id)		TSTMR_REG(id, 0x00)
/* H - Time Stamp Timer Register High */
#define TSTMR_H(id)		TSTMR_REG(id, 0x04)

#define TSTMR_HIGH_OFFSET	0
#define TSTMR_HIGH_MASK		REG_24BIT_MASK
#define TSTMR_HIGH(value)	_GET_FV(TSTMR_HIGH, value)

static inline uint32_t tstmr_read_counter_low(void)
{
	return __raw_readl(TSTMR_L(A));
}
static inline uint32_t tstmr_read_counter_high(void)
{
	uint32_t hi = __raw_readl(TSTMR_H(A));
	return TSTMR_HIGH(hi);
}

/* The TSTMR LOW read should occur first, followed by the TSTMR HIGH read */
static inline uint64_t tstmr_read_counter(void)
{
	uint32_t hi, lo, tmp;

	do {
		hi = tstmr_read_counter_high();
		lo = tstmr_read_counter_low();
		tmp = tstmr_read_counter_high();
	} while (hi != tmp);
	return (uint64_t)hi << 32 | lo;
}

#endif /* __TSTMR_VEGA_H_INCLUDE__ */
