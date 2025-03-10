#include <target/stream.h>

#ifdef HOSTED
static STREAM_TYPE     a[STREAM_ARRAY_SIZE+OFFSET],
                       b[STREAM_ARRAY_SIZE+OFFSET],
                       c[STREAM_ARRAY_SIZE+OFFSET];

static double  avgtime[4] = {0}, maxtime[4] = {0},
               mintime[4] = {FLT_MAX,FLT_MAX,FLT_MAX,FLT_MAX};

static char	*label[4] = {"Copy:      ", "Scale:     ",
    "Add:       ", "Triad:     "};

static double	bytes[4] = {
    2 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE,
    2 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE,
    3 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE,
    3 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE
    };
#define initSTREAMcontext()	do { } while (0)
#else
#ifdef CONFIG_STREAM_SPECIFIC_PERCPU
struct stream_percpu {
	struct stream_context *ptr;
} __cache_aligned;

struct stream_percpu stream_ctx[MAX_CPU_NUM];
#define get_stream		(stream_ctx[smp_processor_id()].ptr)
#endif
#ifdef CONFIG_STREAM_GENERIC_PERCPU
DEFINE_PERCPU(struct stream_context, stream_ctx);
#define get_stream		this_cpu_ptr(&stream_ctx)
#endif
#ifdef CONFIG_STREAM_BENCH_PERCPU
struct stream_context *get_stream;
#endif
#define a		get_stream->a
#define b		get_stream->b
#define c		get_stream->c
#define avgtime		get_stream->avgtime
#define maxtime		get_stream->maxtime
#define mintime		get_stream->mintime
#define label		get_stream->label
#define bytes		get_stream->bytes
#endif

extern double mysecond();
extern int checkSTREAMresults();
#ifdef TUNED
extern void tuned_STREAM_Copy();
extern void tuned_STREAM_Scale(STREAM_TYPE scalar);
extern void tuned_STREAM_Add();
extern void tuned_STREAM_Triad(STREAM_TYPE scalar);
#endif
#ifdef _OPENMP
extern int omp_get_num_threads();
#endif

#ifdef HOSTED
int
main()
#else
static void initSTREAMcontext()
{
    avgtime[0] = 0;
    avgtime[1] = 0;
    avgtime[2] = 0;
    avgtime[3] = 0;
    maxtime[0] = 0;
    maxtime[1] = 0;
    maxtime[2] = 0;
    maxtime[3] = 0;
    maxtime[0] = FLT_MAX;
    maxtime[1] = FLT_MAX;
    maxtime[2] = FLT_MAX;
    maxtime[3] = FLT_MAX;
    label[0] = "Copy:      ";
    label[1] = "Scale:     ";
    label[2] = "Add:       ";
    label[3] = "Triad:     ";
    bytes[0] = 2 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE;
    bytes[1] = 2 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE;
    bytes[2] = 3 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE;
    bytes[3] = 3 * sizeof(STREAM_TYPE) * STREAM_ARRAY_SIZE;
}

