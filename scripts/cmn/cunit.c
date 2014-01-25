#include <host/cunit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

#define CUNIT_DEFAULT_FAILURE		128
#define CUNIT_DEFAULT_ERROR		128
#define CUNIT_DEFAULT_LISTENER		1
#define CUNIT_DEFAULT_TESTS		200
#define CUNIT_VECTOR_INCREMENT		64

#define CUNIT_FATAL_ERROR		127
#define CUNIT_UNCAUGHT_EXCEPTION	128

#define CUNIT_MAXIMUM_ROOTS		50

/*
 * This is a non-nesting use of setjmp/longjmp used solely for the purpose
 * of exiting the test runner in a clean way. By separating it out from
 * the exception mechanism above, we don't need to worry about whether or
 * not we are throwing an exception or exiting the runner in an
 * CUNIT_CATCH block.
 */
#define CUNIT_START						\
	{							\
		jmp_buf __env__;				\
		cunit_set_runner_scope(&(__env__));		\
		if (setjmp(__env__) == 0) {

#define CUNIT_EXIT(__code__)					\
		cunit_set_exit_code(__code__);			\
		longjmp(*((jmp_buf*)cunit_get_runner_scope()),1)

#define CUNIT_FINISH(__code__)					\
		} else {					\
			int __code__ = cunit_get_exit_code();

#define CUNIT_END_FINISH					\
		}						\
	}

typedef unsigned long cell;   /* Must be 32 bits long! */
typedef void *cunit_object_t;
typedef struct _cunit_vector_t *Vector;
typedef struct _cunit_listener_t *TestListener;
typedef struct _cunit_printer_t *ResultPrinter;

typedef TestResult (CUNITCBK *TestMain)(TestRunner, Test);

/* error or failure collector */
typedef void (CUNITCBK *ResultCollector)(TestListener, Test, Throwable);
typedef void (CUNITCBK *TestStart)(TestListener, Test);
typedef void (CUNITCBK *TestEnd)(TestListener, Test);

typedef int (*CaseCounter)(IN Test);
typedef void (*CaseRunner)(IN Test, IN TestResult);
typedef void (*CaseDestroyer)(IN Test);

/*=========================================================================
 * Data structure definitions
 *=======================================================================*/
typedef struct _cunit_vector_t {
	cunit_object_t *objects;
	int size;
	int coount;
} cunit_vector_t;

typedef union _cunit_cell_t {
	cell cell;
	cell *pointer;
} cunit_cell_t;

typedef struct _cunit_throwable_t {
	int type;
	char *message;
	int line;
	char *file;
} cunit_throwable_t;

typedef struct _cunit_runner_t {
	TestMain main;
	//TestResult result;
} cunit_runner_t;

typedef struct _cunit_listener_t {
	/* error occurred. */
	ResultCollector add_error;
	/* failure occurred. */
	ResultCollector add_failure;  
	/* test started flag */
	TestStart start_test;
	/* test ended flag */
	TestEnd end_test; 
} cunit_listener_t;

typedef struct _cunit_result_t {
	/* failure collector */
	Throwable *failures;
	int failure_count;
	int failure_size;
	
	/* error collector */
	Throwable *errors;
	int error_count;
	int error_size;
	
	/* registered listeners */
	TestListener *listeners;
	int listener_count;
	int listener_size;
	
	/* run tests */
	int runs;
	/* succeeded test count */
	int success_count;
	/* run seconds */
	double run_seconds;
	/* milli-seconds precision */
	unsigned short run_millitm;
	
	/* runner stopped */
	int stop;
} cunit_result_t;

typedef struct _cunit_test_t {
	/* test name */
	char *name;
	/* test counter */
	CaseCounter counter;
	/* test runner */
	CaseRunner runner;
	/* test destroyer */
	CaseDestroyer destroyer;
} cunit_test_t;

typedef struct _cunit_suite_t {
	/* invisible super class */
	cunit_test_t;
	
	/* test vector */
	Test *tests;
	int count;
	int size;
} cunit_suite_t;

