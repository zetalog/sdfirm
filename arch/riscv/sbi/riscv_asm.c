/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

#include <target/sbi.h>

#if 0
/* determine CPU extension, return non-zero support */
int misa_extension_imp(char ext)
{
	unsigned long misa = csr_read(CSR_MISA);

	if (misa) {
		if ('A' <= ext && ext <= 'Z')
			return misa & (1 << (ext - 'A'));
		if ('a' <= ext && ext <= 'z')
			return misa & (1 << (ext - 'a'));
		return 0;
	}

	return sbi_platform_misa_extension(sbi_platform_thishart_ptr(), ext);
}

int misa_xlen(void)
{
	long r;

	if (csr_read(CSR_MISA) == 0)
		return sbi_platform_misa_xlen(sbi_platform_thishart_ptr());

	__asm__ __volatile__(
		"csrr   t0, misa\n\t"
		"slti   t1, t0, 0\n\t"
		"slli   t1, t1, 1\n\t"
		"slli   t0, t0, 1\n\t"
		"slti   t0, t0, 0\n\t"
		"add    %0, t0, t1"
		: "=r"(r)
		:
		: "t0", "t1");

	return r ? r : -1;
}

void misa_string(int xlen, char *out, unsigned int out_sz)
{
	unsigned int i, pos = 0;
	const char valid_isa_order[] = "iemafdqclbjtpvnhkorwxyzg";

	if (!out)
		return;

	if (5 <= (out_sz - pos)) {
		out[pos++] = 'r';
		out[pos++] = 'v';
		switch (xlen) {
		case 1:
			out[pos++] = '3';
			out[pos++] = '2';
			break;
		case 2:
			out[pos++] = '6';
			out[pos++] = '4';
			break;
		case 3:
			out[pos++] = '1';
			out[pos++] = '2';
			out[pos++] = '8';
			break;
		default:
			sbi_panic("%s: Unknown misa.MXL encoding %d",
				   __func__, xlen);
			return;
		}
	}

	for (i = 0; i < array_size(valid_isa_order) && (pos < out_sz); i++) {
		if (misa_extension_imp(valid_isa_order[i]))
			out[pos++] = valid_isa_order[i];
	}

	if (pos < out_sz)
		out[pos++] = '\0';
}
#endif

unsigned long csr_read_num(int csr_num)
{
#define switchcase_csr_read(__csr_num, __val)		\
	case __csr_num:					\
		__val = csr_read(__csr_num);		\
		break;
#define switchcase_csr_read_2(__csr_num, __val)		\
	switchcase_csr_read(__csr_num + 0, __val)	\
	switchcase_csr_read(__csr_num + 1, __val)
#define switchcase_csr_read_4(__csr_num, __val)		\
	switchcase_csr_read_2(__csr_num + 0, __val)	\
	switchcase_csr_read_2(__csr_num + 2, __val)
#define switchcase_csr_read_8(__csr_num, __val)		\
	switchcase_csr_read_4(__csr_num + 0, __val)	\
	switchcase_csr_read_4(__csr_num + 4, __val)
#define switchcase_csr_read_16(__csr_num, __val)	\
	switchcase_csr_read_8(__csr_num + 0, __val)	\
	switchcase_csr_read_8(__csr_num + 8, __val)
#define switchcase_csr_read_32(__csr_num, __val)	\
	switchcase_csr_read_16(__csr_num + 0, __val)	\
	switchcase_csr_read_16(__csr_num + 16, __val)
#define switchcase_csr_read_64(__csr_num, __val)	\
	switchcase_csr_read_32(__csr_num + 0, __val)	\
	switchcase_csr_read_32(__csr_num + 32, __val)

	unsigned long ret = 0;

	switch (csr_num) {
	switchcase_csr_read_16(CSR_PMPCFG0, ret)
	switchcase_csr_read_64(CSR_PMPADDR0, ret)
	switchcase_csr_read(CSR_MCYCLE, ret)
	switchcase_csr_read(CSR_MINSTRET, ret)
	switchcase_csr_read(CSR_MHPMCOUNTER(3), ret)
	switchcase_csr_read_4(CSR_MHPMCOUNTER(4), ret)
	switchcase_csr_read_8(CSR_MHPMCOUNTER(8), ret)
	switchcase_csr_read_16(CSR_MHPMCOUNTER(16), ret)
	switchcase_csr_read(CSR_MCOUNTINHIBIT, ret)
	switchcase_csr_read(CSR_MHPMEVENT(3), ret)
	switchcase_csr_read_4(CSR_MHPMEVENT(4), ret)
	switchcase_csr_read_8(CSR_MHPMEVENT(8), ret)
	switchcase_csr_read_16(CSR_MHPMEVENT(16), ret)
#if __riscv_xlen == 32
	switchcase_csr_read(CSR_MCYCLEH, ret)
	switchcase_csr_read(CSR_MINSTRETH, ret)
	switchcase_csr_read(CSR_MHPMCOUNTERH(3), ret)
	switchcase_csr_read_4(CSR_MHPMCOUNTERH(4), ret)
	switchcase_csr_read_8(CSR_MHPMCOUNTERH(8), ret)
	switchcase_csr_read_16(CSR_MHPMCOUNTERH(16), ret)
	/**
	 * The CSR range MHPMEVENT[3-16]H are available only if sscofpmf
	 * extension is present. The caller must ensure that.
	 */
	switchcase_csr_read(CSR_MHPMEVENTH(3), ret)
	switchcase_csr_read_4(CSR_MHPMEVENTH(4), ret)
	switchcase_csr_read_8(CSR_MHPMEVENTH(8), ret)
	switchcase_csr_read_16(CSR_MHPMEVENTH(16), ret)
#endif

	default:
		sbi_printf("%s: Unknown CSR %#x", __func__, csr_num);
		bh_panic();
		break;
	}

	return ret;

#undef switchcase_csr_read_64
#undef switchcase_csr_read_32
#undef switchcase_csr_read_16
#undef switchcase_csr_read_8
#undef switchcase_csr_read_4
#undef switchcase_csr_read_2
#undef switchcase_csr_read
}

