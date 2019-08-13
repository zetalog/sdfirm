#ifndef __DELAY_RISCV_H_INCLUDE__
#define __DELAY_RISCV_H_INCLUDE__

#define __delay(count)					\
	do {						\
		__asm__ __volatile__ (			\
			"1:			\n"	\
			"addi	%0, %0, -1	\n"	\
			"bnez	%0, 1b		\n"	\
			: : "r" (count) : "memory");	\
	} while (0)
#define ARCH_HAVE_DELAY 1

#endif /* __DELAY_RISCV_H_INCLUDE__ */
