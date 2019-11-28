#ifndef __TSC_GEM5_H_INCLUDE__
#define __TSC_GEM5_H_INCLUDE__

#include <target/arch.h>
#include <asm/timer.h>

/* kHz based */
#define TSC_FREQ	(SYSTICK_HW_FREQUENCY/1000)
#define TSC_MAX		((ULL(1) << 56) - 1)

#endif /* __TSC_GEM5_H_INCLUDE__ */
