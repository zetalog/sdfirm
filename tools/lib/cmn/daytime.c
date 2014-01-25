#include <time.h>
#include <stdio.h>
#include <sys/timeb.h>
#ifdef WIN32
#include <winsock2.h>
#endif
#include <sys/time.h>

#undef timezone
struct timezone {
	int tv_minuteswest;
	int tv_dsttime;
};

#define TICKSPERSEC                 10000000
#define TICKSPERMSEC                10000
#define SECSPERDAY                  86400
#define SECSPERHOUR                 3600
#define SECSPERMIN                  60
#define MINSPERHOUR                 60
#define HOURSPERDAY                 24
#define EPOCHWEEKDAY                1 /* Jan 1, 1601 was Monday */
#define DAYSPERWEEK                 7
#define EPOCHYEAR                   1601
#define DAYSPERNORMALYEAR           365
#define DAYSPERLEAPYEAR             366
#define MONSPERYEAR                 12
#define DAYSPERQUADRICENTENNIUM     (365 * 400 + 97)
#define DAYSPERNORMALCENTURY        (365 * 100 + 24)
#define DAYSPERNORMALQUADRENNIUM    (365 * 4 + 1)

/* 1601 to 1970 is 369 years plus 89 leap days */
#define SECS_1601_TO_1970  ((369 * 365 + 89) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1970 (SECS_1601_TO_1970 * TICKSPERSEC)
/* 1601 to 1980 is 379 years plus 91 leap days */
#define SECS_1601_TO_1980  ((379 * 365 + 91) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1980 (SECS_1601_TO_1980 * TICKSPERSEC)

int gettimeofday(struct timeval *tp, struct timezone *tz)
{
	SYSTEMTIME utc;
	FILETIME ft;
	LARGE_INTEGER li;

	GetSystemTime(&utc);
	SystemTimeToFileTime(&utc, &ft);

	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;

	tp->tv_sec = (ULONG)((li.QuadPart - TICKS_1601_TO_1970) / TICKSPERSEC);
	tp->tv_usec = (ULONG)(li.QuadPart % 10);

	return 0;
}

int settimeofday(const struct timeval *tp, const struct timezone *tz)
{
	FILETIME ft;
	LARGE_INTEGER li;
	SYSTEMTIME utc;
	
	li.QuadPart = tp->tv_sec * (ULONGLONG)TICKSPERSEC + TICKS_1601_TO_1970;
	li.QuadPart += tp->tv_usec * 10;

	ft.dwLowDateTime = li.LowPart;
	ft.dwHighDateTime = li.HighPart;

	FileTimeToSystemTime(&ft, &utc);
	SetSystemTime(&utc);

	return 0;
}

struct tm *localtime_r(const time_t *l_clock, struct tm *result)
{
	memcpy(result, localtime(l_clock), sizeof(*result));
	return result;
}

int clock_gettime(int c, struct timespec *ts)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	TIMEVAL_TO_TIMESPEC(&tv, ts);
	return 0;
}
