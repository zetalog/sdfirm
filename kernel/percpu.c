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
 * @(#)percpu.c: per-CPU variable storage implementation
 * $Id: percpu.c,v 1.1 2019-12-17 16:36:00 zhenglv Exp $
 */

#include <target/heap.h>
#include <target/smp.h>
#include <target/percpu.h>
#include <target/panic.h>
#include <target/console.h>

uint64_t __percpu_offset[NR_CPUS];
caddr_t __percpu_alloc;
int __percpu_pages;
bool smp_initialized = false;

void percpu_init(void)
{
	size_t size, i;
	caddr_t ptr;

	if (smp_processor_id() != smp_boot_cpu)
		return;

	size = PERCPU_END - PERCPU_START;
	__percpu_pages = ALIGN_UP(size * NR_CPUS, PAGE_SIZE) / PAGE_SIZE;
	ptr = (caddr_t)page_alloc_pages(__percpu_pages);
	BUG_ON(!ptr || __percpu_alloc);
	__percpu_alloc = ptr;
	con_log("SMP allocating PERCPU area %016llx(%d).\n",
		(uint64_t)__percpu_alloc, __percpu_pages);
	for (i = 0; i < NR_CPUS; i++, ptr += size) {
		__percpu_offset[i] = ((uint64_t)ptr) - PERCPU_START;
		con_log("CPU%d area: %016llx\n",
			i, PERCPU_START + __percpu_offset[i]);
		if (i == smp_boot_cpu)
			memory_copy(PERCPU_START + __percpu_offset[i],
				    PERCPU_START, size);
		else
			memory_set(PERCPU_START + __percpu_offset[i],
				   0, size);
	}
	smp_initialized = true;
}
