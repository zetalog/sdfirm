#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#ifndef PAGE_SIZE
#define PAGE_SIZE			4096
#endif
#define MSG_MAP_BASE			0xFF601FF000		    
#define MSG_REG_BASE			0xF00
#define MSG_DUMP_CTRL			0xF0
#define MSG_DUMP_START_REQ		1
#define MSG_DUMP_START_REP		2
#define MSG_DUMP_STOP_REQ		3
#define MSG_DUMP_STOP_REP		0

static int litmus_mem_fd = -1;
static void *litmus_mem_map = MAP_FAILED;

int litmus_dump_init(void)
{
	litmus_mem_fd = open( "/dev/mem", O_RDWR | O_SYNC);
	if (litmus_mem_fd == -1) {
		perror("open /dev/mem");
		return -ENODEV;
	}
	litmus_mem_map = (void *)mmap(0, PAGE_SIZE,
				      PROT_READ | PROT_WRITE, MAP_SHARED,
				      litmus_mem_fd, MSG_MAP_BASE);
	if (litmus_mem_map == MAP_FAILED) {
		perror("mmap /dev/mem");
		return -ENOMEM;
	}
	return 0;
}

void litmus_dump_exit(void)
{
	if (litmus_mem_map != MAP_FAILED) {
		munmap(litmus_mem_map, PAGE_SIZE);
		litmus_mem_map = MAP_FAILED;
	}
	if (litmus_mem_fd != -1) {
		close(litmus_mem_fd);
		litmus_mem_fd = -1;
	}
}

static void litmus_dump_writel(uint32_t val, uint32_t reg)
{
	if (litmus_mem_map == MAP_FAILED)
		return;
	*((uint32_t *)((uint8_t *)litmus_mem_map + MSG_REG_BASE + reg)) = val;
}

static uint32_t litmus_dump_readl(uint32_t reg)
{
	if (litmus_mem_map == MAP_FAILED)
		return 0;
	return *((uint32_t *)((uint8_t *)litmus_mem_map + MSG_REG_BASE + reg));
}

bool litmus_dumping = false;

static void __litmus_dump_start(void)
{
	litmus_dump_writel(MSG_DUMP_START_REQ, MSG_DUMP_CTRL);
#ifdef LITMUS_DUMP_WAIT
	while (litmus_dump_readl(MSG_DUMP_CTRL) != MSG_DUMP_START_REP);
#endif
}

static void __litmus_dump_stop(void)
{
	litmus_dump_writel(MSG_DUMP_STOP_REQ, MSG_DUMP_CTRL);
#ifdef LITMUS_DUMP_WAIT
	while (litmus_dump_readl(MSG_DUMP_CTRL) != MSG_DUMP_STOP_REP);
#endif
}

void litmus_dump_start(void)
{
	litmus_dumping = true;
}

void litmus_dump_stop(void)
{
	litmus_dumping = false;
}

typedef unsigned long long tsc_t;
extern tsc_t timeofday(void);

void __attribute__((__no_instrument_function__))
__cyg_profile_func_enter(void *this_func, void *call_site)
{
	if (this_func == timeofday) {
		if (litmus_dumping)
			__litmus_dump_start();
		else
			__litmus_dump_stop();
	}
}

void __attribute__((__no_instrument_function__))
__cyg_profile_func_exit(void *this_func, void *call_site)
{
	if (this_func == timeofday) {
		if (litmus_dumping)
			litmus_dumping = false;
	}
}

#ifdef LITMUS_DUMP_TEST
void main(void)
{
	litmus_dump_init();
	litmus_dump_start();
	litmus_dump_stop();
	litmus_dump_exit();
}
#endif
