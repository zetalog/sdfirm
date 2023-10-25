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
cmn_id_t ccix_mmap_count;
cmn_id_t ccix_raid_value;
struct cmn600_ccix_ha_mmap ccix_mmap[CMN_MAX_HA_MMAP_COUNT];

uint64_t cmn600_cml_base(void)
{
	if (cmn600_hw_chip_id() != 0)
		return cmn600_hw_chip_base() * cmn600_hw_chip_id();
	else
		return 0;
}

static void cmn_cml_get_caps(void)
{
}

int cmn600_cml_get_config(void)
{
	if (cmn_rn_sam_int_count == 0)
		return -ENODEV;

	ccix_ra_count = cmn_rn_sam_int_count + cmn_rn_sam_ext_count;
	ccix_sa_count = cmn_sa_count;
	cmn_cml_get_caps();

	return 0;
}

static bool cmn_cml_smp_enable(void)
{
	if (cmn_revision() == CMN_r2p0) {
		if (!(__raw_readq(CMN_cxg_ra_unit_info(CMN_CXRA_BASE)) &
		      CMN_ra_smp_mode) ||
		    !(__raw_readq(CMN_cxg_ra_unit_info(CMN_CXHA_BASE)) &
		      CMN_ha_smp_mode))
			return true;
	} else if (cmn_revision() >= CMN_r3p0) {
		cmn_setq(CMN_ra_smp_mode_en, CMN_cxg_ra_aux_ctl(CMN_CXRA_BASE));
		cmn_setq(CMN_ha_smp_mode_en, CMN_cxg_ra_aux_ctl(CMN_CXHA_BASE));
		cmn_setq(CMN_la_smp_mode_en, CMN_cxla_aux_ctl(CMN_CXLA_BASE));
		return true;
	}
	return false;
}

static void cmn_cml_setup(void)
{
	cmn_id_t local_ra_count;
	cmn_id_t rnf_ldid;
	cmn_id_t offset_id;
	cmn_id_t agent_id;
	cmn_id_t remote_agent_id;
	cmn_id_t unique_remote_rnf_ldid;
	unsigned int i;
	unsigned int block;
	uint8_t raid_ldid;

	local_ra_count = cmn_rn_sam_int_count + cmn_rn_sam_ext_count;
	cmn_cml_smp_enable();

	ccix_raid_value = 0;
	offset_id = cmn600_hw_chip_id() * local_ra_count;

	for (rnf_ldid = 0; rnf_ldid < cmn_rnf_count; rnf_ldid++) {
		agent_id = ccix_raid_value + offset_id;

		/* Program raid in CXRA LDID to RAID LUT. */
		cmn_writeq_mask(CMN_ldid_raid(rnf_ldid, agent_id),
				CMN_ldid_raid(rnf_ldid, CMN_ldid_raid_MASK),
				CMN_cxg_ra_rnf_ldid_to_raid(CMN_CXRA_BASE,
							    rnf_ldid));
		cmn_setq(CMN_ldid_raid_valid(rnf_ldid),
			 CMN_cxg_ra_rnf_ldid_to_raid_val(CMN_CXRA_BASE));
		ccix_raid_value++;

		/* Program agent to linkid LUT for remote agents in
		 * CXRA/CXHA/CXLA.
		 */
		cmn_writeq_mask(CMN_agent_linkid(agent_id, cml_link_id),
				CMN_agent_linkid(agent_id,
						 CMN_agent_linkid_MASK),
				CMN_cxg_ra_agentid_to_linkid(CMN_CXRA_BASE,
							     agent_id));
		cmn_writeq_mask(CMN_agent_linkid(agent_id, cml_link_id),
				CMN_agent_linkid(agent_id,
						 CMN_agent_linkid_MASK),
				CMN_cxg_ha_agentid_to_linkid(CMN_CXHA_BASE,
							     agent_id));
		cmn_writeq_mask(CMN_agent_linkid(agent_id, cml_link_id),
				CMN_agent_linkid(agent_id,
						 CMN_agent_linkid_MASK),
				CMN_cxla_agentid_to_linkid(CMN_CXLA_BASE,
							   agent_id));
		cmn_setq(CMN_agent_linkid_valid(agent_id),
			 CMN_cxg_ra_agentid_to_linkid_val(CMN_CXRA_BASE));
		cmn_setq(CMN_agent_linkid_valid(agent_id),
			 CMN_cxg_ha_agentid_to_linkid_val(CMN_CXHA_BASE));
		cmn_setq(CMN_agent_linkid_valid(agent_id),
			 CMN_cxla_agentid_to_linkid_val(CMN_CXLA_BASE));

		/* The HN-F ldid to CHI node id valid bit for local RN-F
		 * agents is already programmed.
		 */
#if 0
		cxg_ha_id = cmn600_hw_chip_id();
		cxg_ha_node_id = 0x0;
		cxg_ha_id_remote = cml_remote_ha_mmap[0].ha_id;
#endif
		cmn_writeq(CMN_ccix_haid(cmn600_hw_chip_id()),
			   CMN_cxg_ha_id(CMN_CXHA_BASE));
	}

	/* unique_remote_rnf_ldid is used to keep track of the ldid of the
	 * remote RNF agents.
	 */
	unique_remote_rnf_ldid = cmn_rnf_count;

	for (i = 0; i < cml_remote_rnf_count; i++) {
		block = i / cmn_rnf_count;

		/* The remote_agent_id should not include the current
		 * chip's agent ids. If `block` is less than the current
		 * chip_id, then include the agent ids of chip 0 till (not
		 * including) current chip. If the block is equal or
		 * greater than the current chip, then include the agent
		 * id from next chip till the max chip.
		 */
		if (block < cmn600_hw_chip_id())
			remote_agent_id = i + (cmn_rnf_count * block);
		else
			remote_agent_id = i + (cmn_rnf_count * block) +
					  local_ra_count;

		/* Program the CXHA raid to ldid LUT. */
		raid_ldid = CMN_rnf_raid_to_ldid(unique_remote_rnf_ldid, 1);
		cmn_writeq_mask(CMN_raid_ldid(remote_agent_id, raid_ldid),
				CMN_raid_ldid(remote_agent_id,
					      CMN_raid_ldid_MASK),
				CMN_cxg_ha_rnf_raid_to_ldid(CMN_CXHA_BASE,
							    remote_agent_id));
		cmn_setq(CMN_raid_ldid_valid(raid_ldid),
			 CMN_cxg_ha_rnf_raid_to_ldid_val(CMN_CXHA_BASE));

		/* Program HN-F ldid to CHI node id for remote RN-F
		 * agents.
		 */

		unique_remote_rnf_ldid++;
	}

	offset_id = cmn600_hw_chip_id() * local_ra_count;

	for (i = 0; i < cmn_rnd_count; i++) {
	}

	for (i = 0; i < cmn_rni_count; i++) {
	}
}

void cmn600_cml_set_config(void)
{
	cmn_id_t region_index;
	struct cmn600_memregion *region;

	cmn_cml_setup();
	for (region_index = 0; region_index < cmn_mmap_count; region_index++) {
		region = &cmn_mmap_table[region_index];
		if (region->type == CMN600_REGION_TYPE_CCIX)
			cmn600_setup_rnsam(region->node_id);
	}
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
	int ret;

	ret = cmn600_cml_get_config();
	if (ret < 0)
		return;
	cmn600_cml_set_config();
}
