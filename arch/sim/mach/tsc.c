#include "mach.h"
#include <target/tsc.h>

uint16_t tsc_hw_freq;

uint16_t tsc_hw_get_frequency(void)
{
	LARGE_INTEGER large_int;
	QueryPerformanceFrequency(&large_int);
	return (uint16_t)(large_int.QuadPart/10000/1000);
}

tsc_count_t tsc_hw_read_counter(void)
{
	LARGE_INTEGER large_int;
	QueryPerformanceCounter(&large_int);
	return (tsc_count_t)(large_int.QuadPart/10000);
}

void tsc_hw_ctrl_init(void)
{
	tsc_hw_freq = tsc_hw_get_frequency();
}