typedef struct _cunit_case_t {
	/* invisible super class */
	cunit_test_t;
	
	/* case caller */
	TestCall call;
	/* bare test setUp */
	TestCall set_up;
	/* bare test tearDown */
	TestCall tear_down;
} cunit_case_t;

static Throwable cunit_new_throwable(CUNIT_SOURCE_LINE_DECL,
				     IN int type,
				     IN const char *message);

static char *cunit_format_message(IN char *message, IN int size,
				  IN const char *format, ...);
static char *cunit_format_messagev(IN char *message, IN int size,
				   IN const char *format,
				   IN va_list pvar);
static void cunit_output_message(IN const char *format, ...);
static void cunit_output_messagev(IN const char *oformat,
				  IN va_list pvar);
static void cunit_set_runner_scope(IN void *env);

static void *cunit_get_runner_scope(void);
static void cunit_set_exit_code(IN int code);
static int cunit_get_exit_code(void);

static TestResult cunit_new_result(void);
static int cunit_should_stop(TestResult result);
static void cunit_add_failure(IN TestResult result,
			      IN Test test,
			      IN Throwable failure);
static void cunit_add_error(IN TestResult result,
			    IN Test test,
			    IN Throwable error);
static void cunit_delete_result(IN TestResult result);
static void cunit_start_test(IN TestResult result,
			     IN Test test);
static void cunit_end_test(IN TestResult result,
			   IN Test test);
static void cunit_register_listener(IN TestResult result,
				    IN TestListener listener);

static void cunit_run_suite(TestSuite suite, TestResult result);
static void cunit_delete_suite(IN TestSuite suite);
static int cunit_count_suite(TestSuite suite);

static int cunit_count_cases(IN TestCase caze);
static void cunit_delete_case(IN TestCase caze);
static void cunit_run_case(IN TestCase caze, IN TestResult result);

static TestRunner _default_runner;

/*=========================================================================
 * Local variables
 *=======================================================================*/
/* CUNIT_TRY/CUNIT_CATCH throwable scope */
static cunit_scope_t _throwable_struct = {
	/* env =           */ NULL,
	/* throwable =     */ NULL,
	/* roots_count   = */ 0,
	/* outer =         */ NULL
};
static ThrowableScope _throwable_scope = &_throwable_struct;

int _roots_length;
cunit_cell_t _temporary_roots[CUNIT_MAXIMUM_ROOTS];
void* _runner_scope = NULL;
int _exit_code = 0;

CUNITEXT int cunit_should_stop(TestResult result)
{
	return result->stop;
}

/*
 * result contructor.
 */
CUNITEXT TestResult cunit_new_result(void)
{
	TestResult result = malloc(sizeof (struct _cunit_result_t));
	
	if (result) {
		memset(result, 0, sizeof (struct _cunit_result_t));
		
		result->success_count = 0;
		result->runs = 0;
		
		/* initialize Failure vector */
		result->failure_count = 0;
		result->failures = (Throwable *)malloc(sizeof (Throwable)*CUNIT_DEFAULT_FAILURE);
		if (result->failures)
			result->failure_size = CUNIT_DEFAULT_FAILURE;
		
		/* initialize Error vector */
		result->error_count = 0;
		result->errors = (Throwable *)malloc(sizeof (Throwable)*CUNIT_DEFAULT_ERROR);
		if (result->errors)
			result->error_size = CUNIT_DEFAULT_ERROR;
		
		/* initialize TestListener vector */
		result->listener_count = 0;
		result->listeners = (TestListener *)malloc(sizeof (TestListener)*CUNIT_DEFAULT_LISTENER);
		if (result->listeners)
			result->listener_size = CUNIT_DEFAULT_LISTENER;
		
		result->stop = FALSE;
	} else {
		cunit_fatal_error("Memory allocation failure!");
	}
	return result;
}

