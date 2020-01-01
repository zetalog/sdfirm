/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#ifndef __RISCV_ASM_H__
#define __RISCV_ASM_H__

#include <target/generic.h>
#include <target/page.h>

/* clang-format off */

#if __SIZEOF_POINTER__ == 8
#define BITS_PER_LONG		64
#elif __SIZEOF_POINTER__ == 4
#define BITS_PER_LONG		32
#endif

/* clang-format on */

#ifndef __ASSEMBLY__
unsigned long csr_read_num(int csr_num);
void csr_write_num(int csr_num, unsigned long val);
int pmp_set(unsigned int n, unsigned long prot, unsigned long addr,
	    unsigned long log2len);
int pmp_get(unsigned int n, unsigned long *prot_out, unsigned long *addr_out,
	    unsigned long *log2len_out);

static inline int misa_extension(char ext)
{
	return csr_read(CSR_MISA) & (1 << (ext - 'A'));
}

static inline int misa_xlen(void)
{
	return ((long)csr_read(CSR_MISA) < 0) ? 64 : 32;
}

static inline void misa_string(char *out, unsigned int out_sz)
{
	unsigned long i, val = csr_read(CSR_MISA);

	for (i = 0; i < 26; i++) {
		if (val & (1 << i)) {
			*out = 'A' + i;
			out++;
		}
	}
	*out = '\0';
	out++;
}
#endif /* !__ASSEMBLY__ */

#endif
