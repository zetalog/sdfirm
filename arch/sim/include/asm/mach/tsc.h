#ifndef __TSC_MACH_H_INCLUDE__
#define __TSC_MACH_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>

#define TSC_MAX				0xFFFFFFFF
#define TSC_FREQ			tsc_hw_freq

uint16_t tsc_hw_freq;

tsc_count_t tsc_hw_read_counter(void);

#endif /* __TSC_MACH_H_INCLUDE__ */