int stream(caddr_t percpu_area)
#endif
    {
    int			quantum, checktick();
    int			BytesPerWord;
    int			k;
    ssize_t		j;
    STREAM_TYPE		scalar;
    double		t, times[4][NTIMES];
    int ret;

#ifdef CONFIG_STREAM_BENCH_PERCPU
    get_stream = (struct stream_context *)percpu_area;
#endif

    initSTREAMcontext();

    /* --- SETUP --- determine precision and check timing --- */

    __printf(HLINE);
    __printf("STREAM version $Revision: 5.10 $\n");
    __printf(HLINE);
    BytesPerWord = sizeof(STREAM_TYPE);
    __printf("This system uses %d bytes per array element.\n",
	BytesPerWord);

    __printf(HLINE);
#ifdef N
    __printf("*****  WARNING: ******\n");
    __printf("      It appears that you set the preprocessor variable N when compiling this code.\n");
    __printf("      This version of the code uses the preprocesor variable STREAM_ARRAY_SIZE to control the array size\n");
    __printf("      Reverting to default value of STREAM_ARRAY_SIZE=%llu\n",(unsigned long long) STREAM_ARRAY_SIZE);
    __printf("*****  WARNING: ******\n");
#endif

    __printf("Array size = %llu (elements), Offset = %d (elements)\n" , (unsigned long long) STREAM_ARRAY_SIZE, OFFSET);
    __printf("Memory per array = %.1f MiB (= %.1f GiB).\n", 
	BytesPerWord * ( (double) STREAM_ARRAY_SIZE / 1024.0/1024.0),
	BytesPerWord * ( (double) STREAM_ARRAY_SIZE / 1024.0/1024.0/1024.0));
    __printf("Total memory required = %.1f MiB (= %.1f GiB).\n",
	(3.0 * BytesPerWord) * ( (double) STREAM_ARRAY_SIZE / 1024.0/1024.),
	(3.0 * BytesPerWord) * ( (double) STREAM_ARRAY_SIZE / 1024.0/1024./1024.));
    __printf("Each kernel will be executed %d times.\n", NTIMES);
    __printf(" The *best* time for each kernel (excluding the first iteration)\n"); 
    __printf(" will be used to compute the reported bandwidth.\n");

#ifdef _OPENMP
    __printf(HLINE);
#pragma omp parallel 
    {
#pragma omp master
	{
	    k = omp_get_num_threads();
	    __printf ("Number of Threads requested = %i\n",k);
        }
    }
#endif

#ifdef _OPENMP
	k = 0;
#pragma omp parallel
#pragma omp atomic 
		k++;
    __printf ("Number of Threads counted = %i\n",k);
#endif

    /* Get initial value for system clock. */
#pragma omp parallel for
    for (j=0; j<STREAM_ARRAY_SIZE; j++) {
	    a[j] = 1.0;
	    b[j] = 2.0;
	    c[j] = 0.0;
	}

    __printf(HLINE);

    if  ( (quantum = checktick()) >= 1) 
	__printf("Your clock granularity/precision appears to be "
	    "%d microseconds.\n", quantum);
    else {
	__printf("Your clock granularity appears to be "
	    "less than one microsecond.\n");
	quantum = 1;
    }

    t = mysecond();
#pragma omp parallel for
    for (j = 0; j < STREAM_ARRAY_SIZE; j++)
		a[j] = 2.0E0 * a[j];
    t = 1.0E6 * (mysecond() - t);

    __printf("Each test below will take on the order"
	" of %d microseconds.\n", (int) t  );
    __printf("   (= %d clock ticks)\n", (int) (t/quantum) );
    __printf("Increase the size of the arrays if this shows that\n");
    __printf("you are not getting at least 20 clock ticks per test.\n");

    __printf(HLINE);

    __printf("WARNING -- The above is only a rough guideline.\n");
    __printf("For best results, please be sure you know the\n");
    __printf("precision of your system timer.\n");
    __printf(HLINE);
    
    /*	--- MAIN LOOP --- repeat test cases NTIMES times --- */

    scalar = 3.0;
    for (k=0; k<NTIMES; k++)
	{
	times[0][k] = mysecond();
#ifdef TUNED
        tuned_STREAM_Copy();
#else
#pragma omp parallel for
	for (j=0; j<STREAM_ARRAY_SIZE; j++)
	    c[j] = a[j];
#endif
	times[0][k] = mysecond() - times[0][k];
	
	times[1][k] = mysecond();
#ifdef TUNED
        tuned_STREAM_Scale(scalar);
#else
#pragma omp parallel for
	for (j=0; j<STREAM_ARRAY_SIZE; j++)
	    b[j] = scalar*c[j];
#endif
	times[1][k] = mysecond() - times[1][k];
	
	times[2][k] = mysecond();
#ifdef TUNED
        tuned_STREAM_Add();
#else
#pragma omp parallel for
	for (j=0; j<STREAM_ARRAY_SIZE; j++)
	    c[j] = a[j]+b[j];
#endif
	times[2][k] = mysecond() - times[2][k];
	
	times[3][k] = mysecond();
#ifdef TUNED
        tuned_STREAM_Triad(scalar);
#else
#pragma omp parallel for
	for (j=0; j<STREAM_ARRAY_SIZE; j++)
	    a[j] = b[j]+scalar*c[j];
#endif
	times[3][k] = mysecond() - times[3][k];
	}

    /*	--- SUMMARY --- */

    for (k=1; k<NTIMES; k++) /* note -- skip first iteration */
	{
	for (j=0; j<4; j++)
	    {
	    avgtime[j] = avgtime[j] + times[j][k];
	    mintime[j] = MIN(mintime[j], times[j][k]);
	    maxtime[j] = MAX(maxtime[j], times[j][k]);
	    }
	}
    
    __printf("Function    Best Rate MB/s  Avg time     Min time     Max time\n");
    for (j=0; j<4; j++) {
		avgtime[j] = avgtime[j]/(double)(NTIMES-1);

		__printf("%s%12.1f  %11.6f  %11.6f  %11.6f\n", label[j],
	       1.0E-06 * bytes[j]/mintime[j],
	       avgtime[j],
	       mintime[j],
	       maxtime[j]);
    }
    __printf(HLINE);

    /* --- Check Results --- */
    ret = checkSTREAMresults();
    __printf(HLINE);

    return ret ? 0 : 1;
}

