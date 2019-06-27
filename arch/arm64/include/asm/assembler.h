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

/*
 * Pseudo-ops for PC-relative adr/ldr/str <reg>, <symbol> where
 * <symbol> is within the range +/- 4 GB of the PC.
 */
	/*
	 * @dst: destination register (64 bit wide)
	 * @sym: name of the symbol
	 * @tmp: optional scratch register to be used if <dst> == sp, which
	 *       is not allowed in an adrp instruction
	 */
	.macro	adr_l, dst, sym, tmp=
	.ifb	\tmp
	adrp	\dst, \sym
	add	\dst, \dst, :lo12:\sym
	.else
	adrp	\tmp, \sym
	add	\dst, \tmp, :lo12:\sym
	.endif
	.endm

	/*
	 * @dst: destination register (32 or 64 bit wide)
	 * @sym: name of the symbol
	 * @tmp: optional 64-bit scratch register to be used if <dst> is a
	 *       32-bit wide register, in which case it cannot be used to hold
	 *       the address
	 */
	.macro	ldr_l, dst, sym, tmp=
	.ifb	\tmp
	adrp	\dst, \sym
	ldr	\dst, [\dst, :lo12:\sym]
	.else
	adrp	\tmp, \sym
	ldr	\dst, [\tmp, :lo12:\sym]
	.endif
	.endm

	/*
	 * @src: source register (32 or 64 bit wide)
	 * @sym: name of the symbol
	 * @tmp: mandatory 64-bit scratch register to calculate the address
	 *       while <src> needs to be preserved.
	 */
	.macro	str_l, src, sym, tmp
	adrp	\tmp, \sym
	str	\src, [\tmp, :lo12:\sym]
	.endm
#endif

#endif /* __ARM64_ASSEMBLER_H_INCLUDE__ */
