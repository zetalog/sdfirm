#ifndef __GPT_QDF2400_H_INCLUDE__
#define __GPT_QDF2400_H_INCLUDE__

#include <asm/timer.h>

/* msec based */
#define GPT_MAX_TIMEOUT					\
	(SYSTICK_HW_COUNTER(ULL(0xFFFFFFFFFFFFFFFF)) /	\
	 TSC_FREQ)

#endif /* __GPT_QDF2400_H_INCLUDE__ */
