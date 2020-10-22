#ifndef __LINPACK_H_INCLUDE__
#define __LINPACK_H_INCLUDE__

#ifdef HOSTED
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#define __unused		__attribute__((__unused__))
#else
#include <target/generic.h>
#include <target/bench.h>
#include <target/percpu.h>
#include <target/jiffies.h>
#include <target/spinlock.h>
#endif
#include <time.h>

#define UNROLL
#define DP

#ifdef SP
#define REAL float
#define ZERO 0.0
#define ONE 1.0
#define PREC "Single"
#endif

#ifdef DP
#define REAL double
#define ZERO 0.0e0
#define ONE 1.0e0
#define PREC "Double"
#endif

#ifdef ROLL
#define ROLLING "Rolled"
#endif
#ifdef UNROLL
#define ROLLING "Unrolled"
#endif

/* VERSION */
#ifdef CONFIG_CC_OPT_SPEED
    #define options   "Optimised"
    #define opt "1"
#else
    #define options   "Non-optimised"
    #define opt "0"
#endif

#define USECS_PER_SEC	(1 * 1000 * 1000)

typedef REAL raa200_t[200*200];
typedef REAL raa201_t[200*201];
typedef REAL ra200_t[200];
typedef int ia200_t[200];
typedef REAL atime_t[9][15];

struct linpack_context {
	raa200_t aa;
	raa201_t a;
	ra200_t b;
	ra200_t x;
	ia200_t ipvt;
	atime_t atime;
	time_t startUsecs;
	time_t usecs;
	uint16_t phase;
};

#ifdef HOSTED
#define __printf(...)			printf(__VA_ARGS__)
#define __fprintf(fp, ...)		fprintf(fp, __VA_ARGS__)
#define __print_time(t)			print_time(t)
#define spin_lock(__lock)		do { } while (0)
#define spin_unlock(__lock)		do { } while (0)
#define smp_processor_id()		0
#define __define_testfn(__func, __size, __align, __flags, __repeat, __timeout)
#else
#ifdef CONFIG_TEST_VERBOSE
#define __printf(...)			printf(__VA_ARGS__)
#define __fprintf(fp, ...)		printf(__VA_ARGS__)
#define __print_time(t)			print_time(t)
#else
#define __printf(fmt, ...)		do { } while (0)
#define __fprintf(fp, fmt, ...)		do { } while (0)
#define __print_time(t)			do { } while (0)
#endif
#endif
#define do_printf(...)			__printf(__VA_ARGS__)
#define do_fprintf(fp, ...)		__fprintf(fp, __VA_ARGS__)
#define do_spin_lock(__lock)		spin_lock(__lock)
#define do_spin_unlock(__lock)		spin_unlock(__lock)
#define fabs		__builtin_fabs

void print_time(int row);

#endif /* __LINPACK_H_INCLUDE__ */
