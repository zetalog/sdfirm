/*
 * a timing utilities library
 *
 * Requires 64bit integers to work.
 *
 * %W% %@%
 *
 * Copyright (c) 2000 Carl Staelin.
 * Copyright (c) 1994-1998 Larry McVoy.
 * Distributed under the FSF GPL with
 * additional restriction that results may published only if
 * (1) the benchmark is unmodified, and
 * (2) the version in the sccsid below is included in the report.
 * Support for this development by Sun Microsystems is gratefully acknowledged.
 */
#define  _LIB /* bench.h needs this */
#include "bench.h"
#include "time.h"

static  int     long_enough;
static  int     compute_enough();
static volatile uint64  use_result_dummy;
static          uint64  iterations;
static          void    init_timing(void);
uint64   gettime(void);
static uint64   start_tv, stop_tv; 

static uint32_t _clock()
{
    struct timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return tp.tv_sec * 1000000ull + tp.tv_nsec / 1000ull;
}
/* biggest to smallest */
void
insertsort(uint64 u, uint64 n, result_t *r)
{
        int     i, j;

        if (u == 0) return;

#ifdef _DEBUG
        fprintf(stderr, "\tinsertsort(%llu, %llu, %p)\n", u, n, r);
#endif /* _DEBUG */
        for (i = 0; i < r->N; ++i) {
                if (u/(double)n > r->v[i].u/(double)r->v[i].n) {
                        for (j = r->N; j > i; --j) {
                                r->v[j] = r->v[j - 1];
                        }
                        break;
                }
        }
        r->v[i].u = u;
        r->v[i].n = n;
        r->N++;
}

static result_t  _results;
static result_t* results = &_results;

result_t*
get_results()
{
        return (results);
}

void
set_results(result_t *r)
{
        results = r;
        save_median();
}

void
save_minimum()
{
        if (results->N == 0) {
                save_n(1);
                settime(0);
        } else {
                save_n(results->v[results->N - 1].n);
                settime(results->v[results->N - 1].u);
        }
}

void
save_median()
{
        int     i = results->N / 2;
        uint64  u, n;

        if (results->N == 0) {
                n = 1;
                u = 0;
        } else if (results->N % 2) {
                n = results->v[i].n;
                u = results->v[i].u;
        } else {
                n = (results->v[i].n + results->v[i-1].n) / 2;
                u = (results->v[i].u + results->v[i-1].u) / 2;
        }
#ifdef _DEBUG
        fprintf(stderr, "save_median: N=%d, n=%lu, u=%lu\n", results->N, (unsigned long)n, (unsigned long)u);
#endif /* _DEBUG */
save_n(n); settime(u);
}


/*
 * The inner loop tracks bench.h but uses a different results array.
 */
static long *
one_op(register long *p)
{
        BENCH_INNER(p = (long *)*p;, 0);
        return (p);
}

static long *
two_op(register long *p)
{
        BENCH_INNER(p = (long *)*p; p = (long*)*p;, 0);
        return (p);
}


static long     *p = (long *)&p;
static long     *q = (long *)&q;

double
l_overhead(void)
{
        int     i;
        uint64  N_save, u_save;
        static  double overhead;
        static  int initialized = 0;
        result_t one, two, *r_save;

        init_timing();
        if (initialized) return (overhead);

        initialized = 1;
        r_save = get_results(); N_save = get_n(); u_save = gettime();
        insertinit(&one);
        insertinit(&two);
        for (i = 0; i < TRIES; ++i) {
                use_pointer((void*)one_op(p));
                if (gettime() > t_overhead())
                        insertsort(gettime() - t_overhead(), get_n(), &one);
                use_pointer((void *)two_op(p));
                if (gettime() > t_overhead())
                        insertsort(gettime() - t_overhead(), get_n(), &two);
        }
        /*
         * u1 = (n1 * (overhead + work))
         * u2 = (n2 * (overhead + 2 * work))
         * ==> overhead = 2. * u1 / n1 - u2 / n2
         */
        set_results(&one);
        save_minimum();
        overhead = 2. * gettime() / (double)get_n();

        set_results(&two);
        save_minimum();
        overhead -= gettime() / (double)get_n();

        if (overhead < 0.) overhead = 0.;       /* Gag */

        set_results(r_save); save_n(N_save); settime(u_save);
        return (overhead);
}

/*
 * Figure out the timing overhead.  This has to track bench.h
 */
uint64
t_overhead(void)
{
        uint64          N_save, u_save;
        static int      initialized = 0;
        static uint64   overhead = 0;
        result_t        *r_save;

        init_timing();
        if (initialized) return (overhead);

        initialized = 1;
        if (get_enough(0) <= 50000) {
                /* it is not in the noise, so compute it */
                int             i;
                result_t        r;

                r_save = get_results(); N_save = get_n(); u_save = gettime();
                insertinit(&r);
                for (i = 0; i < TRIES; ++i) {
                        BENCH_INNER(_clock(), 0);
                        insertsort(gettime(), get_n(), &r);
                }
                set_results(&r);
                save_minimum();
                overhead = gettime() / get_n();

                set_results(r_save); save_n(N_save); settime(u_save);
        }
        return (overhead);
}

