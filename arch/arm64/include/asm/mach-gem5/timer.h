#ifndef __TIMER_GEM5_H_INCLUDE__
#define __TIMER_GEM5_H_INCLUDE__

#include <target/types.h>
#include <target/tsc.h>

#define SYSTICK_HW_COUNTER_OFFSET	0
#define SYSTICK_HW_COUNTER_MASK		((ULL(1) << 56) - 1)
#define SYSTICK_HW_COUNTER(value)	_FV(SYSTICK_HW_COUNTER, value)

/* mHz based */
#define SYSTICK_HW_FREQUENCY	ULL(0x1800000)

#endif /* __TIMER_GEM5_H_INCLUDE__ */