CUNITEXT void cunit_register_listener(IN TestResult result,
				      IN TestListener listener)
{
	if (result) {
		if (result->listener_count < result->listener_size) {
			result->listeners[result->listener_count] = listener;
			result->listener_count++;
		} else {
			TestListener *listeners = (TestListener *)malloc(sizeof (TestListener)*
									 (result->listener_size+CUNIT_VECTOR_INCREMENT));
			if (!listeners) {
				cunit_fatal_error("Memory allocation failed!");
			} else {
				if (result->listeners) {
					memcpy(listeners, result->listeners,
					       sizeof (TestListener)*result->listener_size);
				}
				result->listener_size += CUNIT_VECTOR_INCREMENT;
				if (result->listeners)
					free(result->listeners);
				result->listeners = listeners;
				cunit_register_listener(result, listener);
			}
		}
	}
}

CUNITEXT void cunit_add_failure(IN TestResult result,
				IN Test test, IN Throwable failure)
{
	if (result) {
		if (result->failure_count < result->failure_size) {
			int index;
			
			result->failures[result->failure_count] = failure;
			result->failure_count++;
			
			for (index = 0; index < result->listener_count; index++) {
				TestListener listener = result->listeners[index];
				if (listener && listener->add_failure)
					listener->add_failure(listener, test, failure);
			}
		} else {
			Throwable *failures = (Throwable *)malloc(sizeof (Throwable)*(result->failure_size+CUNIT_VECTOR_INCREMENT));
			if (!failures) {
				cunit_fatal_error("Memory allocation failed!");
			} else {
				if (result->failures) {
					memcpy(failures, result->failures,
					       sizeof (Throwable)*result->failure_size);
				}
				result->failure_size += CUNIT_VECTOR_INCREMENT;
				if (result->failures)
					free(result->failures);
				result->failures = failures;
				cunit_add_failure(result, test, failure);
			}
		}
	}
}

CUNITEXT void cunit_add_error(IN TestResult result,
			      IN Test test, IN Throwable error)
{
	if (result) {
		if (result->error_count < result->error_size) {
			int index;
			
			result->errors[result->error_count] = error;
			result->error_count++;
			
			for (index = 0; index < result->listener_count; index++) {
				TestListener listener = result->listeners[index];
				if (listener && listener->add_error)
					listener->add_error(listener, test, error);
			}
		} else {
			Throwable *errors = (Throwable *)malloc(sizeof (Throwable)*(result->error_size+CUNIT_VECTOR_INCREMENT));
			if (!errors) {
				cunit_fatal_error("Memory allocation failed!");
			} else {
				if (result->errors) {
					memcpy(errors, result->errors,
					       sizeof (Throwable)*result->error_size);
				}
				result->error_size += CUNIT_VECTOR_INCREMENT;
				if (result->errors)
					free(result->errors);
				result->errors = errors;
				cunit_add_error(result, test, error);
			}
		}
	}
}

CUNITEXT void cunit_start_test(IN TestResult result, IN Test test)
{
	int count = 0;
	int index;
	
	if (test)
		count = test->counter(test);
	result->runs += count;
	
	for (index = 0; index < result->listener_count; index++) {
		TestListener listener = result->listeners[index];
		if (listener && listener->start_test)
			listener->start_test(listener, test);
	}
}

CUNITEXT void cunit_end_test(IN TestResult result, IN Test test)
{
	int index;
	
	for (index = 0; index < result->listener_count; index++) {
		TestListener listener = result->listeners[index];
		if (listener && listener->end_test)
			listener->end_test(listener, test);
	}
}

/*
 * result destructor.
 */
CUNITEXT void cunit_delete_result(IN TestResult result)
{
	int index;
	
	if (result) {
		if (result->errors) {
			for (index = 0; index < result->error_count; index++)
				cunit_delete_throwable(result->errors[index]);
			free(result->errors);
		}
		if (result->failures) {
			for (index = 0; index < result->failure_count; index++)
				cunit_delete_throwable(result->failures[index]);
			free(result->failures);
		}
		if (result->listeners) {
			/*
			 * Do not destroy listener here.
			 * Listeners should be maintained by runner
			 * implementation as it may be permanent.
			 */
			free(result->listeners);
		}
		free(result);
	}
}