int
get_enough(int e)
{
        init_timing();
        return (long_enough > e ? long_enough : e);
}


static void
init_timing(void)
{
        static  int done = 0;

        if (done) return;
        done = 1;
        long_enough = compute_enough();
        t_overhead();
        l_overhead();
}

void
start()
{
        start_tv  = _clock();
}

/*
 * Stop timing and return real time in microseconds.
 */
uint64
stop()
{      
        stop_tv = _clock();
        
        return ( stop_tv - start_tv);
}

uint64
now(void)
{
        
        return (_clock());
}

double
Now(void)
{

        return (_clock() * 1.0);
}

uint64
delta(void)
{
        static uint64 last = 0;
        uint64 t;
        uint64 diff;

	t = _clock();
        if (last) {
		diff = t - last;
                last = t;
                return (diff);
        } else {
                last = t;
                return (0);
        }
}

double
Delta(void)
{
        uint64 t;
        uint64 diff;
	t = _clock();
        diff = t - start_tv;
        return (diff * 1.0);
}


void
save_n(uint64 n)
{
        iterations = n;
}

uint64
get_n(void)
{
        return (iterations);
}

/*
 * Make the time spend be usecs.
 */
void
settime(uint64 usecs)
{
	start_tv = 0;
        stop_tv = usecs;
}


uint64
tvdelta(uint64 start, uint64 stop)
{
	uint64 td; 
	td = stop - start;
        return (td);
}

void
tvsub(uint64 * tdiff, uint64  t1, uint64  t0)
{
	if (t1 < t0) 
		*tdiff = 0;
	else
		*tdiff = t1 - t0;
}

uint64
gettime(void)
{
       // return (tvdelta(start_tv, stop_tv));
	return stop_tv - start_tv;
}

typedef long TYPE;
//static TYPE **
 TYPE **
enough_duration(register long N, register TYPE ** p)
{
#define ENOUGH_DURATION_TEN(one)        one one one one one one one one one one
        while (N-- > 0) {
                ENOUGH_DURATION_TEN(p = (TYPE **) *p;);
        }
        return (p);
}

//static uint64
uint64
duration(long N)
{
        uint64  usecs;
        TYPE   *x = (TYPE *)&x;
        TYPE  **p = (TYPE **)&x;

        start(0);
        p = enough_duration(N, p);
        usecs = stop(0, 0);
        use_pointer((void *)p); 
        return (usecs);
}

/*
 * find the minimum time that work "N" takes in "tries" tests
 */
//static uint64
uint64
time_N(iter_t N)
{
        int     i;
        uint64  usecs;
        result_t r, *r_save;

        r_save = get_results();
        insertinit(&r);
        for (i = 1; i < TRIES; ++i) {
                usecs = duration(N);
                insertsort(usecs, N, &r);
        }
        set_results(&r);
        save_minimum();
        usecs = gettime();
        set_results(r_save);
        return (usecs);
}


/*
 * return the amount of work needed to run "enough" microseconds
 */
static iter_t
find_N(int enough)
{
        int             tries;
        static iter_t   N = 10000;
        static uint64   usecs = 0;

        if (!usecs) usecs = time_N(N);

        for (tries = 0; tries < 10; ++tries) {
                if (0.98 * enough < usecs && usecs < 1.02 * enough)
                        return (N);
                if (usecs < 1000)
                        N *= 10;
                else {
                        double  n = N;

                        n /= usecs;
                        n *= enough; 
                        N = n + 1;
                }
                usecs = time_N(N);
        }
        return (0);
}

/*
 * We want to verify that small modifications proportionally affect the runtime
 */
static double test_points[] = {1.015, 1.02, 1.035};
//static int
int
test_time(int enough)
{
        int     i;
        iter_t  N;
        uint64  usecs, expected, baseline, diff;

        if ((N = find_N(enough)) == 0)
                return (0);

        baseline = time_N(N); 
        
        for (i = 0; i < sizeof(test_points) / sizeof(double); ++i) {
                usecs = time_N((int)((double) N * test_points[i]));
                expected = (uint64)((double)baseline * test_points[i]);
                diff = expected > usecs ? expected - usecs : usecs - expected;
                if (diff / (double)expected > 0.0025)
                        return (0);
        }
        return (1);
} 

/*
 * We want to find the smallest timing interval that has accurate timing
 */
static int     possibilities[] = { 5000, 10000, 50000, 100000 };
static int
compute_enough()
{
        int     i;

        for (i = 0; i < sizeof(possibilities) / sizeof(int); ++i) {
                if (test_time(possibilities[i]))
                        return (possibilities[i]);
        }

        /*
         * if we can't find a timing interval that is sufficient,
         * then use SHORT as a default.
         */
        return (SHORT);
}
void
use_int(int result) { use_result_dummy += result; }

void
use_pointer(void *result) { use_result_dummy += (long)result; }

int
sizeof_result(int repetitions)
{
        if (repetitions <= TRIES)
                return (sizeof(result_t));
        return (sizeof(result_t) + (repetitions - TRIES) * sizeof(value_t));
}

void
insertinit(result_t *r)
{
        r->N = 0;
}

