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

#ifdef CONFIG_CONSOLE_VERBOSE
static const char * const cmn_type2name[] = {
	[CMN_INVAL] = "<Invalid>",
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
};

static const char * const cmn_cml_type2name[] = {
	[CMN_CXRA - CMN_CML] = "CXRA",
	[CMN_CXHA - CMN_CML] = "CXHA",
	[CMN_CXLA - CMN_CML] = "CXLA",

};

const char *cmn_node_type_name(uint16_t node_type)
{
	if (node_type <= CMN_RN_SAM)
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
};

const char *cmn_mem_region_name(uint8_t type)
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

const char *cmn_revision_name(uint8_t revision)
{
	if (revision >= ARRAY_SIZE(cmn_rev2name))
		return "<Unknown>";

	return cmn_rev2name[revision];
}
#endif

caddr_t cmn_bases[NR_CMN_NODES];
cmn_id_t cmn_nr_nodes;
cmn_nid_t cmn_cxra_id = CMN_INVAL_ID;
cmn_nid_t cmn_cxla_id = CMN_INVAL_ID;
cmn_nid_t cmn_cxha_id = CMN_INVAL_ID;
cmn_id_t cmn_hnf_count;
cmn_id_t cmn_rnd_count;
cmn_id_t cmn_rni_count;
cmn_id_t cmn_rnf_count;
cmn_id_t cmn_cxha_count;
cmn_id_t cmn_rn_sam_int_count;
cmn_id_t cmn_rn_sam_ext_count;
bool cmn600_initialized = false;

uint8_t cmn_hnf_ids[CMN_MAX_HNF_COUNT];
uint8_t cmn_rnd_ids[CMN_MAX_RND_COUNT];
uint8_t cmn_rni_ids[CMN_MAX_RND_COUNT];
uint8_t cmn_rn_sam_ext_ids[CMN_MAX_RN_SAM_EXT_COUNT];
uint8_t cmn_rn_sam_int_ids[CMN_MAX_RN_SAM_INT_COUNT];
uint8_t cmn_hnf_scgs[CMN_MAX_HNF_COUNT];

#define CMN600_HNF_CACHE_GROUP_ENTRIES_MAX		32
#define CMN600_HNF_CACHE_GROUP_ENTRIES_PER_GROUP	4
#define CMN600_HNF_CACHE_GROUP_ENTRY_BITS_WIDTH		12

#if 0
static int cmn_hnf_cache_group_count(void)
{
	return (cmn_hnf_count + CMN600_HNF_CACHE_GROUP_ENTRIES_PER_GROUP - 1) /
	       CMN600_HNF_CACHE_GROUP_ENTRIES_PER_GROUP;
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

static void cmn_hnf_cal_config_scg(caddr_t hnf, cmn_lid_t lid)
{
	/* If CAL mode is set, add only even numbered hnd node to
	 * cmn_rnsam_sys_cache_grp_hn_nodeid registers
	 */
	if (((cmn_node_id(hnf) % 2) == 0) && cmn_cal_supported())
		cmn_hnf_scgs[lid / cal_mode_factor] = cmn_node_id(hnf);
	else
		cmn_hnf_scgs[lid] = cmn_node_id(hnf);
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
				con_dbg(CMN_MODNAME ": SCG: %d/%d, CAL: en\n",
					region_index, CMN_MAX_SCGS);
				__raw_setq(CMN_scg_hnf_cal_mode_en(region_index),
					   CMN_rnsam_sys_cache_grp_cal_mode(rnsam));
			} else
				__raw_clearq(CMN_scg_hnf_cal_mode_en(region_index),
					     CMN_rnsam_sys_cache_grp_cal_mode(rnsam));
		}
	}
}
#else
#define cmn_hnf_cal_validate()		do { } while (0)
#define cmn_hnf_cal_process(hnf)	do { } while (0)
#define cmn_hnf_cal_enable_scg(scg)	do { } while (0)
#define cmn_hnf_cal_apply_scg(rnsam)	do { } while (0)