CUNITEXP ThrowableScope CUNITAPI cunit_get_scope(void)
{
	return _throwable_scope;
}

CUNITEXP void CUNITAPI cunit_set_scope(ThrowableScope scope)
{
	_throwable_scope = scope;
}

CUNITEXP int CUNITAPI cunit_get_roots_length(void)
{
	return _roots_length;
}

CUNITEXP void CUNITAPI cunit_set_roots_length(int length)
{
	_roots_length = length;
}

CUNITEXP void CUNITAPI cunit_set_roots_cell(int index, int cell)
{
	_temporary_roots[index].cell = cell;
}

CUNITEXP void CUNITAPI cunit_set_roots_pointer(int index, void *pointer)
{
	_temporary_roots[index].pointer = (cell *)pointer;
}

/*
 * throwable constructor.
 */
CUNITEXT Throwable cunit_new_throwable(CUNIT_SOURCE_LINE_DECL,
                                       IN int type, IN const char *message)
{
	Throwable exception = malloc(sizeof (cunit_throwable_t));
	if (exception) {
		memset(exception, 0, sizeof (cunit_throwable_t));
		exception->type = type;
		if (message)
			exception->message = strdup(message);
		if (file)
			exception->file = strdup(file);
		exception->line = line;
	}
	return exception;
}

/*
 * throwable destructor.
 */
CUNITEXP void CUNITAPI cunit_delete_throwable(IN Throwable exception)
{
	if (exception) {
		/* destroy throwable message */
		if (exception->message) {
			free(exception->message);
			exception->message = NULL;
		}
		/* destroy throwable file name */
		if (exception->file) {
			free(exception->file);
			exception->file = NULL;
		}
		free(exception);
	}
}

CUNITEXT void cunit_set_runner_scope(IN void *env)
{
	_runner_scope = env;
}

CUNITEXT void *cunit_get_runner_scope(void)
{
	return _runner_scope;
}

CUNITEXT void cunit_set_exit_code(IN int code)
{
	_exit_code = code;
}

CUNITEXT int cunit_get_exit_code(void)
{
	return _exit_code;
}

/*=========================================================================
 * FUNCTION:      cunit_fatal_error()
 * TYPE:          internal error handling operation
 * OVERVIEW:      Report a fatal error indicating that the execution
 *                of erroneous code might have endangered the
 *                integrity of the CUNIT. CUNIT will be stopped. This
 *                operation should be called only the from inside the
 *                CUNIT runner scope.
 * INTERFACE:
 *   parameters:  error message string.
 *   returns:     <nothing>
 *=======================================================================*/
CUNITEXP void CUNITAPI cunit_fatal_error(IN const char* message)
{
	cunit_output_message(message);
	CUNIT_EXIT(CUNIT_FATAL_ERROR);
}

/*=========================================================================
 * FUNCTION:      cunit_run_test()
 * TYPE:          test running kick off operation
 * OVERVIEW:      Run specified unit tests, collects and displays results.
 *                This function will use default TestRunner known as
 *                CuiRunner.
 * INTERFACE:
 *   parameters:  unit tests.
 *   returns:     <nothing>
 *=======================================================================*/
CUNITEXP TestResult CUNITAPI cunit_run_test(IN TestRunner runner,
					    IN Test test)
{
	TestResult result = NULL;
	
	if (!runner)
		runner = cunit_get_default();
	CUNIT_START {
		if (!runner)
			cunit_fatal_error("Can not run using null runner object!");
		result = runner->main(runner, test);
	} CUNIT_FINISH(code) {
		cunit_output_message("\nFatal error exit with code (%d)!\n", code);
	} CUNIT_END_FINISH

	return result;
}

CUNITEXP TestRunner CUNITAPI cunit_get_default(void)
{
	return _default_runner;
}

CUNITEXP void CUNITAPI cunit_set_default(IN TestRunner runner)
{
	_default_runner = runner;
}

/*
CUNITEXP void CUNITAPI cunit_stop_test(TestRunner runner)
{
	runner->result->stop = TRUE;
}
*/