void csr_write_num(int csr_num, unsigned long val)
{
#define switchcase_csr_write(__csr_num, __val)		\
	case __csr_num:					\
		csr_write(__csr_num, __val);		\
		break;
#define switchcase_csr_write_2(__csr_num, __val)	\
	switchcase_csr_write(__csr_num + 0, __val)	\
	switchcase_csr_write(__csr_num + 1, __val)
#define switchcase_csr_write_4(__csr_num, __val)	\
	switchcase_csr_write_2(__csr_num + 0, __val)	\
	switchcase_csr_write_2(__csr_num + 2, __val)
#define switchcase_csr_write_8(__csr_num, __val)	\
	switchcase_csr_write_4(__csr_num + 0, __val)	\
	switchcase_csr_write_4(__csr_num + 4, __val)
#define switchcase_csr_write_16(__csr_num, __val)	\
	switchcase_csr_write_8(__csr_num + 0, __val)	\
	switchcase_csr_write_8(__csr_num + 8, __val)
#define switchcase_csr_write_32(__csr_num, __val)	\
	switchcase_csr_write_16(__csr_num + 0, __val)	\
	switchcase_csr_write_16(__csr_num + 16, __val)
#define switchcase_csr_write_64(__csr_num, __val)	\
	switchcase_csr_write_32(__csr_num + 0, __val)	\
	switchcase_csr_write_32(__csr_num + 32, __val)

	switch (csr_num) {
	switchcase_csr_write_16(CSR_PMPCFG(0), val)
	switchcase_csr_write_64(CSR_PMPADDR(0), val)
	switchcase_csr_write(CSR_MCYCLE, val)
	switchcase_csr_write(CSR_MINSTRET, val)
	switchcase_csr_write(CSR_MHPMCOUNTER(3), val)
	switchcase_csr_write_4(CSR_MHPMCOUNTER(4), val)
	switchcase_csr_write_8(CSR_MHPMCOUNTER(8), val)
	switchcase_csr_write_16(CSR_MHPMCOUNTER(16), val)
#if __riscv_xlen == 32
	switchcase_csr_write(CSR_MCYCLEH, val)
	switchcase_csr_write(CSR_MINSTRETH, val)
	switchcase_csr_write(CSR_MHPMCOUNTERH(3), val)
	switchcase_csr_write_4(CSR_MHPMCOUNTERH(4), val)
	switchcase_csr_write_8(CSR_MHPMCOUNTERH(8), val)
	switchcase_csr_write_16(CSR_MHPMCOUNTERH(16), val)
	switchcase_csr_write(CSR_MHPMEVENTH(3), val)
	switchcase_csr_write_4(CSR_MHPMEVENTH(4), val)
	switchcase_csr_write_8(CSR_MHPMEVENTH(8), val)
	switchcase_csr_write_16(CSR_MHPMEVENTH(16), val)
#endif
	switchcase_csr_write(CSR_MCOUNTINHIBIT, val)
	switchcase_csr_write(CSR_MHPMEVENT(3), val)
	switchcase_csr_write_4(CSR_MHPMEVENT(4), val)
	switchcase_csr_write_8(CSR_MHPMEVENT(8), val)
	switchcase_csr_write_16(CSR_MHPMEVENT(16), val)

	default:
		sbi_printf("%s: Unknown CSR %#x", __func__, csr_num);
		bh_panic();
		break;
	}

#undef switchcase_csr_write_64
#undef switchcase_csr_write_32
#undef switchcase_csr_write_16
#undef switchcase_csr_write_8
#undef switchcase_csr_write_4
#undef switchcase_csr_write_2
#undef switchcase_csr_write
}