static void cmn_hnf_cal_config_scg(caddr_t hnf, cmn_lid_t lid)
{
	cmn_hnf_scgs[lid] = cmn_node_id(hnf);
}
#endif

static void cmn600_process_hnf(caddr_t hnf)
{
	cmn_lid_t lid;
	unsigned int region_sub_count = 0;
	unsigned int region_index;
	struct cmn600_memregion *region;
	uint64_t base;

	lid = cmn_logical_id(hnf);

	cmn_hnf_cal_process(hnf);

	BUG_ON(lid >= cmn_snf_count);

	cmn_hnf_cal_config_scg(hnf, lid);

	/* Set target node */
	__raw_writeq(cmn_snf_table[lid], CMN_hnf_sam_control(hnf));

#ifdef CONFIG_CMN600_CML
	if (cmn600_hw_chip_id() != 0)
		base = cmn600_hw_chip_addr_space() * cmn600_hw_chip_id();
	else
#endif
		base = 0;

	for (region_index = 0; region_index < cmn_mmap_count; region_index++) {
		region = &cmn_mmap_table[region_index];

		if (region->type != CMN600_REGION_TYPE_SYSCACHE_SUB)
			continue;

		__raw_writeq(CMN_sam_range_nodeid(region->node_id) |
			     CMN_sam_range_size(__ilog2_u64((region->size) / CMN_SAM_GRANU)) |
			     CMN_sam_range_base_addr((region->base + base) / CMN_SAM_GRANU) |
			     CMN_sam_range_valid,
			     CMN_hnf_sam_memregion(hnf, region_sub_count));

		con_dbg(CMN_MODNAME ": %s: HN-F SAM %d: ID: %d, [%016llx - %016llx]\n",
			cmn_mem_region_name(region->type),
			region_sub_count, region->node_id,
			region->base + base,
			region->base + base + region->size);

		region_sub_count++;
	}

	__raw_writeq(CMN_ppu_policy(CMN_ppu_policy_ON) |
		     CMN_ppu_op_mode(CMN_ppu_op_mode_FAM) |
		     CMN_ppu_dyn_en, CMN_hnf_ppu_pwpr(hnf));
}

static int cmn_max_tgt_nodes(void)
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

static void cmn600_discovery_external(caddr_t node, caddr_t xp)
{
	uint8_t dev_type;
	cmn_pid_t xp_pid;

	xp_pid = cmn_node_pid(node);
	dev_type = cmn_mxp_device_type(xp, xp_pid);
	if ((dev_type == CMN_MXP_CXRH) ||
	    (dev_type == CMN_MXP_CXHA) ||
	    (dev_type == CMN_MXP_CXRA)) {
		cmn_cxla_id = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		con_dbg(CMN_MODNAME ": CXLA external (%d, %d) ID: %d, LID: %d\n",
			(int)cmn_node_x(node), (int)cmn_node_y(node),
			(int)cmn_node_id(node), (int)cmn_logical_id(node));
	} else {
		/* External RN-SAM node */
		cmn_rn_sam_ext_count++;
		BUG_ON(cmn_rn_sam_ext_count > CMN_MAX_RN_SAM_EXT_COUNT);
		con_dbg(CMN_MODNAME ": RN_SAM external (%d, %d) ID: %d, LID: %d\n",
			(int)cmn_node_x(node), (int)cmn_node_y(node),
			(int)cmn_node_id(node), (int)cmn_logical_id(node));
	}
}

static void cmn600_discovery_internal(caddr_t node)
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
		BUG_ON(cmn_rn_sam_int_count > CMN_MAX_RN_SAM_INT_COUNT);
		cmn_rn_sam_int_count++;
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
	case CMN_CXRA:
		cmn_cxra_id = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	case CMN_CXHA:
		cmn_cxha_id = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		cmn_cxha_count++;
		break;
	default:
		/* Nothing to be done for other node types */
		break;
	}
	con_dbg(CMN_MODNAME ": %s ID: %d, LID: %d\n",
		cmn_node_type_name(cmn_node_type(node)),
		cmn_node_id(node), cmn_logical_id(node));
}

