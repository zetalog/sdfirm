#ifndef __PWM_UNLEASHED_H_INCLUDE__
#define __PWM_UNLEASHED_H_INCLUDE__

#include <target/arch.h>

#define PWM_BASE(n)		(PWM0_BASE + (n) * 0x1000)
#define PWM_REG(n, offset)	(PWM_BASE(n) + (offset))

#define PWMCFG(n)		PWM_REG(n, 0x00)
#define PWMCOUNT(n)		PWM_REG(n, 0x08)
#define PWMS(n)			PWM_REG(n, 0x10)
#define PWMCMP(n, cmp)		PWM_REG(n, 0x20 + 0x04 * (cmp))

#ifdef CONFIG_SIFIVE_PWM_CMPWIDTH
#define PWM_CMPWIDTH		CONFIG_SIFIVE_PWM_CMPWIDTH
#else
#define PWM_CMPWIDTH		16
#endif

/* PWMCFG */
#define PWMSCALE_OFFSET		0
#define PWMSCALE_MASK		REG_4BIT_MASK
#define PWMSCALE(value)		_SET_FV(PWMSCALE, value)
#define PWMSTICKY		_BV(8)
#define PWMZEROCMP		_BV(9)
#define PWMDEGLITCH		_BV(10)
#define PWMENALWAYS		_BV(12)
#define PWMENONESHOT		_BV(13)
#define PWMCMPCENTER(n)		_BV(16+(n))
#define PWMCMPGANG(n)		_BV(24+(n))
#define PWMCMPIP(n)		_BV(28+(n))

#define pwm_disable(n)					\
	__raw_writel(0, PWMCFG(n))
#define pwm_enable_always(n)				\
	do {						\
		__raw_setl(PWMENALWAYS, PWMCFG(n));	\
		__raw_clearl(PWMENONESHOT, PWMCFG(n));	\
	} while (0)
#define pwm_enable_oneshot(n)				\
	do {						\
		__raw_clearl(PWMENALWAYS, PWMCFG(n));	\
		__raw_setl(PWMENONESHOT, PWMCFG(n));	\
	} while (0)

#endif /* __PWM_UNLEASHED_H_INCLUDE__ */
