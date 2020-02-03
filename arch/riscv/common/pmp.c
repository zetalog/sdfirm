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
 * @(#)pmp.c: RISCV physical memory protection (PMP) implementation
 * $Id: pmp.c,v 1.1 2020-01-02 22:50:00 zhenglv Exp $
 */

#include <target/arch.h>

static inline unsigned long csr_read_pmpcfg(int n)
{
	unsigned long ret = 0;

	switch (n) {
	case 0:
		ret = csr_read(CSR_PMPCFG(0));
		break;
	case 1:
		ret = csr_read(CSR_PMPCFG(1));
		break;
	case 2:
		ret = csr_read(CSR_PMPCFG(2));
		break;
	case 3:
		ret = csr_read(CSR_PMPCFG(3));
		break;
	default:
		break;
	}
	return ret;
}

static inline unsigned long csr_read_pmpaddr(int n)
{
	unsigned long ret = 0;

	switch (n) {
	case 0:
		ret = csr_read(CSR_PMPADDR(0));
		break;
	case 1:
		ret = csr_read(CSR_PMPADDR(1));
		break;
	case 2:
		ret = csr_read(CSR_PMPADDR(2));
		break;
	case 3:
		ret = csr_read(CSR_PMPADDR(3));
		break;
	case 4:
		ret = csr_read(CSR_PMPADDR(4));
		break;
	case 5:
		ret = csr_read(CSR_PMPADDR(5));
		break;
	case 6:
		ret = csr_read(CSR_PMPADDR(6));
		break;
	case 7:
		ret = csr_read(CSR_PMPADDR(7));
		break;
	case 8:
		ret = csr_read(CSR_PMPADDR(8));
		break;
	case 9:
		ret = csr_read(CSR_PMPADDR(9));
		break;
	case 10:
		ret = csr_read(CSR_PMPADDR(10));
		break;
	case 11:
		ret = csr_read(CSR_PMPADDR(11));
		break;
	case 12:
		ret = csr_read(CSR_PMPADDR(12));
		break;
	case 13:
		ret = csr_read(CSR_PMPADDR(13));
		break;
	case 14:
		ret = csr_read(CSR_PMPADDR(14));
		break;
	case 15:
		ret = csr_read(CSR_PMPADDR(15));
		break;
	default:
		break;
	};
	return ret;
}

void csr_write_pmpcfg(int n, unsigned long val)
{
	switch (n) {
	case 0:
		csr_write(CSR_PMPCFG(0), val);
		break;
	case 1:
		csr_write(CSR_PMPCFG(1), val);
		break;
	case 2:
		csr_write(CSR_PMPCFG(2), val);
		break;
	case 3:
		csr_write(CSR_PMPCFG(3), val);
		break;
	default:
		break;
	}
}

void csr_write_pmpaddr(int n, unsigned long val)
{
	switch (n) {
	case 0:
		csr_write(CSR_PMPADDR(0), val);
		break;
	case 1:
		csr_write(CSR_PMPADDR(1), val);
		break;
	case 2:
		csr_write(CSR_PMPADDR(2), val);
		break;
	case 3:
		csr_write(CSR_PMPADDR(3), val);
		break;
	case 4:
		csr_write(CSR_PMPADDR(4), val);
		break;
	case 5:
		csr_write(CSR_PMPADDR(5), val);
		break;
	case 6:
		csr_write(CSR_PMPADDR(6), val);
		break;
	case 7:
		csr_write(CSR_PMPADDR(7), val);
		break;
	case 8:
		csr_write(CSR_PMPADDR(8), val);
		break;
	case 9:
		csr_write(CSR_PMPADDR(9), val);
		break;
	case 10:
		csr_write(CSR_PMPADDR(10), val);
		break;
	case 11:
		csr_write(CSR_PMPADDR(11), val);
		break;
	case 12:
		csr_write(CSR_PMPADDR(12), val);
		break;
	case 13:
		csr_write(CSR_PMPADDR(13), val);
		break;
	case 14:
		csr_write(CSR_PMPADDR(14), val);
		break;
	case 15:
		csr_write(CSR_PMPADDR(15), val);
		break;
	default:
		break;
	};
}

static unsigned long ctz(unsigned long x)
{
	unsigned long ret = 0;

	while (!(x & 1UL)) {
		ret++;
		x = x >> 1;
	}
	return ret;
}

int pmp_set(int n, unsigned long prot,
	    phys_addr_t addr, unsigned long log2len)
{
	int pmpcfg_index, pmpcfg_shift;
	unsigned long cfgmask, pmpcfg;
	unsigned long addrmask, pmpaddr;

	/* check parameters */
	if (n >= PMP_COUNT || log2len > __riscv_xlen || log2len < PMP_SHIFT)
		return -EINVAL;

	/* calculate PMP register and offset */
#if __riscv_xlen == 32
	pmpcfg_index = n >> 2;
	pmpcfg_shift = (n & 3) << 3;
#elif __riscv_xlen == 64
	pmpcfg_index = (n >> 2) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#else
	pmpcfg_index = -1;
	pmpcfg_shift = -1;
#endif
	if (pmpcfg_index < 0 || pmpcfg_shift < 0)
		return -ENOTSUP;

	/* encode PMP config */
	prot |= (log2len == PMP_SHIFT) ? PMP_A_NA4 : PMP_A_NAPOT;
	cfgmask = ~(0xff << pmpcfg_shift);
	pmpcfg	= csr_read_pmpcfg(pmpcfg_index) & cfgmask;
	pmpcfg |= ((prot << pmpcfg_shift) & ~cfgmask);

	/* encode PMP address */
	if (log2len == PMP_SHIFT) {
		pmpaddr = (addr >> PMP_SHIFT);
	} else {
		if (log2len == __riscv_xlen) {
			pmpaddr = -UL(1);
		} else {
			addrmask = (UL(1) << (log2len - PMP_SHIFT)) - 1;
			pmpaddr	 = ((addr >> PMP_SHIFT) & ~addrmask);
			pmpaddr |= (addrmask >> 1);
		}
	}

	/* write csrs */
	csr_write_pmpaddr(n, pmpaddr);
	csr_write_pmpcfg(pmpcfg_index, pmpcfg);
	return 0;
}

int pmp_get(int n, unsigned long *prot_out,
	    phys_addr_t *addr_out, unsigned long *log2len_out)
{
	int pmpcfg_index, pmpcfg_shift;
	unsigned long cfgmask, pmpcfg;
	unsigned long prot;
	unsigned long t1, addr, log2len;

	/* check parameters */
	if (n >= PMP_COUNT || !prot_out || !addr_out || !log2len_out)
		return -EINVAL;
	*prot_out = *addr_out = *log2len_out = 0;

	/* calculate PMP register and offset */
#if __riscv_xlen == 32
	pmpcfg_index = n >> 2;
	pmpcfg_shift = (n & 3) << 3;
#elif __riscv_xlen == 64
	pmpcfg_index = (n >> 2) & ~1;
	pmpcfg_shift = (n & 7) << 3;
#else
	pmpcfg_index = -1;
	pmpcfg_shift = -1;
#endif
	if (pmpcfg_index < 0 || pmpcfg_shift < 0)
		return -ENOTSUP;

	/* decode PMP config */
	cfgmask = (0xff << pmpcfg_shift);
	pmpcfg	= csr_read_pmpcfg(pmpcfg_index) & cfgmask;
	prot	= pmpcfg >> pmpcfg_shift;

	/* decode PMP address */
	if ((prot & PMP_A) == PMP_A_NAPOT) {
		addr = csr_read_pmpaddr(n);
		if (addr == -1UL) {
			addr	= 0;
			log2len = __riscv_xlen;
		} else {
			t1	= ctz(~addr);
			addr	= (addr & ~((1UL << t1) - 1)) << PMP_SHIFT;
			log2len = (t1 + PMP_SHIFT + 1);
		}
	} else {
		addr	= csr_read_pmpaddr(n) << PMP_SHIFT;
		log2len = PMP_SHIFT;
	}

	/* return details */
	*prot_out = prot;
	*addr_out = addr;
	*log2len_out = log2len;
	return 0;
}

int pmp_dump(int argc, char **argv)
{
	unsigned long prot, l2l;
	phys_addr_t addr;
	size_t size;
	int i;

	for (i = 0; i < PMP_COUNT; i++) {
		pmp_get(i, &prot, &addr, &l2l);
		if (!(prot & PMP_A))
			continue;
		if (l2l < __riscv_xlen)
			size = (1UL << l2l);
		else
			size = 0;
#if __riscv_xlen == 32
		printf("PMP%d: 0x%08lx-0x%08lx (A",
#else
		printf("PMP%d: 0x%016lx-0x%016lx (A",
#endif
		       i, (unsigned long)addr,
		       (unsigned long)addr + size - 1);
		if (prot & PMP_L)
			printf(",L");
		if (prot & PMP_R)
			printf(",R");
		if (prot & PMP_W)
			printf(",W");
		if (prot & PMP_X)
			printf(",X");
		printf(")\n");
	}
	return 0;
}
