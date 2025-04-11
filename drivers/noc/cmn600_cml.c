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

cmn_id_t ccix_ra_count;
cmn_id_t ccix_sa_count;

struct cmn600_ccix_ha_mmap cml_ha_mmap_table_local[CMN_MAX_HA_MMAP_COUNT];
cmn_id_t cml_ha_mmap_count_local;

uint64_t cmn600_cml_base(caddr_t base, cmn_id_t chip, bool ccix)
{
	if (chip == CMN600_CHIP_LOCAL)
		return base;
	if (ccix)
		return base;
	return (cmn600_hw_chip_base(base, chip));
}

int cmn600_cml_get_config(void)
{
	if (cmn_rn_sam_int_count == 0)
		return -ENODEV;

	ccix_ra_count = cmn_rn_sam_int_count + cmn_rn_sam_ext_count;
	ccix_sa_count = cmn_sa_count;

	return 0;
}

static bool cmn_cml_smp_enable(cmn_id_t link)
{
	if (cmn_revision() == CMN_r2p0) {
		if (!(__raw_readq(CMN_cxg_ra_unit_info(cmn600_cxra_base(link))) &
		      CMN_ra_smp_mode) ||
		    !(__raw_readq(CMN_cxg_ha_unit_info(cmn600_cxha_base(link))) &
		      CMN_ha_smp_mode))
			return true;
	} else if (cmn_revision() >= CMN_r3p0) {
		cmn_setq(CMN_ra_smp_mode_en, CMN_cxg_ra_aux_ctl(cmn600_cxra_base(link)),
			 "CMN_cxg_ra_aux_ctl", -1);
		cmn_setq(CMN_ha_smp_mode_en, CMN_cxg_ha_aux_ctl(cmn600_cxha_base(link)),
			 "CMN_cxg_ha_aux_ctl", -1);
		cmn_setq(CMN_la_smp_mode_en, CMN_cxla_aux_ctl(cmn600_cxla_base(link)),
			 "CMN_cxla_aux_ctl", -1);
		return true;
	}
	return false;
}

static void cmn_cml_setup_agentid_to_linkid(cmn_id_t agent_id, cmn_id_t link)
{
	cmn_writeq_mask(CMN_agent_linkid(agent_id, link),
			CMN_agent_linkid(agent_id,
					 CMN_agent_linkid_MASK),
			CMN_cxg_ra_agentid_to_linkid(cmn600_cxra_base(link),
						     agent_id),
			"CMN_cxg_ra_agentid_to_linkid", agent_id);
	cmn_writeq_mask(CMN_agent_linkid(agent_id, link),
			CMN_agent_linkid(agent_id,
					 CMN_agent_linkid_MASK),
			CMN_cxg_ha_agentid_to_linkid(cmn600_cxha_base(link),
						     agent_id),
			"CMN_cxg_ha_agentid_to_linkid", agent_id);
	cmn_writeq_mask(CMN_agent_linkid(agent_id, link),
			CMN_agent_linkid(agent_id,
					 CMN_agent_linkid_MASK),
			CMN_cxla_agentid_to_linkid(cmn600_cxla_base(link),
						   agent_id),
			"CMN_cxla_agentid_to_linkid", agent_id);
	cmn_setq(CMN_agent_linkid_valid(agent_id),
		 CMN_cxg_ra_agentid_to_linkid_val(cmn600_cxra_base(link)),
		 "CMN_cxg_ra_agentid_to_linkid_val", -1);
	cmn_setq(CMN_agent_linkid_valid(agent_id),
		 CMN_cxg_ha_agentid_to_linkid_val(cmn600_cxha_base(link)),
		 "CMN_cxg_ha_agentid_to_linkid_val", -1);
	cmn_setq(CMN_agent_linkid_valid(agent_id),
		 CMN_cxla_agentid_to_linkid_val(cmn600_cxla_base(link)),
		 "CMN_cxla_agentid_to_linkid_val", -1);
}

