#ifndef __TSC_LM3S9B92_H_INCLUDE__
#define __TSC_LM3S9B92_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <asm/reg.h>
#include <asm/mach/mem.h>
#include <asm/mach/clk.h>
#include <asm/mach/tick.h>
#include <asm/mach/timer.h>

#ifdef CONFIG_TSC_LM3S9B92_GPT1
#define TSC_MAX				0xFFFFFFFF
#define TSC_FREQ			CLK_GPT32
#else
#define TSC_MAX				SYSTICK_HW_MAX_COUNT
#define TSC_FREQ			__SYSTICK_HW_FREQ
#endif

#endif /* __TSC_LM3S9B92_H_INCLUDE__ */
