#include "core_util.c"
#include "core_list_join.c"
#include "core_matrix.c"
#include "core_state.c"

#ifndef HOSTED
#if USE_CLOCK
#define CORETIMETYPE	clock_t
#endif
#if USE_TSC
#define CORETIMETYPE	tsc_count_t
#endif

struct coremark_context {
#ifdef CONFIG_COREMARK_MEM_STATIC
	ee_u8 static_memblk[TOTAL_DATA_SIZE];
#endif /* CONFIG_COREMARK_MEM_STATIC */
	ee_s16 static_total_errors;
	CORETIMETYPE start_time_val;
	CORETIMETYPE stop_time_val;
};

#ifdef CONFIG_COREMARK_STATIC_SPECIFIC
struct coremark_percpu {
	struct coremark_context *ptr;
} __cache_aligned;

struct coremark_percpu coremark_ctx[MAX_CPU_NUM];
#define get_coremark		(coremark_ctx[smp_processor_id()].ptr)
#endif /* CONFIG_COREMARK_STATIC_SPECIFIC */
#ifdef CONFIG_COREMARK_STATIC_PERCPU
DEFINE_PERCPU(struct coremark_context, coremark_ctx);
#define get_coremark		this_cpu_ptr(&coremark_ctx)
#endif /* CONFIG_COREMARK_STATIC_PERCPU */
#ifdef CONFIG_COREMARK_MEM_STATIC
#define static_memblk		get_coremark->static_memblk
#endif /* CONFIG_COREMARK_MEM_STATIC */
#define static_total_errors	get_coremark->static_total_errors
#define start_time_val		get_coremark->start_time_val
#define stop_time_val		get_coremark->stop_time_val
#endif /* HOSTED */

#include "core_portme.c"
#include "core_main.c"

#ifndef HOSTED
#define COREMARK_ARGC	8
#ifdef CONFIG_COREMARK_RUN_PERFORMANCE
#define COREMARK_ARGV	\
	{ "coremark", "0x0", "0x0", "0x66", "0", "7", "1", "2000" }
#endif /* CONFIG_COREMARK_RUN_PERFORMANCE */
#ifdef CONFIG_COREMARK_RUN_VALIDATION
#define COREMARK_ARGV	\
	{ "coremark", "0x3415", "0x3415", "0x66", "0", "7", "1", "2000" }
#endif /* CONFIG_COREMARK_RUN_VALIDATION */
#ifdef CONFIG_COREMARK_RUN_PROFILE
#define COREMARK_ARGV	\
	{ "coremark", "8", "8", "8", "0", "7", "1", "1200" }
#endif /* CONFIG_COREMARK_RUN_PROFILE */

int coremark(caddr_t percpu_area)
{
	char *argv[COREMARK_ARGC] = COREMARK_ARGV;

	__coremark(COREMARK_ARGC, argv);
	return static_total_errors ? 0 : 1;
}

__define_testfn(coremark,
		/* Globals are controlled not only by coremark main, so the
		 * following bench allocation cannot work:
		 *   sizeof(struct coremark_context),
		 */
		0,
		SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
#endif /* HOSTED */