CUNITEXP int CUNITAPI cunit_count_tests(Test test)
{
	if (!test)
		return 0;
	return test->counter(test);
}

CUNITEXP char * CUNITAPI cunit_get_name(IN Test test)
{
	if (test)
		return test->name;
	else
		return NULL;
}

/*
 * case constructor.
 */
CUNITEXP TestCase CUNITAPI cunit_new_case(IN const char *name,
					  IN TestCall call)
{
	return cunit_new_case_ex(name, call, NULL, NULL);
}

CUNITEXP TestCase CUNITAPI cunit_new_case_ex(IN const char *name,
					     IN TestCall call,
					     TestCall set_up,
					     TestCall tear_down)
{
	TestCase caze = (TestCase)malloc(sizeof (cunit_case_t));
	if (caze) {
		memset((void *)caze, 0, sizeof (cunit_case_t));
		caze->runner = (CaseRunner)cunit_run_case;
		caze->counter = (CaseCounter)cunit_count_cases;
		caze->destroyer = (CaseDestroyer)cunit_delete_case;
		if (name)
			caze->name = strdup(name);
		caze->call = call;
		caze->set_up = set_up;
		caze->tear_down = tear_down;
	}
	return caze;
}

/*
 * case counter.
 */
CUNITEXT int cunit_count_cases(IN TestCase caze)
{
	if (caze && caze->runner) {
		/* always return 1 */
		return 1;
	} else {
		/* as counter may be called seperately, do not raise
		 * fatal error
		 */
		return 0;
	}
}

/*
 * case runner.
 */
CUNITEXT void cunit_run_case(IN TestCase caze, IN TestResult result)
{
	int caught = FALSE;
	
	/* mark start of test case */
	cunit_start_test(result, (Test)caze);
	if (caze->set_up)
		(caze->set_up)();
	/* run case in the TRY CATCH scope */
	CUNIT_TRY {
		if (caze && caze->call) {
			/* callback case */
			(caze->call)();
		} else {
			cunit_fatal_error("Can not run null case object!");
		}
	} CUNIT_CATCH(e) {
		if (e->type == CUNIT_ASSERTION) {
			cunit_add_failure(result, (Test)caze, e);
		} else {
			cunit_add_error(result, (Test)caze, e);
		}
		caught = TRUE;
	} CUNIT_END_CATCH
	if (caze->tear_down)
		(caze->tear_down)();
	if (!caught) {
		result->success_count++;
	}
	/* mark end of test case */
	cunit_end_test(result, (Test)caze);
}

/*
 * case destructor.
 */
CUNITEXT void cunit_delete_case(IN TestCase caze)
{
	if (caze) {
		if (((Test)caze)->name)
			free(((Test)caze)->name);
		free(caze);
	}
}

/*
 * suite constructor.
 */
CUNITEXP TestSuite CUNITAPI cunit_new_suite(IN const char *name)
{
	TestSuite suite = malloc(sizeof (cunit_suite_t));
	if (suite) {
		/* initialize test suite */
		memset(suite, 0, sizeof (cunit_suite_t));
		suite->runner = (CaseRunner)cunit_run_suite;
		suite->counter = (CaseCounter)cunit_count_suite;
		suite->destroyer = (CaseDestroyer)cunit_delete_suite;
		/* create default test vector */
		suite->count = 0;
		suite->tests = (Test *)malloc(sizeof (Test) *CUNIT_DEFAULT_TESTS);
		if (suite->tests)
			suite->size = CUNIT_DEFAULT_TESTS;
		/* clone test name if any */
		if (name)
			suite->name = strdup(name);
	}
	return suite;
}

/*
 * add a test.
 */