static cmn_id_t cmn_cml_local_ra_count(void)
{
	return cmn_rn_sam_int_count + cmn_rn_sam_ext_count -
		cmn_cxha_count;
}

static cmn_id_t cmn_cml_ha_id(cmn_id_t chip, cmn_id_t local_ra_count)
{
	cmn_id_t offset_id;

	offset_id = chip * local_ra_count;
	/* Since offset_id is ensured to be exclusive to any other remote
	 * RA_ID, thus it will be used as HA_ID.
	 */
	return offset_id;
}

static void cmn_cml_setup_ra_sam_addr_region(cmn_id_t link)
{
	cmn_id_t local_ra_count;
	cmn_id_t i;
	cmn_id_t chip;
	cmn_id_t offset_id;
	cmn_id_t local_chip;

	local_ra_count = cmn_cml_local_ra_count();
	local_chip = cmn600_hw_chip_id();
	for (i = 0; i < cml_ha_mmap_count_remote; i++) {
		chip = cml_ha_mmap_table_remote[i].chip_id;
		if (chip == local_chip)
			continue;

		BUG_ON(cml_ha_mmap_table_remote[i].size % SZ_64K);
		BUG_ON(cml_ha_mmap_table_remote[i].size &
		       (cml_ha_mmap_table_remote[i].size - 1));
		BUG_ON(cml_ha_mmap_table_remote[i].base %
		       cml_ha_mmap_table_remote[i].size);

		offset_id = cmn_cml_ha_id(chip, local_ra_count);
		cmn_writeq(CMN_reg_size(cml_ha_mmap_table_remote[i].size) |
			   CMN_reg_base_addr(cml_ha_mmap_table_remote[i].base) |
			   CMN_reg_ha_tgtid(offset_id) |
			   CMN_reg_valid,
			   CMN_cxg_ra_sam_addr_region(cmn600_cxra_base(link), i),
			   "CMN_cxg_ra_sam_addr_region", i);
	}
}

static bool cmn_cml_read_link_reg(cmn_id_t link, uint8_t type,
				  uint64_t cond, bool ctl, bool set)
{
	uint64_t val1, val2;

	switch (type) {
	case CMN_MXP_CXRA:
		if (ctl)
			val1 = __raw_readq(
				CMN_cxg_cxprtcl_link_ctl(cmn600_cxra_base(link),
							 link));
		else
			val1 = __raw_readq(
				CMN_cxg_cxprtcl_link_status(cmn600_cxra_base(link),
							    link));
		if (set)
			return val1 & cond;
		else
			return !(val1 & cond);
		break;

	case CMN_MXP_CXHA:
		if (ctl)
			val1 = __raw_readq(
				CMN_cxg_cxprtcl_link_ctl(cmn600_cxha_base(link),
							 link));
		else
			val1 = __raw_readq(
				CMN_cxg_cxprtcl_link_status(cmn600_cxha_base(link),
							    link));
		if (set)
			return val1 & cond;
		else
			return !(val1 & cond);
		break;

	case CMN_MXP_CXRH:
		if (ctl) {
			val1 = __raw_readq(
				CMN_cxg_cxprtcl_link_ctl(cmn600_cxra_base(link),
							 link));
			val2 = __raw_readq(
				CMN_cxg_cxprtcl_link_ctl(cmn600_cxha_base(link),
							 link));
		} else {
			val1 = __raw_readq(
				CMN_cxg_cxprtcl_link_status(cmn600_cxra_base(link),
							    link));
			val2 = __raw_readq(
				CMN_cxg_cxprtcl_link_status(cmn600_cxha_base(link),
							    link));
		}
		if (set)
			return (val1 & cond) && (val2 &cond);
		else
			return !(val1 & cond) && !(val2 & cond);
		break;

	default:
		break;
	}
	return true;
}

static void cmn_cml_wait_link_ctl(cmn_id_t link, uint8_t type,
				  uint64_t cond, bool set)
{
	while (!cmn_cml_read_link_reg(link, type, cond, true, set));
}

