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

	/* tcr_set_idmap_t0sz - update TCR.T0SZ so that we can load the ID
	 * map
	 */
	.macro	tcr_set_idmap_t0sz, valreg, tmpreg
#if (defined(CONFIG_CPU_64v8_2_LVA) && (VMSA_VA_SIZE_SHIFT != 52)) || \
    (!defined(CONFIG_CPU_64v8_2_LVA) && (VMSA_VA_SIZE_SHIFT != 48))
	ldr_l	\tmpreg, idmap_t0sz
	bfi	\valreg, \tmpreg, #TCR_T0SZ_OFFSET, #TCR_TxSZ_WIDTH
#endif
	.endm

	.macro	init_sp_early
#ifdef CONFIG_SMP
	mrs	x0, MPIDR_EL1
	and	x1, x0, #0xff
	and	x0, x0, #0xff00
	lsr	x0, x0, #7
	add	x0, x0, x1

	add	x0, x0, #1
	lsl	x0, x0, #PERCPU_STACK_SHIFT
	ldr	x3, =PERCPU_STACKS_START
	add	x0, x3, x0
	mov	sp, x0
#else
	ldr	x0, =PERCPU_STACKS_END
	mov	sp, x0
#endif
	.endm

	.macro	init_bss
	adr_l	x0, __bss_start
	adr_l	x1, __bss_stop
	mov	x2, xzr
bss_init_loop:
	stp	x2, x2, [x0], #16
	cmp	x0, x1
	b.lt	bss_init_loop
	dsb	ishst
	.endm
#endif

#endif /* __ARM64_ASSEMBLER_H_INCLUDE__ */
