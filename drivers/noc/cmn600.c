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
#include <target/cmdline.h>

#define CMN_MODNAME	"n100"

#define CMN_HNF_MAPPING_RANGE_BASED	0
#define CMN_HNF_MAPPING_HASHED_3SN	1
#define CMN_HNF_MAPPING_HASHED_6SN	2
#define CMN_HNF_MAPPING_DIRECT		3
#define CMN_HNF_MAPPING_STRIPED		(~CMN_HNF_MAPPING_RANGE_BASED)

#ifdef CONFIG_CONSOLE_VERBOSE
static const char * const cmn_type2name[] = {
	[CMN_INVAL] = "<M>",
	[CMN_DVM] = "DVM",
	[CMN_CFG] = "CFG",
	[CMN_DTC] = "DTC",
	[CMN_HNI] = "HN-I",
	[CMN_HNF] = "HN-F",
	[CMN_XP] = "XP",
	[CMN_SBSX] = "SBSX",
	[CMN_RNI] = "RN-I",
	[CMN_RND] = "RN-D",
	[CMN_RN_SAM] = "RN-SAM",
	/* Pseudo types */
	[CMN_RNF] = "RN-F",
	[CMN_SNF] = "SN-F",
};

static const char * const cmn_cml_type2name[] = {
	[CMN_CXRA - CMN_CML] = "CXRA",
	[CMN_CXHA - CMN_CML] = "CXHA",
	[CMN_CXLA - CMN_CML] = "CXLA",
};

static const char *cmn_dev2name[] = {
	[CMN_MXP_INVAL] = "<M>",
	[CMN_MXP_RNI] = "RN-I",
	[CMN_MXP_RND] = "RN-D",
	[CMN_MXP_RNF_CHIB] = "RN-F CHI Issue B",
	[CMN_MXP_RNF_CHIB_ESAM] = "RN-F CHI Issue B ESAM",
	[CMN_MXP_RNF_CHIA] = "RN-F CHI Issue A",
	[CMN_MXP_RNF_CHIA_ESAM] = "RN-F CHI Issue A ESAM",
	[CMN_MXP_HNT] = "HN-T",
	[CMN_MXP_HNI] = "HN-I",
	[CMN_MXP_HND] = "HN-D",
	[CMN_MXP_SNF] = "SN-F",
	[CMN_MXP_SBSX] = "SBSX",
	[CMN_MXP_HNF] = "HN-F",
	[CMN_MXP_CXHA] = "CXHA",
	[CMN_MXP_CXRA] = "CXRA",
	[CMN_MXP_CXRH] = "CXRH",
};

const char *cmn600_dev_type_name(uint8_t dev_type)
{
	if (dev_type <= CMN_MXP_CXRH)
		return cmn_dev2name[dev_type];

	return cmn_dev2name[CMN_MXP_INVAL];
}

const char *cmn600_node_type_name(uint16_t node_type)
{
	if (node_type <= CMN_RN_SAM)
		return cmn_type2name[node_type];
	/* Pseudo names */
	if (node_type <= CMN_SNF)
		return cmn_type2name[node_type];

	if (node_type >= CMN_CML &&
	    node_type <= CMN_CML_MAX)
		return cmn_cml_type2name[node_type - CMN_CML];

	return cmn_type2name[CMN_INVAL];
}

static const char *const cmn_mmap2name[] = {
	[CMN600_MEMORY_REGION_TYPE_IO] = "I/O",
	[CMN600_MEMORY_REGION_TYPE_SYSCACHE] = "System Cache",
	[CMN600_REGION_TYPE_SYSCACHE_SUB] = "Sub-System Cache",
	[CMN600_REGION_TYPE_CCIX] = "CCIX",
	[CMN600_REGION_TYPE_SYSCACHE_NONHASH] = "System Cache Non-hash",
	[CMN600_MEMORY_REGION_TYPE_SYSCACHE_SECONDARY] = "System Cache secondary",
};

const char *cmn600_mem_region_name(uint8_t type)
{
	if (type >= ARRAY_SIZE(cmn_mmap2name))
		return "<Invalid>";

	return cmn_mmap2name[type];
}

static const char *const cmn_rev2name[] = {
	[CMN_r1p0] = "r1p0",
	[CMN_r1p1] = "r1p1",
	[CMN_r1p2] = "r1p2",
	[CMN_r1p3] = "r1p3",
	[CMN_r2p0] = "r2p0",
	[CMN_r3p0] = "r3p0",
	[CMN_r3p1] = "r3p1",
	[CMN_r3p2] = "r3p2",
};

const char *cmn600_revision_name(uint8_t revision)
{
	if (revision >= ARRAY_SIZE(cmn_rev2name))
		return "<Unknown>";

	return cmn_rev2name[revision];
}
#endif

caddr_t cmn_bases[NR_CMN_NODES];
cmn_id_t cmn_nr_nodes;
cmn_id_t cmn_xp_count;
cmn_id_t cmn_hnf_count;
cmn_id_t cmn_rnd_count;
cmn_id_t cmn_rni_count;
cmn_id_t cmn_hni_count;
cmn_id_t cmn_rnf_count;
cmn_id_t cmn_dtc_count;
cmn_id_t cmn_sbsx_count;
cmn_id_t cmn_snf_count;
cmn_id_t cmn_cxla_count;
cmn_id_t cmn_cxra_count;
cmn_id_t cmn_cxha_count;
cmn_id_t cmn_scg_count;
cmn_id_t cmn_rn_sam_int_count;
cmn_id_t cmn_rn_sam_ext_count;
bool cmn600_initialized = false;

cmn_nid_t cmn_xp_ids[CMN_MAX_MXP_COUNT];
cmn_nid_t cmn_hnf_ids[CMN_MAX_HNF_COUNT];
cmn_nid_t cmn_rnd_ids[CMN_MAX_RND_COUNT];
cmn_nid_t cmn_rni_ids[CMN_MAX_RND_COUNT];
cmn_nid_t cmn_hni_ids[CMN_MAX_HNI_COUNT];
cmn_nid_t cmn_dtc_ids[CMN_MAX_DTC_COUNT];
cmn_nid_t cmn_sbsx_ids[CMN_MAX_SBSX_COUNT];
cmn_nid_t cmn_rn_sam_ext_ids[CMN_MAX_RN_SAM_EXT_COUNT];
cmn_nid_t cmn_rn_sam_int_ids[CMN_MAX_RN_SAM_INT_COUNT];
cmn_nid_t cmn_cxra_ids[CMN_MAX_CXG_COUNT];
cmn_nid_t cmn_cxla_ids[CMN_MAX_CXG_COUNT];
cmn_nid_t cmn_cxha_ids[CMN_MAX_CXG_COUNT];
cmn_nid_t cmn_hnf_scgs[CMN_MAX_HNF_COUNT];

#ifdef CONFIG_CMN600_DEBUG
static void cmn_debug_init(void)
{
	console_init();
}

void cmn_writeq(uint64_t v, caddr_t a, const char *n, int i)
{
	if (i < 0)
		con_dbg(CMN_MODNAME ": %016llx=%016llx %s\n",
			(uint64_t)a, v, n);
	else
		con_dbg(CMN_MODNAME ": %016llx=%016llx %s%d\n",
			(uint64_t)a, v, n, i);
	__raw_writeq(v, a);
}
#endif

