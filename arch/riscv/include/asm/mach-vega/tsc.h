#ifndef __TSC_VEGA_H_INCLUDE__
#define __TSC_VEGA_H_INCLUDE__

#include <asm/mach/tstmr.h>

#define TSC_FREQ	(TSTMR_FREQ/1000)
#define TSC_MAX		((ULL(1) << 56) - 1)

#define tsc_hw_read_counter()	tstmr_read_counter()
#define tsc_hw_ctrl_init()

#endif /* __TSC_VEGA_H_INCLUDE__ */
