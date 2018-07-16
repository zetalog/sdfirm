#ifndef __DELAY_ARM64_H_INCLUDE__
#define __DELAY_ARM64_H_INCLUDE__

#define __delay(count)					\
	do {						\
		__asm__ __volatile__ (			\
			"1:			\n"	\
			"sub	%0, %0, #1	\n"	\
			"cmp	%0, xzr		\n"	\
			"bne	1b		\n"	\
			: : "r" (count) : "memory");	\
	} while (0)
#define ARCH_HAVE_DELAY 1

#endif /* __DELAY_ARM64_H_INCLUDE__ */