#ifdef CONFIG_CMN600_HNF_CAL
static unsigned int cal_mode_factor = 1;
static bool scg_region_enabled[CMN_MAX_SCGS] = { false, false, false, false };

static void cmn_hnf_cal_validate(void)
{
	if ((cmn_hnf_count % 2) != 0) {
		con_err(CMN_MODNAME ": HN-F count %d should be even for cal mode\n",
			cmn_hnf_count);
		BUG();
	}
}

static void cmn_hnf_cal_process(caddr_t hnf)
{
	/* If CAL mode is set, only even numbered hnf node should be added
	 * to the sys_cache_grp_hn_nodeid registers and hnf_count should
	 * be incremented only for the even numbered hnf nodes.
	 */
	if (((cmn_node_id(hnf)) % 2 == 1) && cmn_cal_supported()) {
		/* Factor to manipulate the group and bit_pos */
		cal_mode_factor = 2;

		/* Reduce the cmn_hnf_count as the current hnf node is not
		 * getting included in the sys_cache_grp_hn_nodeid
		 * register.
		 */
		cmn_hnf_count--;
	}
}

static void cmn_hnf_cal_config_scg(caddr_t hnf, cmn_id_t id)
{
	/* If CAL mode is set, add only even numbered hnd node to
	 * cmn_rnsam_sys_cache_grp_hn_nodeid registers
	 */
	if (((cmn_node_id(hnf) % 2) == 0) && cmn_cal_supported())
		cmn_hnf_scgs[id / cal_mode_factor] = cmn_node_id(hnf);
	else
		cmn_hnf_scgs[id] = cmn_node_id(hnf);
}

static void cmn_hnf_cal_enable_scg(unsigned int scg)
{
	unsigned int scg_region = 0;

	scg_region = ((2 * scg) / 2) + (scg % 2);
	BUG_ON(scg_region >= CMN_MAX_SCGS);
	scg_region_enabled[scg_region] = true;
}

static void cmn_hnf_cal_apply_scg(caddr_t rnsam)
{
	unsigned int region_index;

	if (cmn_cal_supported()) {
		for (region_index = 0; region_index < CMN_MAX_SCGS; region_index++) {
			if (scg_region_enabled[region_index]) {
#ifdef CONFIG_CMN600_DEBUG_CONFIGURE
				con_dbg(CMN_MODNAME ": SCG: %d/%d, CAL: en\n",
					region_index, CMN_MAX_SCGS);
#endif
				cmn_setq(CMN_scg_hnf_cal_mode_en(region_index),
					 CMN_rnsam_sys_cache_grp_cal_mode(rnsam));
			} else
				cmn_clearq(CMN_scg_hnf_cal_mode_en(region_index),
					   CMN_rnsam_sys_cache_grp_cal_mode(rnsam));
		}
	}
}
#else
#define cmn_hnf_cal_validate()		do { } while (0)
#define cmn_hnf_cal_process(hnf)	do { } while (0)
#define cmn_hnf_cal_enable_scg(scg)	do { } while (0)
#define cmn_hnf_cal_apply_scg(rnsam)	do { } while (0)

static void cmn_hnf_cal_config_scg(caddr_t hnf, cmn_id_t id)
{
	cmn_hnf_scgs[id] = cmn_node_id(hnf);
}
#endif
/*
static void cmn_hnf_cal_config_ocm(caddr_t hnf)
{
	cmn_writeq(hnf_cfg_ctl(hnf) |
		CMN_hnf_ocm_en(0x1) |
		CMN_hnf_ocm_always_en(0x0),
		CMN_hnf_cfg_ctl(hnf),
		"CMN_hnf_cfg_ctl", -1);
}

static void cmn_hnf_cal_disable_ocm(caddr_t hnf)
{
	cmn_writeq(hnf_cfg_ctl(hnf) |
		CMN_hnf_ocm_en(0x0) |
		CMN_hnf_ocm_always_en(0x0),
		CMN_hnf_cfg_ctl(hnf),
		"CMN_hnf_cfg_ctl", -1);
}

static void cmn_hnf_cfg_slc_lockways(caddr_t hnf, uint64_t ways, uint64_t num_hnf)
{
	cmn_writeq(hnf_slc_lock_ways(hnf) |
		CMN_hnf_slc_lockways_ways(ways) |
		CMN_hnf_slc_lockways_num_hnf(num_hnf),
		CMN_hnf_slc_lock_ways(hnf),
		"CMN_hnf_cfg_ctl", -1);
	cmn_writeq(hnf_slc_lock_base0(hnf) |
		CMN_hnf_slc_base0(),
		CMN_hnf_slc_lock_base(hnf,0),
		"CMN_hnf_slc_lock_base", -1);
}

static void cmn_hnf_slc_lock_enable(caddr_t hnf)
{
	cmn_writeq(hnf_cfg_ctl(hnf) |
		CMN_hnf_ocm_en(0x1) |
		CMN_hnf_ocm_always_en(0x0),
		CMN_hnf_cfg_ctl(hnf),
		"CMN_hnf_cfg_ctl", -1);
}

static void cmn_hnf_slc_lock_disable(caddr_t hnf)
{
	cmn_writeq(hnf_cfg_ctl(hnf) |
		CMN_hnf_ocm_en(0x0) |
		CMN_hnf_ocm_always_en(0x0),
		CMN_hnf_cfg_ctl(hnf),
		"CMN_hnf_cfg_ctl", -1);
}


static void cmn_hnf_abf(uint64_t hnf, uint64_t abf_mode, uint64_t saddr, uint64_t eaddr)
{
	cmn_writeq(hnf_abf_lo_addr(hnf) |
		   CMN_abf_lo_addr(saddr),
		   CMN_hnf_abf_lo_addr(hnf),
		   "CMN_hnf_abf_lo_addr", -1);
	cmn_writeq(hnf_abf_hi_addr(hnf) |
		   CMN_abf_hi_addr(eaddr),
		   CMN_hnf_abf_hi_addr(hnf),
		   "CMN_hnf_abf_hi_addr", -1);
	cmn_writeq(hnf_abf_pr(hnf) |
		   CMN_abf_mode(abf_mode) |
		   CMN_abf_enable(1),
		   CMN_hnf_abf_pr(hnf),
		   "CMN_hnf_abf_lo_addr", -1);
}

void cmn_hnf_abf_done(int hnf_id_idx)
{
	uint64_t abf_sr = 0x0;

	while ((abf_sr & 0xf) == 0x0) {
		//wait_ns(10000);
		abf_sr = hnf_abf_sr(CMN_HNF_BASE(cmn_hnf_ids[hnf_id_idx]));
	}
}
*/
uint8_t cmn_hnf_mapping(void)
{
	if (cmn_snf_count == 3)
		return CMN_HNF_MAPPING_HASHED_3SN;
	else if (cmn_snf_count == 6)
		return CMN_HNF_MAPPING_HASHED_6SN;
	else if (is_power_of_2(cmn_snf_count))
		return CMN_HNF_MAPPING_DIRECT;
	else
		return CMN_HNF_MAPPING_RANGE_BASED;
}

