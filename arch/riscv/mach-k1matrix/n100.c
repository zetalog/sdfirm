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

/* ===========================================================================
 * CMN configurations
 * =========================================================================== */
cmn_nid_t cmn_snf_table[] = {
	32,
};
cmn_id_t cmn_snf_count = ARRAY_SIZE(cmn_snf_table);
cmn_id_t cmn_sa_count = 0;

struct cmn600_memregion cmn_mmap_table[] = {
#ifdef CONFIG_CMN600_SAM_RANGE_BASED
	{
		.base = 0,
		.size = SZ_512G,
		.type = CMN600_MEMORY_REGION_TYPE_SYSCACHE,
		.node_id = 12,
	},
	{
		.base = 0,
		.size = SZ_512G,
		.type = CMN600_MEMORY_REGION_TYPE_SYSCACHE,
		.node_id = 44,
	},
#endif
	/* Non-hashed region 0 SYS I/O (global) */
	{
		.base = SYS_IO_GLOBAL_BASE,
		.size = SZ_2G,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 0,
	},
	/* Non-hashed region 1 PCIe0 MMIO */
	{
		.base = PCIE0_SLV_MEM_BASE,
		.size = SZ_128M,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 36,
	},
	/* Non-hashed region 2 PCIe1 MMIO */
	{
		.base = PCIE1_SLV_MEM_BASE,
		.size = SZ_128M,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 72,
	},
	/* Non-hashed region 3 SYS SRAM (local) */
	{
		.base = SRAM0_BASE,
		.size = SZ_128M,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 0,
	},
	/* Non-hashed region 4 SYS I/O (local) */
	{
		.base = SYS_IO_LOCAL_BASE,
		.size = SZ_4G,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 0,
	},
#ifdef CONFIG_CMN600_CML
	/* Non-hased region 5 SYS I/O, PCIe0 MMIO DEV, PCIe1 MMIO DEV (remote) */
	{
		.base = DIE1_BASE + SYS_IO_GLOBAL_BASE,
		.size = SZ_4G,
		.type = CMN600_REGION_TYPE_CCIX,
		.node_id = 64,
	},
	/* Non-hased region 6 PCIe0 MMIO, PCIe1 MMIO (remote) */
	{
		.base = DIE1_BASE + PCIE0_SLV_MEM_BASE,
		.size = SZ_256M,
		.type = CMN600_REGION_TYPE_CCIX,
		.node_id = 64,
	},
	/* Non-hashed region 7 DDR (remote) */
	{
		.base = DIE1_BASE + DDR_BASE,
		.size = SZ_4G,
		.type = CMN600_REGION_TYPE_CCIX,
		.node_id = 64,
	},
#endif
	/* Non-hashed region 8 PCIe0 MMIO DEV */
	{
		.base = PCIE0_SLV_CFG_BASE,
		.size = SZ_128M,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 36,
	},
	/* Non-hashed region 9 PCIe1 MMIO DEV */
	{
		.base = PCIE1_SLV_CFG_BASE,
		.size = SZ_128M,
		.type = CMN600_MEMORY_REGION_TYPE_IO,
		.node_id = 72,
	},
	/* system cache group 0 DDR (local) */
#ifdef CONFIG_CMN600_SAM_RANGE_BASED
	{
		.base = DDR_BASE,
		.size = SZ_4G,
		.type = CMN600_REGION_TYPE_SYSCACHE_SUB,
		.node_id = 32,
	}
#else
	{
		.base = DDR_BASE,
		.size = SZ_4G,
		.type = CMN600_MEMORY_REGION_TYPE_SYSCACHE,
		.node_id = 32,
	},
#endif
};
cmn_id_t cmn_mmap_count = ARRAY_SIZE(cmn_mmap_table);

/* ===========================================================================
 * CML (CCIX CXG) configurations
 * =========================================================================== */
cmn_id_t cml_link_id = 0;
cmn_id_t cml_rnf_count_remote = 2;
/* PCIe bus# */
uint8_t cml_pcie_bus_num = CMN_pcie_bdf(0, 0, 0);
uint8_t cml_pcie_tlp_tc = 1;

struct cmn600_ccix_ha_mmap cml_ha_mmap_table_remote[] = {
	{
		/* The region covers SYS_IO_GLOBAL/PCIE0_CFG/PCIE1_CFG */
		0,
		.base = SYS_IO_GLOBAL_BASE,
		.size = SZ_4G,
	},
	{
		/* The region covers PCIE0_MEM/PCIE1_MEM */
		0,
		.base = PCIE0_SLV_MEM_BASE,
		.size = SZ_256M,
	},
	{
		0,
		.base = DDR_BASE,
		.size = SZ_4G,
	},
};
cmn_id_t cml_ha_mmap_count_remote = ARRAY_SIZE(cml_ha_mmap_table_remote);

void k1matrix_n100_d2d_init(void)
{
	cmn600_cml_init();
}

void k1matrix_n100_init(void)
{
	cmn600_initialized = false;
	cmn600_init();
}