static void cmn_cml_wait_link_status(cmn_id_t link, uint8_t type,
				     uint64_t cond, bool set)
{
	while (!cmn_cml_read_link_reg(link, type, cond, false, set));
}

static void cmn_cml_start_ccix_link(cmn_id_t link)
{
	uint8_t max_pkt_size;

#ifdef CONFIG_CMN600_CML_TLP
	cmn_setq(CMN_la_pktheader, CMN_cxla_ccix_prop_configured(cmn600_cxla_base(link)),
		 "CMN_cxla_ccix_prop_configured", -1);
#else
	cmn_clearq(CMN_la_pktheader, CMN_cxla_ccix_prop_configured(cmn600_cxla_base(link)),
		   "CMN_cxla_ccix_prop_configured", -1);
#endif

#ifdef CONFIG_CMN600_CML_NO_MESSAGE_PACK
	cmn_setq(CMN_la_nomessagepack,
		 CMN_cxla_ccix_prop_configured(cmn600_cxla_base(link)),
		 "CMN_cxla_ccix_prop_configured", -1);
#else
	if (!cmn_ccix_no_message_pack(link))
		cmn_clearq(CMN_la_nomessagepack,
			   CMN_cxla_ccix_prop_configured(cmn600_cxla_base(link)),
			   "CMN_cxla_ccix_prop_configured", -1);
#endif

	max_pkt_size = cmn_ccix_max_packet_size(link);
	cmn_writeq_mask(CMN_la_maxpacketsize(max_pkt_size),
			CMN_la_maxpacketsize(CMN_la_maxpacketsize_MASK),
			CMN_cxla_ccix_prop_configured(cmn600_cxla_base(link)),
                        "CMN_cxla_ccix_prop_configured", -1);

	cmn_writeq_mask(CMN_link_pcie_bdf(link, cml_pcie_bus_num),
			CMN_link_pcie_bdf(link, CMN_link_pcie_bdf_MASK),
			CMN_cxla_linkid_to_pcie_bus_num(cmn600_cxla_base(link), link),
			"CMN_cxla_linkid_to_pcie_bus_num", link);
	/* Set up TC1 for PCIe so CCIx uses VC1. */
	cmn_writeq_mask(CMN_tlp_vendor(CCIX_VENDOR_ID) |
			CMN_tlp_tc(cml_pcie_tlp_tc),
			CMN_tlp_vendor(CMN_tlp_vendor_MASK) |
			CMN_tlp_tc(CMN_tlp_tc_MASK),
			CMN_cxla_tlp_hdr_fields(cmn600_cxla_base(link)),
			"CMN_cxla_tlp_hdr_fields", -1);
	con_dbg(CMN_MODNAME ": PCIe TLP header: %016llx\n",
		__raw_readq(CMN_cxla_tlp_hdr_fields(cmn600_cxla_base(link))));

	con_dbg(CMN_MODNAME ": Enabling CCIX %d...\n", link);
	cmn_writeq(CMN_lnk_link_en,
		   CMN_cxg_cxprtcl_link_ctl(cmn600_cxra_base(link), link),
		   "CMN_cxg_ra_cxprtcl_link_ctl", link);
	cmn_writeq(CMN_lnk_link_en,
		   CMN_cxg_cxprtcl_link_ctl(cmn600_cxha_base(link), link),
		   "CMN_cxg_ha_cxprtcl_link_ctl", link);
	cmn_cml_wait_link_ctl(link, CMN_MXP_CXRH,
			      CMN_lnk_link_en, true);
	cmn_cml_wait_link_ctl(link, CMN_MXP_CXRH, CMN_lnk_link_up, false);
	cmn_cml_wait_link_status(link, CMN_MXP_CXRH, CMN_lnk_link_down, true);
	cmn_cml_wait_link_status(link, CMN_MXP_CXRH, CMN_lnk_link_ack, false);

	con_dbg(CMN_MODNAME ": Requesting CCIX %d...\n", link);
	cmn_setq(CMN_lnk_link_req,
		 CMN_cxg_cxprtcl_link_ctl(cmn600_cxra_base(link), link),
		 "CMN_cxg_ra_cxprtcl_link_ctl", link);
	cmn_setq(CMN_lnk_link_req,
		 CMN_cxg_cxprtcl_link_ctl(cmn600_cxha_base(link), link),
		 "CMN_cxg_ha_cxprtcl_link_ctl", link);
	cmn_cml_wait_link_status(link, CMN_MXP_CXRH, CMN_lnk_link_ack, true);
	cmn_cml_wait_link_status(link, CMN_MXP_CXRH, CMN_lnk_link_down, false);

	con_dbg(CMN_MODNAME ": Bringing up CCIX %d...\n", link);
	cmn_setq(CMN_lnk_link_up,
		 CMN_cxg_cxprtcl_link_ctl(cmn600_cxra_base(link), link),
		 "CMN_cxg_ra_cxprtcl_link_ctl", link);
	cmn_setq(CMN_lnk_link_up,
		 CMN_cxg_cxprtcl_link_ctl(cmn600_cxha_base(link), link),
		 "CMN_cxg_ha_cxprtcl_link_ctl", link);
	cmn_cml_wait_link_ctl(link, CMN_MXP_CXRH, CMN_lnk_link_up, true);
}