static void cmn_configure_hnf_sam_hashed(caddr_t hnf)
{
	cmn_nid_t nodeid=0;

	nodeid = cmn600_hw_snf_hashed(cmn_node_id(hnf));
	switch (cmn_hnf_mapping()) {
	case CMN_HNF_MAPPING_DIRECT:
		/* TODO: support cmn_snf_count using hnf tgt ids */
		cmn_writeq(CMN_hn_cfg_sn_nodeid(0,
			   nodeid),
			   CMN_hnf_sam_control(hnf),
			   "CMN_hnf_sam_control", -1);
		con_dbg(CMN_MODNAME ": hnf%ld-snf%d\n", CMN_hnf_sam_control(hnf), nodeid);
		break;
	case CMN_HNF_MAPPING_HASHED_3SN:
		/* TODO: top address bit 0/1 calculation */
		cmn_writeq(CMN_hn_cfg_three_sn_en |
			   CMN_hn_cfg_sn_nodeid(0, cmn_snf_table[0]) |
			   CMN_hn_cfg_sn_nodeid(1, cmn_snf_table[1]) |
			   CMN_hn_cfg_sn_nodeid(2, cmn_snf_table[2]),
			   CMN_hnf_sam_control(hnf),
			   "CMN_hnf_sam_control", -1);
		break;
	case CMN_HNF_MAPPING_HASHED_6SN:
		/* TODO: top address bit 0/1/2 calculation */
		cmn_writeq(CMN_hn_cfg_six_sn_en |
			   CMN_hn_cfg_sn_nodeid(0, cmn_snf_table[0]) |
			   CMN_hn_cfg_sn_nodeid(1, cmn_snf_table[1]) |
			   CMN_hn_cfg_sn_nodeid(2, cmn_snf_table[2]),
			   CMN_hnf_sam_control(hnf),
			   "CMN_hnf_sam_control", -1);
		cmn_writeq(CMN_hn_cfg_sn_nodeid(0, cmn_snf_table[4]) |
			   CMN_hn_cfg_sn_nodeid(1, cmn_snf_table[5]) |
			   CMN_hn_cfg_sn_nodeid(2, cmn_snf_table[6]),
			   CMN_hnf_sam_6sn_nodeid(hnf),
			   "CMN_hnf_sam_control", -1);
		break;
	default: /* CMN_HNF_MAPPING_RANGE_BASED */
		break;
	}
}

#ifdef CONFIG_CMN600_SAM_RANGE_BASED
static void cmn_configure_hnf_sam_range_based(caddr_t hnf)
{
	cmn_id_t region_index;
	cmn_id_t region_sub_count = 0;
	struct cmn600_memregion *region;
	caddr_t base;

	for (region_index = 0; region_index < cmn_mmap_count; region_index++) {
		region = &cmn_mmap_table[region_index];

		if (region->type != CMN600_REGION_TYPE_SYSCACHE_SUB)
			continue;

		base = cmn600_cml_base(region->base, region->chip_id, false);
		cmn_writeq(CMN_sam_range_nodeid(region->node_id) |
			   CMN_sam_range_size(__ilog2_u64((region->size) / CMN_SAM_GRANU)) |
			   CMN_sam_range_base_addr(base / CMN_SAM_GRANU) |
			   CMN_sam_range_valid,
			   CMN_hnf_sam_memregion(hnf, region_sub_count),
			   "CMN_hnf_sam_memregion", region_sub_count);

#ifdef CONFIG_CMN600_DEBUG_CONFIGURE
		con_dbg(CMN_MODNAME ": %s: HN-F SAM %d: ID: %d, [%016llx - %016llx]\n",
			cmn600_mem_region_name(region->type),
			region_sub_count, region->node_id,
			(uint64_t)base, (uint64_t)(base + region->size));
#endif

		region_sub_count++;
	}
}
#else
#define cmn_configure_hnf_sam_range_based(hnf)	do { } while (0)
#endif

static void cmn600_configure_hnf_sam(caddr_t hnf, cmn_id_t id)
{
	cmn_hnf_cal_process(hnf);
	cmn_hnf_cal_config_scg(hnf, id);

	/* Set target node */
	cmn_configure_hnf_sam_hashed(hnf);
	cmn_configure_hnf_sam_range_based(hnf);

	cmn_writeq(CMN_ppu_policy(CMN_ppu_policy_ON) |
		   CMN_ppu_op_mode(CMN_ppu_op_mode_FAM) |
		   CMN_ppu_dyn_en, CMN_hnf_ppu_pwpr(hnf),
		   "CMN_hnf_ppu_pwpr", -1);
}

cmn_id_t cmn600_max_tgt_nodes(void)
{
	switch (cmn_revision()) {
	case CMN_r1p0:
	case CMN_r1p1:
	case CMN_r1p3:
		return 2 * 4;
	case CMN_r1p2:
	case CMN_r2p0:
		return 3 * 4;
	case CMN_r3p0:
	case CMN_r3p1:
	case CMN_r3p2:
		return 5 * 4;
	}
	return 0;
}

#if 0
static uint64_t cmn_xp_masked(cmn_id_t xp_index, cmn_id_t node_index)
{
	uint64_t mask;

	if (node_index == 0)
		mask = cmn_hw_snf_p0_mask(cmn_ddr_mask) |
		       cmn_hw_hnf_p0_mask(cmn_cpu_mask);
	else
		mask = cmn_hw_snf_p1_mask(cmn_ddr_mask) |
		       cmn_hw_hnf_p1_mask(cmn_cpu_mask);
	return mask & (1 << xp);
}
#endif

