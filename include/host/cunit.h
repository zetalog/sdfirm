/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2003
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@hzcnc.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)cuint.h: C language unit test library external header
 * $Id: cunit.h,v 1.5 2009-08-18 09:51:32 zhenglv Exp $
 */

#ifndef __CUNIT_H_INCLUDE__
#define __CUNIT_H_INCLUDE__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#   include <windows.h>
#   ifdef CUNIT_DLL
#       define CUNITIMP __declspec(dllimport)
#       if defined(CUNIT_IMP)  /* define this only while developing cunit */
#           define CUNITEXP __declspec(dllexport)
#       else
#           define CUNITEXP CUNITIMP
#       endif
#   else
#       define CUNITEXP extern
#       define CUNITIMP
#   endif
#   define CUNITEXT extern
#   define CUNITINT static
#   define CUNITAPI WINAPI
#   define CUNITCBK CALLBACK
#else   /* not tested, define anyway */
#   define CUNITEXP 
#   define CUNITIMP 
#   define CUNITAPI /* used for API interfaces */
#   define CUNITCBK /* used for callbacks */
#   define CUNITEXT extern
#   define CUNITINT static
#endif

#include <setjmp.h>

/* function's out parameter */
#define OUT
/* function's in parameter */
#define IN
/* function's in/out parameter */
#define INOUT

#ifndef NULL
#define NULL		(void *)0
#endif

#ifndef NOTNULL
#define NOTNULL		!NULL
#endif

#ifndef FALSE
#define FALSE		0
#endif

#ifndef TRUE
#define TRUE		!FALSE
#endif

/*
 * Following codes implements a C unit test framework. Please use
 * documented mcros.
 */
/*=========================================================================
 * Data structure definitions
 *=======================================================================*/
typedef struct _cunit_test_t        *Test;
typedef struct _cunit_result_t      *TestResult;
typedef struct _cunit_case_t        *TestCase;
typedef struct _cunit_suite_t       *TestSuite;
typedef struct _cunit_runner_t      *TestRunner;

typedef void (CUNITCBK *TestCall)(void);

/*=========================================================================
 * Macro definitions
 *=======================================================================*/
#define CUNIT_RUN_SUITE(suite)	cunit_run_test(NULL, suite())
#define CUNIT_RUN_CASE(caze)	cunit_run_test(NULL, \
					       (Test)CUNIT_NEW_CASE(caze))

#define CUNIT_NEW_SUITE(name)	cunit_new_suite(#name)
#define CUNIT_NEW_CASE(call)	cunit_new_case(#call, call)
#define CUNIT_NEW_CASE_EX(call, set_up, tear_down) \
				cunit_new_case_ex(#call, call, set_up, \
						  tear_down)

/*=========================================================================
 * MACRO:         CUNIT_ADD_CASE()
 * TYPE:          add a case into suite
 * OVERVIEW:      Add a TestCall function into a TestSuite as a TestCase.
 *                This macro wrappers two things a programmer should do
 *                to actually add case into suite.
 * INTERFACE:
 *   parameters:  case caller
 *   returns:     <none>
 * NOTE:          If you want to use an alternative name for you case,
 *                you may use cunit_new_case instead.
 *=======================================================================*/
#define CUNIT_ADD_CASE(suite, name)				\
	cunit_add_test(suite, (Test)CUNIT_NEW_CASE(name))
#define CUNIT_ADD_CASE_EX(suite, name, set_up, tear_down)	\
	cunit_add_test(suite,					\
		       (Test)CUNIT_NEW_CASE_EX(name, set_up, tear_down))
#define CUNIT_ADD_SUITE(suite, cases)				\
	cunit_add_test(suite, cases())