static void cmn_cml_ha_config_rnf(cmn_id_t link,
				  cmn_id_t raid, cmn_id_t ldid,
				  bool rnf, bool remote,
				  cmn_id_t chip)
{
	uint8_t raid_ldid;
	uint32_t phys_id;
	unsigned int i;
	cmn_nid_t nid;

	/* Program the CXHA raid to ldid LUT. */
	raid_ldid = CMN_rnf_raid_to_ldid(ldid, rnf ? 1 : 0);
	cmn_writeq_mask(CMN_raid_ldid(raid, raid_ldid),
			CMN_raid_ldid(raid, CMN_raid_ldid_MASK),
			CMN_cxg_ha_rnf_raid_to_ldid(cmn600_cxha_base(link), raid),
			"CMN_cxg_ha_rnf_raid_to_ldid", raid);
	cmn_setq(CMN_raid_ldid_valid(raid),
		 CMN_cxg_ha_rnf_raid_to_ldid_val(cmn600_cxha_base(link)),
		 "CMN_cxg_ha_rnf_raid_to_ldid_val", -1);

	/* Program HN-F ldid to CHI node id for remote RN-F
	 * agents.
	 */
	for (i = 0; i < cmn_hnf_count; i++) {
		if (chip == cmn600_hw_chip_id())
			nid = cmn600_local_rnf_nid(ldid);
		else
			nid = cmn600_hw_ha_nid(chip);
		phys_id = CMN_valid_ra |
			  CMN_nodeid_ra(nid) |
			  (remote ? CMN_remote_ra : 0);
		cmn_writeq_mask(CMN_phys_id(ldid, phys_id),
				CMN_phys_id(ldid, CMN_phys_id_MASK),
				CMN_hnf_rn_phys_id(CMN_HNF_BASE(cmn_hnf_ids[i]), ldid),
				"CMN_hnf_rn_phys_id", ldid);
	}
}

