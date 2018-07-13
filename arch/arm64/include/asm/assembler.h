#ifndef __ARM64_ASSEMBLER_H_INCLUDE__
#define __ARM64_ASSEMBLER_H_INCLUDE__

#include <target/config.h>

#ifdef __ASSEMBLY__
#define ENDPIPROC(x)			\
	.globl	__pi_##x;		\
	.type 	__pi_##x, %function;	\
	.set	__pi_##x, x;		\
	.size	__pi_##x, . - x;	\
	ENDPROC(x)
#endif

#endif /* __ARM64_ASSEMBLER_H_INCLUDE__ */
