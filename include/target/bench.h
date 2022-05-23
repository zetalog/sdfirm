/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2019
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zhenglv@hotmail.com
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
 * @(#)bench.h: CPU test bench interfaces
 * $Id: bench.h,v 1.0 2019-12-20 15:14:00 zhenglv Exp $
 */

#ifndef __BENCH_H_INCLUDE__
#define __BENCH_H_INCLUDE__

#include <target/arch.h>
#include <target/jiffies.h>

#define BENCH_INPUT(align)		\
	__testfn_start = .;		\
	KEEP (*(.testfn))		\
	__testfn_end = .;

#define BENCH_SECTION(align)		\
	.test_array ALIGN(align) : {	\
		BENCH_INPUT(align)	\
	}

/* Auto APC Test Framework (cpu_local_exec):
 *
 * Running Flags:
 * SYNC:  Can be defined by a single test function. Tests flagged with
 *        SYNC will start together from all APCs as long as SYNC flag
 *        is also invoked by the dispatcher caller.
 * RAND:  Can be invoked by the dispatcher caller to run all tests in
 *        randomized order. This is useful for stress tests.
 * NOSMP: Cannot be run in SMP mode, if SMP is configured and multiple
 *        testing cores are selected, each core executes the case
 *        serially.
 * SHARE: Memory chunk allocated for the case should be shared between
 *        the runnning cores.
 * META:  A case that implements a pattern running style for all other
 *        non-meta cases.
 * SIMPLE:A case that is used by bench_simple, and shouldn't be run
 *        directly by "bench" command".
 */
#define CPU_EXEC_SYNC	0x00000001
#define CPU_EXEC_RAND	0x00000002
#define CPU_EXEC_NOSMP	0x00000004
#define CPU_EXEC_SHARE	0x00000008
#define CPU_EXEC_META	0x80000000
#define CPU_EXEC_SIMPLE	0x40000000 /* used by bench_simple */

#define CPU_EXEC_SUCCESS	1
#define CPU_EXEC_FAILURE	0

#define CPU_WAIT_INFINITE		-1

#ifdef __ASSEMBLY__

#ifndef LINKER_SCRIPT
	.macro define_asm_testfn, _name:req, _size:req, _align=128, _repeat=1, _flags=0, _timeout=CPU_WAIT_INFINITE
	.pushsection .testfn, "a"
	.align 3
	.type __testfn0_\_name, %object
	.pushsection .rodata
	9001:
	.ascii "\_name"
	.byte 0
	.popsection
	.quad 9001b
	.quad \_name
	.long \_size
	.long \_align
	.long \_flags
	.long \_repeat
	.quad \_timeout
	.popsection
	.endm
#endif

#else /* __ASSEMBLY__ */

typedef int (*percpu_entry)(caddr_t percpu_area);

struct cpu_exec_test {
	char *name;
	percpu_entry func;
	uint32_t alloc_size;
	uint32_t alloc_align;
	uint32_t flags;
	uint32_t repeat;
	uint64_t timeout;
} __align(8);

extern struct cpu_exec_test __testfn_start[];
extern struct cpu_exec_test __testfn_end[];

#define __define_testfn(__func, __size, __align, __flags, __repeat, __timeout)	\
	static struct cpu_exec_test __testfn_##__func __used		\
	__attribute__((__section__(".testfn"))) = {			\
		.name = #__func,					\
		.func = __func,						\
		.alloc_size = __size,					\
		.alloc_align = __align,					\
		.flags = __flags,					\
		.repeat = __repeat,					\
		.timeout = __timeout,					\
	}

#define c_testfn_repeat(fn, size, align, repeat)		\
	__define_testfn(fn, size, align, 0, repeat, CPU_WAIT_INFINITE)
#define c_testfn_repeat_sync(fn, size, align, repeat, timeout)	\
	__define_testfn(fn, size, align, CPU_EXEC_SYNC, repeat, timeout)
#define c_testfn(fn, size, align)				\
	c_testfn_repeat(fn, size, align, 1)
#define c_testfn_sync(fn, size, align, timeout)			\
	c_testfn_repeat_sync(fn, size, align, 1, timeout)

int bench_exec(struct cpu_exec_test *start, int nr_tests,
	       uint64_t init_cpu_mask, uint32_t flags,
	       tick_t timeout, void (*notify)(bool));
int bench_didt(uint64_t init_cpu_mask, struct cpu_exec_test *fn,
	       tick_t interval, tick_t period, int repeats,
	       tick_t start_delay);
int bench_simple(uint64_t cpu_mask, struct cpu_exec_test *fn, bool sync);
struct cpu_exec_test *bench_test_find(const char *name);
uint64_t bench_get_cpu_mask(void);

#ifdef CONFIG_TEST_BENCH
void bench_init(void);
#else
#define bench_init()		do { } while (0)
#endif

#endif /* __ASSEMBLY__ */

#endif /* __BENCH_H_INCLUDE__ */
