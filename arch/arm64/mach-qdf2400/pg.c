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
 * @(#)pg.c: partial goods wrappers
 * $Id: pg.c,v 1.279 2019-04-14 10:19:18 zhenglv Exp $
 */

#include <target/bitops.h>
#include <target/cpus.h>
#include <asm/io.h>

static uint64_t pseudo_cpu_mask = 0;
static uint64_t pseudo_llc_mask = 0;
static uint64_t pseudo_ddr_mask = 0;

bool pg_override_permitted(void)
{
	if (__raw_readl(FEATURE_CONFIG0) & FEATURE_PARTIAL_GOOD_DISABLE)
		return false;
	return true;
}

static void update_cpu_llc_masks(uint64_t cpu_mask, uint64_t llc_mask)
{
	if (!pg_override_permitted())
		return;

	__raw_writel_mask(LODWORD(cpu_mask), LODWORD(ALL_CPU_MASK),
			  PARTIAL_GOOD_CPU_REG_LSB);
	__raw_writel_mask(HIDWORD(cpu_mask), HIDWORD(ALL_CPU_MASK),
			  PARTIAL_GOOD_CPU_REG_MSB);
	__raw_writel_mask(PARTIAL_GOOD_REG_SELECT | LODWORD(llc_mask),
			  PARTIAL_GOOD_REG_SELECT | LODWORD(ALL_LLC_MASK),
			  PARTIAL_GOOD_L3_REG);
}

uint64_t ____get_cpu_mask(void)
{
        uint64_t cpu_mask;

        cpu_mask = __raw_readl(PARTIAL_GOOD_CPU_PORT_MSB);
        cpu_mask = (cpu_mask << 32) |
                   __raw_readl(PARTIAL_GOOD_CPU_PORT_LSB);
        return (~cpu_mask) & ALL_CPU_MASK;
}

void ____set_cpu_mask(uint64_t cpu_mask)
{
	update_cpu_llc_masks(cpu_mask, ____get_llc_mask());
}

uint64_t ____get_llc_mask(void)
{
        uint64_t llc_mask;

        llc_mask = __raw_readl(PARTIAL_GOOD_L3_PORT);
        return (~llc_mask) & ALL_LLC_MASK;
}

void ____set_llc_mask(uint64_t llc_mask)
{
	update_cpu_llc_masks(____get_cpu_mask(), llc_mask);
}

#ifdef CONFIG_QDF2400_PSEUDO_CPU_MASK
static uint64_t default_cpu_mask = ARCH_CPU_MASK;

static inline uint64_t __get_cpu_mask(void)
{
	return default_cpu_mask;
}

static inline void __set_cpu_mask(uint64_t cpu_mask)
{
	default_cpu_mask = cpu_mask;
}
#else
static inline uint64_t __get_cpu_mask(void)
{
	return ____get_cpu_mask();
}

void __set_cpu_mask(uint64_t cpu_mask)
{
	____set_cpu_mask(cpu_mask);
}
#endif

#ifdef CONFIG_QDF2400_PSEUDO_LLC_MASK
static uint64_t default_llc_mask = ARCH_LLC_MASK;

static inline uint64_t __get_llc_mask(void)
{
	/* It's meaningless to define MAX_LLCS, while LLC_MASK is useful
	 * for testing patterns with destined bad LLC interleaves due to
	 * the reason that number of good LLC interleaves must be ensured
	 * for configuring 32MB TCM memory region.
	 */
	return default_llc_mask;
}

static inline void __set_llc_mask(uint64_t llc_mask)
{
	default_llc_mask = llc_mask;
}
#else
static inline uint64_t __get_llc_mask(void)
{
	return ____get_llc_mask();
}

void __set_llc_mask(uint64_t llc_mask)
{
	____set_llc_mask(llc_mask);
}
#endif

#ifdef CONFIG_QDF2400_PSEUDO_DDR_MASK
static inline uint64_t __get_ddr_mask(void)
{
	return ARCH_DDR_MASK;
}
#else
static inline uint64_t __get_ddr_mask(void)
{
	uint64_t mask = 0;
	uint32_t ddr_chan, dimm_slot;

	/* Go through each DIMM SPD and get the associated memory size and total
	 * it up.
	 */
	for (ddr_chan = 0; ddr_chan < DDR_MAX_NUM_CHANS; ++ddr_chan) {
		for (dimm_slot = 0;
		     dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN; ++dimm_slot) {
			/* Checking that we can get the Serial EEPROM size is enough to
			 * determine if the slot is populated.
			 */
			if (spd_read_dimm_spd_bytes_total(ddr_chan,
							  dimm_slot) > 0)
				mask |= _BV(ddr_chan * DDR_MAX_NUM_SLOTS_PER_CHAN + dimm_slot);
		}
	}
	return mask;
}
#endif

uint64_t get_cpu_mask(void)
{
	if (pseudo_cpu_mask)
		return pseudo_cpu_mask;
	else
		return __get_cpu_mask();
}

void set_cpu_mask(uint64_t cpu_mask)
{
	if (cpu_mask)
		pseudo_cpu_mask = __get_cpu_mask() & cpu_mask;
	else
		pseudo_cpu_mask = __get_cpu_mask();
	__set_cpu_mask(pseudo_cpu_mask);
}

uint64_t get_llc_mask(void)
{
	if (pseudo_llc_mask)
		return pseudo_llc_mask;
	else
		return __get_llc_mask();
}

void set_llc_mask(uint64_t llc_mask)
{
	if (llc_mask)
		pseudo_llc_mask = __get_llc_mask() & llc_mask;
	else
		pseudo_llc_mask = __get_llc_mask();
	__set_llc_mask(pseudo_llc_mask);
}

uint64_t get_ddr_mask(void)
{
	if (pseudo_ddr_mask)
		return pseudo_ddr_mask;
	else
		return __get_ddr_mask();
}

void set_ddr_mask(uint64_t ddr_mask)
{
	if (ddr_mask)
		pseudo_ddr_mask = __get_ddr_mask() & ddr_mask;
	else
		pseudo_ddr_mask = __get_ddr_mask();
}
