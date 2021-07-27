#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <pthread.h>

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

#ifdef LITMUS_DUMP_TEST
void parse_cmd(int argc, char **argv, void *def, void *p)
{
}

void *pb_create(int nprocs)
{
	return NULL;
}

void pb_free(void *p)
{
}
#else
#include "utils.h"
#endif

static int litmus_mem_fd = -1;
static void *litmus_mem_map = MAP_FAILED;
static int litmus_dump_count = 0;
static pthread_mutex_t litmus_dump_lock;

int litmus_dump_init(void)
{
	pthread_mutex_init(&litmus_dump_lock, NULL);
#ifndef LITMUS_DUMP_TEST_INSTRUMENT
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
#endif
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
	pthread_mutex_destroy(&litmus_dump_lock);
}

static void __attribute__((__no_instrument_function__))
litmus_dump_writel(uint32_t val, uint32_t reg)
{
	if (litmus_mem_map == MAP_FAILED)
		return;
	*((uint32_t *)((uint8_t *)litmus_mem_map + MSG_REG_BASE + reg)) = val;
}

static uint32_t __attribute__((__no_instrument_function__))
litmus_dump_readl(uint32_t reg)
{
	if (litmus_mem_map == MAP_FAILED)
		return 0;
	return *((uint32_t *)((uint8_t *)litmus_mem_map + MSG_REG_BASE + reg));
}

#ifdef LITMUS_DUMP_TEST_INSTRUMENT
static void __litmus_dump_debug(bool start)
{
	if (start)
		printf("B: %d\n", litmus_dump_count);
	else
		printf("E: %d\n", litmus_dump_count);
}
#else
#define __litmus_dump_debug(start)		do { } while (0)
#endif

static void __litmus_dump_start(void)
{
	__litmus_dump_debug(true);
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
	__litmus_dump_debug(false);
}

void __attribute__((__no_instrument_function__))
__cyg_profile_func_enter(void *this_func, void *call_site)
{
	pthread_mutex_lock(&litmus_dump_lock);
	if (this_func == pb_create) {
		if (litmus_dump_count == 1)
			__litmus_dump_start();
		litmus_dump_count++;
	}
	pthread_mutex_unlock(&litmus_dump_lock);
}

void __attribute__((__no_instrument_function__))
__cyg_profile_func_exit(void *this_func, void *call_site)
{
	pthread_mutex_lock(&litmus_dump_lock);
	if (this_func == pb_free) {
		litmus_dump_count--;
		if (litmus_dump_count == 1)
			__litmus_dump_stop();
	}
	if (this_func == parse_cmd) {
		if (litmus_dump_count != 0) {
			printf("Fatal: pb_create/pb_free not paired!\n");
			litmus_dump_count = 0;
		}
	}
	pthread_mutex_unlock(&litmus_dump_lock);
}

#ifdef LITMUS_DUMP_TEST
static void test(void)
{
	void *p0, *p1;

	parse_cmd(0, NULL, NULL, NULL);
	p0 = pb_create(0);
	p1 = pb_create(0);
	pb_free(p1);
	pb_free(p0);
}

void main(void)
{
	litmus_dump_init();
	test();
	test();
	test();
	test();
	test();
	test();
	test();
	test();
	test();
	test();
	test();
	test();
	litmus_dump_exit();
}
#endif