CUNITEXP void CUNITAPI cunit_add_test(IN TestSuite suite, IN Test test)
{
	if (suite) {
		if (suite->count < suite->size) {
			/* unused block exists */
			suite->tests[suite->count] = test;
			suite->count++;
		} else {
			/* extends vector */
			Test *tests = (Test *)malloc(sizeof (Test)*(suite->size+CUNIT_VECTOR_INCREMENT));
			if (!tests) {
				cunit_fatal_error("Memory allocation failed!");
				return;
			} else {
				/* copy old vector */
				if (suite->tests) {
					memcpy(tests, suite->tests,
					       sizeof (Test)*suite->size);
				}
				suite->size += CUNIT_VECTOR_INCREMENT;
				/* release old vector */
				if (suite->tests)   /* need not this? */
					free(suite->tests);
				suite->tests = tests;
				/* readd element */
				cunit_add_test(suite, test);
			}
		}
	}
}

/*
 * suite runner.
 */
CUNITEXT void cunit_run_suite(TestSuite suite, TestResult result)
{
	Test test;
	int index;
	
	/* sanity check */
	if (suite == NULL)
		cunit_fatal_error("Can not run null suite object!");
	for (index  = 0; index < suite->count; index++) {
		/* break if runner should be stopped */
		if (cunit_should_stop(result))
			break;
		/* fetch and run test */
		test = suite->tests[index];
		if (test && test->runner)
			test->runner(test, result);
	}
}

/*
 * suite counter.
 */
CUNITEXT int cunit_count_suite(TestSuite suite)
{
	Test test;
	int count = 0, index;
	
	/* as counter may be called seperately, do not raise fatal
	 * error
	 */
	if (!suite)
		return 0;
	for (index  = 0; index < suite->count; index++) {
		test = suite->tests[index];
		/* call test counter */
		if (test) {
			if (test->counter)
				count += test->counter(test);
			else if (test->runner)
				count += 1;
		}
	}
	return count;
}

/*
 * suite destructor.
 */
CUNITEXT void cunit_delete_suite(TestSuite suite)
{
	if (suite) {
		if (suite->tests) {
			/* destroy maintained tests */
			int index;
			for (index = 0; index < suite->count; index++)
				suite->tests[index]->destroyer(suite->tests[index]);
			free(suite->tests);
		}
		/* destroy test name */
		if (suite->name)
			free(suite->name);
		free(suite);
	}
}

CUNITEXT char *cunit_format_message(IN char *message, IN int size,
				    IN const char *format, ...)
{
	char *result;
	va_list args;
	va_start(args, format);
	result = cunit_format_messagev(message, size, format, args);
	va_end(args);
	return result;
}

