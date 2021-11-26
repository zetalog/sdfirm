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
 * @(#)vaisra_pma.h: VAISRA physical memory attributes (PMA) implementation
 * $Id: vaisra_pma.h,v 1.1 2020-07-05 23:43:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/bitops.h>

static inline unsigned long csr_read_pmacfg(int n)
{
	unsigned long ret = 0;

	switch (n) {
	case 0:
		ret = csr_read(CSR_PMACFG(0));
		break;
	case 1:
		ret = csr_read(CSR_PMACFG(1));
		break;
	case 2:
		ret = csr_read(CSR_PMACFG(2));
		break;
	case 3:
		ret = csr_read(CSR_PMACFG(3));
		break;
	default:
		break;
	}
	return ret;
}

static inline unsigned long csr_read_pmaaddr(int n)
{
	unsigned long ret = 0;

	switch (n) {
	case 0:
		ret = csr_read(CSR_PMAADDR(0));
		break;
	case 1:
		ret = csr_read(CSR_PMAADDR(1));
		break;
	case 2:
		ret = csr_read(CSR_PMAADDR(2));
		break;
	case 3:
		ret = csr_read(CSR_PMAADDR(3));
		break;
	case 4:
		ret = csr_read(CSR_PMAADDR(4));
		break;
	case 5:
		ret = csr_read(CSR_PMAADDR(5));
		break;
	case 6:
		ret = csr_read(CSR_PMAADDR(6));
		break;
	case 7:
		ret = csr_read(CSR_PMAADDR(7));
		break;
	case 8:
		ret = csr_read(CSR_PMAADDR(8));
		break;
	case 9:
		ret = csr_read(CSR_PMAADDR(9));
		break;
	case 10:
		ret = csr_read(CSR_PMAADDR(10));
		break;
	case 11:
		ret = csr_read(CSR_PMAADDR(11));
		break;
	case 12:
		ret = csr_read(CSR_PMAADDR(12));
		break;
	case 13:
		ret = csr_read(CSR_PMAADDR(13));
		break;
	case 14:
		ret = csr_read(CSR_PMAADDR(14));
		break;
	case 15:
		ret = csr_read(CSR_PMAADDR(15));
		break;
	default:
		break;
	};
	return ret;
}

static void csr_write_pmacfg(int n, unsigned long val)
{
	switch (n) {
	case 0:
		csr_write(CSR_PMACFG(0), val);
		break;
	case 1:
		csr_write(CSR_PMACFG(1), val);
		break;
	case 2:
		csr_write(CSR_PMACFG(2), val);
		break;
	case 3:
		csr_write(CSR_PMACFG(3), val);
		break;
	default:
		break;
	}
}

static void csr_write_pmaaddr(int n, unsigned long val)
{
	switch (n) {
	case 0:
		csr_write(CSR_PMAADDR(0), val);
		break;
	case 1:
		csr_write(CSR_PMAADDR(1), val);
		break;
	case 2:
		csr_write(CSR_PMAADDR(2), val);
		break;
	case 3:
		csr_write(CSR_PMAADDR(3), val);
		break;
	case 4:
		csr_write(CSR_PMAADDR(4), val);
		break;
	case 5:
		csr_write(CSR_PMAADDR(5), val);
		break;
	case 6:
		csr_write(CSR_PMAADDR(6), val);
		break;
	case 7:
		csr_write(CSR_PMAADDR(7), val);
		break;
	case 8:
		csr_write(CSR_PMAADDR(8), val);
		break;
	case 9:
		csr_write(CSR_PMAADDR(9), val);
		break;
	case 10:
		csr_write(CSR_PMAADDR(10), val);
		break;
	case 11:
		csr_write(CSR_PMAADDR(11), val);
		break;
	case 12:
		csr_write(CSR_PMAADDR(12), val);
		break;
	case 13:
		csr_write(CSR_PMAADDR(13), val);
		break;
	case 14:
		csr_write(CSR_PMAADDR(14), val);
		break;
	case 15:
		csr_write(CSR_PMAADDR(15), val);
		break;
	default:
		break;
	};
}

static void __pma_cfg(int n, unsigned long attr)
{
	unsigned long cfgmask, pmacfg;
	int pmacfg_index, pmacfg_shift;
	bool tor = ((attr & PMA_A) == PMA_A_TOR);

	/* calculate PMA register and offset */
	pmacfg_index = REG64_16BIT_INDEX(tor ? n + 1 : n);
	pmacfg_shift = REG64_16BIT_OFFSET(tor ? n + 1 : n);

	cfgmask = ~(UL(0xffff) << pmacfg_shift);
	pmacfg	= csr_read_pmacfg(pmacfg_index) & cfgmask;
	pmacfg |= ((attr << pmacfg_shift) & ~cfgmask);

	csr_write_pmacfg(pmacfg_index, pmacfg);
}

int pma_set(int n, unsigned long attr, phys_addr_t addr, unsigned long log2len)
{
	unsigned long addrmask, pmaaddr;
	bool tor = !IS_ALIGNED(addr, PMA_GRAIN_ALIGN) ||
		   log2len < PMA_GRAIN_SHIFT;

	/* check parameters */
	if (n >= PMA_COUNT || log2len > __riscv_xlen || log2len < PMA_SHIFT)
		return -EINVAL;

	/* encode PMA config */
	attr |= tor ? PMA_A_TOR :
		((log2len == PMA_SHIFT) ? PMA_A_NA4 : PMA_A_NAPOT);

	if (tor) {
		if (n == 0 && addr == 0)
			csr_write_pmaaddr(n, addr + (UL(1) << log2len));
		else {
			csr_write_pmaaddr(n, addr);
			csr_write_pmaaddr(n + 1, addr + (UL(1) << log2len));
		}
		__pma_cfg(n, attr);
		return 2;
	}

	/* encode PMA address */
	if (log2len == PMA_SHIFT) {
		pmaaddr = (addr >> PMA_SHIFT);
	} else {
		if (log2len == __riscv_xlen) {
			pmaaddr = -UL(1);
		} else {
			addrmask = (UL(1) << (log2len - PMA_SHIFT)) - 1;
			pmaaddr	 = ((addr >> PMA_SHIFT) & ~addrmask);
			pmaaddr |= (addrmask >> 1);
		}
	}

	/* write csrs */
	csr_write_pmaaddr(n, pmaaddr);
	__pma_cfg(n, attr);
	return 1;
}