static void cmn_cml_setup(cmn_id_t link)
{
	cmn_id_t local_ra_count;
	cmn_id_t local_chip;
	cmn_id_t rnf_ldid;
	cmn_id_t rnd_ldid;
	cmn_id_t rni_ldid;
	cmn_id_t offset_id;
	cmn_id_t agent_id;
	cmn_id_t remote_agent_id;
	cmn_id_t unique_remote_rnf_ldid;
	cmn_id_t i;
	cmn_id_t block;
	cmn_id_t raid;
	cmn_id_t rnf_count_remote;

	cmn_cml_smp_enable(link);

	/* HA_ID and RA count calculation */
	local_ra_count = cmn_cml_local_ra_count();
	local_chip = cmn600_hw_chip_id();
	offset_id = cmn_cml_ha_id(local_chip, local_ra_count);

	raid = 0;
	for (rnf_ldid = 0; rnf_ldid < cmn_rnf_count; rnf_ldid++) {
		/* TODO: use logical_id of RN-F */

		/* Determine agentid of the remote agents */
		agent_id = raid + offset_id;

		/* Program raid in CXRA LDID to RAID LUT. */
		cmn_writeq_mask(CMN_ldid_raid(rnf_ldid, agent_id),
				CMN_ldid_raid(rnf_ldid, CMN_ldid_raid_MASK),
				CMN_cxg_ra_rnf_ldid_to_raid(cmn600_cxra_base(link),
							    rnf_ldid),
				"CMN_cxg_ra_rnf_ldid_to_raid", rnf_ldid);
		cmn_setq(CMN_ldid_raid_valid(rnf_ldid),
			 CMN_cxg_ra_rnf_ldid_to_raid_val(cmn600_cxra_base(link)),
			 "CMN_cxg_ra_rnf_ldid_to_raid_val", -1);
		raid++;

		/* Program agent to linkid LUT for remote agents in
		 * CXRA/CXHA/CXLA.
		 */
		cmn_cml_setup_agentid_to_linkid(agent_id, link);

		/* The HN-F ldid to CHI node id valid bit for local RN-F
		 * agents is already programmed.
		 */
		cmn_writeq(CMN_ccix_haid(offset_id),
			   CMN_cxg_ha_id(cmn600_cxha_base(link)),
			   "CMN_cxg_ha_id", -1);

#ifdef CONFIG_CMN600_CML_HA_RAID_RNF_LOCAL
		cmn_cml_ha_config_rnf(link, agent_id, rnf_ldid, 1, 0,
				      local_chip);
#endif
	}

	/* unique_remote_rnf_ldid is used to keep track of the ldid of the
	 * remote RNF agents.
	 */
	unique_remote_rnf_ldid = cmn_rnf_count;
	rnf_count_remote = cmn_rnf_count * (CMN_MAX_CHIPS - 1);
	for (i = 0; i < rnf_count_remote; i++) {
		block = i / cmn_rnf_count;

		/* The remote_agent_id should not include the current
		 * chip's agent ids. If `block` is less than the current
		 * chip ID, then include the agent ids of chip 0 till (not
		 * including) current chip. If the block is equal or
		 * greater than the current chip, then include the agent
		 * id from next chip till the max chip.
		 */
#ifdef CONFIG_CMN600_CML_RA_COUNT_ALIGNED
		if (block < local_chip)
			remote_agent_id = (i % cmn_rnf_count) +
					  (local_ra_count * block);
		else
			remote_agent_id = (i % cmn_rnf_count) +
					  (local_ra_count * (block + 1));
#else
		if (block < local_chip)
			remote_agent_id = i + (cmn_rnf_count * block);
		else
			remote_agent_id = i + (cmn_rnf_count * block) +
					  local_ra_count;
#endif

		cmn_cml_ha_config_rnf(link, remote_agent_id,
				      unique_remote_rnf_ldid, 1, 1,
				      block < local_chip ? block : block + 1);
		unique_remote_rnf_ldid++;
	}

	for (i = 0; i < cmn_rnd_count; i++) {
		rnd_ldid = cmn_logical_id(__CMN_RND_BASE(i));

		/* Determine agentid of the remote agents */
		agent_id = raid + offset_id;

		/* Program raid values in CXRA LDID to RAID LUT */
		cmn_writeq_mask(CMN_ldid_raid(rnd_ldid, agent_id),
				CMN_ldid_raid(rnd_ldid, CMN_ldid_raid_MASK),
				CMN_cxg_ra_rnd_ldid_to_raid(cmn600_cxra_base(link),
							    rnd_ldid),
				"CMN_cxg_ra_rnd_ldid_to_raid", rnd_ldid);
		cmn_setq(CMN_ldid_raid_valid(rnd_ldid),
			 CMN_cxg_ra_rnd_ldid_to_raid_val(cmn600_cxra_base(link)),
			 "CMN_cxg_ra_rnd_ldid_to_raid_val", -1);
		raid++;

		/* Program agentid to linkid LUT for remote agents */
		cmn_cml_setup_agentid_to_linkid(agent_id, link);
	}

	for (i = 0; i < cmn_rni_count; i++) {
		rni_ldid = cmn_logical_id(__CMN_RNI_BASE(i));

		/* Determine agentid of the remote agents */
		agent_id = raid + offset_id;

		/* Program raid values in CXRA LDID to RAID LUT */
		cmn_writeq_mask(CMN_ldid_raid(rni_ldid, agent_id),
				CMN_ldid_raid(rni_ldid, CMN_ldid_raid_MASK),
				CMN_cxg_ra_rni_ldid_to_raid(cmn600_cxra_base(link),
							    rni_ldid),
				"CMN_cxg_ra_rni_ldid_to_raid", rni_ldid);
		cmn_setq(CMN_ldid_raid_valid(rni_ldid),
			 CMN_cxg_ra_rni_ldid_to_raid_val(cmn600_cxra_base(link)),
			 "CMN_cxg_ra_rni_ldid_to_raid_val", -1);
		raid++;

		/* Program agentid to linkid LUT for remote agents */
		cmn_cml_setup_agentid_to_linkid(agent_id, link);
	}

	cmn_cml_setup_ra_sam_addr_region(link);
	cmn_cml_start_ccix_link(link);
}

