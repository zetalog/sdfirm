/*
 * Arm SCP/MCP Software
 * Copyright (c) 2017-2022, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2023
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
 * @(#)cmn600.c: ARM CoreLink CMN-600 coherent mesh network implementation
 * $Id: cmn600.c,v 1.1 2023-09-06 15:21:00 zhenglv Exp $
 */

#include <target/noc.h>
#include <target/panic.h>

#define CMN_MODNAME	"n100"

uint8_t ccix_mmap_count;
struct cmn600_ccix_ha_mmap ccix_mmap[CMN_MAX_HA_MMAP_COUNT];

uint64_t cmn_cml_base_offset(void)
{
	if (cmn600_hw_chip_id() != 0)
		return cmn600_hw_chip_base() * cmn600_hw_chip_id();
	else
		return 0;
}

static void cmn_cml_capabilities(void)
{
}

void cmn600_cml_detect_mmap(void)
{
	unsigned int region_index;
	struct cmn600_memregion *region;

	for (region_index = 0; region_index < cmn_mmap_count; region_index++) {
		region = &cmn_mmap_table[region_index];

		if (region->type == CMN600_REGION_TYPE_CCIX) {
			ccix_mmap[ccix_mmap_count].base = region->base;
			ccix_mmap[ccix_mmap_count].size = region->size;
			ccix_mmap_count++;
		}
	}
}

void cmn600_cml_init(void)
{
}
