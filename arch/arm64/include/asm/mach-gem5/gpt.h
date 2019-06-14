#ifndef __GPT_GEM5_H_INCLUDE__
#define __GPT_GEM5_H_INCLUDE__

#include <target/config.h>
#include <asm/reg.h>
#include <asm/timer.h>

/* msec based */
#define GPT_MAX_TIMEOUT					\
	(SYSTICK_HW_COUNTER(ULL(0xFFFFFFFFFFFFFFFF)) /	\
	 TICKS_TO_MILISECONDS)

#endif /* __GPT_GEM5_H_INCLUDE__ */