/*=========================================================================
 * MACRO:         CUNIT_SUITE_MAPS
 * TYPE:          map macros for suite creation
 * OVERVIEW:      These macros will be eclapsed as:
 *                Test suite_name() {
 *                    TestSuite suite = CUNIT_NEW_SUITE(suite_name);
 *                    CUNIT_ADD_SUITE(suite, sub_suite);
 *                    CUNIT_ADD_CASE(suite, sub_case);
 *                    return (Test)suite;
 *                }
 *                This eases the creation of suites.
 *=======================================================================*/
#define CUNIT_BEGIN_SUITE(name)					\
	Test name(void) {					\
		TestSuite suite = CUNIT_NEW_SUITE(name);
#define CUNIT_INCLUDE_SUITE(name)				\
		CUNIT_ADD_SUITE(suite, name);
#define CUNIT_INCLUDE_CASE(name)				\
		CUNIT_ADD_CASE(suite, name);
#define CUNIT_END_SUITE						\
		return (Test)suite;				\
	}

/*=========================================================================
 * Function definitions
 *=======================================================================*/
CUNITEXP TestRunner CUNITAPI cunit_get_default(void);
CUNITEXP void CUNITAPI cunit_set_default(IN TestRunner runner);
CUNITEXP TestResult CUNITAPI cunit_run_test(IN TestRunner runner,
					    IN Test test);
CUNITEXP TestSuite CUNITAPI cunit_new_suite(IN const char *name);
CUNITEXP void CUNITAPI cunit_add_test(IN TestSuite suite, IN Test test);
CUNITEXP TestCase CUNITAPI cunit_new_case(IN const char *name,
					  IN TestCall call);
CUNITEXP TestCase CUNITAPI cunit_new_case_ex(IN const char *name,
					     IN TestCall call,
					     TestCall set_up,
					     TestCall tear_down);
CUNITEXP char * CUNITAPI cunit_get_name(IN Test test);

/*
 * Following codes implements a TRY CATCH THROW clause for C language
 * programmers. Do not use undocumented functions, macros directly,
 * use documented ones instead.
 */
/*=========================================================================
 * Data structure definitions
 *=======================================================================*/
typedef struct _cunit_scope_t       *ThrowableScope;
typedef struct _cunit_throwable_t   *Throwable;

/*
 * This struct models the nesting of try/catch scopes.
 */
typedef struct _cunit_scope_t {
	/*
	 * State relevant to the platform dependent mechanism used to
	 * implement the flow of control (e.g. setjmp/longjmp).
	 */
	jmp_buf* env;
	/* An Throwable object. */
	Throwable throwable;
	/* The number of roots at the point of CUNIT_TRY. */
	int roots_count;
	/* The enclosing try/catch (if any). */
	struct _cunit_scope_t* outer;
} cunit_scope_t;

/*=========================================================================
 * Function definitions
 *=======================================================================*/
CUNITEXP ThrowableScope CUNITAPI cunit_get_scope(void);
CUNITEXP void CUNITAPI cunit_set_scope(ThrowableScope scope);
CUNITEXP int CUNITAPI cunit_get_roots_length(void);
CUNITEXP void CUNITAPI cunit_set_roots_length(int length);
CUNITEXP void CUNITAPI cunit_set_roots_pointer(int index, void *pointer);

/*=========================================================================
 * Macro definitions
 *=======================================================================*/
#define CUNIT_START_ROOTS       { int _tmp_roots_ = cunit_get_roots_length();
#define CUNIT_END_ROOTS           cunit_set_roots_length(_tmp_roots_);  }
#define CUNIT_DECLARE_ROOT(_type_, _var_, _value_)			\
	_type_ _var_ = (						\
	_var_ = (_type_)_value_,					\
	cunit_set_roots_length(cunit_get_roots_length()+1),		\
	cunit_set_roots_pointer(cunit_get_roots_length(),		\
				(void *)&_var_),			\
	_var_)

