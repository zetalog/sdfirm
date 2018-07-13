#ifndef __DELAY_ARM64_H_INCLUDE__
#define __DELAY_ARM64_H_INCLUDE__

#if 0
static inline void __delay(lps_t count)
{
	__asm__("1:			\n\t"
		"sub	%0, %0, #1	\n\t"
		"bne	1b		\n\t"
		: : "r"(count));
}
#define ARCH_HAVE_DELAY 1
#endif

#endif /* __DELAY_ARM64_H_INCLUDE__ */
