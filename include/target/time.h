#ifndef __TIME_H_INCLUDE__
#define __TIME_H_INCLUDE__

#define TIME_OVERFLOW_HALF		((1<<((sizeof(time_t)*BITS_PER_UNIT)-1))-1)

#define time_after(t, u)					\
	((t > u) ||						\
	 ((u > TIME_OVERFLOW_HALF) &&				\
	  ((t-TIME_OVERFLOW_HALF) > (u-TIME_OVERFLOW_HALF))))

#endif /* __TIME_H_INCLUDE__ */
