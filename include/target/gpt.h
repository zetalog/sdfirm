#ifndef __GPT_H_INCLUDE__
#define __GPT_H_INCLUDE__

#include <target/generic.h>

#ifndef __ASSEMBLY__
#if defined(CONFIG_TIMER_31BIT)
typedef int32_t timeout_t;
#define MAX_TIMEOUT	0x7FFFFFFF
#elif defined(CONFIG_TIMER_16BIT)
typedef int16_t timeout_t;
#define MAX_TIMEOUT	0x7FFF
#else
typedef int8_t timeout_t;
#define MAX_TIMEOUT	0x7F
#endif
#endif

#include <driver/gpt.h>

/* General Purpose Timers support */

/* timer devices can be used to implement:
 * 1. periodic tick device
 * 2. one shot timer device
 */
#ifdef CONFIG_TICK
#define gpt_periodic_start()		gpt_hw_periodic_start()
#define gpt_periodic_restart()		gpt_hw_periodic_restart()
#else
#define gpt_oneshot_timeout(tout_ms)	gpt_hw_oneshot_timeout(tout_ms)
#endif

#endif /* __GPT_H_INCLUDE__ */
