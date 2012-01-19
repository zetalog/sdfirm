#ifndef __TYPE_H_INCLUDE__
#define __TYPE_H_INCLUDE__

#include <time.h>
#include <assert.h>

typedef unsigned char boolean;
#define false			(unsigned char)0
#define true			!false

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned __int64 uint64_t;
typedef char int8_t;
typedef short int16_t;
typedef long int32_t;
#define ARCH_HAVE_SIZE_T 1

#ifdef WIN32
#include <winsock.h>

#define ENDIAN_BIG	1
#define ENDIAN_LITTLE	2
#define inline		__inline

typedef __int64 int64_t;
#define ARCH_HAVE_UINT64_T 1

#undef timezone
struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

#define sleep(x)	Sleep(x*1000)
#define msleep(x)	Sleep(x);
#define usleep(x)	Sleep(x/1000)

int gettimeofday(struct timeval *tp, struct timezone *tz);
int settimeofday(const struct timeval *tp, const struct timezone *tz);
size_t strlcpy(char *dst, const char *src, size_t size);
#endif
typedef unsigned int sockfd_t;

#ifndef __init
#define __init
#endif

#ifndef __exit
#define __exit
#endif

#ifndef BUG_ON
#define BUG_ON(expr)	assert(!(expr))
#endif

#endif /* __TYPE_H_INCLUDE__ */
