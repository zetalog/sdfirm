#ifndef __JIFFIES_H_INCLUDE__
#define __JIFFIES_H_INCLUDE__

#include <target/tsc.h>

#define HZ			1000
#define MSECS_PER_SEC		1000000

#ifndef __ASSEMBLY__
typedef uint32_t tick_t;
#endif

#define time_after(a, b)	((int32_t)((b) - (a)) < 0)
#define time_before(a, b)	time_after(b, a)
#define time_after_eq(a, b)	((int32_t)((a) - (b)) >= 0)
#define time_before_eq(a, b)	time_after_eq(b, a)

#ifndef __ASSEMBLY__
#if defined(CONFIG_TICK) || defined(CONFIG_TIMER) || \
    defined(CONFIG_TICK_PERIODIC)
void tick_handler(void);
#else
#define tick_handler()			do { } while (0)
#endif

/* tick_update_tsc is only used by CALIBRATE_TICK. */
#if defined (CONFIG_TICK) && !defined (CONFIG_TICK_PERIODIC)
void tick_update_tsc(tsc_count_t counter);
#else
#define tick_update_tsc(counter)
#endif

#ifdef CONFIG_TICK
extern volatile tick_t jiffies;
#define tick_get_counter()		jiffies
void tick_init(void);
#else
#define TSC_PER_HZ			(TSC_FREQ * 1000 / HZ)
#define tick_get_counter()		div32u(tsc_read_counter(), TSC_PER_HZ)
#define tick_init()			do { } while (0)
#endif
#endif

#endif /* __JIFFIES_H_INCLUDE__ */
