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
 * @(#)mem.h: memory range allocation interface
 * $Id: mem.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __MEM_H_INCLUDE__
#define __MEM_H_INCLUDE__

#include <target/generic.h>

#ifndef __ASSEMBLY__
#include <asm/mem.h>

#ifdef CONFIG_MEM_MAX_REGIONS
#define NR_MEM_REGIONS		CONFIG_MEM_MAX_REGIONS
#else
#define NR_MEM_REGIONS		3 /* In case RAM/ROM overlaps */
#endif /* CONFIG_MEM_MAX_REGIONS */

struct mem_region {
	phys_addr_t base;
	phys_addr_t size;
};

struct mem_type {
	unsigned long cnt;	/* number of regions */
	unsigned long max;	/* size of the allocated array */
	phys_addr_t total_size;	/* size of all regions */
	struct mem_region *regions;
};

#ifdef CONFIG_MEM
phys_addr_t mem_start(void);
phys_addr_t mem_end(void);
void mem_add(phys_addr_t base, phys_addr_t size);
void mem_reserve(phys_addr_t base, phys_addr_t size);
void mem_remove(phys_addr_t base, phys_addr_t size);
void mem_walk_memory(bool (*iter)(struct mem_region *, void *),
		     void *data);
void mem_walk_reserved(bool (*iter)(struct mem_region *, void *),
		       void *data);
phys_addr_t mem_alloc(phys_addr_t size, phys_addr_t align);
void mem_free(phys_addr_t base, phys_addr_t size);
void mem_init(void);
void mem_free_all(void);
void mem_dump_all(void);
#else
#define mem_start()		RAM_START
#define mem_end()		RAMEND
#define mem_add(base, size)	do { } while (0)
#define mem_reserve(base, size)	do { } while (0)
#define mem_remove(base, size)	do { } while (0)
#define mem_init()		do { } while (0)
#endif
#endif /* __ASSEMBLY__ */

#endif /* __MEM_H_INCLUDE__ */
