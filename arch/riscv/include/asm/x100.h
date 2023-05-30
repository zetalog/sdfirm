#ifndef __CSR_X100_H_INCLUDE__
#define __CSR_X100_H_INCLUDE__

#include <asm/spacemit.h>

#define CSR_BPC		0x7D0 /* basic performance control */
#define CSR_PPC		0x7D1 /* prefetch performance control */
#define CSR_ICGC	0xBF8 /* ICG control */
#define CSR_QRKC	0xBF9 /* bug quirks control */

#if defined(__ASSEMBLY__) && !defined(__DTS__) && !defined(LINKER_SCRIPT)
	.macro	x100_smp_init
	spacemit_smp_init
	.endm
	.macro	x100_init
	spacemit_init
	.endm
#endif

#endif /* __CSR_X100_H_INCLUDE__ */