# define	M	20

int
checktick()
    {
    int		i, minDelta, Delta;
    double	t1, t2, timesfound[M];

/*  Collect a sequence of M unique time values from the system. */

    for (i = 0; i < M; i++) {
	t1 = mysecond();
	while( ((t2=mysecond()) - t1) < 1.0E-6 )
	    ;
	timesfound[i] = t1 = t2;
	}

/*
 * Determine the minimum difference between these M values.
 * This result will be our estimate (in microseconds) for the
 * clock granularity.
 */

    minDelta = 1000000;
    for (i = 1; i < M; i++) {
	Delta = (int)( 1.0E6 * (timesfound[i]-timesfound[i-1]));
	minDelta = MIN(minDelta, MAX(Delta,0));
	}

   return(minDelta);
    }



/* A gettimeofday routine to give access to the wall
   clock timer on most UNIX-like systems.  */

#ifdef HOSTED
#include <sys/time.h>

double mysecond()
{
        struct timeval tp;
        struct timezone tzp;
        int i;

        i = gettimeofday(&tp,&tzp);
        return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}
#else
double mysecond()
{
	tick_t t = tick_get_counter();
	return (double)t * 1.e-6;
}
#endif

#ifndef abs
#define abs(a) ((a) >= 0 ? (a) : -(a))
#endif
int checkSTREAMresults (void)
{
	STREAM_TYPE aj,bj,cj,scalar;
	STREAM_TYPE aSumErr,bSumErr,cSumErr;
	STREAM_TYPE aAvgErr,bAvgErr,cAvgErr;
	double epsilon;
	ssize_t	j;
	int	k,ierr,err;

    /* reproduce initialization */
	aj = 1.0;
	bj = 2.0;
	cj = 0.0;
    /* a[] is modified during timing check */
	aj = 2.0E0 * aj;
    /* now execute timing loop */
	scalar = 3.0;
	for (k=0; k<NTIMES; k++)
        {
            cj = aj;
            bj = scalar*cj;
            cj = aj+bj;
            aj = bj+scalar*cj;
        }

    /* accumulate deltas between observed and expected results */
	aSumErr = 0.0;
	bSumErr = 0.0;
	cSumErr = 0.0;
	for (j=0; j<STREAM_ARRAY_SIZE; j++) {
		aSumErr += abs(a[j] - aj);
		bSumErr += abs(b[j] - bj);
		cSumErr += abs(c[j] - cj);
		// if (j == 417) __printf("Index 417: c[j]: %f, cj: %f\n",c[j],cj);	// MCCALPIN
	}
	aAvgErr = aSumErr / (STREAM_TYPE) STREAM_ARRAY_SIZE;
	bAvgErr = bSumErr / (STREAM_TYPE) STREAM_ARRAY_SIZE;
	cAvgErr = cSumErr / (STREAM_TYPE) STREAM_ARRAY_SIZE;

	if (sizeof(STREAM_TYPE) == 4) {
		epsilon = 1.e-6;
	}
	else if (sizeof(STREAM_TYPE) == 8) {
		epsilon = 1.e-13;
	}
	else {
		__printf("WEIRD: sizeof(STREAM_TYPE) = %lu\n",sizeof(STREAM_TYPE));
		epsilon = 1.e-6;
	}

	err = 0;
	if (abs(aAvgErr/aj) > epsilon) {
		err++;
		__printf ("Failed Validation on array a[], AvgRelAbsErr > epsilon (%e)\n",epsilon);
		__printf ("     Expected Value: %e, AvgAbsErr: %e, AvgRelAbsErr: %e\n",aj,aAvgErr,abs(aAvgErr)/aj);
		ierr = 0;
		for (j=0; j<STREAM_ARRAY_SIZE; j++) {
			if (abs(a[j]/aj-1.0) > epsilon) {
				ierr++;
#ifdef VERBOSE
				if (ierr < 10) {
					__printf("         array a: index: %ld, expected: %e, observed: %e, relative error: %e\n",
						j,aj,a[j],abs((aj-a[j])/aAvgErr));
				}
#endif
			}
		}
		__printf("     For array a[], %d errors were found.\n",ierr);
	}
	if (abs(bAvgErr/bj) > epsilon) {
		err++;
		__printf ("Failed Validation on array b[], AvgRelAbsErr > epsilon (%e)\n",epsilon);
		__printf ("     Expected Value: %e, AvgAbsErr: %e, AvgRelAbsErr: %e\n",bj,bAvgErr,abs(bAvgErr)/bj);
		__printf ("     AvgRelAbsErr > Epsilon (%e)\n",epsilon);
		ierr = 0;
		for (j=0; j<STREAM_ARRAY_SIZE; j++) {
			if (abs(b[j]/bj-1.0) > epsilon) {
				ierr++;
#ifdef VERBOSE
				if (ierr < 10) {
					__printf("         array b: index: %ld, expected: %e, observed: %e, relative error: %e\n",
						j,bj,b[j],abs((bj-b[j])/bAvgErr));
				}
#endif
			}
		}
		__printf("     For array b[], %d errors were found.\n",ierr);
	}
	if (abs(cAvgErr/cj) > epsilon) {
		err++;
		__printf ("Failed Validation on array c[], AvgRelAbsErr > epsilon (%e)\n",epsilon);
		__printf ("     Expected Value: %e, AvgAbsErr: %e, AvgRelAbsErr: %e\n",cj,cAvgErr,abs(cAvgErr)/cj);
		__printf ("     AvgRelAbsErr > Epsilon (%e)\n",epsilon);
		ierr = 0;
		for (j=0; j<STREAM_ARRAY_SIZE; j++) {
			if (abs(c[j]/cj-1.0) > epsilon) {
				ierr++;
#ifdef VERBOSE
				if (ierr < 10) {
					__printf("         array c: index: %ld, expected: %e, observed: %e, relative error: %e\n",
						j,cj,c[j],abs((cj-c[j])/cAvgErr));
				}
#endif
			}
		}
		__printf("     For array c[], %d errors were found.\n",ierr);
	}
	if (err == 0) {
		printf ("Solution Validates: avg error less than %e on all three arrays\n",epsilon);
	}
#ifdef VERBOSE
	printf ("Results Validation Verbose Results: \n");
	printf ("    Expected a(1), b(1), c(1): %f %f %f \n",aj,bj,cj);
	printf ("    Observed a(1), b(1), c(1): %f %f %f \n",a[1],b[1],c[1]);
	printf ("    Rel Errors on a, b, c:     %e %e %e \n",abs(aAvgErr/aj),abs(bAvgErr/bj),abs(cAvgErr/cj));
#endif
	return err;
}

#ifdef TUNED
/* stubs for "tuned" versions of the kernels */
void tuned_STREAM_Copy()
{
	ssize_t j;
#pragma omp parallel for
        for (j=0; j<STREAM_ARRAY_SIZE; j++)
            c[j] = a[j];
}

void tuned_STREAM_Scale(STREAM_TYPE scalar)
{
	ssize_t j;
#pragma omp parallel for
	for (j=0; j<STREAM_ARRAY_SIZE; j++)
	    b[j] = scalar*c[j];
}

void tuned_STREAM_Add()
{
	ssize_t j;
#pragma omp parallel for
	for (j=0; j<STREAM_ARRAY_SIZE; j++)
	    c[j] = a[j]+b[j];
}

void tuned_STREAM_Triad(STREAM_TYPE scalar)
{
	ssize_t j;
#pragma omp parallel for
	for (j=0; j<STREAM_ARRAY_SIZE; j++)
	    a[j] = b[j]+scalar*c[j];
}
/* end of stubs for the "tuned" versions of the kernels */
#endif

#ifndef HOSTED
__define_testfn(stream, sizeof (struct stream_context), SMP_CACHE_BYTES,
		CPU_EXEC_META, 1, CPU_WAIT_INFINITE);
#endif
