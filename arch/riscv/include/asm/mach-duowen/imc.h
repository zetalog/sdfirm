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
 * @(#)reg.h: DUOWEN space and register definitions
 * $Id: reg.h,v 1.1 2019-09-02 11:12:00 zhenglv Exp $
 */

#ifndef __IMC_DUOWEN_H_INCLUDE__
#define __IMC_DUOWEN_H_INCLUDE__

#include <target/arch.h>
#include <target/types.h>
#include <target/amba.h>

#define IMC_CSR_REG(offset)		(IMC_CSR_BASE + (offset))
#define IMC_CSR_BOOT_ADDR		IMC_CSR_REG(0x00)
#define IMC_CSR_CORE_ID			IMC_CSR_REG(0x04)
#define IMC_CSR_CLUSTER_ID		IMC_CSR_REG(0x08)
#define IMC_CSR_CLOCK_EN		IMC_CSR_REG(0x0C)

/* CORE_ID (offset 0x04) */
#define IMC_CORE_ID_OFFSET		0
#define IMC_CORE_ID_MASK		REG_4BIT_MASK
#define IMC_CORE_ID(value)		_GET_FV(IMC_CORE_ID, value)
/* CLUSTER_ID (offset 0x08) */
#define IMC_CLUSTER_ID_OFFSET		0
#define IMC_CLUSTER_ID_MASK		REG_6BIT_MASK
#define IMC_CLUSTER_ID(value)		_GET_FV(IMC_CLUSTER_ID, value)
/* CLOCK_EN (offset 0x0C) */
#define IMC_CLOCK_EN			_BV(0)
#define imc_core_id()			\
	IMC_CORE_ID(__raw_readl(IMC_CSR_CORE_ID))
#define imc_cluster_id()		\
	IMC_CLUSTER_ID(__raw_readl(IMC_CSR_CLUSTER_ID))
#define imc_enable_clock()		\
	__raw_setl(IMC_CLOCK_EN, IMC_CSR_CLOCK_EN)
#define imc_disable_clock()		\
	__raw_clearl(IMC_CLOCK_EN, IMC_CSR_CLOCK_EN)

/* IMC -> SFAB address translation
 *
 * When IMC is accessing the addresses that are external to the IMC, the
 * bus transactions are delivered to SLAVE1, and ADDR_TRANS fabric converts
 * the 32-bit IMC addresses to the 44-bit APC addresses, and relays the
 * transactions to the system fabric (SFAB).
 * The translation algorithm is:
 *  if (in_addr[31:24] == ADDR_I[31:24])
 *    out_addr[43:0] = {ADDR_O[43:24], in_addr[23:0]}
 * If no address translation entry is matched, the default rule is:
 *  out_addr[43:0] = {12'h0FF, 1'b0, in_addr[30:0]}
 */
#define IMC_ADDR_TRANS_LO(n)		IMC_CSR_REG(0x40 + (n) * 0x08)
#define IMC_ADDR_TRANS_HI(n)		IMC_CSR_REG(0x44 + (n) * 0x08)

#define IMC_AT_ENTRIES			6
/* IMC_ADDR_TRANS_LO */
#define IMC_AT_IN_31_24_OFFSET		0
#define IMC_AT_IN_31_24_MASK		REG_8BIT_MASK
#define IMC_AT_IN_31_24(value)		_SET_FV(IMC_AT_IN_31_24, value)
/* IMC_ADDR_TRANS_HI */
#define IMC_AT_VALID			_BV(31)
#define IMC_AT_BURST_OFFSET		28
#define IMC_AT_BURST_MASK		REG_2BIT_MASK
#define IMC_AT_BURST(value)		_SET_FV(IMC_AT_BURST, value)
#define IMC_AT_CACHE_OFFSET		24
#define IMC_AT_CACHE_MASK		REG_4BIT_MASK
#define IMC_AT_CACHE(value)		_SET_FV(IMC_AT_CACHE, value)
#define IMC_AT_PROT_OFFSET		20
#define IMC_AT_PROT_MASK		REG_3BIT_MASK
#define IMC_AT_PROT(value)		_SET_FV(IMC_AT_PROT, value)
#define IMC_AT_OUT_43_24_OFFSET		0
#define IMC_AT_OUT_43_24_MASK		REG_20BIT_MASK
#define IMC_AT_OUT_43_24(value)		_SET_FV(IMC_AT_OUT_43_24, value)

/* IMC_AT attributes, see AXI_AXPROT/AXI_AXCACHE/AXBURST */
#define IMC_AT_ATTR_PROT_OFFSET		0
#define IMC_AT_ATTR_PROT_MASK		IMC_AT_PROT_MASK
#define IMC_AT_ATTR_SET_PROT(value)	_SET_FV(IMC_AT_ATTR_PROT, value)
#define IMC_AT_ATTR_GET_PROT(value)	_GET_FV(IMC_AT_ATTR_PROT, value)
#define IMC_AT_ATTR_CACHE_OFFSET	4
#define IMC_AT_ATTR_CACHE_MASK		IMC_AT_CACHE_MASK
#define IMC_AT_ATTR_SET_CACHE(value)	_SET_FV(IMC_AT_ATTR_CACHE, value)
#define IMC_AT_ATTR_GET_CACHE(value)	_GET_FV(IMC_AT_ATTR_CACHE, value)
#define IMC_AT_ATTR_BURST_OFFSET	8
#define IMC_AT_ATTR_BURST_MASK		IMC_AT_BURST_MASK
#define IMC_AT_ATTR_SET_BURST(value)	_SET_FV(IMC_AT_ATTR_BURST, value)
#define IMC_AT_ATTR_GET_BURST(value)	_GET_FV(IMC_AT_ATTR_BURST, value)
#define IMC_AT_ATTR(burst, cache, prot)	\
	(IMC_AT_ATTR_SET_BURST(burst) |	\
	 IMC_AT_ATTR_SET_CACHE(cache) | \
	 IMC_AT_ATTR_SET_PROT(prot))
#define imc_sfab_set_valid(n)		\
	__raw_setl(IMC_AT_VALID, IMC_ADDR_TRANS_HI(n));
#define imc_sfab_set_invalid(n)		\
	__raw_clearl(IMC_AT_VALID, IMC_ADDR_TRANS_HI(n));
#define imc_sfab_set_attr(n, burst, cache, prot)		\
	__raw_writel_mask(IMC_AT_BURST(burst) |			\
			  IMC_AT_CACHE(cache) |			\
			  IMC_AT_PROT(prot),			\
			  IMC_AT_BURST(IMC_AT_BURST_MASK) |	\
			  IMC_AT_CACHE(IMC_AT_CACHE_MASK) |	\
			  IMC_AT_PROT(IMC_AT_PROT_MASK),	\
			  IMC_ADDR_TRANS_HI(n))
#define IMC_AT_ADDR_I_OFFSET		24
#define IMC_AT_ADDR_I_MASK		IMC_AT_IN_31_24_MASK
#define IMC_AT_ADDR_I(value)		_GET_FV(IMC_AT_ADDR_I, value)
#define IMC_AT_ADDR_O_OFFSET		24
#define IMC_AT_ADDR_O_MASK		IMC_AT_OUT_43_24_MASK
#define IMC_AT_ADDR_O(value)		_GET_FV(IMC_AT_ADDR_O, value)
#define imc_sfab_set_addr_i(n, addr)				\
	__raw_writel_mask(IMC_AT_IN_31_24(IMC_AT_ADDR_I(addr)),	\
			  IMC_AT_IN_31_24(IMC_AT_ADDR_I_MASK),	\
			  IMC_ADDR_TRANS_LO(n))
#define imc_sfab_set_addr_o(n, addr)				\
	__raw_writel_mask(IMC_AT_OUT_43_24(IMC_AT_ADDR_O(addr)),\
			  IMC_AT_OUT_43_24(IMC_AT_ADDR_O_MASK),	\
			  IMC_ADDR_TRANS_HI(n))

#ifndef __ASSEMBLY__
typedef uint16_t imc_at_attr_t;
void imc_sfab_remap(int n, uint32_t in_addr, uint64_t out_addr,
		    imc_at_attr_t attr);
#endif

#endif /* __IMC_DUOWEN_H_INCLUDE__ */
