#ifndef __TIME_STD_H_INCLUDE__
#define __TIME_STD_H_INCLUDE__

#include <target/jiffies.h>

typedef unsigned long time_t;
typedef tsc_count_t clock_t;

#define CLOCKS_PER_SEC		((clock_t)1000000)

clock_t clock(void);

#endif /* __TIME_STD_H_INCLUDE__ */
