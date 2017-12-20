#ifndef __JIFFIES_H_INCLUDE__
#define __JIFFIES_H_INCLUDE__

#include <target/config.h>
#include <target/generic.h>
#include <target/tsc.h>

typedef uint32_t tick_t;

#define HZ				1000

void tick_handler(void);

/* tick_update_tsc is only used by CALIBRATE_TICK. */
#if defined (CONFIG_TICK) && !defined (CONFIG_TICK_PERIODIC)
void tick_update_tsc(tsc_count_t counter);
#else
#define tick_update_tsc(counter)
#endif

#ifdef CONFIG_TICK
extern volatile tick_t jiffies;
#endif

#endif /* __JIFFIES_H_INCLUDE__ */
