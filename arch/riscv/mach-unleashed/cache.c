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
 * @(#)cache.c: FU540 (unleashed) cache controller implementation
 * $Id: cache.c,v 1.1 2019-10-23 14:49:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/atomic.h>
#include <asm/mach/cache.h>

/* Info accessors */
static inline uint8_t ccache_banks(uint64_t base_addr)
{
	return ((uint8_t *)(base_addr + CCACHE_INFO))[CCACHE_INFO_BANKS];
}

static inline uint8_t ccache_ways(uint64_t base_addr)
{
	return ((uint8_t *)(base_addr + CCACHE_INFO))[CCACHE_INFO_WAYS];
}

static inline uint8_t ccache_lgSets(uint64_t base_addr)
{
	return ((uint8_t *)(base_addr + CCACHE_INFO))[CCACHE_INFO_LG_SETS];
}

static inline uint8_t ccache_lgBlockBytes(uint64_t base_addr)
{
	return ((uint8_t *)
		(base_addr + CCACHE_INFO))[CCACHE_INFO_LG_BLOCKBYTES];
}

/* The size of memory that fits within a single way */
static inline uint64_t ccache_stride(uint64_t base_addr)
{
	return ccache_banks(base_addr) <<
	       (ccache_lgSets(base_addr) + ccache_lgBlockBytes(base_addr));
}

/* Block memory access until operation completed */
static inline void ccache_barrier_0(void)
{
	asm volatile("fence rw, io" : : : "memory" );
}

static inline void ccache_barrier_1(void)
{
	asm volatile("fence io, rw" : : : "memory" );
}

/* flush64 takes a byte-aligned address, while flush32 takes an address
 * right-shifted by 4
 */
static inline void ccache_flush64(uint64_t base_addr, uint64_t flush_addr)
{
	ccache_barrier_0();
	*(volatile uint64_t *)(base_addr + CCACHE_FLUSH64) = flush_addr;
	ccache_barrier_1();
}

static inline void ccache_flush32(uint64_t base_addr, uint32_t flush_addr)
{
	ccache_barrier_0();
	*(volatile uint32_t *)(base_addr + CCACHE_FLUSH32) = flush_addr;
	ccache_barrier_1();
}

/* The next time the cache updates an SRAM, inject this bit error */
static inline void ccache_inject_error_data(uint64_t base_addr, uint8_t bit)
{
	ccache_barrier_0();
	*(volatile uint32_t *)(base_addr + CCACHE_INJECT) =
		CCACHE_ECC_TOGGLE_DATA | bit;
	ccache_barrier_1();
}

static inline void ccache_inject_error_meta(uint64_t base_addr, uint8_t bit)
{
	ccache_barrier_0();
	*(volatile uint32_t *)(base_addr + CCACHE_INJECT) =
		CCACHE_ECC_TOGGLE_META | bit;
	ccache_barrier_1();
}

/* Readout the most recent failed address */
static inline uint64_t ccache_data_fail_address(uint64_t base_addr)
{
	ccache_barrier_0();
	return *(volatile uint64_t *)
	       (base_addr + CCACHE_DATA_FAIL + CCACHE_ECC_ADDR);
}

static inline uint64_t ccache_data_fix_address(uint64_t base_addr)
{
	ccache_barrier_0();
	return *(volatile uint64_t *)
	       (base_addr + CCACHE_DATA_FIX  + CCACHE_ECC_ADDR);
}

static inline uint64_t ccache_meta_fix_address(uint64_t base_addr)
{
	ccache_barrier_0();
	return *(volatile uint64_t *)
	       (base_addr + CCACHE_META_FIX  + CCACHE_ECC_ADDR);
}

/* Reading the failure count atomically clears the interrupt */
static inline uint32_t ccache_data_fail_count(uint64_t base_addr)
{
	uint32_t out;

	ccache_barrier_0();
	out = *(volatile uint32_t *)
	      (base_addr + CCACHE_DATA_FAIL + CCACHE_ECC_COUNT);
	ccache_barrier_1();
	return out;
}

static inline uint32_t ccache_data_fix_count(uint64_t base_addr)
{
	uint32_t out;

	ccache_barrier_0();
	out = *(volatile uint32_t *)
	      (base_addr + CCACHE_DATA_FIX  + CCACHE_ECC_COUNT);
	ccache_barrier_1();
	return out;
}

static inline uint32_t ccache_meta_fix_count(uint64_t base_addr)
{
	uint32_t out;

	ccache_barrier_0();
	out = *(volatile uint32_t *)
	      (base_addr + CCACHE_META_FIX  + CCACHE_ECC_COUNT);
	ccache_barrier_1();
	return out;
}

typedef uint32_t atomic32_t;
typedef uint64_t atomic64_t;

/* Enable ways; allow cache to use these ways */
static inline uint8_t ccache_enable_ways(uint64_t base_addr, uint8_t value)
{
	uint32_t old;
	volatile atomic32_t *enable = (atomic32_t *)(base_addr + CCACHE_ENABLE);

	ccache_barrier_0();
#if 0
	old = atomic_xchg(enable, value);
#else
	old = *enable;
	*enable = value;
#endif
	ccache_barrier_1();
	return old;
}

/* Lock ways; prevent client from using these ways */
static inline uint64_t ccache_lock_ways(uint64_t base_addr,
					int client, uint64_t ways)
{
	uint64_t old;
	volatile atomic64_t *ways_v = (atomic64_t *)(base_addr + CCACHE_WAYS);

	ccache_barrier_0();
#if 0
	old = atomic_test_and_clear(&ways_v[client], ways);
#else
	old = ways_v[client];
	ways_v[client] &= ~ways;
#endif
	ccache_barrier_1();
	return old;
}

/* Unlock ways; allow client to use these ways */
static inline uint64_t ccache_unlock_ways(uint64_t base_addr,
					  int client, uint64_t ways)
{
	uint64_t old;
	volatile atomic64_t *ways_v = (atomic64_t *)(base_addr + CCACHE_WAYS);

	ccache_barrier_0();
#if 0
	old = atomic_test_and_set(&ways_v[client], ways);
#else
	old = ways_v[client];
	ways_v[client] |= ways;
#endif
	ccache_barrier_1();
	return old;
}

/* Swap ways; allow client to use ONLY these ways */
static inline uint64_t ccache_flip_ways(uint64_t base_addr,
					int client, uint64_t ways)
{
	uint64_t old;
	volatile atomic64_t *ways_v = (atomic64_t *)(base_addr + CCACHE_WAYS);

	ccache_barrier_0();
#if 0
	old = atomic_xchg(&ways_v[client], ways);
#else
	old = ways_v[client];
	ways_v[client] = ways;
#endif
	ccache_barrier_1();
	return old;
}

void board_cache_init(void)
{
	ccache_enable_ways(L2_CC_BASE, 14);
}