#define CUNIT_TRY							\
	{								\
		cunit_scope_t __scope__;				\
		int __state__;						\
		jmp_buf __env__;					\
		__scope__.outer = cunit_get_scope();			\
		cunit_set_scope(&__scope__);				\
		cunit_get_scope()->env = &__env__;			\
		cunit_get_scope()->roots_count = cunit_get_roots_length();\
		if ((__state__ = setjmp(__env__)) == 0) {

/*
 * Any non-null Throwable instance passed into a CUNIT_CATCH clause
 * is protected as a temporary root.
 */
#define CUNIT_CATCH(__throwable__)					\
        }								\
        cunit_set_scope(__scope__.outer);				\
        cunit_set_roots_length(__scope__.roots_count);			\
        if (__state__ != 0) {						\
		CUNIT_START_ROOTS					\
		CUNIT_DECLARE_ROOT(Throwable,				\
				   __throwable__,__scope__.throwable);

#define CUNIT_END_CATCH							\
			CUNIT_END_ROOTS					\
		}							\
	}

#define CUNIT_THROW(__throwable__)					\
	{								\
		Throwable __t__ = __throwable__;			\
		if (__t__ == NULL)					\
			cunit_fatal_error("CUNIT_THROW called with NULL");\
		cunit_get_scope()->throwable = __t__;			\
		longjmp(*((jmp_buf*)cunit_get_scope()->env), 1);	\
	}

/*
 * Following codes implements a Throwable (Assertion or Exception)
 * class for C language programmers. Do not use undocumented
 * functions, macros directly, use documented ones instead.
 */
/*=========================================================================
 * Macro definitions
 *=======================================================================*/
/* Throwable types */
#define CUNIT_EXCEPTION		0
#define CUNIT_ASSERTION		1

#define CUNIT_SOURCE_LINE	__FILE__, __LINE__
#define CUNIT_SOURCE_LINE_DECL	IN const char *file, IN const int line

/*=========================================================================
 * MACRO:         CUNIT_RAISE_EXCEPTION()
 * TYPE:          external exception handling operation
 * OVERVIEW:      Raise an exception.  This operation is intended
 *                to be used from within the TestCase.  It should
 *                be used for reporting only those exceptions and
 *                errors that are known to be "safe" and "harmless".
 *                The exception will cause Test error to be
 *                collected in the TestResult.
 * INTERFACE:
 *   parameters:  exception message
 *   returns:     <nothing>
 * NOTE:          Since this operation needs to allocate two new objects
 *                it should not be used for reporting memory-related
 *                problems.
 *=======================================================================*/
#define CUNIT_RAISE_EXCEPTION(m)	\
	cunit_raise_exception(CUNIT_SOURCE_LINE, m)

/*=========================================================================
 * MACRO:         CUNIT_DELETE_EXCEPTION()
 * TYPE:          external exception destroying operation
 * OVERVIEW:      Destroy an exception.  This operation is intended
 *                to be used inside CUNIT_CATCH.  When TestCase
 *                need to handling exception itself, it should
 *                either destroy or throw caught exception.
 * INTERFACE:
 *   parameters:  exception class
 *   returns:     <nothing>
 * NOTE:          Since this operation needs to allocate two new objects
 *                it should not be used for reporting memory-related
 *                problems.
 *=======================================================================*/
#define CUNIT_DELETE_EXCEPTION(e)	\
	cunit_delete_throwable(e)

#define CUNIT_RAISE_ASSERTION(m)	\
	cunit_raise_assertion(CUNIT_SOURCE_LINE, m)

/*=========================================================================
 * Function definitions
 *=======================================================================*/
/*=========================================================================
 * FUNCTION:      cunit_raise_exception()
 * TYPE:          internal exception handling operation
 * OVERVIEW:      Raise an exception.  This operation is intended
 *                to be used from within the runner only.  It should
 *                be used for reporting only those exceptions and
 *                errors that are known to be "safe" and "harmless",
 *                i.e., the internal consistency and integrity of
 *                the test case should not be endangered in any way.
 * INTERFACE:
 *   parameters:  exception class name
 *   returns:     <nothing>
 * NOTE:          Since this operation needs to allocate two new objects
 *                it should not be used for reporting memory-related
 *                problems.
 *=======================================================================*/
