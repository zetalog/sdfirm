#ifndef __TSC_GUANGMU_H_INCLUDE__
#define __TSC_GUANGMU_H_INCLUDE__

#include <asm/clint.h>

#define FREQ_RTC		10000000
#define TSC_FREQ		(FREQ_RTC/1000)
#define TSC_MAX			ULL(0xFFFFFFFFFFFFFFFF)

#if defined(CONFIG_RISCV_COUNTERS) || defined(CONFIG_SBI)
#define tsc_hw_read_counter()	rdtime()
#else
#define tsc_hw_read_counter()	clint_read_mtime()
#endif

#define tsc_hw_ctrl_init()	do { } while (0)

#endif /* __TSC_GUANGMU_H_INCLUDE__ */