CUNITEXT char *cunit_format_messagev(IN char *message, IN int size,
                                     IN const char *format, IN va_list pvar)
{
#ifdef WIN32
	if (_vsnprintf(message, size-1, format, pvar) < 0) {
#else
	if (vsnprintf(message, size-1, format, pvar) < 0) {
#endif
		message[size-1] = '\0';
	}
	return message;
}

CUNITEXT void cunit_output_message(IN const char *format, ...)
{
	va_list args;
	va_start(args, format);
	cunit_output_messagev(format, args);
	va_end(args);
}

static void replace_percentm(const char *inbuffer,
			     char *outbuffer, int olen)
{
	register const char *t2;
	register char *t1, ch;
	if (!outbuffer || !inbuffer)
		return;
	olen--;
	for (t1 = outbuffer;
	     (ch = *inbuffer) && t1-outbuffer < olen;
	     ++inbuffer) {
		if (inbuffer[0] == '%' && inbuffer[1] == 'm') {
			for (++inbuffer,
			     t2 = (errno > 10000) ?
			     strerror(errno) : strerror(errno);
			     (t2 && t1-outbuffer < olen) && (*t1 = *t2++);
			     t1++);
		} else {
			*t1++ = ch;
		}
	}
	*t1 = '\0';
}

CUNITEXT void cunit_output_messagev(IN const char *oformat,
				    IN va_list pvar)
{
#define FMT_BUFLEN 2*4096 + 2*10
	char fmt_cpy[FMT_BUFLEN], format[FMT_BUFLEN + 1];
	int serrno = errno;
	
	if (!oformat)
		return;
	format[0] = '\0';
	replace_percentm(oformat, format + strlen(format),
			 sizeof(format) - strlen(format));
#ifdef WIN32
	if (_vsnprintf(fmt_cpy, FMT_BUFLEN-1, format, pvar) < 0) {
#else
	if (vsnprintf(fmt_cpy, FMT_BUFLEN-1, format, pvar) < 0) {
#endif
		fmt_cpy[FMT_BUFLEN-1] = '\0';
	}
#ifdef _MSC_VER
	/*
	 * Output to the debug window.
	 */
	OutputDebugStringA(fmt_cpy);
#else
#endif
	fprintf(stdout, "%s", fmt_cpy);
	errno = serrno; /* restore errno, just in case...? */
	return;
}

#define CUNIT_MESSAGE_LENGTH    256

CUNITEXP void CUNITAPI cunit_raise_assertion(CUNIT_SOURCE_LINE_DECL,
					     IN const char* message)
{
	CUNIT_THROW(cunit_new_throwable(file, line, CUNIT_ASSERTION, message))
}

CUNITEXP void CUNITAPI cunit_equals_char(CUNIT_SOURCE_LINE_DECL,
					 IN char expected,
					 IN char actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(char) '%c' does not equal '%c'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_short(CUNIT_SOURCE_LINE_DECL,
					  IN short expected,
					  IN short actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(short) '%d' does not equal '%d'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_int(CUNIT_SOURCE_LINE_DECL,
					IN int expected,
					IN int actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(int) '%d' does not equal '%d'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_long(CUNIT_SOURCE_LINE_DECL,
					 IN long expected,
					 IN long actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(long) '%d' does not equal '%d'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_float(CUNIT_SOURCE_LINE_DECL,
					  IN float expected,
					  IN float actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(float) '%f' does not equal '%f'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_double(CUNIT_SOURCE_LINE_DECL,
					   IN double expected,
					   IN double actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(double) '%f' does not equal '%f'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_uchar(CUNIT_SOURCE_LINE_DECL,
					  IN unsigned char expected,
					  IN unsigned char actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(unsigned char) '%c' does not equal '%c'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_ushort(CUNIT_SOURCE_LINE_DECL,
					   IN unsigned short expected,
					   IN unsigned short actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(unsigned short) '%u' does not equal '%u'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_uint(CUNIT_SOURCE_LINE_DECL,
					 IN unsigned int expected,
					 IN unsigned int actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(unsigned int) '%u' does not equal '%u'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_ulong(CUNIT_SOURCE_LINE_DECL,
					  IN unsigned long expected,
					  IN unsigned long actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(unsigned long) '%u' does not equal '%u'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_pointer(CUNIT_SOURCE_LINE_DECL,
					    IN void *expected,
					    IN void *actual)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (expected != actual) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Value(void *) '%p' does not equal '%p'.",
							   actual, expected));
	}
}

CUNITEXP void CUNITAPI cunit_equals_null(CUNIT_SOURCE_LINE_DECL,
					 IN void *object)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (object != NULL) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Object '%p' is not NULL.",
							   object));
	}
}

CUNITEXP void CUNITAPI cunit_equals_notnull(CUNIT_SOURCE_LINE_DECL,
					    IN void *object)
{
	char message[CUNIT_MESSAGE_LENGTH];
	
	if (object == NULL) {
		cunit_raise_assertion(file, line,
				      cunit_format_message(message,
							   CUNIT_MESSAGE_LENGTH,
							   "Object '%p' is NULL.",
							   object));
	}
}

/*=========================================================================
 * Default Runner
 *=======================================================================*/
/*=========================================================================
 * Local function definitions
 *=======================================================================*/
static void pause_runner(IN int wait);
static TestResult run_scope(IN TestRunner runner,
			    IN Test test, IN int wait);
static void print_result(IN TestResult result,
			 IN double run_seconds,
                         IN unsigned short run_millisec);

/*=========================================================================
 * Callback definitions
 *=======================================================================*/
static TestResult CUNITCBK run_console(IN TestRunner runner,
				       IN Test test);
static void CUNITCBK add_error(IN TestListener listener,
			       IN Test test, IN Throwable error);
static void CUNITCBK add_failure(IN TestListener listener,
				 IN Test test, IN Throwable error);
static void CUNITCBK start_test(IN TestListener listener,
				IN Test test);
static void CUNITCBK end_test(IN TestListener listener,
			      IN Test test);

/*=========================================================================
 * Default variables
 *=======================================================================*/
typedef struct _cunit_printer_t {
	cunit_listener_t;
	int success;
} cunit_printer_t;

/* default listener */
static cunit_printer_t _listener_imp = {
	{
		add_error,
		add_failure,
		start_test,
		end_test,
	},
	TRUE,
};
static TestListener _default_listener = (TestListener)&_listener_imp;

/* default runner */
static cunit_runner_t _runner_imp = {
	run_console,
};
static TestRunner _default_runner = (TestRunner)&_runner_imp;

/*
 * listener end_test callback.
 */
static void CUNITCBK end_test(IN TestListener listener,
			      IN Test test)
{
	if (((ResultPrinter)listener)->success == TRUE) {
		cunit_output_message("cunit: test %s ends success.\n", test->name);
	}
}

/*
 * listener start_test callback.
 */
static void CUNITCBK start_test(IN TestListener listener,
				IN Test test)
{
	cunit_output_message("cunit: test %s begins.\n", test->name);
	((ResultPrinter)listener)->success = TRUE;
}

/*
 * listener add_error callback.
 */
static void CUNITCBK add_error(IN TestListener listener,
			       IN Test test, IN Throwable error)
{
	cunit_output_message("cunit: test %s ends error.\n", test->name);
	cunit_output_message("%s(%d), %s\n",
			     error->file, error->line,
			     error->message);
	((ResultPrinter)listener)->success = FALSE;
}

/*
 * listener add_failure callback.
 */
static void CUNITCBK add_failure(IN TestListener listener,
				 IN Test test, IN Throwable failure)
{
	cunit_output_message("cunit: test %s ends failure.\n", test->name);
	cunit_output_message("%s(%d), %s\n",
			     failure->file, failure->line,
			     failure->message);
	((ResultPrinter)listener)->success = FALSE;
}

/*
 * runner callback.
 */
static TestResult CUNITCBK run_console(IN TestRunner runner, IN Test test)
{
	return run_scope(runner, test, FALSE);
}

/*
 * console result printer.
 */
CUNITEXT void print_result(IN TestResult result,
			   IN double run_seconds,
			   IN unsigned short run_millisec)
{
	cunit_output_message("run %d cases. success %d, failure %d, error %d.\ntotal time %.0f.%hu seconds\n",
			     result->runs, result->success_count,
			     result->failure_count, result->error_count,
			     run_seconds, run_millisec);
	return;
}

/*
 * pause runner at console.
 */
static void pause_runner(IN int wait)
{
	int ch;
	if (!wait) return;
	ch = getc(stdin);
}

/*
 * console runner with pauser.
 */
static TestResult run_scope(IN TestRunner runner,
			    IN Test test, int wait)
{
	struct timeb start_time, end_time;
	unsigned short run_millisec;
	double run_seconds;
	
	/* create TestResult object */
	TestResult result = cunit_new_result();
	
	/* register listener */
	cunit_register_listener(result, _default_listener);
	
	/* run tests, record time */
	ftime(&start_time);
	test->runner(test, result);
	ftime(&end_time);
	if (end_time.millitm > start_time.millitm) {
		run_millisec = end_time.millitm - start_time.millitm;
		run_seconds = difftime(end_time.time, start_time.time);
	} else {
		run_millisec = end_time.millitm+1000 - start_time.millitm;
		run_seconds = difftime(end_time.time, start_time.time);
	}
	
	/* output result */
	print_result(result, run_seconds, run_millisec);
	/* recycle resources */
	cunit_delete_result(result);
	test->destroyer(test);
	
	/* pause runner if needed */
	pause_runner(wait);
	return result;
}
