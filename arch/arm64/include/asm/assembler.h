#ifndef __ARM64_ASSEMBLER_H_INCLUDE__
#define __ARM64_ASSEMBLER_H_INCLUDE__

#include <target/config.h>
#include <asm/vmsa.h>

#ifdef __ASSEMBLY__
#define ENDPIPROC(x)			\
	.globl	__pi_##x;		\
	.type 	__pi_##x, %function;	\
	.set	__pi_##x, x;		\
	.size	__pi_##x, . - x;	\
	ENDPROC(x)

	.macro save_and_disable_daif, flags
	mrs	\flags, daif
	msr	daifset, #0xf
	.endm

	.macro disable_daif
	msr	daifset, #0xf
	.endm

	.macro enable_daif
	msr	daifclr, #0xf
	.endm

	.macro	restore_daif, flags:req
	msr	daif, \flags
	.endm

	.macro	offset_ttbr1, ttbr
#ifdef CONFIG_CPU_64v8_2_LVA
	orr	\ttbr, \ttbr, #TTBR1_BADDR_4852_OFFSET
#endif
	.endm

	.macro	phys_to_ttbr, ttbr, phys
#ifdef CONFIG_CPU_64v8_2_LVA
	orr	\ttbr, \phys, \phys, lsr #46
	and	\ttbr, \ttbr, #TTBR_BADDR_MASK_52
#else
	mov	\ttbr, \phys
#endif
	.endm

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

	.macro	call_l, sym, tmp=x27
        mov     \tmp, lr
	bl      \sym
	mov     lr, \tmp
	.endm
#endif

#endif /* __ARM64_ASSEMBLER_H_INCLUDE__ */
