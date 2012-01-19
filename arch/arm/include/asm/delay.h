#ifndef __DELAY_ARM_H_INCLUDE__
#define __DELAY_ARM_H_INCLUDE__

static inline void __delay(lps_t count)
{
	__asm__("1:		\n\t"
		"subs	%0, #1	\n\t"
		"bne	1b	\n\t"
		:
		: "r"(count));
}
#define ARCH_HAVE_DELAY 1

#endif /* __DELAY_ARM_H_INCLUDE__ */