void cmn600_discovery(void)
{
	int xp_count, xp_index;
	int node_count, node_index;
	caddr_t xp, node;

	BUG_ON(cmn_node_type(CMN_CFGM_BASE) != CMN_CFG);

	con_dbg(CMN_MODNAME ": discovery:\n");
	xp_count = cmn_child_count(CMN_CFGM_BASE);
	for (xp_index = 0; xp_index < xp_count; xp_index++) {
		xp = cmn_child_node(CMN_CFGM_BASE, xp_index);

		con_dbg(CMN_MODNAME ": XP (%d, %d) ID: %d, LID: %d\n",
			(int)cmn_node_x(xp), (int)cmn_node_y(xp),
			(int)cmn_node_id(xp), (int)cmn_logical_id(xp));

		node_count = cmn_child_count(xp);
		for (node_index = 0; node_index < node_count; node_index++) {
			node = cmn_child_node(xp, node_index);

			if (cmn_child_external(xp, node_index))
				cmn600_discovery_external(node, xp);
			else
				cmn600_discovery_internal(node);
		}
	}

	/* RN-F nodes doesn't have node type identifier and hence the count
	 * cannot be determined during the discovery process. RN-F count
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

	con_dbg(CMN_MODNAME ": Total internal RN-SAM: %d\n", cmn_rn_sam_int_count);
	con_dbg(CMN_MODNAME ": Total external RN-SAM: %d\n", cmn_rn_sam_ext_count);
	con_dbg(CMN_MODNAME ": Total HN-F: %d\n", cmn_hnf_count);
	con_dbg(CMN_MODNAME ": Total RN-F: %d\n", cmn_rnf_count);
	con_dbg(CMN_MODNAME ": Total RN-D: %d\n", cmn_rnd_count);
	con_dbg(CMN_MODNAME ": Total RN-I: %d\n", cmn_rni_count);

	if (cmn_cxla_id != CMN_INVAL_ID)
		con_dbg(CMN_MODNAME ": CCIX CXLA node id %016lx\n", CMN_CXLA_BASE);
	if (cmn_cxra_id != CMN_INVAL_ID)
		con_dbg(CMN_MODNAME ": CCIX CXRA node id %016lx\n", CMN_CXRA_BASE);
	if (cmn_cxha_id != CMN_INVAL_ID)
		con_dbg(CMN_MODNAME ": CCIX CXHA node id %016lx\n", CMN_CXHA_BASE);
}

static void cmn600_configure_rn_sam_ext(caddr_t node, unsigned int xrnsam)
{
	BUG_ON(xrnsam >= cmn_rn_sam_ext_count);
	cmn_rn_sam_ext_ids[xrnsam] = cmn_nr_nodes;
	cmn_bases[cmn_nr_nodes++] = node;
}

static void cmn600_configure_rn_sam_int(caddr_t node, unsigned int irnsam)
{
	BUG_ON(irnsam >= cmn_rn_sam_int_count);
	cmn_rn_sam_int_ids[irnsam] = cmn_nr_nodes;
	cmn_bases[cmn_nr_nodes++] = node;
}

void cmn600_configure(void)
{
	unsigned int xrnsam;
	unsigned int irnsam;
	int xp_count, xp_index;
	int node_count, node_index;
	caddr_t xp, node;

	BUG_ON(cmn_node_type(CMN_CFGM_BASE) != CMN_CFG);

	xrnsam = 0;
	irnsam = 0;

	con_dbg(CMN_MODNAME ": configure: revision=%s\n",
		cmn_revision_name(cmn_revision()));

	xp_count = cmn_child_count(CMN_CFGM_BASE);
	for (xp_index = 0; xp_index < xp_count; xp_index++) {
		xp = cmn_child_node(CMN_CFGM_BASE, xp_index);
		BUG_ON(cmn_node_type(xp) != CMN_XP);

		/* Traverse nodes */
		node_count = cmn_child_count(xp);
		for (node_index = 0; node_index < node_count; node_index++) {
			node = cmn_child_node(xp, node_index);

			if (cmn_child_external(xp, node_index)) {
				cmn_pid_t xp_pid;
				uint8_t dev_type;

				xp_pid = cmn_child_node_pid(xp, node_index);
				dev_type = cmn_mxp_device_type(xp, xp_pid);
				if ((dev_type == CMN_MXP_CXRH) ||
				    (dev_type == CMN_MXP_CXHA) ||
				    (dev_type == CMN_MXP_CXRA))
					continue;

				cmn600_configure_rn_sam_ext(node, xrnsam);
				xrnsam++;
			} else {
				switch (cmn_node_type(node)) {
				case CMN_RN_SAM:
					cmn600_configure_rn_sam_int(node, irnsam);
					irnsam++;
					break;
				case CMN_HNF:
					cmn600_process_hnf(node);
					break;
				}
			}
		}
	}

	con_dbg(CMN_MODNAME ": Total nodes: %d\n", cmn_nr_nodes);
}