void cmn600_cml_set_config(cmn_id_t link)
{
	cmn_id_t region_index;
	struct cmn600_memregion *region;

	cmn_debug_enable();
	cmn_cml_setup(link);
	cmn_debug_disable();
	for (region_index = 0; region_index < cmn_mmap_count; region_index++) {
		region = &cmn_mmap_table[region_index];
		if (region->type == CMN600_REGION_TYPE_CCIX)
			cmn600_configure_rn_sam_ext(region->node_id);
	}
}

void cmn600_cml_enable_sf(cmn_id_t link)
{
	cmn_setq(CMN_lnk_snoopdomain_req,
		 CMN_cxg_cxprtcl_link_ctl(cmn600_cxha_base(link), link),
		 "CMN_cxg_ha_cxprtcl_link_ctl", link);
	cmn_cml_wait_link_status(link, CMN_MXP_CXHA,
				 CMN_lnk_snoopdomain_ack, true);
}

void cmn600_cml_enable_dvm(cmn_id_t link)
{
	cmn_setq(CMN_lnk_dvmdomain_req,
		 CMN_cxg_cxprtcl_link_ctl(cmn600_cxra_base(link), link),
		 "CMN_cxg_ra_cxprtcl_link_ctl", link);
	cmn_cml_wait_link_status(link, CMN_MXP_CXRA,
				 CMN_lnk_dvmdomain_ack, true);
}

void cmn600_cml_detect_mmap(void)
{
	unsigned int region_index;
	struct cmn600_memregion *region;

	for (region_index = 0; region_index < cmn_mmap_count; region_index++) {
		region = &cmn_mmap_table[region_index];

		if (region->type == CMN600_REGION_TYPE_CCIX) {
			cml_ha_mmap_table_local[cml_ha_mmap_count_local].base = region->base;
			cml_ha_mmap_table_local[cml_ha_mmap_count_local].size = region->size;
			cml_ha_mmap_count_local++;
		}
	}
}

void cmn600_cml_init(void)
{
	int ret;

	ret = cmn600_cml_get_config();
	if (ret < 0)
		return;
	cmn600_cml_set_config(cml_link_id);
	cmn600_cml_enable_sf(cml_link_id);
	cmn600_cml_enable_dvm(cml_link_id);
}
