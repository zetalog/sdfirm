#include <target/percpu.h>
#include <target/raven.h>
#include <target/cpus.h>

typedef uint8_t raven_array_t[RAVEN_ARRAY_SIZE];
typedef uint8_t raven_l1miss_t[RAVEN_L1MISS_L2HIT_SIZE];

#ifdef CONFIG_RAVEN_GLOBAL_VAR
#ifdef CONFIG_RAVEN_EXEC_VAR
#define __raven_var	__always_cache
#else
#define __raven_var
#endif
#define RAVEN_CPU_EXEC_FLAGS	CPU_EXEC_SYNC

uint64_t x5_data;
uint64_t x7_data;
__raven_var raven_array_t test_array __aligned(8);
#else
#define RAVEN_CPU_EXEC_FLAGS	0
#endif

#ifdef CONFIG_RAVEN_PERCPU_VAR
#define RAVEN_CPU_EXEC_SIZE	(sizeof (raven_l1miss_t) + 16)

DEFINE_PERCPU(uint64_t, x5_data_percpu);
DEFINE_PERCPU(uint64_t, x7_data_percpu);
DEFINE_PERCPU(raven_l1miss_t, test_array_l1miss);
#else
#define RAVEN_CPU_EXEC_SIZE	0
#endif

int raven(caddr_t area)
{
	int nr_tests = ((uintptr_t)__testfn_end - (uintptr_t)__testfn_start) /
		       sizeof (struct cpu_exec_test);
	int ret;

	ret = cpu_local_exec(__testfn_start, nr_tests,
			     cpu_local_get_cpu_mask(),
			     RAVEN_CPU_EXEC_FLAGS,
			     CPU_WAIT_INFINITE, NULL);
	return ret ? 0 : 1;
}
__define_testfn(raven, RAVEN_CPU_EXEC_SIZE, SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