#ifdef CONFIG_CMN600_CML
static void cmn600_setup_cml(void)
{
	int i;

	/* Capture CCIX Host Topology */
	for (i = 0; i < cmn_mmap_count; i++) {
		if (cmn_mmap_table[i].type == CMN600_REGION_TYPE_CCIX) {
			ccix_mmap_idx = cmn_ccix_host_mmap_count;
			BUG_ON(ccix_mmap_idx >= MAX_HA_MMAP_ENTRIES);

			cmn_ccix_host_mmap[cmn_ccix_mmap_idx].base =
				cmn_mmap_table[i].base;
			cmn_ccix_host_mmap[ccix_mmap_idx].size =
				cmn_mmap_table[i].size;
			cmn_ccix_host_mmap_count++;
		}
	}
}
#else
#define cmn600_setup_cml()	do { } while (0)
#endif

#define CMN_MAX_HASH_MEM_REGIONS		4
#define CMN_MAX_NON_HASH_MEM_REGIONS_PER_GROUP	4
#define CMN_MAX_NON_HASH_MEM_REGIONS(n)		\
	((n) * CMN_MAX_NON_HASH_MEM_REGIONS_PER_GROUP)

static void cmn600_setup_sam(caddr_t rnsam)
{
	uint64_t base;
	unsigned int region_index;
	struct cmn600_memregion *region;
	cmn_lid_t lid;
	cmn_nid_t nid;
	unsigned int region_io_count = 0;
	unsigned int region_sys_count = 0;
	unsigned int region_type;
	uint32_t memregion;
	unsigned int tgt_nodes;

	tgt_nodes = cmn_max_tgt_nodes();
	BUG_ON(tgt_nodes == 0);

	for (region_index = 0; region_index < cmn_mmap_count; region_index++) {
		region = &cmn_mmap_table[region_index];
#ifdef CONFIG_CMN600_CML
		if (cmn600_hw_chip_id() != 0) {
			if (region->type == CMN_REGION_TYPE_CCIX)
				base = 0;
			else if (region->type == CMN600_MEMORY_REGION_TYPE_SYSCACHE)
				base = region->base;
			else
				base = cmn600_hw_chip_addr_space() * cmn600_hw_chip_id() +
				       region->base;
		} else
#endif
			base = region->base;

		con_dbg(CMN_MODNAME ": %s: RN SAM %d: ID: %d, [%016llx - %016llx]\n",
			cmn_mem_region_name(region->type), region_index,
			region->node_id, base, base + region->size);

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
			__raw_writeq_mask(CMN_region(region_io_count, memregion),
					  CMN_region(region_io_count, CMN_region_MASK),
					  CMN_rnsam_non_hash_mem_region(rnsam, region_io_count));

			if (region_io_count < CMN_MAX_NON_HASH_TGT_NODES_20)
				__raw_writeq_mask(CMN_nodeid(region_io_count, region->node_id),
						  CMN_nodeid(region_io_count, CMN_nodeid_MASK),
						  CMN_rnsam_non_hash_tgt_nodeid(rnsam, region_io_count));
			else
				__raw_writeq_mask(CMN_nodeid(region_io_count, region->node_id),
						  CMN_nodeid(region_io_count, CMN_nodeid_MASK),
						  CMN_rnsam_non_hash_tgt_nodeid2(rnsam, region_io_count));

			region_io_count++;
			break;

		case CMN600_MEMORY_REGION_TYPE_SYSCACHE:
			if (region_sys_count >= CMN_MAX_HASH_MEM_REGIONS) {
				con_err(CMN_MODNAME ": SYS count %d > limit %d\n",
					region_sys_count, CMN_MAX_HASH_MEM_REGIONS);
				BUG();
			}

			memregion = CMN_valid_region(CMN_region_target_HNF,
						     region->base, region->size);
			__raw_writeq_mask(CMN_region(region_sys_count, memregion),
					  CMN_region(region_sys_count, CMN_region_MASK),
					  CMN_rnsam_sys_cache_grp_region(rnsam, region_sys_count));

			cmn_hnf_cal_enable_scg(region_sys_count);
			region_sys_count++;
			break;

		case CMN600_REGION_TYPE_SYSCACHE_NONHASH:
			memregion = CMN_valid_nonhash_region(CMN_region_target_HNI,
							     region->base,
							     region->size);
			__raw_writeq_mask(CMN_region(region_sys_count, memregion),
					  CMN_region(region_sys_count, CMN_region_MASK),
					  CMN_rnsam_sys_cache_grp_region(rnsam, region_sys_count));
			__raw_writeq_mask(CMN_nodeid(region_sys_count - 1, region->node_id),
					  CMN_nodeid(region_sys_count - 1, CMN_nodeid_MASK),
					  CMN_rnsam_sys_cache_grp_nonhash_nodeid(rnsam,
										 region_sys_count - 1));
			region_sys_count++;
			break;

		case CMN600_REGION_TYPE_SYSCACHE_SUB:
			/* Do nothing. System cache sub-regions are handled by HN-Fs */
			break;

		default:
			BUG();
			break;
		}
	}

	for (lid = 0; lid < cmn_hnf_count; lid++) {
		nid = cmn_hnf_scgs[lid];
		__raw_writeq_mask(CMN_nodeid(lid, nid),
				  CMN_nodeid(lid, CMN_nodeid_MASK),
				  CMN_rnsam_sys_cache_grp_hn_nodeid(rnsam, lid));
		con_dbg(CMN_MODNAME ": SCG: %d/%d, ID: %d\n",
			lid, cmn_hnf_count, nid);
	}
	__raw_writeq(cmn_hnf_count, CMN_rnsam_sys_cache_group_hn_count(rnsam));

	cmn_hnf_cal_apply_scg(rnsam);

