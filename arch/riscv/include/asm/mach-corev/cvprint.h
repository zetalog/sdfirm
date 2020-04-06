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
 * @(#)cvprint.h: corev-v testbench specific console definitions
 * $Id: cvprint.h,v 1.1 2020-04-07 00:29:00 zhenglv Exp $
 */

#ifndef __PRINT_COREV_H_INCLUDE__
#define __PRINT_COREV_H_INCLUDE__

#include <target/arch.h>

#define CVPRINT_OUTPORT		0x10000000
#define CVPRINT_INPORT		0x10000004
#define CVPRINT_STATUS		0x10000008

#define CVPRINT_RXF		_BV(0)
#define CVPRINT_TXF		_BV(1)

#define pulp_tx_full()		(!!(__raw_readl(CVPRINT_STATUS) & CVPRINT_TXF))
#define pulp_rx_full()		(!!(__raw_readl(CVPRINT_STATUS) & CVPRINT_RXF))
#define pulp_putchar(ch)					\
	do {							\
		while (pulp_tx_full());				\
		__raw_writel((uint32_t)ch, CVPRINT_OUTPORT);	\
	} while (0)
#define pulp_getchar()		__raw_readl(CVPRINT_INPORT)
#define pulp_poll()		pulp_rx_full()

#endif /* __PRINT_COREV_H_INCLUDE__ */
