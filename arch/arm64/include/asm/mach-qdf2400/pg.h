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
 * @(#)pg.h: partial goods definitions
 * $Id: pg.h,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#ifndef __QDF2400_PG_H_INCLUDE__
#define __QDF2400_PG_H_INCLUDE__

#include <target/compiler.h>

#define MAX_CPU_NUM		48
#define MAX_LLC_NUM		(MAX_CPU_NUM/2)
#define ALL_CPU_MASK		((ULL(1) << MAX_CPU_NUM) - 1)
#define ALL_LLC_MASK		((ULL(1) << MAX_LLC_NUM) - 1)

#define CPU_IMC			0xFF
#define NR_EXTRA_CPU		1

#define CPUS_PER_CLUSTER	2
#define CPU_CLUSTER_MASK	((1ULL << CPUS_PER_CLUSTER) - 1)
#define CPU_TO_CLUSTER(cpu)	((cpu) / CPUS_PER_CLUSTER)
#define CLUSTER_TO_CPU_MASK(duplex)	\
        (CPU_CLUSTER_MASK << ((duplex) * CPUS_PER_CLUSTER))
#define MAX_CPU_CLUSTERS	(MAX_CPU_NUM/CPUS_PER_CLUSTER)

#define CPUS_PER_RAIL		6
#define CLUSTERS_PER_RAIL	3
#define CPU_RAIL_MASK		((1ULL << CPUS_PER_RAIL) - 1)
#define CPU_TO_RAIL(cpu)	((cpu) / CPUS_PER_RAIL)
#define RAIL_TO_CPU_MASK(cluster)	\
        (CPU_RAIL_MASK << ((cluster) * CPUS_PER_RAIL))
#define MAX_CPU_RAILS		(MAX_CPU_NUM/CPUS_PER_RAIL)

#ifndef __ASSEMBLY__
static inline uint32_t get_apc_mask(uint64_t cpu_mask)
{
	uint64_t apc_mask = cpu_mask;

	apc_mask = (((apc_mask) & ULL(0xaaaaaaaaaaaaaaaa)) >> 1) |
		    ((apc_mask) & ULL(0x5555555555555555));
	apc_mask = (((apc_mask) & ULL(0x4444444444444444)) >> 1) |
		    ((apc_mask) & ULL(0x1111111111111111));
	apc_mask = (((apc_mask) & ULL(0x3030303030303030)) >> 2) |
		    ((apc_mask) & ULL(0x0303030303030303));
	apc_mask = (((apc_mask) & ULL(0x0f000f000f000f00)) >> 4) |
		    ((apc_mask) & ULL(0x000f000f000f000f));
	apc_mask = (((apc_mask) & ULL(0x00ff000000ff0000)) >> 8) |
		    ((apc_mask) & ULL(0x000000ff000000ff));
	apc_mask = (((apc_mask) & ULL(0x0000ffff00000000)) >> 16) |
		    ((apc_mask) & ULL(0x000000000000ffff));
	return (uint32_t)apc_mask;
}

static inline uint16_t get_apr_mask(uint32_t apc_mask)
{
	uint32_t apr_mask = apc_mask;

	apr_mask = ( (apr_mask) & 0x49249249) |
		   (((apr_mask) & 0x92492492) >> 1) |
		   (((apr_mask) & 0x24924924) >> 2);
	apr_mask = ( (apr_mask) & 0x41041041) |
		   (((apr_mask) & 0x08208208) >> 2);
	apr_mask = ( (apr_mask) & 0x03003003) |
		   (((apr_mask) & 0x400C00C0) >> 4);
	apr_mask = ( (apr_mask) & 0x0700000F) |
		   (((apr_mask) & 0x0000F000) >> 8);
	apr_mask = ( (apr_mask) & 0x000000FF) |
		   (((apr_mask) & 0x07000000) >> 16);
	return (uint16_t)apr_mask;
}

#define PARTIAL_GOOD_CPU_REG_LSB	SEC_CTL_REG(0x38F4)
#define PARTIAL_GOOD_CPU_REG_MSB	SEC_CTL_REG(0x38F8)
#define PARTIAL_GOOD_L3_REG		SEC_CTL_REG(0x38FC)
#define PARTIAL_GOOD_CPU_PORT_LSB	SEC_CTL_REG(0x3900)
#define PARTIAL_GOOD_CPU_PORT_MSB	SEC_CTL_REG(0x3904)
#define PARTIAL_GOOD_L3_PORT		SEC_CTL_REG(0x3908)

/* PARTIAL_GOOD_L3_REG */
#define PARTIAL_GOOD_REG_SELECT		_BV(31)

/* TODO: use monaco definitions */
#define DDR_MAX_NUM_CHANS		6

/* Fused PG */
void ____set_cpu_mask(uint64_t cpu_mask);
uint64_t ____get_cpu_mask(void);
void ____set_llc_mask(uint64_t cpu_mask);
uint64_t ____get_llc_mask(void);
bool pg_override_permitted(void);

/* Pseudo aware PG */
void set_cpu_mask(uint64_t cpu_mask);
uint64_t get_cpu_mask(void);
void set_llc_mask(uint64_t llc_mask);
uint64_t get_llc_mask(void);
void set_ddr_mask(uint64_t ddr_mask);
uint64_t get_ddr_mask(void);
#endif /* __ASSEMBLY__ */

#endif /* __QDF2400_PG_H_INCLUDE__ */
