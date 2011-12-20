#ifndef __SYS_TIME_H_INCLUDE__
#define __SYS_TIME_H_INCLUDE__


struct timespec {
	time_t tv_sec;			/* Seconds.  */
	long int tv_nsec;		/* Nanoseconds.  */
};

#ifndef TIMESPEC_TO_TIMEVAL
# define TIMESPEC_TO_TIMEVAL(tv, ts) {				\
	(tv)->tv_sec = (ts)->tv_sec;				\
	(tv)->tv_usec = (ts)->tv_nsec / 1000;			\
}
#endif

#ifndef TIMEVAL_TO_TIMESPEC
# define TIMEVAL_TO_TIMESPEC(tv, ts) {				\
	(ts)->tv_sec = (tv)->tv_sec;				\
	(ts)->tv_nsec = (tv)->tv_usec * 1000;			\
}
#endif

/* Convenience macros for operations on timevals.
   NOTE: `timercmp' does not work for >= or <=.  */
#define	timerisset(tvp)		((tvp)->tv_sec || (tvp)->tv_usec)
#define	timeradd(a, b, result)						\
	do {								\
		(result)->tv_sec = (a)->tv_sec + (b)->tv_sec;		\
		(result)->tv_usec = (a)->tv_usec + (b)->tv_usec;	\
		if ((result)->tv_usec >= 1000000) {			\
			++(result)->tv_sec;				\
			(result)->tv_usec -= 1000000;			\
		}							\
	} while (0)
#define	timersub(a, b, result)						\
	do {								\
		(result)->tv_sec = (a)->tv_sec - (b)->tv_sec;		\
		(result)->tv_usec = (a)->tv_usec - (b)->tv_usec;	\
		if ((result)->tv_usec < 0) {				\
			--(result)->tv_sec;				\
			(result)->tv_usec += 1000000;			\
		}							\
	} while (0)

int gettimeofday(struct timeval *tv, struct timezone *tz);

#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#endif
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif
#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID 2
#endif
#ifndef CLOCK_THREAD_CPUTIME_ID
#define CLOCK_THREAD_CPUTIME_ID 3
#endif
#ifndef CLOCK_REALTIME_HR
#define CLOCK_REALTIME_HR 4
#endif
#ifndef CLOCK_MONOTONIC_HR
#define CLOCK_MONOTONIC_HR 5
#endif
int clock_gettime(int c, struct timespec *ts);

#endif /* __SYS_TIME_H_INCLUDE__ */
