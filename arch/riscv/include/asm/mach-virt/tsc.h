#ifndef __TSC_VIRT_H_INCLUDE__
#define __TSC_VIRT_H_INCLUDE__

#include <asm/clint.h>

#define FREQ_RTC		10000000
#define TSC_FREQ		(FREQ_RTC/1000)
#define TSC_MAX			ULL(0xFFFFFFFFFFFFFFFF)

#define tsc_hw_read_counter()	clint_read_mtime()
#define tsc_hw_ctrl_init()

#endif /* __TSC_VIRT_H_INCLUDE__ */