#ifdef CONFIG_CMN600_HNF_SAM_DIRECT
	for (i = 0; i < (cmn_snf_count/2); i++) {
		__raw_writeq(CMN_nodeid(i, cmn_snf_table[i*2]),
			     CMN_rnsam_sys_cache_grp_sn_nodeid(rnsam, i));
	}
#endif
	__raw_writeq(CMN_sam_nstall_req(CMN_sam_unstall_req),
		     CMN_rnsam_status(rnsam));
}

void cmn600_init(void)
{
	caddr_t root_node_pointer;
	unsigned int rnsam;

	if (cmn600_initialized)
		return;

	root_node_pointer = CMN_ROOT_NODE_POINTER(CMN_HND_NID);
	cmn_bases[CMN_CFGM_ID] = CMN_PERIPH_BASE + root_node_pointer;
	cmn_nr_nodes = 1;

	cmn600_discovery();
	/* TODO: Dynamic internal/external RN_SAM nodes and HNF cache groups */
	cmn600_configure();

	/* Setup internal RN-SAM nodes */
	for (rnsam = 0; rnsam < cmn_rn_sam_int_count; rnsam++)
		cmn600_setup_sam(CMN_RN_SAM_INT_BASE(cmn_rn_sam_int_ids[rnsam]));
	/* Setup CCIX host */
	cmn600_setup_cml();

	cmn600_initialized = true;
}
