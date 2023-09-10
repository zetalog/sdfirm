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
uint8_t cmn_hnf_cache_groups[CMN_MAX_HNF_COUNT];

static void cmn600_process_hnf(caddr_t hnf)
{
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
		con_log("cmn600: CXLA external (%d, %d) ID: %d, LID: %d\n",
			(int)cmn_node_x(node), (int)cmn_node_y(node),
			(int)cmn_node_id(node), (int)cmn_logical_id(node));
	} else {
		/* External RN-SAM node */
		cmn_rn_sam_ext_count++;
		BUG_ON(cmn_rn_sam_ext_count > CMN_MAX_RN_SAM_EXT_COUNT);
		con_log("cmn600: RN_SAM external (%d, %d) ID: %d, LID: %d\n",
			(int)cmn_node_x(node), (int)cmn_node_y(node),
			(int)cmn_node_id(node), (int)cmn_logical_id(node));
	}
}

static void cmn600_discovery_internal(caddr_t node)
{
	switch (cmn_node_type(node)) {
	case CMN_HNF:
		if (cmn_hnf_count >= CMN_MAX_HNF_COUNT) {
			con_err("cmn600: HN-F count %d >= limit %d\n",
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
			con_err("cmn600: RN-D count %d >= limit %d\n",
				cmn_rnd_count, CMN_MAX_RND_COUNT);
			BUG();
		}
		cmn_rnd_ids[cmn_rnd_count++] = cmn_nr_nodes;
		cmn_bases[cmn_nr_nodes++] = node;
		break;
	case CMN_RNI:
		if (cmn_rni_count >= CMN_MAX_RNI_COUNT) {
			con_err("cmn600: RN-I count %d >= limit %d\n",
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
	con_dbg("cmn600: %s ID: %d, LID: %d\n",
		cmn_node_type_name(cmn_node_type(node)),
		cmn_node_id(node), cmn_logical_id(node));
}

void cmn600_discovery(void)
{
	int xp_count, xp_index;
	int node_count, node_index;
	caddr_t xp, node;

	BUG_ON(cmn_node_type(CMN_CFGM_BASE) != CMN_CFG);

	con_dbg("cmn600: discovery:\n");
	xp_count = cmn_child_count(CMN_CFGM_BASE);
	for (xp_index = 0; xp_index < xp_count; xp_index++) {
		xp = cmn_child_node(CMN_CFGM_BASE, xp_index);

		con_dbg("cmn600: XP (%d, %d) ID: %d, LID: %d\n",
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
	if (cmn_rnf_count >= CMN_MAX_RNF_COUNT) {
		con_err("cmn600: RN-F count %d >= limit %d\n",
			cmn_rnf_count, CMN_MAX_RNF_COUNT);
		BUG();
	}
#ifdef CONFIG_CMN600_CAL
	if ((cmn_hnf_count % 2) != 0) {
		con_err("cmn600: HN-F count %d should be even for cal mode\n";
			cmn_hnf_count);
		BUG();
	}
#endif

	con_dbg("cmn600: Total internal RN-SAM: %d\n", cmn_rn_sam_int_count);
	con_dbg("cmn600: Total external RN-SAM: %d\n", cmn_rn_sam_ext_count);
	con_dbg("cmn600: Total HN-F: %d\n", cmn_hnf_count);
	con_dbg("cmn600: Total RN-F: %d\n", cmn_rnf_count);
	con_dbg("cmn600: Total RN-D: %d\n", cmn_rnd_count);
	con_dbg("cmn600: Total RN-I: %d\n", cmn_rni_count);

	if (cmn_cxla_id != CMN_INVAL_ID)
		con_dbg("cmn600: CCIX CXLA node id %016lx\n", CMN_CXLA_BASE);
	if (cmn_cxra_id != CMN_INVAL_ID)
		con_dbg("cmn600: CCIX CXRA node id %016lx\n", CMN_CXRA_BASE);
	if (cmn_cxha_id != CMN_INVAL_ID)
		con_dbg("cmn600: CCIX CXHA node id %016lx\n", CMN_CXHA_BASE);
}

static void cmn600_configure_rn_sam_ext(caddr_t xp, unsigned int xrnsam)
{
	BUG_ON(xrnsam >= cmn_rn_sam_ext_count);
	cmn_rn_sam_ext_ids[xrnsam] = cmn_nr_nodes;
	cmn_bases[cmn_nr_nodes++] = xp;
}

static void cmn600_configure_rn_sam_int(caddr_t xp, unsigned int irnsam)
{
	BUG_ON(irnsam >= cmn_rn_sam_int_count);
	cmn_rn_sam_int_ids[irnsam] = cmn_nr_nodes;
	cmn_bases[cmn_nr_nodes++] = xp;
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

	con_log("cmn600: configure:\n");
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

				cmn600_configure_rn_sam_ext(xp, xrnsam);
				xrnsam++;
			} else {
				switch (cmn_node_type(node)) {
				case CMN_RN_SAM:
					cmn600_configure_rn_sam_int(xp, irnsam);
					irnsam++;
					break;
				case CMN_HNF:
					cmn600_process_hnf(node);
					break;
				}
			}
		}
	}
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

static void cmn600_setup_sam(caddr_t rnsam)
{
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
		cmn600_setup_sam(CMN_RN_SAM_INT_BASE(rnsam));
	/* Setup CCIX host */
	cmn600_setup_cml();

	cmn600_initialized = true;
}
