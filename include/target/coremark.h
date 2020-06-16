#ifndef __COREMARK_H_INCLUDE__
#define __COREMARK_H_INCLUDE__

#ifndef TOTAL_DATA_SIZE
#ifdef CONFIG_COREMARE_DATA_SIZE
#define TOTAL_DATA_SIZE CONFIG_COREMARE_DATA_SIZE
#else
#define TOTAL_DATA_SIZE (2 * 1000)
#endif
#endif

struct coremark_context {
	unsigned char static_memblk[TOTAL_DATA_SIZE];
	int result;
};

#endif /* __COREMARK_H_INCLUDE__ */