CUNITEXP void CUNITAPI cunit_raise_exception(CUNIT_SOURCE_LINE_DECL,
					     IN const char* message);
CUNITEXP void CUNITAPI cunit_raise_assertion(CUNIT_SOURCE_LINE_DECL,
					     IN const char* message);
/*=========================================================================
 * FUNCTION:      cunit_delete_throwable()
 * TYPE:          throwable destroying operation
 * OVERVIEW:      Destroy an exception.  This operation is intended
 *                to be used inside CUNIT_CATCH.  When TestCase
 *                need to handling exception itself, it should
 *                either destroy or throw caught exception.
 * INTERFACE:
 *   parameters:  exception class
 *   returns:     <nothing>
 * NOTE:          Since this operation needs to allocate two new objects
 *                it should not be used for reporting memory-related
 *                problems.
 *=======================================================================*/
CUNITEXP void CUNITAPI cunit_delete_throwable(IN Throwable exception);
CUNITEXP void CUNITAPI cunit_fatal_error(IN const char* message);
CUNITEXP int CUNITAPI cunit_throwable_type(IN Throwable throwable);
CUNITEXP char * CUNITAPI cunit_throwable_message(IN Throwable throwable);

/* assertion utilities */
#define CUNIT_ASSERT_EQUALS(type, expected, actual)	\
	cunit_equals_##type(CUNIT_SOURCE_LINE, expected, actual)
#define CUNIT_ASSERT_TRUE(condition)			\
	cunit_equals_int(CUNIT_SOURCE_LINE, TRUE, condition)

/*=========================================================================
 * Function definitions
 *=======================================================================*/
CUNITEXP void CUNITAPI cunit_equals_char(CUNIT_SOURCE_LINE_DECL,
					 IN char expected,
					 IN char actual);
CUNITEXP void CUNITAPI cunit_equals_short(CUNIT_SOURCE_LINE_DECL,
					  IN short expected,
					  IN short actual);
CUNITEXP void CUNITAPI cunit_equals_int(CUNIT_SOURCE_LINE_DECL,
					IN int expected,
					IN int actual);
CUNITEXP void CUNITAPI cunit_equals_long(CUNIT_SOURCE_LINE_DECL,
					 IN long expected,
					 IN long actual);
CUNITEXP void CUNITAPI cunit_equals_float(CUNIT_SOURCE_LINE_DECL,
					  IN float expected,
					  IN float actual);
CUNITEXP void CUNITAPI cunit_equals_double(CUNIT_SOURCE_LINE_DECL,
					   IN double expected,
					   IN double actual);
CUNITEXP void CUNITAPI cunit_equals_uchar(CUNIT_SOURCE_LINE_DECL,
					  IN unsigned char expected,
					  IN unsigned char actual);
CUNITEXP void CUNITAPI cunit_equals_ushort(CUNIT_SOURCE_LINE_DECL,
					   IN unsigned short expected,
					   IN unsigned short actual);
CUNITEXP void CUNITAPI cunit_equals_uint(CUNIT_SOURCE_LINE_DECL,
					 IN unsigned int expected,
					 IN unsigned int actual);
CUNITEXP void CUNITAPI cunit_equals_ulong(CUNIT_SOURCE_LINE_DECL,
					  IN unsigned long expected,
					  IN unsigned long actual);
CUNITEXP void CUNITAPI cunit_equals_pointer(CUNIT_SOURCE_LINE_DECL,
					    IN void *expected,
					    IN void *actual);
CUNITEXP void CUNITAPI cunit_equals_null(CUNIT_SOURCE_LINE_DECL,
					 void *object);
CUNITEXP void CUNITAPI cunit_equals_notnull(CUNIT_SOURCE_LINE_DECL,
					    void *object);

#ifdef __cplusplus
}
#endif

#endif /* __CUNIT_H_INCLUDE__ */
