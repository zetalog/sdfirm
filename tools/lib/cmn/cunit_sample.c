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
 * @(#)cunit_sample.c: cunit example program
 * $Id: cunit_sample.c,v 1.2 2011-09-15 02:06:35 zhenglv Exp $
 */

/* the only header should be included */
#include <host/cunit.h>

/*=========================================================================
 * CASE:          sample_case_failure
 * TYPE:          case showing failure causes
 * OVERVIEW:      Programmer can call CUNIT_ASSERT_EQUALS to check values.
 *                If value does not equal, CUNIT_ASSERT_EQUALS will cause
 *                a test failure and will not execute more.
 * INTERFACE:
 *   parameters:  <none>
 *   returns:     <none>
 * NOTE:          User should add CUNITCBK to avoid stack failure.
 *=======================================================================*/
void CUNITCBK sample_case_failure(void)
{
	char b = 'b';
	CUNIT_ASSERT_EQUALS(char, 'a', b);
	/* never reached endless loop */
	while (1) ;
}

/*=========================================================================
 * CASE:          sample_case_success
 * TYPE:          case showing success causes
 * OVERVIEW:      If value checked by CUNIT_ASSERT_EQUALS does equal, cunit
 *                will run case on. If no error or failure caught, cunit
 *                will record it as success.
 * INTERFACE:
 *   parameters:  <none>
 *   returns:     <none>
 * NOTE:          User should add CUNITCBK to avoid stack failure.
 *=======================================================================*/
void CUNITCBK sample_case_success(void)
{
	char a = 'a', b = 'b';
	CUNIT_ASSERT_EQUALS(char, 'a', a);
	CUNIT_ASSERT_EQUALS(char, 'b', b);
}

#if 0
/*=========================================================================
 * CASE:          sample_case_error
 * TYPE:          case showing error causes
 * OVERVIEW:      Exception raised using CUNIT_RAISE_EXCEPTION will be
 *                caught by TestRunner or case itself. If it's caught
 *                by TestRunner, a TestError will appears, if it's
 *                caught by case itself in the CUNIT_TRY, CUNIT_CATCH
 *                scope, and will not be thrown on, catcher should use
 *                CUNIT_DELETE_THROWABLE to drop it.
 * INTERFACE:
 *   parameters:  <none>
 *   returns:     <none>
 * NOTE:          User should add CUNITCBK to avoid stack failure.
 *=======================================================================*/
void CUNITCBK sample_case_error(void)
{
	CUNIT_TRY {
		/*
		 * Use CUNIT_RAISE_EXCEPTION create and throw a new
		 * exception.
		 * This action will stop TestRunner and cause an
		 * TestError.
		 */
		CUNIT_RAISE_EXCEPTION("Exceptions will be caught as errors.");
	} CUNIT_CATCH(e) {
		/*
		 * Exception and Assertions can be caught use this
		 * CUNIT_TRY,
		 * CUNIT_CATCH scope. Caller can use
		 * CUNIT_DELETE_THROWABLE to destroy caught Throwable
		 * instance, or can throw it back to cunit TestRunner
		 * using CUNIT_THROW.
		 */
		CUNIT_THROW(e);
	} CUNIT_END_CATCH
}
#endif

/*=========================================================================
 * SUITE:         sample_suite_1
 * TYPE:          suite packaged three cases
 * OVERVIEW:      This function demonstrates how to construct test suite
 *                using cunit.
 * INTERFACE:
 *   parameters:  <none>
 *   returns:     test class
 * NOTE:          Test must be returned, for cunit_run_test. You can use
 *                alternative name for suite.
 *=======================================================================*/
Test sample_suite_1(void)
{
	/* create test suite */
	TestSuite suite = CUNIT_NEW_SUITE(sample_suite_1);
	/* add test cases to the suite */
	CUNIT_ADD_CASE(suite, sample_case_success);
	CUNIT_ADD_CASE(suite, sample_case_failure);
	/* CUNIT_ADD_CASE(suite, sample_case_error); */
	return (Test)suite;
}

/*=========================================================================
 * SUITE:         sample_suite_2
 * TYPE:          suite packaged three cases
 * OVERVIEW:      This suite demonstrates different running sequence
 *                from sample_suite_1.
 * INTERFACE:
 *   parameters:  <none>
 *   returns:     test class
 * NOTE:          Test must be returned, for cunit_run_test.
 *=======================================================================*/
Test sample_suite_2(void)
{
	TestSuite suite = CUNIT_NEW_SUITE(sample_suite_2);
	/* CUNIT_ADD_CASE(suite, sample_case_error); */
	CUNIT_ADD_CASE(suite, sample_case_failure);
	CUNIT_ADD_CASE(suite, sample_case_success);
	return (Test)suite;
}

/*=========================================================================
 * SUITE:         sample_suite_mix
 * TYPE:          suite packaged suites & cases
 * OVERVIEW:      This function demonstrates mixture packaged suite usage.
 *                TestSuite can contain not only TestCase(s), but also
 *                TestSuite(s). This example also shows the usage of map
 *                macros.
 * INTERFACE:
 *   parameters:  <none>
 *   returns:     test class
 * NOTE:          Test must be returned, for cunit_run_test.
 *=======================================================================*/
CUNIT_BEGIN_SUITE(sample_suite_mix)
	CUNIT_INCLUDE_SUITE(sample_suite_1)
	CUNIT_INCLUDE_SUITE(sample_suite_2)
	CUNIT_INCLUDE_CASE(sample_case_success)
	CUNIT_INCLUDE_CASE(sample_case_failure)
	/* CUNIT_INCLUDE_CASE(sample_case_error) */
CUNIT_END_SUITE

/*=========================================================================
 * Runner:        main
 * TYPE:          c program entrance
 * OVERVIEW:      This function demonstrates use of cunit default
 *                TestRunner to run TestSuite(s) and TestCase(s).
 * INTERFACE:
 *   parameters:  command arguments
 *   returns:     exit code
 * NOTE:          TestResult will be printed into the debug context and
 *                STDOUT.
 *=======================================================================*/
int main(int argc, char **argv)
{
	/* run test suite */
	CUNIT_RUN_SUITE(sample_suite_mix);
	/* run single test case */
	CUNIT_RUN_CASE(sample_case_success);
	return 0;
}
