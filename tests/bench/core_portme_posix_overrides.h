/*
Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Original Author: Shay Gal-on
*/

/* Topic: Description
        This file contains additional configuration constants required to execute on
   different platforms over and above the POSIX defaults
*/
#ifndef CORE_PORTME_POSIX_OVERRIDES_H
#define CORE_PORTME_POSIX_OVERRIDES_H

/* CoreMark Porting Guide
 *
 * Following files can be directly copied from coremark newest version:
 *  o core_list_join.c
 *  o core_matrix.c
 *  o core_state.c
 *  o core_util.c
 * Following files need to be modified:
 *  x posix/core_portme.h:           Comment out HAS_PRINTF
 *  x coremark.h:                    Add ifdef COREMARK_H stuffs
 *  x core_portme_posix_overridex.h: Reimplementation is required
 *  x core_portme.c:                 Convert malloc/free to ee_malloc/ee_free
 *                                   Add USE_TSC implementation
 *                                   Add ifdef HOSTED around start_time_val
 *                                   Add ifdef HOSTED around stop_time_val
 *  x core_main.c:                   Convert main to COREMARK_MAIN
 *                                   Convert ee_printf to printf for the result
 *                                   Add ifdef HOSTED around static_memblk
 *                                   Add static_total_errors assignment
 */
/* CoreMark Configurations Converted to Kconfig:
 * RUN:
 *  -DVALIDATION_RUN=1
 *  -DPERFORMANCE_RUN=1
 *  -DPROFILE_RUN=1
 * SEED_METHOD:
 *  -DSEED_METHOD=SEED_VOLATILE
 *  -DSEED_METHOD=SEED_ARG
 *  -DSEED_METHOD=SEED_FUNC
 * MEM_METHOD:
 *  -DMEM_METHOD=MEM_STATIC
 *  -DMEM_METHOD=MEM_STACK
 *  -DMEM_METHOD=MEM_MALLOC
 *
 * Multithreading support:
 *  -DMULTITHREAD=N
 * Options when MULTITHREAD > 1:
 *  -DUSE_PTHREAD=1
 *  -DUSE_FORK=1
 *  -DUSE_SOCKET=1
 *
 * Other options:
 *  -DTOTAL_DATA_SIZE=?
 *  -DITERATIONS=?
 *  -DCORE_DEBUG=1
 *  -DPRINT_ARGS=1
 *  -DMEM_LOCATION=1
 *  -DMEM_LOCATION_UNSPEC=1
 *  -DCOMPILER_REQUIRES_SORT_RETURN=1
 */

#ifdef CONFIG_COREMARK_RUN_VALIDATION
#define VALIDATION_RUN		1
#endif
#ifdef CONFIG_COREMARK_RUN_PERFORMANCE
#define PERFORMANCE_RUN		1
#endif
#ifdef CONFIG_COREMARK_RUN_PROFILE
#define PROFILE_RUN		1
#endif

#ifdef CONFIG_COREMARK_SEED_VOLATILE
#define SEED_METHOD		SEED_VOLATILE
#endif
#ifdef CONFIG_COREMARK_SEED_ARG
#define SEED_METHOD		SEED_ARG
#endif
#ifdef CONFIG_COREMARK_SEED_FUNC
#define SEED_METHOD		SEED_FUNC
#endif

#ifdef CONFIG_COREMARK_MEM_STATIC
#define MEM_METHOD		MEM_STATIC
#define MEM_LOCATION		"Static"
#endif
#ifdef CONFIG_COREMARK_MEM_STACK
#define MEM_METHOD		MEM_STACK
#define MEM_LOCATION		"Stack"
#endif
#ifdef CONFIG_COREMARK_MEM_MALLOC
#define MEM_METHOD		MEM_MALLOC
#define MEM_LOCATION		"Heap"
#endif

#ifdef CONFIG_COREMARK_DEBUG
#define CORE_DEBUG		1
#else
#define CORE_DEBUG		0
#endif
#ifdef CONFIG_COREMARK_PRINT_ARGS
#define PRINT_ARGS		1
#else
#define PRINT_ARGS		0
#endif

#ifdef CONFIG_COREMARK_MULTITHREAD
#define MULTITHREAD		NR_CPUS
#define USE_CPU_BENCH		1
#else
#define MULTITHREAD		1
#endif

#ifdef CONFIG_COREMARK_ITERATIONS
#define ITERATIONS		CONFIG_COREMARK_ITERATIONS
#else
#define ITERATIONS		0
#endif

/* Stop compiler's complaints */
#define CALLGRIND_RUN			0
#define MICA				0
#define COMPILER_REQUIRES_SORT_RETURN	1

#ifdef HOSTED
#define HAS_PRINTF		1
#define ee_malloc(size)		malloc(size)
#define ee_free(ptr)		free(ptr)
#define COREMARK_MAIN		main
#else /* HOSTED */
#include <target/heap.h>
#include <target/bench.h>
#include <target/percpu.h>

#define ee_malloc(size)		(void *)heap_alloc(size)
#define ee_free(ptr)		heap_free((caddr_t)ptr)

/* Standard library specific options:
 *  -DHAS_FLOAT=1
 *  -DHAS_STDIO=1
 *  -DHAS_PRINTF=1
 *  -DUSE_CLOCK=1
 *  -DHAS_TIME_H=1
 */
#define HAS_STDIO		1
#ifdef CONFIG_FP
#define HAS_FLOAT		1
#else /* CONFIG_FP */
#undef HAS_FLOAT
#endif /* CONFIG_FP */
#define HAS_TIME_H		1
#ifdef CONFIG_TEST_VERBOSE
#define ee_printf(...)		printf(__VA_ARGS__)
#else /* CONFIG_TEST_VERBOSE */
#define ee_printf(fmt, ...)	do { } while (0)
#endif /* CONFIG_TEST_VERBOSE */
#ifdef CONFIG_COREMARK_TIME_CLOCK
#define USE_CLOCK		1
#endif /* CONFIG_COREMARK_TIME_CLOCK */
#ifdef CONFIG_COREMARK_TIME_TSC
#define USE_TSC			1
#endif /* CONFIG_COREMARK_TIME_TSC */
#define MAIN_HAS_NORETURN	0

#define COREMARK_MAIN		__coremark
#endif /* HOSTED */

#endif
