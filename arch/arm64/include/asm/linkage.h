#ifndef __LINKAGE_ARM64_H_INCLUDE__
#define __LINKAGE_ARM64_H_INCLUDE__

#define __ALIGN .align 2
#define __ALIGN_STR ".align 2"

#ifdef __ASSEMBLY__

#define ENDPROC(name) \
  .type name, %function; \
  END(name)

#define ENDPIPROC(x)			\
	.globl	__pi_##x;		\
	.type 	__pi_##x, %function;	\
	.set	__pi_##x, x;		\
	.size	__pi_##x, . - x;	\
	ENDPROC(x)

#endif /* __ASSEMBLY__ */

#endif /* __LINKAGE_ARM64_H_INCLUDE__ */