static void cmn600_discover_external(caddr_t node, caddr_t xp)
{
	uint8_t dev_type;
	cmn_pid_t xp_pid;

	xp_pid = cmn_node_pid(node);
	dev_type = cmn_mxp_device_type(xp, xp_pid);
	if ((dev_type == CMN_MXP_CXRH) ||
	    (dev_type == CMN_MXP_CXHA) ||
	    (dev_type == CMN_MXP_CXRA)) {
		cmn_cxla_ids[cmn_cxla_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
	} else {
		/* External RN-SAM node */
		if (cmn_rn_sam_ext_count >= CMN_MAX_RN_SAM_EXT_COUNT) {
			con_err(CMN_MODNAME ": RN_SAM external count %d >= limit %d\n",
				cmn_rn_sam_ext_count, CMN_MAX_RN_SAM_EXT_COUNT);
			BUG();
		}
		cmn_rn_sam_ext_ids[cmn_rn_sam_ext_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
	}
#ifdef CONFIG_CMN600_DEBUG_DISCOVER
	con_dbg(CMN_MODNAME ": 0x%016llx %s ID: %d, LID: %d\n",
		(uint64_t)node, cmn600_node_type_name(cmn_node_type(node)),
		cmn_node_id(node), cmn_logical_id(node));
#endif
}

static void cmn600_discover_internal(caddr_t node)
{
	switch (cmn_node_type(node)) {
	case CMN_HNF:
		if (cmn_hnf_count >= CMN_MAX_HNF_COUNT) {
			con_err(CMN_MODNAME ": HN-F count %d >= limit %d\n",
				cmn_hnf_count, CMN_MAX_HNF_COUNT);
			BUG();
		}
		cmn_hnf_ids[cmn_hnf_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	case CMN_RN_SAM:
		if (cmn_rn_sam_int_count >= CMN_MAX_RN_SAM_INT_COUNT) {
			con_err(CMN_MODNAME ": RN_SAM internal count %d >= limit %d\n",
				cmn_rn_sam_int_count, CMN_MAX_RN_SAM_INT_COUNT);
			BUG();
		}
		cmn_rn_sam_int_ids[cmn_rn_sam_int_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	case CMN_RND:
		if (cmn_rnd_count >= CMN_MAX_RND_COUNT) {
			con_err(CMN_MODNAME ": RN-D count %d >= limit %d\n",
				cmn_rnd_count, CMN_MAX_RND_COUNT);
			BUG();
		}
		cmn_rnd_ids[cmn_rnd_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	case CMN_RNI:
		if (cmn_rni_count >= CMN_MAX_RNI_COUNT) {
			con_err(CMN_MODNAME ": RN-I count %d >= limit %d\n",
				cmn_rni_count, CMN_MAX_RNI_COUNT);
			BUG();
		}
		cmn_rni_ids[cmn_rni_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	case CMN_SBSX:
		if (cmn_sbsx_count >= CMN_MAX_SBSX_COUNT) {
			con_err(CMN_MODNAME ": SBSX count %d >= limit %d\n",
				cmn_sbsx_count, CMN_MAX_SBSX_COUNT);
			BUG();
		}
		cmn_sbsx_ids[cmn_sbsx_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	case CMN_DTC:
		if (cmn_dtc_count >= CMN_MAX_DTC_COUNT) {
			con_err(CMN_MODNAME ": DTC count %d >= limit %d\n",
				cmn_dtc_count, CMN_MAX_DTC_COUNT);
			BUG();
		}
		cmn_dtc_ids[cmn_dtc_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	case CMN_CXRA:
		cmn_cxra_ids[cmn_cxra_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	case CMN_CXHA:
		cmn_cxha_ids[cmn_cxha_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	case CMN_HNI:
		cmn_hni_ids[cmn_hni_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	default:
		/* Nothing to be done for other node types */
		break;
	}
#ifdef CONFIG_CMN600_DEBUG_DISCOVER
	con_dbg(CMN_MODNAME ": 0x%016llx %s ID: %d, LID: %d\n",
		(uint64_t)node, cmn600_node_type_name(cmn_node_type(node)),
		cmn_node_id(node), cmn_logical_id(node));
#endif
}

#ifdef ARCH_HAVE_XP_TABLE
static const uint8_t cmn_dev2count[] = {
	[CMN_MXP_INVAL] = 0,
	[CMN_MXP_RNI] = 2,
	[CMN_MXP_RND] = 3, /* TBD: need to confirm */
	[CMN_MXP_RNF_CHIB] = 0,
	[CMN_MXP_RNF_CHIB_ESAM] = 1,
	[CMN_MXP_RNF_CHIA] = 0,
	[CMN_MXP_RNF_CHIA_ESAM] = 1,
	[CMN_MXP_HNT] = 2,
	[CMN_MXP_HNI] = 1,
	[CMN_MXP_HND] = 3,
	[CMN_MXP_SNF] = 0,
	[CMN_MXP_SBSX] = 1,
	[CMN_MXP_HNF] = 1,
	[CMN_MXP_CXHA] = 3, /* TBD: need to confirm */
	[CMN_MXP_CXRA] = 2, /* TBD: need to confirm */
	[CMN_MXP_CXRH] = 4,
};

const uint8_t cmn600_dev_node_count(uint16_t dev_type)
{
	if (dev_type <= CMN_MXP_CXRH)
		return cmn_dev2count[dev_type];

	return cmn_dev2count[CMN_MXP_INVAL];
}

void cmn600_discover_xp(caddr_t xp)
{
	cmn_id_t node_index, port_index, port_node_index;
	cmn_id_t node_count;
	cmn_id_t port_node_count;
	cmn_nid_t xp_pid, nid;
	caddr_t node;
	cmn_id_t pid;
	uint8_t dev_type;

	node_count = cmn_child_count(xp);
	node_index = 0;
	for (port_index = 0; port_index < 2; port_index++) {
		pid = 1 - port_index;
		xp_pid = CMN_XP_PID(cmn_node_x(xp), cmn_node_y(xp), pid);
		nid = CMN_XP_PID2NID(xp_pid, 0);
		dev_type = cmn_mxp_device_type(xp, pid);
		port_node_count = cmn600_dev_node_count(dev_type);
		if (cmn600_hw_xp_masked(nid, dev_type)) {
			con_dbg(CMN_MODNAME ": XP%d(%d,%d,%d) skip %d %s nodes\n",
				xp_pid, (uint8_t)cmn_node_x(xp),(uint8_t)cmn_node_y(xp),
				pid, port_node_count, cmn600_dev_type_name(dev_type));
		} else {
			if (node_count < port_node_count) {
				con_err(CMN_MODNAME ": XP%d(%d,%d,%d) %d nodes exceeding %d\n",
					xp_pid, (uint8_t)cmn_node_x(xp), (uint8_t)cmn_node_y(xp),
					pid, port_node_count, node_count);
				return;
			}
			for (port_node_index = 0; port_node_index < port_node_count; port_node_index++) {
				node = cmn_child_node(xp, node_index + port_node_index);
				if (cmn_child_external(xp, node_index + port_node_index))
					cmn600_discover_external(node, xp);
				else
					cmn600_discover_internal(node);
			}
		}
		node_count -= port_node_count;
		node_index += port_node_count;
	}
}
#else
void cmn600_discover_xp(caddr_t xp)
{
	cmn_id_t node_index;
	cmn_id_t node_count;
	caddr_t node;

	node_count = cmn_child_count(xp);
	for (node_index = 0; node_index < node_count; node_index++) {
		node = cmn_child_node(xp, node_index);
		if (cmn_child_external(xp, node_index))
			cmn600_discover_external(node, xp);
		else
			cmn600_discover_internal(node);
	}
}
#endif

void cmn600_discover(void)
{
	cmn_id_t xp_index;
	cmn_id_t xp_count;
	caddr_t xp;

	BUG_ON(cmn_node_type(CMN_CFGM_BASE) != CMN_CFG);

	con_dbg(CMN_MODNAME ": discovery:\n");
	xp_count = cmn_child_count(CMN_CFGM_BASE);
	for (xp_index = 0; xp_index < xp_count; xp_index++) {
		xp = cmn_child_node(CMN_CFGM_BASE, xp_index);
		cmn_xp_ids[cmn_xp_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = xp;

#ifdef CONFIG_CMN600_DEBUG_DISCOVER
		con_dbg(CMN_MODNAME ": XP (%d, %d) ID: %d, LID: %d\n",
			(int)cmn_node_x(xp), (int)cmn_node_y(xp),
			(int)cmn_node_id(xp), (int)cmn_logical_id(xp));
#endif

		cmn600_discover_xp(xp);
	}

	/* RN-F nodes doesn't have node type identifier and hence the count
	 * cannot be determined during the discover process. RN-F count
	 * will be total RN-SAM count - total RN-D, RN-I and CXHA count.
	 */
	cmn_rnf_count = cmn_rn_sam_int_count + cmn_rn_sam_ext_count -
		(cmn_rnd_count + cmn_rni_count + cmn_cxha_count);
	if (cmn_rnf_count > CMN_MAX_RNF_COUNT) {
		con_err(CMN_MODNAME ": RN-F count %d >= limit %d\n",
			cmn_rnf_count, CMN_MAX_RNF_COUNT);
		BUG();
	}
	cmn_hnf_cal_validate();
	cmn_snf_count = cmn600_hw_snf_count();
	con_dbg(CMN_MODNAME ": Total internal RN-SAM: %d\n", cmn_rn_sam_int_count);
	con_dbg(CMN_MODNAME ": Total external RN-SAM: %d\n", cmn_rn_sam_ext_count);
	con_dbg(CMN_MODNAME ": Total HN-F: %d\n", cmn_hnf_count);
	con_dbg(CMN_MODNAME ": Total RN-F: %d\n", cmn_rnf_count);
	con_dbg(CMN_MODNAME ": Total RN-D: %d\n", cmn_rnd_count);
	con_dbg(CMN_MODNAME ": Total RN-I: %d\n", cmn_rni_count);
	con_dbg(CMN_MODNAME ": Total HN-I: %d\n", cmn_hni_count);
	con_dbg(CMN_MODNAME ": Total SN-F: %d\n", cmn_snf_count);
	con_dbg(CMN_MODNAME ": Total CXHA: %d\n", cmn_cxha_count);
	con_dbg(CMN_MODNAME ": Total CXRA: %d\n", cmn_cxra_count);
	con_dbg(CMN_MODNAME ": Total CXLA: %d\n", cmn_cxla_count);
	con_dbg(CMN_MODNAME ": Total nodes: %d\n", cmn_nr_nodes);
}

#define CMN_MAX_HASH_MEM_REGIONS		4
#define CMN_MAX_NON_HASH_MEM_REGIONS_PER_GROUP	4
#define CMN_MAX_NON_HASH_MEM_REGIONS(n)		\
	((n) * CMN_MAX_NON_HASH_MEM_REGIONS_PER_GROUP)

static void cmn600_configure_rn_sam(caddr_t rnsam)
{
	caddr_t base;
	caddr_t region_base;
	cmn_id_t region_index;
	struct cmn600_memregion *region;
	cmn_lid_t lid;
	cmn_nid_t nid;
	cmn_id_t region_io_count = 0;
	cmn_id_t region_sys_count = 0;
	cmn_id_t region_sys2_count = 0;
	unsigned int region_type;
	uint32_t memregion;
	cmn_id_t tgt_nodes;
	cmn_id_t hnf;
	cmn_id_t snf;
	cmn_id_t lid_base;
	uint8_t scg;
	cmn_id_t hnfs_per_scg;
	size_t scg_size;

	tgt_nodes = cmn600_max_tgt_nodes();
	BUG_ON(tgt_nodes == 0);
	for (region_index = 0; region_index < cmn_mmap_count; region_index++) {
		region = &cmn_mmap_table[region_index];
		/* TODO: Should rely on chip id to fill remote regions */
		if (region->type == CMN600_REGION_TYPE_CCIX)
			base = cmn600_cml_base(region->base, region->chip_id, true);
#ifdef CONFIG_CMN600_SAM_RANGE
		else if (region->type == CMN600_MEMORY_REGION_TYPE_SYSCACHE)
			base = region->base;
#endif
		else
			base = cmn600_cml_base(region->base, region->chip_id, false);
#ifdef CONFIG_CMN600_DEBUG_CONFIGURE
		con_dbg(CMN_MODNAME "configuring...\n");
		con_dbg(CMN_MODNAME ": %s-%d: RN SAM %d/%d: ID: %d, [%016llx - %016llx]\n",
			cmn600_mem_region_name(region->type), region->type,
			region_index, cmn_mmap_count,
			region->node_id, (uint64_t)base, (uint64_t)(base + region->size));
#endif
		switch (region->type) {
		case CMN600_MEMORY_REGION_TYPE_IO:
		case CMN600_REGION_TYPE_CCIX:
			if (region_io_count > tgt_nodes) {
				con_err(CMN_MODNAME ": IO count %d > limit %d\n",
					region_io_count, tgt_nodes);
				BUG();
			}
			region_type = region->type == CMN600_MEMORY_REGION_TYPE_IO ?
				CMN_region_target_HNI : CMN_region_target_CXRA;
			memregion = CMN_valid_region(region_type, base, region->size);
			cmn_writeq_mask(CMN_region(region_io_count, memregion),
					CMN_region(region_io_count, CMN_region_MASK),
					CMN_rnsam_non_hash_mem_region(rnsam, region_io_count),
					"CMN_rnsam_non_hash_mem_region", region_io_count);
			cmn_writeq_mask(CMN_nodeid(region_io_count, region->node_id),
					CMN_nodeid(region_io_count, CMN_nodeid_MASK),
					CMN_rnsam_non_hash_tgt_nodeid(rnsam, region_io_count),
					"CMN_rnsam_non_hash_tgt_nodeid", region_io_count);
			region_io_count++;
			break;

		case CMN600_MEMORY_REGION_TYPE_SYSCACHE:
			if (region_sys_count >= CMN_MAX_HASH_MEM_REGIONS) {
				con_err(CMN_MODNAME ": SYS count %d > limit %d\n",
					region_sys_count, CMN_MAX_HASH_MEM_REGIONS);
				BUG();
			}

			scg_size = region->size / cmn_scg_count;
			region_base = base + (scg_size * region_sys_count);
			memregion = CMN_valid_region(CMN_region_target_HNF,
						     region_base, scg_size);
			if (cmn_scg_count == 2) {
				cmn_writeq_mask(CMN_region(region_sys_count, memregion),
						CMN_region(region_sys_count, CMN_region_MASK),
						CMN_rnsam_sys_cache_grp_region(rnsam, region_sys_count),
						"CMN_rnsam_sys_cache_grp_region", region_sys_count);
				memregion = CMN_valid_region(CMN_region_target_HNF,
							    (region_base + scg_size), scg_size);
				cmn_writeq_mask(CMN_region(region_sys_count, memregion),
						CMN_region(region_sys_count, CMN_region_MASK),
						CMN_rnsam_sys_cache_grp_region(rnsam, 2),
						"CMN_rnsam_sys_cache_grp_region", 2);
			} else {
				for (region_sys_count = 0; region_sys_count < cmn_scg_count; region_sys_count++) {
					cmn_writeq_mask(CMN_region(region_sys_count, memregion),
							CMN_region(region_sys_count, CMN_region_MASK),
							CMN_rnsam_sys_cache_grp_region(rnsam, region_sys_count),
							"CMN_rnsam_sys_cache_grp_region", region_sys_count);
				}
			}
			cmn_hnf_cal_enable_scg(region_sys_count);
			break;

		case CMN600_MEMORY_REGION_TYPE_SYSCACHE_SECONDARY:
			if (region_sys2_count >= CMN_MAX_HASH_MEM_REGIONS) {
				con_err(CMN_MODNAME ": SYS count %d > limit %d\n",
					region_sys2_count, CMN_MAX_HASH_MEM_REGIONS);
				BUG();
			}

			scg_size = region->size / cmn_scg_count;
			region_base = base + (scg_size * region_sys2_count);
			memregion = CMN_valid_region(CMN_region_target_HNF,
						     region_base, scg_size);
			if (cmn_scg_count == 2) {
				cmn_writeq_mask(CMN_region(region_sys2_count, memregion),
						CMN_region(region_sys2_count, CMN_region_MASK),
						CMN_rnsam_sys_cache_grp_secondary_region(rnsam, region_sys2_count),
						"CMN_rnsam_sys_cache_grp_secondary_region", region_sys2_count);
				memregion = CMN_valid_region(CMN_region_target_HNF,
							    (region_base + scg_size), scg_size);
				cmn_writeq_mask(CMN_region(region_sys2_count, memregion),
						CMN_region(region_sys2_count, CMN_region_MASK),
						CMN_rnsam_sys_cache_grp_secondary_region(rnsam, 2),
						"CMN_rnsam_sys_cache_grp_secondary_region", 2);
			} else {
				for (region_sys2_count = 0; region_sys2_count < cmn_scg_count; region_sys2_count++) {
					cmn_writeq_mask(CMN_region(region_sys2_count, memregion),
							CMN_region(region_sys2_count, CMN_region_MASK),
							CMN_rnsam_sys_cache_grp_secondary_region(rnsam, region_sys2_count),
							"CMN_rnsam_sys_cache_grp_secondary_region", region_sys2_count);
				}
			}
			cmn_hnf_cal_enable_scg(region_sys2_count);
			break;

		case CMN600_REGION_TYPE_SYSCACHE_NONHASH:
			memregion = CMN_valid_nonhash_region(CMN_region_target_HNI,
							     region->base,
							     region->size);
			cmn_writeq_mask(CMN_region(region_sys_count, memregion),
					CMN_region(region_sys_count, CMN_region_MASK),
					CMN_rnsam_sys_cache_grp_region(rnsam, region_sys_count),
					"CMN_rnsam_sys_cache_grp_region", region_sys_count);
			cmn_writeq_mask(CMN_nodeid(region_sys_count - 1, region->node_id),
					CMN_nodeid(region_sys_count - 1, CMN_nodeid_MASK),
					CMN_rnsam_sys_cache_grp_nonhash_nodeid(rnsam,
									       region_sys_count - 1),
					"CMN_rnsam_sys_cache_grp_nonhash_nodeid",
					region_sys_count - 1);
			region_sys_count++;
			break;

		case CMN600_REGION_TYPE_SYSCACHE_SUB:
			/* Do nothing. System cache sub-regions are handled by HN-Fs */
			break;

		default:
			con_dbg("Wrong region type %d\n", region->type);
			BUG();
			break;
		}
	}
	
	hnfs_per_scg = cmn_hnf_count / cmn_scg_count;
	lid_base = 0;
	scg = 0;
#ifdef CONFIG_CMN600_SAM_RANGE_BASED
	for (lid = 0; lid < cmn_hnf_count; lid++) {
		nid = cmn_hnf_scgs[lid_base];
		cmn_writeq_mask(CMN_nodeid(lid_base, nid),
				CMN_nodeid(lid_base, CMN_nodeid_MASK),
				CMN_rnsam_sys_cache_grp_hn_nodeid(rnsam, lid_base),
				"CMN_rnsam_sys_cache_grp_hn_nodeid", lid_base);
		lid_base++;
#ifdef CONFIG_CMN600_DEBUG_CONFIGURE
		con_dbg(CMN_MODNAME ": SCG%d: %d/%d, ID: %d\n",
				scg, lid_base, cmn_hnf_count, nid);
#endif
	}
#else	
	while (lid_base < cmn_hnf_count) {
		for (lid = 0; lid < hnfs_per_scg && lid_base < cmn_hnf_count; lid++) {
			nid = cmn_hnf_scgs[lid_base];
			cmn_writeq_mask(CMN_nodeid(lid_base, nid),
					CMN_nodeid(lid_base, CMN_nodeid_MASK),
					CMN_rnsam_sys_cache_grp_hn_nodeid(rnsam, lid_base),
					"CMN_rnsam_sys_cache_grp_hn_nodeid", lid_base);
			lid_base++;
#ifdef CONFIG_CMN600_DEBUG_CONFIGURE
			if( cmn_scg_count == 2 && lid > 8)
			{
				con_dbg(CMN_MODNAME ": SCG2: %d/%d, ID: %d\n",
					lid_base, cmn_hnf_count, nid);
			} else {
				con_dbg(CMN_MODNAME ": SCG%d: %d/%d, ID: %d\n",
					scg, lid_base, cmn_hnf_count, nid);
			}
#endif
		}
		if (hnfs_per_scg < CMN_MAX_HASH_MEM_REGIONS)
			lid_base = ALIGN_UP(hnfs_per_scg, CMN_MAX_HASH_MEM_REGIONS);
		scg++;
	}
#endif
	if (cmn_scg_count == 2)
	{
		cmn_writeq_mask(CMN_scg_hnf_num(0, hnfs_per_scg),
				CMN_scg_hnf_num(0, CMN_scg_hnf_num_MASK),
				CMN_rnsam_sys_cache_group_hn_count(rnsam, 0),
				"CMN_rnsam_sys_cache_group_hn_count", 0);
		cmn_writeq_mask(CMN_scg_hnf_num(2, hnfs_per_scg),
				CMN_scg_hnf_num(2, CMN_scg_hnf_num_MASK),
				CMN_rnsam_sys_cache_group_hn_count(rnsam, 2),
				"CMN_rnsam_sys_cache_group_hn_count", 2);
	} else {
		for (hnf = 0; hnf < cmn_scg_count; hnf++) {
			cmn_writeq_mask(CMN_scg_hnf_num(hnf, hnfs_per_scg),
					CMN_scg_hnf_num(hnf, CMN_scg_hnf_num_MASK),
					CMN_rnsam_sys_cache_group_hn_count(rnsam, hnf),
					"CMN_rnsam_sys_cache_group_hn_count", hnf);
		}
	}
	cmn_hnf_cal_apply_scg(rnsam);

#ifndef CONFIG_CMN600_SAM_RANGE_BASED
	for (snf = 0; snf < cmn_snf_count; snf++) {
		cmn_writeq_mask(CMN_nodeid(snf, cmn_snf_table[snf]),
				CMN_nodeid(snf, CMN_nodeid_MASK),
				CMN_rnsam_sys_cache_grp_sn_nodeid(rnsam, snf),
				"CMN_rnsam_sys_cache_grp_sn_nodeid", snf);
	}
#endif
	cmn_writeq(CMN_sam_nstall_req(CMN_sam_unstall_req),
		   CMN_rnsam_status(rnsam),
		   "CMN_rnsam_status", -1);
}

void cmn600_configure(void)
{
	cmn_id_t i;

	BUG_ON(cmn_node_type(CMN_CFGM_BASE) != CMN_CFG);

	con_dbg(CMN_MODNAME ": configure: revision=%s\n",
		cmn600_revision_name(cmn_revision()));

	cmn_snf_count = cmn600_hw_snf_count();
	cmn_scg_count = cmn600_hw_scg_count();
	/* Setup HN-F nodes */
	for (i = 0; i < cmn_hnf_count; i++)
		cmn600_configure_hnf_sam(CMN_HNF_BASE(cmn_hnf_ids[i]), i);
	/* Setup internal RN-SAM nodes */
	for (i = 0; i < cmn_rn_sam_int_count; i++)
		cmn600_configure_rn_sam(CMN_RN_SAM_INT_BASE(cmn_rn_sam_int_ids[i]));
	con_dbg(CMN_MODNAME "Setup internal RN-SAM nodes\n");
	/* Setup external RN-SAM nodes */
	for (i = 0; i < cmn_rn_sam_ext_count; i++)
		cmn600_configure_rn_sam(CMN_RN_SAM_EXT_BASE(cmn_rn_sam_ext_ids[i]));
	con_dbg(CMN_MODNAME "Setup external RN-SAM nodes\n");
}
/*
void cmn600_disable_ocm()
{
	cmn_id_t i;

	for (i = 0; i < cmn_hnf_count; i++)
		cmn_hnf_cal_disable_ocm(CMN_HNF_BASE(cmn_hnf_ids[i]));
}

void cmn600_flush_hnfs_slc(uint64_t abf_mode,caddr_t saddr, caddr_t eaddr)
{
	cmn_id_t i;
	for(i = 0; i< cmn_hnf_count; i++)
		cmn_hnf_abf(CMN_HNF_BASE(cmn_hnf_ids[i]), abf_mode, saddr, eaddr);
}
*/
void cmn600_configure_rn_sam_ext(cmn_nid_t nid)
{
	cmn_id_t rnsam;
	caddr_t base;

	for (rnsam = 0; rnsam < cmn_rn_sam_ext_count; rnsam++) {
		base = CMN_RN_SAM_EXT_BASE(cmn_rn_sam_ext_ids[rnsam]);
		if (cmn_node_id(base) == nid)
			cmn600_configure_rn_sam(base);
	}
}

void cmn600_init(void)
{
	caddr_t root_node_pointer;

	if (cmn600_initialized)
		return;

	con_dbg(CMN_MODNAME ": cmn_mmap_count is %d.\n", cmn_mmap_count);
	cmn_debug_init();
	root_node_pointer = CMN_ROOT_NODE_POINTER(CMN_HND_NID);
	cmn_bases[CMN_CFGM_ID] = CMN_PERIPH_BASE + root_node_pointer;
	cmn_nr_nodes = 1;
	cmn600_discover();
	/* TODO: Dynamic internal/external RN_SAM nodes and HNF cache groups */
	cmn600_configure();
	/* Capture CCIX host topology */
	cmn600_cml_detect_mmap();
	cmn600_initialized = true;
}

static uint16_t cmn600_node_type(cmn_nid_t nid)
{
	cmn_id_t i;
	caddr_t base;

	for (i = 0; i < cmn_rnd_count; i++) {
		base = cmn_bases[cmn_rnd_ids[i]];
		if (nid == cmn_node_id(base))
			return cmn_node_type(base);
	}
	for (i = 0; i < cmn_cxha_count; i++) {
		base = cmn_bases[cmn_cxha_ids[i]];
		if (nid == cmn_node_id(base))
			return cmn_node_type(base);
	}
	for (i = 0; i < cmn_hnf_count; i++) {
		base = cmn_bases[cmn_hnf_ids[i]];
		if (nid == cmn_node_id(base))
			return cmn_node_type(base);
	}
	for (i = 0; i < cmn_dtc_count; i++) {
		base = cmn_bases[cmn_dtc_ids[i]];
		if (nid == cmn_node_id(base))
			return cmn_node_type(base);
	}
	for (i = 0; i < cmn_sbsx_count; i++) {
		base = cmn_bases[cmn_sbsx_ids[i]];
		if (nid == cmn_node_id(base))
			return cmn_node_type(base);
	}
	for (i = 0; i < cmn_rni_count; i++) {
		base = cmn_bases[cmn_rni_ids[i]];
		if (nid == cmn_node_id(base))
			return cmn_node_type(base);
	}
	for (i = 0; i < cmn_rn_sam_int_count; i++) {
		base = cmn_bases[cmn_rn_sam_int_ids[i]];
		if (nid == cmn_node_id(base))
			return CMN_RNF;
	}
	for (i = 0; i < cmn_rn_sam_ext_count; i++) {
		base = cmn_bases[cmn_rn_sam_ext_ids[i]];
		if (nid == cmn_node_id(cmn_rn_sam_ext_ids[i]))
			return CMN_RNF;
	}
	for (i = 0; i < cmn_snf_count; i++) {
		if (nid == cmn_snf_table[i])
			return CMN_SNF;
	}
	return CMN_INVAL;
}

static bool cmn600_rnsam_is_rnd(cmn_nid_t nid)
{
	cmn_id_t i;

	for (i = 0; i < cmn_rnd_count; i++) {
		if (nid == cmn_node_id(cmn_bases[cmn_rnd_ids[i]]))
			return true;
	}
	return false;
}

static bool cmn600_rnsam_is_rni(cmn_nid_t nid)
{
	cmn_id_t i;

	for (i = 0; i < cmn_rni_count; i++) {
		if (nid == cmn_node_id(cmn_bases[cmn_rni_ids[i]]))
			return true;
	}
	return false;
}

static bool cmn600_rnsam_is_cxha(cmn_nid_t nid)
{
	cmn_id_t i;

	for (i = 0; i < cmn_cxha_count; i++) {
		if (nid == cmn_node_id(cmn_bases[cmn_cxha_ids[i]]))
			return true;
	}
	return false;
}

static bool cmn600_rnsam_is_rnf(cmn_nid_t nid)
{
	cmn_id_t i;

	for (i = 0; i < cmn_rnd_count; i++) {
		if (nid == cmn_node_id(cmn_bases[cmn_rnd_ids[i]]))
			return false;
	}
	for (i = 0; i < cmn_rni_count; i++) {
		if (nid == cmn_node_id(cmn_bases[cmn_rni_ids[i]]))
			return false;
	}
	for (i = 0; i < cmn_cxha_count; i++) {
		if (nid == cmn_node_id(cmn_bases[cmn_cxha_ids[i]]))
			return false;
	}
	return true;
}

static int do_cmn600_dump(int argc, char *argv[])
{
	cmn_id_t x, y;
	cmn_id_t i;
	uint16_t node_type;
	cmn_nid_t nid;
	unsigned long long base;

	if (argc < 3) {
		printf("Revision: %s\n",
			cmn600_revision_name(cmn_revision()));
		for (y = 0; y < CMN_MESH_DIMEN_Y; y++) {
			for (x = 0; x < CMN_MESH_DIMEN_X; x++) {
				if (x == 0) {
					for (i = 0; i < CMN_MESH_DIMEN_X; i++) {
						printf("+--------+--------+");
						if (i == CMN_MESH_DIMEN_X - 1)
							printf("\n");
						else
							printf(" ");
					}
				}
				nid = CMN_NID(x, y, 0, 0);
				node_type = cmn600_node_type(nid);
				printf("|%03d:%-4s", nid,
				       cmn600_node_type_name(node_type));
				printf("|");
				nid = CMN_NID(x, y, 1, 0);
				node_type = cmn600_node_type(nid);
				printf("%03d:%-4s|", nid,
				       cmn600_node_type_name(node_type));
				if (x == CMN_MESH_DIMEN_X - 1) {
					printf("\n");
					for (i = 0; i < CMN_MESH_DIMEN_X; i++) {
						printf("+--------+--------+");
						if (i == CMN_MESH_DIMEN_X - 1)
							printf("\n");
						else
							printf(" ");
					}
					if (y < CMN_MESH_DIMEN_Y - 1) {
						for (i = 0; i < CMN_MESH_DIMEN_X; i++) {
							printf("         |        ");
							if (i == CMN_MESH_DIMEN_X - 1)
								printf("\n");
							else
								printf("  ");
						}
					}
				} else
					printf("-");
			}
		}
		return 0;
	} else {
		if (strcmp(argv[2], "hnf") == 0) {
			for (i = 0; i < cmn_hnf_count; i++) {
				base = cmn_bases[cmn_hnf_ids[i]];
				printf("HN-F-%d: %03d, %016llx\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			return 0;
		}
		if (strcmp(argv[2], "rnd") == 0) {
			for (i = 0; i < cmn_rnd_count; i++) {
				base = cmn_bases[cmn_rnd_ids[i]];
				printf("RN-D-%d: %03d, %016llx\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			for (i = 0; i < cmn_rn_sam_int_count; i++) {
				base = cmn_bases[cmn_rn_sam_int_ids[i]];
				if (!cmn600_rnsam_is_rnd(cmn_node_id(base)))
					continue;
				printf("RN-D-%d SAM: %03d, %016llx I\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			for (i = 0; i < cmn_rn_sam_ext_count; i++) {
				base = cmn_bases[cmn_rn_sam_ext_ids[i]];
				if (!cmn600_rnsam_is_rnd(cmn_node_id(base)))
					continue;
				printf("RN-D-%d SAM: %03d, %016llx E\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			return 0;
		}
		if (strcmp(argv[2], "rni") == 0) {
			for (i = 0; i < cmn_rni_count; i++) {
				base = cmn_bases[cmn_rni_ids[i]];
				printf("RN-I-%d: %03d, %016llx\n",
					cmn_logical_id(base), cmn_node_id(base), base);
			}
			for (i = 0; i < cmn_rn_sam_int_count; i++) {
				base = cmn_bases[cmn_rn_sam_int_ids[i]];
				if (!cmn600_rnsam_is_rni(cmn_node_id(base)))
					continue;
				printf("RN-I-%d SAM: %03d, %016llx I\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			for (i = 0; i < cmn_rn_sam_ext_count; i++) {
				base = cmn_bases[cmn_rn_sam_ext_ids[i]];
				if (!cmn600_rnsam_is_rni(cmn_node_id(base)))
					continue;
				printf("RN-I-%d SAM: %03d, %016llx E\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			return 0;
		}
		if (strcmp(argv[2], "rnf") == 0) {
			for (i = 0; i < cmn_rn_sam_int_count; i++) {
				base = cmn_bases[cmn_rn_sam_int_ids[i]];
				if (!cmn600_rnsam_is_rnf(cmn_node_id(base)))
					continue;
				printf("RN-F-%d SAM: %03d, %016llx I\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			for (i = 0; i < cmn_rn_sam_ext_count; i++) {
				base = cmn_bases[cmn_rn_sam_ext_ids[i]];
				if (!cmn600_rnsam_is_rnf(cmn_node_id(base)))
					continue;
				printf("RN-F-%d SAM: %03d, %016llx E\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			return 0;
		}
		if (strcmp(argv[2], "xp") == 0) {
			for (i = 0; i < cmn_xp_count; i++) {
				base = cmn_bases[cmn_xp_ids[i]];
				printf("XP-%d: %03d, %016llx\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			return 0;
		}
		if (strcmp(argv[2], "dtc") == 0) {
			for (i = 0; i < cmn_dtc_count; i++) {
				base = cmn_bases[cmn_dtc_ids[i]];
				printf("DTC-%d: %03d, %016llx\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			return 0;
		}
		if (strcmp(argv[2], "sbsx") == 0) {
			for (i = 0; i < cmn_sbsx_count; i++) {
				base = cmn_bases[cmn_sbsx_ids[i]];
				printf("SBSX-%d: %03d, %016llx\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			return 0;
		}
		if (strcmp(argv[2], "rnsam") == 0) {
			for (i = 0; i < cmn_rn_sam_int_count; i++) {
				base = cmn_bases[cmn_rn_sam_int_ids[i]];
				printf("RN-SAM-%d: %03d, %016llx I\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			for (i = 0; i < cmn_rn_sam_ext_count; i++) {
				base = cmn_bases[cmn_rn_sam_ext_ids[i]];
				printf("RN-SAM-%d: %03d, %016llx E\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			return 0;
		}
		if (strcmp(argv[2], "snf") == 0) {
			for (i = 0; i < cmn_snf_count; i++)
				printf("SN-F-%d: %03d\n", i, cmn_snf_table[i]);
			return 0;
		}
		if (strcmp(argv[2], "cxg") == 0) {
			for (i = 0; i < cmn_cxha_count; i++) {
				base = cmn_bases[cmn_cxha_ids[i]];
				printf("CXHA-%d: %03d, %016llx\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			for (i = 0; i < cmn_cxra_count; i++) {
				base = cmn_bases[cmn_cxra_ids[i]];
				printf("CXRA-%d: %03d, %016llx\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			for (i = 0; i < cmn_cxla_count; i++) {
				base = cmn_bases[cmn_cxla_ids[i]];
				printf("CXLA-%d: %03d, %016llx\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			for (i = 0; i < cmn_rn_sam_int_count; i++) {
				base = cmn_bases[cmn_rn_sam_int_ids[i]];
				if (!cmn600_rnsam_is_cxha(cmn_node_id(base)))
					continue;
				printf("CXHA-%d SAM: %03d, %016llx I\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			for (i = 0; i < cmn_rn_sam_ext_count; i++) {
				base = cmn_bases[cmn_rn_sam_ext_ids[i]];
				if (!cmn600_rnsam_is_cxha(cmn_node_id(base)))
					continue;
				printf("CXHA-%d SAM: %03d, %016llx E\n",
				       cmn_logical_id(base), cmn_node_id(base), base);
			}
			return 0;
		}
	}
	return -EINVAL;
}

static int do_cmn600(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0) {
		do_cmn600_dump(argc, argv);
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(cmn600, do_cmn600, "Coherent mesh network (" CMN_MODNAME ") commands",
	CMN_MODNAME " dump\n"
	"    - dump CMN mesh network\n"
	CMN_MODNAME " dump [hnf|rnd|rnf|rni|snf|xp|dtc|sbsx|rnsam|cxg]\n"
	"    - dump CMN ndoe information\n"
);
