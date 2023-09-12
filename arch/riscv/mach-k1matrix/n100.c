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
 * @(#)mmu.c: K1Matrix N100 networking on chip implementation
 * $Id: mmu.c,v 1.1 2023-09-06 17:39:00 zhenglv Exp $
 */

#include <target/arch.h>
#include <target/noc.h>

cmn_nid_t cmn_snf_table[] = {
	32,
	32,
};
int cmn_snf_count = ARRAY_SIZE(cmn_snf_table);

struct cmn600_memregion cmn_mmap_table[] = {
	{
		.base = SYS_IO_BASE,
		.size = SZ_4G,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 0
	},
	{
		.base = PCIE0_SLV_BASE,
		.size = SZ_128M,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 36
	},
	{
		.base = PCIE1_SLV_BASE,
		.size = SZ_128M,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 72
	},
	{
		.base = SRAM0_BASE,
		.size = SZ_128M,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 0
	},
	{
		.base = GMAC_BASE,
		.size = SZ_4G,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 0
	},
	{
		.base = DDR_BASE,
		.size = SZ_4G,
		.type = CMN600_REGION_TYPE_SYSCACHE_SUB,
		.node_id = 0
	}
};
int cmn_mmap_count = ARRAY_SIZE(cmn_mmap_table);

void k1matrix_n100_init(void)
{
	cmn600_initialized = false;
	cmn600_init();
}
