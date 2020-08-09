#include <target/arch.h>
#include <target/byte_list.h>

#ifndef __BAREMETAL__
//#include <asm/mach/FabricStatusCode.h>
#else
//#include "fabric_status_code_bm.h"
#endif
#ifndef CONFIG_SBL
#define __ddrcode
#define __ddrconst
#define fabric_report_status_code(a, b, c)
#endif

/* This returns the maximum number of DDR per interleave. */
#define MAX_DDR_INTLV	(DDR_MAX_NUM_CHANS/NUM_INTERLEAVES)

/******************************************************************************
 * Storage buffer to hold the lists of ID used for the master lists as part of
 * byte_list_t
 * Notes:
 *   These lists/flags are available throughout the context of this ring
 *   component and used directly in the specific switch configuration
 *   functions.
 ******************************************************************************/
static uint8_t store_apc_pid[MAX_CLUSTERS];
static uint8_t store_l3u_sid[NUM_INTERLEAVES][MAX_L3_NODES];
static uint8_t store_ddr_sid[NUM_INTERLEAVES][MAX_DDR_INTLV];
static uint8_t store_ddr_cid[NUM_INTERLEAVES][MAX_DDR_INTLV];

/******************************************************************************
 * Lists of ID extracted from the master lists to provide specific subsets of
 * IDs pertaining to specific hardware subcomponent / register configurations.
 * Notes:
 *   These lists/flags are available throughout the context of this ring
 *   component and used directly in the specific switch configuration
 *   functions.
 ******************************************************************************/
static byte_list_t list_apc_pid;
static byte_list_t list_l3u_sid[NUM_INTERLEAVES];
static byte_list_t list_ddr_sid[NUM_INTERLEAVES];
static byte_list_t list_ddr_cid[NUM_INTERLEAVES];

rs_mask_su_t rs_active_mask;

/* Sets of flags used to indicate / track the presence of specific CPU
 * nodes within MBSU and DMSU.
 */
static uint32_t flag_usable_apc[NUM_APC_TYPE][NUM_FCN_TYPE];

/* Sets of flags used to indicate / track the usability of specific CPU
 * nodes within MBSU and DMSU.
 * Set of flag used to indicate / track the usability of specific PBSU
 * nodes on Monaco ring, this is the debugging purpose.
 */
static uint32_t flag_usable_pbsu;

/* This builds the register content for the Secondary Backoff Request. */
static uint32_t rs_get_sbrce(rs_sbrlvl_e rs_sbrlvl, uint32_t rs_sbrmsk)
{
	uint32_t llblvl, maskllblvl;

	if (rs_sbrlvl > RS_SBRLVL_MAX)
		rs_sbrlvl = RS_SBRLVL_15;

	if (RS_SBRLVL_8 <= rs_sbrlvl)
		maskllblvl = 0x08;  // b1000
	else if (RS_SBRLVL_4 <= rs_sbrlvl)
		maskllblvl = 0x0C;  // b1100
	else if (RS_SBRLVL_2 <= rs_sbrlvl)
		maskllblvl = 0x0E;  // b1110
	else if (RS_SBRLVL_1 == rs_sbrlvl)
		maskllblvl = 0x0F;  // b1111
	else /* (RS_SBRLVL_0 == rs_sbrlvl) */ {
		/* b0000, Don't enter llMode but stay in regularMode */
		maskllblvl = 0x00;
	}

	llblvl = (rs_sbrlvl - 1) - (~maskllblvl & 0xF);
	llblvl &= 0x07;
	rs_sbrmsk &= 0x3F;  /* 6 bit of mask */

	/* [31:26]=LFSR_BIT_MASK, [25:22]=LLB_LVL, [21:18]=MASK_LLB_LVL */
	return SU_LFSR_BIT_MASK(rs_sbrmsk) | SU_LFSR_LLB_LVL(llblvl) |
	       SU_LFSR_MASK_LLB_LVL(maskllblvl);
}

#if !defined(CONFIG_TESTOS_EMU) && defined(CONFIG_QCOM_DDRSS)
#define my_spd_read_dimm_spd_bytes_total(chan, slot)	\
	spd_read_dimm_spd_bytes_total(chan, slot)
#else
#ifdef CONFIG_MONACO_USE_PSEUDO_DDR_CHAN_MASK
#define PSEUDO_DDR_CHAN_MASK		CONFIG_MONACO_DDR_CHAN_MASK
#else
#if defined(CONFIG_PRE_S1) || defined(CONFIG_PRE_HAPS)
#define PSEUDO_DDR_CHAN_MASK		(_BV(1) | _BV(4))
#else
#define PSEUDO_DDR_CHAN_MASK		DDR_CHAN_MASK
#endif
#endif

static unsigned int my_spd_read_dimm_spd_bytes_total(uint8_t chan_num,
						     uint8_t slot_num)
{
	int ddr_chan;

	for (ddr_chan = 0; ddr_chan < DDR_MAX_NUM_CHANS; ++ddr_chan) {
		if (PSEUDO_DDR_CHAN_MASK & _BV(ddr_chan))
			return 512;
		else
			return 0;
	}
	return 0;
}
#endif

static uint32_t rs_get_usable_ddrss(uint32_t ddr_max_num_chans)
{
	uint32_t mask = 0;
	uint32_t ddr_chan, dimm_slot;
	bool found_dimm_in_chan;

	/* Go through each DIMM SPD and get the associated memory size and total
	 * it up.
	 */
	for (ddr_chan = 0; ddr_chan < ddr_max_num_chans; ++ddr_chan) {
		found_dimm_in_chan = false;
		for (dimm_slot = 0;
		     dimm_slot < DDR_MAX_NUM_SLOTS_PER_CHAN; ++dimm_slot) {
			/* Checking that we can get the Serial EEPROM size is enough to
			 * determine if the slot is populated.
			 */
			if (my_spd_read_dimm_spd_bytes_total(ddr_chan, dimm_slot) > 0)
				found_dimm_in_chan = true;
		}
		/* Include the channel if it has any memory. */
		if (found_dimm_in_chan)
			mask |= 1 << ddr_chan;
	}
	return mask;
}

/* MACH layer? */
bool rs_check_usable_pbsu(uint32_t pbsu_id)
{
	return !!(flag_usable_pbsu & (1 << (pbsu_id)));
}

static void rs_build_flag_platform(target_mode_e mode)
{
	/* Platform dependent settings for valid PBSUs */
	if ((mode == RUMI42) || (mode == RUMI48))
		flag_usable_pbsu = MONACO_HW_RUMI_PBSU_MASK;
	else if ((mode == PALLADIUM) || (mode == PALLADIUM48))
		flag_usable_pbsu = MONACO_HW_PALLADIUM_PBSU_MASK;
	else if ((mode == SILICON) || (mode == PALLADIUM_Z1))
		flag_usable_pbsu = MONACO_HW_SILICON_PBSU_MASK;
#ifdef CONFIG_PRE_HAPS
		flag_usable_pbsu = 0x1E;
#endif
	rs_active_mask.rs_mask_pbsu = flag_usable_pbsu;
}

static void rs_build_list_l3u_sid(l3intlv_t l3_interleaves)
{
	uint32_t l3_node, num_l3_nodes;
	uint32_t intlv;
	uint8_t  sid;
	uint32_t flag_usable_l3s = 0;

	/* MAX_L3_NODES */
	num_l3_nodes = sizeof(su_hw_l3u_ids);
	bl_init_list(&list_l3u_sid[0], &store_l3u_sid[0][0],
		     sizeof(store_l3u_sid[0]));
	bl_init_list(&list_l3u_sid[1], &store_l3u_sid[1][0],
		     sizeof(store_l3u_sid[1]));
	for (l3_node = 0 ; l3_node < num_l3_nodes; ++l3_node) {
		/* Get the switch ID for the current L3 */
		sid = su_hw_l3u_ids[l3_node];
		for (intlv = 0;
		     intlv < NUM_L3_INTERLEAVES_PER_NODE;
		     ++intlv) {
			if (l3_interleaves & 0x1) {
				bl_add_to_list(&list_l3u_sid[intlv], sid);
				flag_usable_l3s |= (1 << l3_node);
			}
			l3_interleaves = l3_interleaves >> 1;
		}
	}
	/* Update the mask of the active MBSU containing the valid L3 */
	rs_active_mask.rs_mask_mbsu |= flag_usable_l3s;
}

static void rs_build_list_apc_pid(cpucore_t hydra_cores)
{
	uint32_t apc_node, num_apc_node;
	uint8_t  pid;

	/* MAX_CLUSTERS */
	num_apc_node = sizeof(su_hw_apc_ids);
	bl_init_list(&list_apc_pid, &store_apc_pid[0],
		     sizeof(store_apc_pid));
	for (apc_node = 0; apc_node < num_apc_node; ++apc_node) {
		pid = su_hw_apc_ids[apc_node];
		/* Determine if there is a good core in the apc_no. */
		if (hydra_cores & 0x3)
			bl_add_to_list(&list_apc_pid, pid);
		hydra_cores = hydra_cores >> 2;
	}
}

/* This function builds a list of the switch IDs of populated DDR channels
 * for each Monaco interleave. The list is based on SPD information
 * extracted from the DIMMs.
 */
static void rs_build_list_ddr_sid(void)
{
	uint32_t ddr_max_num_chans;
	uint32_t chnmsk, chnidx;
	uint32_t flag_usable_mcsu = 0;

	/* Get a copy of the master list£¬ DDR_MAX_NUM_CHANS */
	ddr_max_num_chans = sizeof(su_hw_ddr_ids);
	/* Make sure the lists are empty for switch id. */
	bl_init_list(&list_ddr_sid[0], &store_ddr_sid[0][0],
		     sizeof(store_ddr_sid[0]));
	bl_init_list(&list_ddr_sid[1], &store_ddr_sid[1][0],
		     sizeof(store_ddr_sid[1]));
	/* Make sure the lists are empty for physical channel id. */
	bl_init_list(&list_ddr_cid[0], &store_ddr_cid[0][0],
		     sizeof(store_ddr_cid[0]));
	bl_init_list(&list_ddr_cid[1], &store_ddr_cid[1][0],
		     sizeof(store_ddr_cid[1]));

	chnmsk = rs_get_usable_ddrss(ddr_max_num_chans);

	/* Build the list of switch id of the active logical channel */
	for (chnidx = 0; chnidx < ddr_max_num_chans; ++chnidx) {
		if (chnmsk & (1 << chnidx)) {
			bl_add_to_list(&list_ddr_sid[chnidx % 2],
				       su_hw_ddr_ids[chnidx]);
			bl_add_to_list(&list_ddr_cid[chnidx % 2],
				       su_hw_ddrss_insts[chnidx]);
			flag_usable_mcsu |= (1 << su_hw_mcsu_insts[chnidx]);
		}
	}
	/* Update the mask of the active MCSU containing the working MDDR */
	rs_active_mask.rs_mask_mcsu = flag_usable_mcsu;

	/* Print the DDR pair info */
	ddr_max_num_chans = bl_get_num_entries(&list_ddr_cid[0]) +
			    bl_get_num_entries(&list_ddr_cid[1]);
	monaco_log(CRITICAL,
		   "| Total %u DDR channels are populated|\n",
		   ddr_max_num_chans);

	for (chnidx = 0; chnidx < (ddr_max_num_chans/2); ++chnidx) {
		monaco_log(CRITICAL,
			   "| DDR Pair%u : DDR CH%u on even and DDR CH%u on odd intlv |\n",
			   chnidx,
			   bl_get_next_entry(&list_ddr_cid[0]),
			   bl_get_next_entry(&list_ddr_cid[1]));
	}
	bl_scan_reset(&list_ddr_cid[0]);
	bl_scan_reset(&list_ddr_cid[1]);
}

/******************************************************************************
 * Description:
 *   This function extracts the set of usable CPU apc_node flags for a given
 *   owner switch type given a set of usable core flags. The resulting flags
 *   are a compressed form of the original flags containing only the flags
 *   for clusters belonging to the indicated switch type. The result will be
 *   saved in the global array of flag_usable_apc[][].
 * Notes:
 *   Generally useful for taking core level usability information (based on
 *   partial goods) and determine which nodes (switches) of a specific type
 *   did not yield any usable CPU clusters. I.e This can be used to compress
 *   CPU partial goods down to the apc_no level for a given switch type to
 *   determine which switches of the type have no usable clusters.
 ******************************************************************************/
static void rs_build_flag_apc(cpucore_t usable_core_flags)
{
	uint32_t apc_node, num_apc_node;
	uint32_t usable_apc_flag;
	uint8_t composite_flag, su_type, inst_id, fcn_idx;

	/* MAX_CLUSTERS */
	num_apc_node = sizeof(su_hw_switch_apc_map);
	usable_apc_flag = 0;

	if (num_apc_node > (sizeof(apc_node)*8))
		monaco_log(CRITICAL,
			   "ERROR: Number of apc_node is bigger than the size of apc_node");

	/* Build the usable apc_no flags for all switches that connect to CPU
	 * clusters.
	 * Indicates which apc_no are usable. I.e. have at least one good intlv.
	 * Ordered by apc_node number.
	 */
	for (apc_node = 0; apc_node < num_apc_node; ++apc_node) {
		/* Determine if there is a good unit in the node. */
		if (usable_core_flags & 0x3)
			usable_apc_flag |= (0x1 << apc_node);
		usable_core_flags = usable_core_flags >> 2;
	}

	for (apc_node = 0; apc_node < num_apc_node; apc_node++) {
		/* Get the owner switch type key from the map. */
		composite_flag = su_hw_switch_apc_map[apc_node];
		su_type = MSTR_SW_GET_TYPE(composite_flag);
		inst_id = MSTR_SW_GET_INST(composite_flag);
		fcn_idx = MSTR_SW_GET_FIDX(composite_flag);
		/* If the apc_node is usable, reflect it in the flags for the given
		 * switch type.
		 */
		if (usable_apc_flag & 0x1)
			flag_usable_apc[su_type][fcn_idx] |= (0x1 << inst_id);
		usable_apc_flag = usable_apc_flag >> 0x1;
	}
	/* Update the mask of the active MBSU and DMSU containing the valid CPU
	 * cores.
	 */
	rs_active_mask.rs_mask_mbsu |= flag_usable_apc[MBSU_CH][0];
	rs_active_mask.rs_mask_dmsu |= flag_usable_apc[DMSU_CH][0];
	rs_active_mask.rs_mask_dmsu |= flag_usable_apc[DMSU_CH][1];
}

/* This function fills in rs_goods structure with the partila goods info. */
static void rs_build_goods_info(rs_goods_t *rs_goods_ptr)
{
	rs_goods_ptr->apc_cnt  = (uint8_t)bl_get_num_entries(&list_apc_pid);
	rs_goods_ptr->ddr_cnt0 = (uint8_t)bl_get_num_entries(&list_ddr_sid[0]);
	rs_goods_ptr->ddr_cnt1 = (uint8_t)bl_get_num_entries(&list_ddr_sid[1]);
	rs_goods_ptr->l3u_cnt0 = (uint8_t)bl_get_num_entries(&list_l3u_sid[0]);
	rs_goods_ptr->l3u_cnt1 = (uint8_t)bl_get_num_entries(&list_l3u_sid[1]);
}

/* This function determines if the partial goods in the systems are valid
 * with the requested setting. If the partial goods in the sysatem are
 * valid but the requested ddr granule is not suitable, this will switch
 * the requested granule size to a valid default setting in 256B.
 * When the partial goods are invalid, this will return an error in order
 * to ask the system to stop booting.
 */
uint32_t rs_check_constraint(rs_goods_t *rs_goods_ptr,
			     mddr_settings_t *mddr_settings)
{
	mddr_ddr_intlv_t granule;
	uint8_t apc_cnt;
	uint8_t ddr_cnt0, ddr_cnt1;
	uint8_t l3u_cnt0, l3u_cnt1;

	granule = mddr_settings->ddr_granule;

	apc_cnt  = rs_goods_ptr->apc_cnt;
	ddr_cnt0 = rs_goods_ptr->ddr_cnt0;
	ddr_cnt1 = rs_goods_ptr->ddr_cnt1;
	l3u_cnt0 = rs_goods_ptr->l3u_cnt0;
	l3u_cnt1 = rs_goods_ptr->l3u_cnt1;

	/* Check DDR granule along with the detected number of DDR channel. */
	if (ddr_cnt0 != ddr_cnt1) {
		monaco_log(CRITICAL,
			   "    #####################################################################\n");
		monaco_log(CRITICAL,
			   "     ERROR: DDR channel counts mismatch across interleaves!!!\n"            );
		monaco_log(CRITICAL,
			   "    #####################################################################\n");
		return -EFAULT;
	}
	/* Ensure the DDR Granule setting is valid. */
	if ((granule != DDR_DIST_GRANULE_256B) &&
	    (granule != DDR_DIST_GRANULE_4KB)  &&
	    (granule != DDR_DIST_GRANULE_8KB)) {
		monaco_log(LOW ,
			   "    #####################################################################\n");
		monaco_log(CRITICAL,
			   "     WARNING: Invalid DDR granule granule requested!!\n"                    );
		monaco_log(CRITICAL,
			   "       Default 256B granule being used for Monaco Configuration!!!\n"       );
		monaco_log(LOW ,
			   "    #####################################################################\n");
		granule = DDR_DIST_GRANULE_256B;
	}
	mddr_settings->ddr_granule = granule;

	/* Check the number of L3 interleaves along with the number of APC
	 * nodes.
	 */

	/* Number of L3 instance is supposed to be non-zero */
	if (!l3u_cnt0 || !l3u_cnt1) {
		monaco_log(LOW,
			   "    #####################################################################\n");
		monaco_log(CRITICAL,
			   "     ERROR: Invalid L3 count!!! Cannot be 0!!\n");
		monaco_log(CRITICAL,
			   "        Even = %d  Odd = %d!!!\n", (int)l3u_cnt0, (int)l3u_cnt1);
		monaco_log(LOW,
			   "    #####################################################################\n");
		return -EINVAL;
	}
	/* L3 affinity is supported in any partial good mode */

	/* Validate the ratio of L3s to CPU clusters */
	if ((l3u_cnt0 == 1 && apc_cnt > MAX_APC_CNT_L3_1) ||
	    (l3u_cnt0 < 4 && apc_cnt > MAX_APC_CNT_L3_23)) {
		monaco_log(LOW,
			   "    #########################################################################\n");
		monaco_log(CRITICAL,
			   "     ERROR: The number of L3s cannot support the number of CPU Clusters!!!   \n");
		monaco_log(CRITICAL,
			   "            Min L3 on interleave %d vs %d Clusters!!!\n", (int)l3u_cnt0, (int)apc_cnt);
		monaco_log(LOW,
			   "    #########################################################################\n");
		return -EINVAL;
	}
	return 0;
}

/* This function determines the register value that enables ROUTING
 * control in each switch.
 */
static void rs_config_route(uint64_t base, rs_type_e su_type,
			    rs_settings_t *settings)
{
	uint8_t swid;
	uint32_t fdr[TRSU_CH+1] = {
		/* FDR bit position within SU_CONFIGURATION_REG */
		[MBSU_CH] = MBSU_FDR,
		[DMSU_CH] = DMSU_FDR,
		[PBSU_CH] = PBSU_FDR,
		[MCSU_CH] = MCSU_FDR,
		[TRSU_CH] = TRSU_FDR,
	};

	if (!settings->rs_eroute_ena)
		return;

        /* SWID manipulation to make the index linear in 0 ~ 31 */
	swid = su_get_linear_sid(
		SU_HW_ID_INSTANCE(__raw_readl(SU_HW_ID(base))));
	if (swid < RS_TOTAL_NUM) {
		__raw_writel(su_hw_erouting[swid][0],
			     SU_BLU_ROUTING_REG_0(base));
		if (su_type != TRSU_CH)
			__raw_writel(su_hw_erouting[swid][1],
				     SU_BLU_ROUTING_REG_1(base));
		__raw_setl(fdr[su_type], SU_CONFIGURATION_REG(base));
	}
}

/* This function determines the register value that enables LFSR
 * (Linear Feedback Shift Registers) control in each switch.
 */
static void rs_config_lfsr(uint64_t base, rs_type_e su_type,
			   rs_settings_t *settings)
{
	bool rs_sbrce_ena = !!(settings->rs_sbrce_ena &&
			       (su_type == MBSU_CH || su_type == DMSU_CH));

	if (settings->rs_lfsr_ena)
		__raw_writel(SU_LFSR_DEFAULT, SU_LFSR_CNTRL_REG(base));
	if (rs_sbrce_ena)
		__raw_writel_mask(rs_get_sbrce(settings->rs_sbrlvl,
					       settings->rs_sbrmsk),
				  SU_LFSR_SBRCE_MASK,
				  SU_LFSR_CNTRL_REG(base));
	if (settings->rs_lfsr_ena || settings->rs_sbrce_ena)
		__raw_setl(SU_LFSR_EN, SU_LFSR_CNTRL_REG(base));
	if (rs_sbrce_ena)
		__raw_setl(SU_LFSR_SBRCE, SU_LFSR_CNTRL_REG(base));
}

/* This function configures Dead Lock Breaker Unit across the Monaco Ring. */
static void rs_config_dlbu(monaco_init_t *init_parms_ptr)
{
	uint32_t idx;

	if (!init_parms_ptr->rs_settings.rs_dlbu_ena)
		return;

	/* 1. Set SU_DLB_THROTTLE_EN within MBSU/DMSU/PBSU/MCSU */
	for (idx = 0; idx < NR_PBSUS; idx++)
		__raw_writel_mask(SU_DLB_THROTTLE_EN,
				  SU_DLB_THROTTLE_EN,
				  SU_CONFIGURATION_REG(PBSU_BASE(idx)));
	for (idx = 0; idx < NR_MBSUS; idx++)
		__raw_writel_mask(SU_DLB_THROTTLE_EN,
				  SU_DLB_THROTTLE_EN,
				  SU_CONFIGURATION_REG(MBSU_BASE(idx)));
	for (idx = 0; idx < NR_DMSUS; idx++)
		__raw_writel_mask(SU_DLB_THROTTLE_EN,
				  SU_DLB_THROTTLE_EN,
				  SU_CONFIGURATION_REG(DMSU_BASE(idx)));
	for (idx = 0; idx < NR_MCSUS; idx++)
		__raw_writel_mask(SU_DLB_THROTTLE_EN,
				  SU_DLB_THROTTLE_EN,
				  SU_CONFIGURATION_REG(MCSU_BASE(idx)));
        /* 2. Set TRSU_DLB_THROTTLE_EN within TRSU */
	for (idx = 0; idx < NR_TRSUS; idx++)
		__raw_writel_mask(TRSU_DLB_THROTTLE_EN,
				  TRSU_DLB_THROTTLE_EN,
				  SU_CONFIGURATION_REG(TRSU_BASE(idx)));

        /* 3. Set the CHAN_SEL field within ADLBU and enable ADLBU */
	if (ARCH_HAS_SU_ADLBU) {
		__raw_writel_mask(MBSU_DLB_CHAN_ALL, MBSU_DLB_CHAN_ALL,
				  SU_CONFIGURATION_REG(SU_ADLBU_BASE));
		__raw_setl(MBSU_DLB_EN, SU_CONFIGURATION_REG(SU_ADLBU_BASE));
	}
        /* 4. Set the CHAN_SEL field within GDLBU and enable GDLBU */
	if (ARCH_HAS_SU_GDLBU) {
		__raw_writel_mask(MBSU_DLB_CHAN_ALL, MBSU_DLB_CHAN_ALL,
				  SU_CONFIGURATION_REG(SU_GDLBU_BASE));
		__raw_setl(MBSU_DLB_EN, SU_CONFIGURATION_REG(SU_GDLBU_BASE));
	}
}

/* This function programs the DCVS registers with different measurement
 * intervals and different threshold counters per performance plan.
 * Notes: Available only for MBSU, DMSU, and PBSU
 */
static void rs_config_bum(uint64_t base, rs_type_e su_type,
			  rs_settings_t *settings)
{
	uint32_t idx;
	uint32_t bum_tx_lo_thld, bum_tx_hi_thld;
	uint32_t bum_rx_lo_thld, bum_rx_hi_thld;
	uint32_t bum_intf[4] = {
		[MBSU_CH] = SU_INTF_RXC_TXC_MASKS(7, 3, 15, 1),
		[DMSU_CH] = SU_INTF_RXC_TXC_MASKS(7, 3, 7, 3),
		[PBSU_CH] = SU_INTF_RXC_TXC_MASKS(7, 3, 10, 3),
		[MCSU_CH] = SU_INTF_RXC_TXC_MASKS(10, 3, 0, 0),
	};

	if (su_type == TRSU_CH || !settings->rs_bum_ena)
		return;

	/* Manage Ring DCVS Control */
	/* Clear all DCVS shadow counters and disable the monitor */
	__raw_writel(SU_DCVS_INTF_MASKS | SU_CLR_SHD_CNT,
		     SU_DCVS_CTRL_REG(base));
	/* Configure DCVS threshold values for all possible interfaces */
	for (idx = 0; idx < RS_BUM_PERFNDX_MAX; idx++) {
		if (idx < settings->rs_perfndx_max) {
			bum_rx_lo_thld = settings->rs_bum_thresh[su_type].rx_lo_thresh[idx];
			bum_rx_hi_thld = settings->rs_bum_thresh[su_type].rx_hi_thresh[idx];
			bum_tx_lo_thld = settings->rs_bum_thresh[su_type].tx_lo_thresh[idx];
			bum_tx_hi_thld = settings->rs_bum_thresh[su_type].tx_hi_thresh[idx];
		} else {
			bum_rx_lo_thld = 0;
			bum_rx_hi_thld = 0;
			bum_tx_lo_thld = 0;
			bum_tx_hi_thld = 0;
		}
		__raw_writel(bum_tx_lo_thld,
			     SU_DCVS_TX_LO_THRESHOLD_REG(base, idx));
		__raw_writel(bum_tx_hi_thld,
			     SU_DCVS_TX_HI_THRESHOLD_REG(base, idx));
		__raw_writel(bum_rx_lo_thld,
			     SU_DCVS_RX_LO_THRESHOLD_REG(base, idx));
		__raw_writel(bum_rx_hi_thld,
			     SU_DCVS_RX_HI_THRESHOLD_REG(base, idx));
	}
	/* Enable all interfaces in both TX/RX */
	__raw_writel(bum_intf[su_type], SU_DCVS_CTRL_REG(base));
	__raw_setl(SU_DCVS_EN, SU_DCVS_CTRL_REG(base));
}

/* This function configures CLK_ON_REG in the following fields.
 *   CODTMR_RST_VAL: Clock On Delay Timer
 *   COHTMR_RST_VAL: Clock On Hysteresis Timer
 *   CLK_ON_REQ_EN : Enable / Disable
 */
static void rs_config_clkon(uint64_t base, rs_type_e su_type,
			    rs_settings_t *settings)
{
	uint32_t tmr_val;

	if (su_type == TRSU_CH)
		return;

	/* Manage Ring ClkOnRequest */
	/* Clock On Delay Timer, min TBD cycles, CODTMR_RST_VAL[25:21] may
	 * cause malfunction if it's too low.
	 * Clock On Hysteresis Timer, min 30 cycles,COHTMR_RST_VAL[10:1],
	 * factor of power vs performance.
	 */
	if (settings->rs_clkon_req_ena) {
		/* Get the register contents with CODTMR and COHTMR */
		if (SU_CODTMR_RST_VAL_MIN > settings->rs_clkon_delay)
			settings->rs_clkon_delay = SU_CODTMR_RST_VAL_MIN;
		if (SU_CODTMR_RST_VAL_MASK < settings->rs_clkon_delay)
			settings->rs_clkon_delay = SU_CODTMR_RST_VAL_MASK;
		if (SU_COHTMR_RST_VAL_MIN > settings->rs_clkon_hysteresis)
			settings->rs_clkon_hysteresis = SU_COHTMR_RST_VAL_MIN;
		if (SU_COHTMR_RST_VAL_MASK < settings->rs_clkon_hysteresis)
			settings->rs_clkon_hysteresis = SU_COHTMR_RST_VAL_MASK;

		/* Program CODTMR and COHTMR first and enable CLK_ON_REQ_EN */
		tmr_val = SU_CODTMR_RST_VAL(settings->rs_clkon_delay) |
			  SU_COHTMR_RST_VAL(settings->rs_clkon_hysteresis);
		__raw_writel(tmr_val, SU_CLK_ON_REQ_REG(base, 0));
		__raw_setl(SU_CLK_ON_REQ_EN, SU_CLK_ON_REQ_REG(base, 0));
		if (su_type == PBSU_CH || su_type == DMSU_CH) {
			__raw_writel(tmr_val, SU_CLK_ON_REQ_REG(base, 1));
			__raw_setl(SU_CLK_ON_REQ_EN, SU_CLK_ON_REQ_REG(base, 1));
		}
	} else {
		__raw_clearl(SU_CLK_ON_REQ_EN, SU_CLK_ON_REQ_REG(base, 0));
		if (su_type == PBSU_CH || su_type == DMSU_CH)
			__raw_clearl(SU_CLK_ON_REQ_EN, SU_CLK_ON_REQ_REG(base, 1));
	}
}

/* This function configures the control register settings of a single SU
 * that are common across all MBSU, DMSU, PBSU, MCSU, and TRSU switches.
 */
static void rs_config_common_ctl(uint64_t base, rs_type_e su_type,
				 rs_settings_t *settings)
{
	/* Enable the error report for PBSU2 */
	if (base == SU_CFGPOS_BASE) {
		__raw_writel(SU_CLR_ERR, SU_ERR_CLR_REG(base));
		__raw_writel(0, SU_ERR_CTRL_REG(base));
		__raw_writel(SU_ERR_CFG_ALL, SU_INT_CTRL_REG(base));
		/* Now able to see the error report
		 * from TCSR_HMSS_SU_ERROR_INTR_MAP_REG at 0xFF02E1001C
		 */
	}
	rs_config_bum(base, su_type, settings);
	rs_config_clkon(base, su_type, settings);
	rs_config_lfsr(base, su_type, settings);
	rs_config_route(base, su_type, settings);
}

/* This function determines the register value that should be written into
 * a single [SWCH]_L3_DDR_CNT_REG register.
 *   list_ddr0_ptr  List of MCSU switch IDs on the even interleave with
 *                  populated DIMM slots.
 *   list_ddr1_ptr  List of MCSU switch IDs on the odd interleave with
 *                  populated DIMM slots.
 *   list_l3u0_ptr  List of MBSU switch IDs on the even interleave with
 *                  valid L3 interleaves.
 *   list_l3u1_ptr  List of MBSU switch IDs on the odd interleave with
 *                  valid L3 interleaves.
 */
static void rs_config_l3_ddr_cnt(uint64_t base,
				 byte_list_t *list_ddr0_ptr,
				 byte_list_t *list_ddr1_ptr,
				 byte_list_t *list_l3u0_ptr,
				 byte_list_t *list_l3u1_ptr)
{
	__raw_writel(SU_L3_EVEN_INTLV_CNT(bl_get_num_entries(list_l3u0_ptr)) |
		     SU_L3_ODD_INTLV_CNT(bl_get_num_entries(list_l3u1_ptr)) |
		     SU_DDR_CH_CNT(bl_get_num_entries(list_ddr0_ptr) +
				   bl_get_num_entries(list_ddr1_ptr)),
		     SU_L3_DDR_CNT_REG(base));
}

static void rs_config_intlv_l3_inst(uint64_t base,
				    byte_list_t *list_l3u0_ptr,
				    byte_list_t *list_l3u1_ptr)
{
	uint32_t idx;
	uint8_t su;

	for (idx = 0; idx < bl_num(list_l3u0_ptr); idx++) {
		su = bl_ent(list_l3u0_ptr, idx);
		__raw_writel_mask(SU_INTLV_L3_INST_L3U_BUS_ID(idx, SU_BUS_ID(su)) |
				  SU_INTLV_L3_INST_L3U_SWCH_ID(idx, SU_SWITCH_ID(su)),
				  SU_INTLV_L3_INST_MASK(idx),
				  SU_EVEN_INTLV_L3_INST_REG(base, idx));
	}
	for (idx = 0; idx < bl_num(list_l3u1_ptr); idx++) {
		su = bl_ent(list_l3u1_ptr, idx);
		__raw_writel_mask(SU_INTLV_L3_INST_L3U_BUS_ID(idx, SU_BUS_ID(su) + 4) |
				  SU_INTLV_L3_INST_L3U_SWCH_ID(idx, SU_SWITCH_ID(su)),
				  SU_INTLV_L3_INST_MASK(idx),
				  SU_ODD_INTLV_L3_INST_REG(base, idx));
	}
}

/* This function calculates the prime mask for BIK0 based on the number of
 * l3 instance per interleave.
 */
static uint32_t rs_get_bik0_pmask(uint32_t val)
{
	uint32_t num_bit;

	if (val == 1) return 0;
#if 1
	asm volatile ("CLZ %w0, %w1" : "=&r" (num_bit): "r" (val-1): "cc");
#else
	num_bit = __fls32(val - 1);
#endif
	num_bit = 32 - num_bit;  /* reg width set to 32bit always */
	return ((uint32_t)(1<<num_bit) - 1);
}

/* TODO: Need an algorithm instead of the hard coding */
static void rs_build_biks_per_intlv(uint32_t l3inst,
				    uint32_t num_of_l3inst_per_intlv,
				    uint32_t *reg0, uint32_t *reg1)
{
	/*--------------------------------------------------------
	 * register name    |    bit  |  field name
	 *--------------------------------------------------------
	 * INTLV_BIK_INST_0 |  27:24  |  BIK0_PRIM_MASK
	 *                  |  31:28  |  BIK0_PRIM
	 *--------------------------------------------------------
	 * INTLV_BIK_INST_1 |   3: 0  |  BIK2_SEC_MASK
	 *                  |   7: 4  |  BIK2_SEC
	 *                  |  11: 8  |  BIK2_PRIM
	 *                  |     12  |  BIK2_EN
	 *                  |  17:14  |  BIK1_SEC_MASK
	 *                  |  21:18  |  BIK1_SEC
	 *                  |  25:22  |  BIK1_PRIM
	 *                  |     26  |  BIK1_EN
	 *--------------------------------------------------------*/
	uint32_t inst0 = 0;
	uint32_t inst1 = 0;
	uint32_t mask0;

	if (l3inst >= num_of_l3inst_per_intlv) {
		*reg0 = inst0;
		*reg1 = inst1;
		return;
	}

	mask0 = rs_get_bik0_pmask(num_of_l3inst_per_intlv);
	inst0 = SU_BIK0_PRIM(l3inst) | SU_BIK0_PRIM_MASK(mask0);

	switch (num_of_l3inst_per_intlv) {
	case 3 :
		if (l3inst == 0)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x3)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x8)|
			        SU_BIK2_EN|SU_BIK2_PRIM(0x7)|
				SU_BIK2_SEC(0x0)|SU_BIK2_SEC_MASK(0x0);
		if (l3inst == 1)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x3)|
				SU_BIK1_SEC(0x8)|SU_BIK1_SEC_MASK(0x8)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x0)|SU_BIK2_SEC_MASK(0x8);
		if (l3inst == 2)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xB)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x0)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x8)|SU_BIK2_SEC_MASK(0x8);
		break;
	case 5 :
		if (l3inst == 0)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x5)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x0)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x0)|SU_BIK2_SEC_MASK(0xC);
		if (l3inst == 1)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xD)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x0);
		if (l3inst == 2)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x6)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x0)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x4)|SU_BIK2_SEC_MASK(0xC);
		if (l3inst == 3)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xE)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x0);
		if (l3inst == 4)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x7)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x0)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x8)|SU_BIK2_SEC_MASK(0x8);
		break;
	case 6 :
		if (l3inst == 0)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x6)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0x7)|
				SU_BIK2_SEC(0x9)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 1)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xE)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x9)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 2)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x6)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0x7)|
				SU_BIK2_SEC(0x8)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 3)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xE)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x8)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 4)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x7)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x8)|
				SU_BIK2_EN|SU_BIK2_PRIM(0x7)|
				SU_BIK2_SEC(0xC)|SU_BIK2_SEC_MASK(0xC);
		if (l3inst == 5)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xF)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x8)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0xC)|SU_BIK2_SEC_MASK(0xC);
		break;
	case 7 :
		if (l3inst == 0)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x7)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x9)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 1)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xF)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0xD)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 2)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x7)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x9);
		if (l3inst == 3)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xF)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x9);
		if (l3inst == 4)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x7)|
				SU_BIK1_SEC(0x8)|SU_BIK1_SEC_MASK(0x9);
		if (l3inst == 5)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xF)|
				SU_BIK1_SEC(0x8)|SU_BIK1_SEC_MASK(0x9);
		if (l3inst == 6)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x7)|
				SU_BIK1_SEC(0x9)|SU_BIK1_SEC_MASK(0x9);
		break;
	case 9 :
		if (l3inst == 0)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x9)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xE)|
				SU_BIK2_SEC(0x1)|SU_BIK2_SEC_MASK(0x3);
		if (l3inst == 1)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xA)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xE)|
				SU_BIK2_SEC(0x3)|SU_BIK2_SEC_MASK(0x3);
		if (l3inst == 2)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0x9)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xE)|
				SU_BIK2_SEC(0x0)|SU_BIK2_SEC_MASK(0x3);
		if (l3inst == 3)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xA)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x2)|SU_BIK2_SEC_MASK(0x3);
		if (l3inst == 4)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xB)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x1)|SU_BIK2_SEC_MASK(0x3);
		if (l3inst == 5)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xB)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x0)|SU_BIK2_SEC_MASK(0x3);
		if (l3inst == 6)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xC)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x3)|SU_BIK2_SEC_MASK(0x3);
		if (l3inst == 7)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xC)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xE)|
				SU_BIK2_SEC(0x2)|SU_BIK2_SEC_MASK(0x3);
		if (l3inst == 8)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xD)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x0);
		break;
	case 10 :
		if (l3inst == 0)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xA)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x1)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 1)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xA)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x0)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 2)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xB)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x5)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 3)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xC)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x9)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 4)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xB)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x4)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 5)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xC)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x8)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 6)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xD)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0xD)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 7)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xD)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1);
		if (l3inst == 8)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xE)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1);
		if (l3inst == 9)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xE)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0xC)|SU_BIK2_SEC_MASK(0xD);;
		break;
	case 11 :
		if (l3inst == 0)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xB)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1);
		if (l3inst == 1)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xB)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1);
		if (l3inst == 2)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xC)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1);
		if (l3inst == 3)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xC)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1);
		if (l3inst == 4)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xD)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1);
		if (l3inst == 5)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xE)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x1);
		if (l3inst == 6)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xD)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x1);
		if (l3inst == 7)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xE)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xE)|
				SU_BIK2_SEC(0x9)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 8)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xF)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x9)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 9)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xF)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xE)|
				SU_BIK2_SEC(0xD)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 10)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xF)|
				SU_BIK1_SEC(0x8)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0xD)|SU_BIK2_SEC_MASK(0xD);
		break;
	case 12 :
		if (l3inst == 0)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xC)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xC)|
				SU_BIK2_SEC(0x9)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 1)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xC)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xD)|
				SU_BIK2_SEC(0xF)|SU_BIK2_SEC_MASK(0xF);
		if (l3inst == 2)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xC)|
				SU_BIK1_SEC(0x8)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xC)|
				SU_BIK2_SEC(0xD)|SU_BIK2_SEC_MASK(0xF);
		if (l3inst == 3)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xD)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xC)|
				SU_BIK2_SEC(0xF)|SU_BIK2_SEC_MASK(0xF);
		if (l3inst == 4)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xD)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xD)|
				SU_BIK2_SEC(0x9)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 5)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xE)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xE)|
				SU_BIK2_SEC(0x9)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 6)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xD)|
				SU_BIK1_SEC(0x8)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xD)|
				SU_BIK2_SEC(0xD)|SU_BIK2_SEC_MASK(0xF);
		if (l3inst == 7)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xE)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xE)|
				SU_BIK2_SEC(0xF)|SU_BIK2_SEC_MASK(0xF);
		if (l3inst == 8)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xE)|
				SU_BIK1_SEC(0x8)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xE)|
				SU_BIK2_SEC(0xD)|SU_BIK2_SEC_MASK(0xF);
		if (l3inst == 9)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xF)|
				SU_BIK1_SEC(0x0)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0xF)|SU_BIK2_SEC_MASK(0xF);
		if (l3inst == 10)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xF)|
				SU_BIK1_SEC(0x1)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0x9)|SU_BIK2_SEC_MASK(0xD);
		if (l3inst == 11)
			inst1 = SU_BIK1_EN|SU_BIK1_PRIM(0xF)|
				SU_BIK1_SEC(0x8)|SU_BIK1_SEC_MASK(0x9)|
				SU_BIK2_EN|SU_BIK2_PRIM(0xF)|
				SU_BIK2_SEC(0xD)|SU_BIK2_SEC_MASK(0xF);
		break;
	}
	*reg0 = inst0;
	*reg1 = inst1;
}

static void rs_config_intlv_bik(uint64_t base,
				byte_list_t *list_l3u0_ptr,
				byte_list_t *list_l3u1_ptr)
{
	uint32_t reg_data_0, reg_data_1;
	uint32_t idx;

	for (idx = 0; idx < MAX_L3_NODES; idx++) {
		rs_build_biks_per_intlv(idx, bl_num(list_l3u0_ptr),
					&reg_data_0, &reg_data_1);
		__raw_writel(reg_data_0,
			     SU_EVEN_INTLV_BIK_INST_0_REG(base, idx));
		__raw_writel(reg_data_1,
			     SU_EVEN_INTLV_BIK_INST_1_REG(base, idx));
		rs_build_biks_per_intlv(idx, bl_num(list_l3u1_ptr),
					&reg_data_0, &reg_data_1);
		__raw_writel(reg_data_0,
			     SU_ODD_INTLV_BIK_INST_0_REG(base, idx));
		__raw_writel(reg_data_1,
			     SU_ODD_INTLV_BIK_INST_1_REG(base, idx));
	}
}

/* This function determines the register value that should be written into
 * a single [SWCH]_INTLV_DDR_DECODE_MODE_REG register.
 * ddr_dist       Indication of the DDR distribution granule to use when
 *                two DDR channels are populated per interleave.
 *                Only meaningful when the channel count is 4.
 * Notes:
 *     Configurability of the DDR granule is only meaningful when the populated
 *     channel count is 4 (2 DDR channels per interleave). For other channel
 *     configurations the granule is always 256B. Detected errors still result
 *     in a usable register value with the granule being 256B.
 */
static void rs_config_ddr_decode_mode(uint64_t base,
				      mddr_ddr_intlv_t ddr_dist)
{
	uint32_t dist_val = SU_DDR_DIST_GRANULE_OF_256;

	if (ddr_dist == DDR_DIST_GRANULE_256B)
		dist_val = SU_DDR_DIST_GRANULE_OF_256;
	else if (ddr_dist == DDR_DIST_GRANULE_4KB)
		dist_val = SU_DDR_DIST_GRANULE_OF_4K;
	else if (ddr_dist == DDR_DIST_GRANULE_8KB)
		dist_val = SU_DDR_DIST_GRANULE_OF_8K;
	__raw_writel(SU_DDR_DIST_CNTL(dist_val),
		     SU_DDR_DECODE_MODE_REG(base));
}

static void rs_config_intlv_ddr_inst(uint64_t base,
				     byte_list_t *list_ddr0_ptr,
				     byte_list_t *list_ddr1_ptr)
{
	uint32_t idx;
	uint8_t su;

	for (idx = 0; idx < bl_num(list_ddr0_ptr); idx++) {
		su = bl_ent(list_ddr0_ptr, idx);
		__raw_writel_mask(SU_INTLV_DDR_INST_DDR_SWCH_ID(idx, SU_SWITCH_ID(su)),
				  SU_INTLV_DDR_INST_MASK(idx),
				  SU_EVEN_INTLV_DDR_INST_REG(base));
	}
	for (idx = 0; idx < bl_num(list_ddr1_ptr); idx++) {
		su = bl_ent(list_ddr1_ptr, idx);
		__raw_writel_mask(SU_INTLV_DDR_INST_DDR_SWCH_ID(idx, SU_SWITCH_ID(su)),
				  SU_INTLV_DDR_INST_MASK(idx),
				  SU_ODD_INTLV_DDR_INST_REG(base));
	}
}

/* This function configures ADDR and MASK register sets in DDR_SEGMENT
 * according to chapter 2.4.2 TN Address decoding in HPG 80-Q1234-1H Rev. A
 */
static void rs_config_ddr_segment(uint64_t base)
{
	uint32_t ddr_idx;
	uint32_t addr, mask;

	for (ddr_idx = 0; ddr_idx < SU_MAX_DDR_SEGMENTS; ddr_idx++) {
		__su_seg_addr_decode(su_hw_ddr_segs[ddr_idx][0],
				     su_hw_ddr_segs[ddr_idx][1],
				     DDR_SEG_PA_MSB, DDR_SEG_PA_LSB,
				     addr, mask);
		__raw_clearl(SU_SEG_EN,
			     SU_DDR_SEGMENT_ADDR_REG(base, ddr_idx));
		__raw_writel(SU_PA_43_31_DDR_BASE(addr),
			     SU_DDR_SEGMENT_ADDR_REG(base, ddr_idx));
		__raw_writel(SU_PA_43_31_SEG_MASK(mask),
			     SU_DDR_SEGMENT_MASK_REG(base, ddr_idx));
		if (SU_ID(base) >= MBSU0 && SU_ID(base) < MBSU0 + NR_MBSUS && ddr_idx == 0)
			__raw_setl(MBSU_SEG_CACHE_EN,
				   SU_DDR_SEGMENT_ADDR_REG(base, ddr_idx));
		__raw_setl(SU_SEG_EN, SU_DDR_SEGMENT_ADDR_REG(base, ddr_idx));
	}
}

/* This function checks if the switch unit located at the given address
 * space is under the restriction or not based on chapter 2.4.2 TN Address
 * decoding in HPG 80-Q1234-1H Rev. A
 */
static bool rs_check_slave_restriction(uint64_t base, uint32_t slv_idx,
				       target_mode_e mode)
{
	uint32_t su_indx;
	rs_type_e su_type = ~PBSU_CH;

	/* 3. Avoid PCIe peer-to-peer access
	 *    PBSU0_SLAVE_SEGMENT_ADDR_A_REG_1[SEG_EN] to Zero
	 *    PBSU1_SLAVE_SEGMENT_ADDR_A_REG_0[SEG_EN] to Zero
	 * 4. Avoid PBSU ICN-TN self-access
	 *    PBSU0_SLAVE_SEGMENT_ADDR_A_REG_0[SEG_EN] to Zero
	 *    PBSU1_SLAVE_SEGMENT_ADDR_A_REG_1[SEG_EN] to Zero
	 *    PBSU2_SLAVE_SEGMENT_ADDR_A_REG_2[SEG_EN] to Zero
	 *    PBSU3_SLAVE_SEGMENT_ADDR_A_REG_3[SEG_EN] to Zero
	 *    PBSU4_SLAVE_SEGMENT_ADDR_A_REG_4[SEG_EN] to Zero
	 * NOTE: Avoid the access to the non-existent TN slave (ex.PCIe0)
	 */
	su_indx = SU_ID(base);

	if (su_indx >= PBSU0 && su_indx < PBSU0 + NR_PBSUS) {
		su_type = PBSU_CH;
		su_indx -= PBSU0;
	}

	if (!rs_check_usable_pbsu(slv_idx))
		return true;

	/* Restrictions in PCIe peer to peer access and PBSU self
	 * access
	 */
	if (su_type == PBSU_CH) {
		if ((su_indx == slv_idx) || (is_pcie_pbsu(su_indx) && is_pcie_pbsu(slv_idx)))
			return true;
	}
	return false;
}

/* This function configures ADDR and MASK register sets in SLAVE_SEGMENT
 * according to chapter 2.4.2 TN Address decoding in Monaco Core HPG.
 */
static void rs_config_slave_segment(uint64_t base, target_mode_e mode)
{
	uint32_t slv_idx;
	uint32_t seg_id, seg_addr, seg_mask;
	uint32_t seg_en = SU_SEG_EN;

#ifdef CONFIG_MONACO_SLAVE_CACHE
	if (SU_ID(base) >= MBSU0 && SU_ID(base) < MBSU0 + NR_MBSUS)
		seg_en = SU_SEG_EN | MBSU_SEG_CACHE_EN;
#endif

	for (slv_idx = 0; slv_idx < SU_MAX_SLAVE_SEGMENTS; slv_idx++) {
		seg_id = su_hw_slave_segs[slv_idx][0];
		/* PCIe_ss_1 has only one controller, pcie#3 on RUMI so
		 * reduce the size to 1TB
		 */
		if ((slv_idx == 1) && (mode != SILICON))
			__su_seg_addr_decode(su_hw_slave_segs[slv_idx][1],
					     0x010000000000 >> 27,
					     SLV_SEG_PA_MSB, SLV_SEG_PA_LSB,
					     seg_addr, seg_mask);
		else
			__su_seg_addr_decode(su_hw_slave_segs[slv_idx][1],
					     su_hw_slave_segs[slv_idx][2],
					     SLV_SEG_PA_MSB, SLV_SEG_PA_LSB,
					     seg_addr, seg_mask);
		__raw_clearl(SU_SEG_EN,
			     SU_SLAVE_SEGMENT_ADDR_A_REG(base, slv_idx));
		if (!rs_check_slave_restriction(base, slv_idx, mode) &&
		    GET_SEG_EN(seg_id)) {
			__raw_writel(SU_SEG_SWCH_ID(GET_SEG_ID(seg_id)) |
				     SU_PA_43_27_SEG_BASE(seg_addr),
				     SU_SLAVE_SEGMENT_ADDR_A_REG(base, slv_idx));
			__raw_writel(SU_PA_43_27_SEG_MASK(seg_mask),
				     SU_SLAVE_SEGMENT_MASK_A_REG(base, slv_idx));
			__raw_setl(seg_en,
				   SU_SLAVE_SEGMENT_ADDR_A_REG(base, slv_idx));
		}
	}
	for (slv_idx = 0; slv_idx < SU_MAX_SLAVE_SEGMENTS; slv_idx++) {
		seg_addr = 0;
		seg_mask = 0;
		if (!rs_check_slave_restriction(base, slv_idx, mode)) {
			if ((slv_idx == 1) &&
			    ((mode == RUMI42) || (mode == RUMI48))) {
				seg_addr = 0x80B1E000; /* 0F00_0000_0000 */
				seg_mask = 0x0001E000; /* 0FFF_FFFF_FFFF */
			}
		}
		/* No use case for SEGMENT B */
		__raw_writel(seg_addr,
			     SU_SLAVE_SEGMENT_ADDR_B_REG(base, slv_idx));
		__raw_writel(seg_mask,
			     SU_SLAVE_SEGMENT_MASK_B_REG(base, slv_idx));
	}
}

/* This function configures the register settings of a single SU that are
 * common across all MBSU, DMSU and PBSU switches.
 *   list_ddr0_ptr  List of MCSU switch IDs on the even interleave with
 *                  populated DIMM slots.
 *   list_ddr1_ptr  List of MCSU switch IDs on the odd interleave with
 *                  populated DIMM slots.
 *   list_l3u0_ptr  List of MBSU switch IDs on the even interleave with
 *                  valid L3 interleaves.
 *   list_l3u1_ptr  List of MBSU switch IDs on the odd interleave with
 *                  valid L3 interleaves.
 * Notes:
 *   MCSU is a slave only and so does not have most of the common
 *   configuration registers.
 */
static void rs_config_common_su(uint64_t base,
				monaco_init_t *params,
				byte_list_t *list_ddr0_ptr,
				byte_list_t *list_ddr1_ptr,
				byte_list_t *list_l3u0_ptr,
				byte_list_t *list_l3u1_ptr)
{
	rs_config_l3_ddr_cnt(base,
			     list_ddr0_ptr, list_ddr1_ptr,
			     list_l3u0_ptr, list_l3u1_ptr);

	rs_config_intlv_l3_inst(base, list_l3u0_ptr, list_l3u1_ptr);
	/* XXX: configure L3 banks */
	if (params->l3_settings.affinity)
		__su_set_affinity_mode(base);
	else /* default mode */
		__su_set_spread_mode(base);
	rs_config_intlv_bik(base, list_l3u0_ptr, list_l3u1_ptr);

	rs_config_intlv_ddr_inst(base, list_ddr0_ptr, list_ddr1_ptr);
	/* XXX: configure DDR channels/segments */
	rs_config_ddr_decode_mode(base,
				  params->mddr_settings.ddr_granule);
	rs_config_ddr_segment(base);
	rs_config_slave_segment(base, params->mode);
	//start address: 0xFFF8000000; size: 128MB
	if(params->l3_settings.fatcm)
	{
		const uint32_t *seg_lists[2] = {(uint32_t []){TCM_START >> FATCM_PA_LSB, 0x8000000 >> FATCM_PA_LSB},
										(uint32_t []){0x0, 0x0}};
		rs_config_fatcm_segment(base, seg_lists);
	}
}

static void rs_config_mcsu(monaco_init_t *params, uint8_t mcsu_id)
{
	uint64_t base = MCSU_BASE(mcsu_id);

	/* MCSU_CONFIGURATION_REG[SNP_RSP_EN] is not used for Rev 1 and
	 * should always be 0 (reset value).
	 */
	rs_config_common_ctl(base, MCSU_CH, &params->rs_settings);
}

static void rs_config_trsu(monaco_init_t *params, uint8_t trsu_id)
{
	uint64_t base = TRSU_BASE(trsu_id);

	rs_config_common_ctl(base, TRSU_CH, &params->rs_settings);
}

/* This function determines the register value that should be written into
 * a single DVM_POS_REG register.
 * NOTES:
 *   Barriers, DVMs and MMIO accesses are routed to dedicated POS. The
 *   POSes are in L3 interleaves and so are impacted by partial goods.
 */
static void rs_config_bar_dvm_pos(uint64_t base,
				  byte_list_t *list_l3u_ptr)
{
	uint32_t idx;
	uint8_t swid;
	uint32_t intlv_id;

	for (idx = 0; idx < NUM_INTERLEAVES; idx++) {
		/* Use the first good L3 from each intlv as the POS for
		 * BAR and DVM.
		 */
		if (bl_num(&list_l3u_ptr[idx]) > 0) {
			swid = bl_ent(&list_l3u_ptr[idx], 0);
			intlv_id = SU_POS_INTLV_BID(SU_BUS_ID(swid) + (4 * idx)) |
				   SU_POS_INTLV_SWCH_ID(SU_SWITCH_ID(swid));
			__raw_writel_mask(intlv_id << SU_POS_INTLV_OFFSET(idx),
					  SU_POS_INTLV_MASK(idx),
					  SU_DVM_POS_REG(base));
			__raw_writel_mask(intlv_id << SU_POS_INTLV_OFFSET(idx),
					  SU_POS_INTLV_MASK(idx),
					  SU_BAR_POS_REG(base));
		}
	}
}

static void rs_config_pbsu_configuration(uint64_t base, uint8_t pbsu_id)
{
	/* Build new reg value and set the flag to block all snoops to prevent
	 * snoops from propagating to non-existent HW.
	 */
	if (!rs_check_usable_pbsu(pbsu_id))
		__raw_setl(PBSU_BLK_ALL_SNP_TO_ICN, SU_CONFIGURATION_REG(base));
}

static void rs_config_pbsu(monaco_init_t *init_parms_ptr, uint8_t pbsu_id)
{
	uint64_t base = PBSU_BASE(pbsu_id);

	rs_config_common_su(base, init_parms_ptr,
			    &list_ddr_sid[0], &list_ddr_sid[1],
			    &list_l3u_sid[0], &list_l3u_sid[1]);
	rs_config_bar_dvm_pos(base, list_l3u_sid);
	rs_config_pbsu_configuration(base, pbsu_id);
	rs_config_common_ctl(base, PBSU_CH, &init_parms_ptr->rs_settings);
}

static void rs_config_dmsu_configuration(uint64_t base, uint8_t dmsu_id)
{
	/* DMSUs have 2 clusters and so there are 2 flags per switch. */
	if (0x1 & (flag_usable_apc[DMSU_CH][0] >> dmsu_id))
		__raw_setl(DMSU_LCL_MSTR0_AVAIL, SU_CONFIGURATION_REG(base));
	else
		__raw_clearl(DMSU_LCL_MSTR0_AVAIL, SU_CONFIGURATION_REG(base));
	if (0x1 & (flag_usable_apc[DMSU_CH][1] >> dmsu_id))
		__raw_setl(DMSU_LCL_MSTR1_AVAIL, SU_CONFIGURATION_REG(base));
	else
		__raw_clearl(DMSU_LCL_MSTR1_AVAIL, SU_CONFIGURATION_REG(base));
}

static void rs_config_dmsu(monaco_init_t *init_parms_ptr, uint8_t dmsu_id)
{
	uint64_t base = DMSU_BASE(dmsu_id);

	rs_config_common_su(base, init_parms_ptr,
			    &list_ddr_sid[0], &list_ddr_sid[1],
			    &list_l3u_sid[0], &list_l3u_sid[1]);
	rs_config_bar_dvm_pos(base, list_l3u_sid);
	rs_config_dmsu_configuration(base, dmsu_id);
	rs_config_common_ctl(base, DMSU_CH, &init_parms_ptr->rs_settings);
}

/* This function determines the register value that should be written into
 * a single MBSU_CONFIGURATION_REG register.
 * Notes:
 *   The L3 flags control QLL access to the configuration registers of the
 *   L3.
 */
static void rs_config_mbsu_configuration(uint64_t base, uint8_t mbsu_id,
					 bool l3_auto_clk_gate_en,
					 l3intlv_t l3_interleaves)
{
	uint32_t reg_data = 0;

	if (0x1 & (flag_usable_apc[MBSU_CH][0] >> mbsu_id))
		reg_data |= MBSU_LCL_MSTR_AVAIL;
	if (0x1 & (l3_interleaves >> (2 * mbsu_id)))
		reg_data |= MBSU_LCL_L3_AVAIL_E;
	if ((0x2 & (l3_interleaves >> (2 * mbsu_id))) >> 1)
		reg_data |= MBSU_LCL_L3_AVAIL_O;
	if (l3_auto_clk_gate_en)
		reg_data |= MBSU_L3_QUIESCE_EN;
	__raw_writel_mask(reg_data,
			  (MBSU_LCL_MSTR_AVAIL |
			   MBSU_LCL_L3_AVAIL_E |
			   MBSU_LCL_L3_AVAIL_O |
			   MBSU_L3_QUIESCE_EN),
			  SU_CONFIGURATION_REG(base));
}

static void rs_config_mbsu_mnc_node_type_cnt(uint64_t base)
{
	uint8_t icn_cnt;
	uint8_t tn_cnt;

	/* For emulation, reduce the count to adjust for missing PCIe
	 * PBSU.
	 */
	tn_cnt = icn_cnt = 0;
	while (tn_cnt < NR_PBSUS) {
		if (rs_check_usable_pbsu(tn_cnt))
			icn_cnt++;
		tn_cnt++;
	}
	tn_cnt = icn_cnt;
	__raw_writel(MBSU_FCN_CNT(bl_num(&list_apc_pid)) |
		     MBSU_ICN_CNT(icn_cnt) |
		     MBSU_TN_CNT(tn_cnt),
		     MBSU_MNC_NODE_TYPE_CNT_REG(base));
}

/* This function informs the snoop filter about the current Hydra and L3
 * configuration. The available Hydra's vs. active L3's/intlv will
 * determine the PIDs/BIDs of the FCNs to be tracked by the snoop filter.
 */
static void rs_config_mbsu_sft_pid(uint64_t su_cfg_addr,
				   byte_list_t *pid_list_ptr)
{
	uint8_t pid;
	uint8_t fcn_cnt, use_cnt, max_use_cnt, idx;
	uint32_t min_l3_cnt;
	uint8_t sft_pid[MAX_CLUSTERS];

	/* Take the minimum number of even and odd l3 interleaves */
	min_l3_cnt = min(bl_num(&list_l3u_sid[0]),
			 bl_num(&list_l3u_sid[1]));

	/* Determine how many fields will use the same pid value. Fields
	 * of the register are populated based on the minimum number of L3
	 * instances on an intlv.
	 * SFT_PID[n,n+1,n+2,n+3] fields must be programmed with same FCN
	 * Port ID;
	 * SFT_PID[n,n+1] fields must be programmed with same FCN Port ID;
	 * Each SFT_PID# field maps to a single FCN Port ID.
	 */
	if (min_l3_cnt == 1)
		max_use_cnt = 4;
	else if (min_l3_cnt < 4)
		max_use_cnt = 2;
	else
		max_use_cnt = 1;
	fcn_cnt = 0, idx = 0;
	while (idx < bl_num(pid_list_ptr)) {
		/* Get the next ID from the list. */
		pid = bl_ent(pid_list_ptr, idx);
		for (use_cnt = 0; use_cnt < max_use_cnt; use_cnt++) {
			/* Update with a valid PID */
			if (fcn_cnt < MAX_CLUSTERS)
				sft_pid[fcn_cnt++] = pid;
		}
		idx++;
	}
	fcn_cnt = 0;
	while (fcn_cnt < MAX_CLUSTERS) {
		pid = sft_pid[fcn_cnt];
		if (fcn_cnt < (bl_num(pid_list_ptr) * max_use_cnt)) {
			__raw_writel_mask((MBSU_SFT_BID(SU_BUS_ID(pid)) |
					   MBSU_SFT_PID(SU_SWITCH_ID(pid))) <<
					  MBSU_SFT_OFFSET(fcn_cnt),
					  MBSU_SFT_MASK(fcn_cnt),
					  MBSU_SFT_PID_REG(su_cfg_addr, fcn_cnt));
		} else {
			__raw_writel_mask(MBSU_SFT_PID_UNUSED <<
					  MBSU_SFT_OFFSET(fcn_cnt),
					  MBSU_SFT_MASK(fcn_cnt),
					  MBSU_SFT_PID_REG(su_cfg_addr, fcn_cnt));
		}
		fcn_cnt++;
	}
}

static void rs_config_mbsu(monaco_init_t *init_parms_ptr, uint8_t mbsu_id)
{
	uint64_t base = MBSU_BASE(mbsu_id);

	rs_config_common_su(base, init_parms_ptr,
			    &list_ddr_sid[0], &list_ddr_sid[1],
			    &list_l3u_sid[0], &list_l3u_sid[1]);

	/* Set the number of valid FCN/ICN/TN */
	rs_config_mbsu_mnc_node_type_cnt(base);
	/* Configure SFT PID with good L3 interleaves */
	rs_config_mbsu_sft_pid(base, &list_apc_pid);
	/* Map Port IDs of good Hydra clusters for snoop filter. Actual
	 * register settings depend on the minimum number of L3 instances
	 * found on either intlv.
	 */
	rs_config_mbsu_configuration(base, mbsu_id,
				     init_parms_ptr->l3_settings.auto_gate_clk_en,
				     (l3intlv_t)get_llc_mask());
	rs_config_bar_dvm_pos(base, list_l3u_sid);
	rs_config_common_ctl(base, MBSU_CH, &init_parms_ptr->rs_settings);
}

typedef void (rs_config_fn_t)(monaco_init_t *params, uint8_t su_id);

static rs_config_fn_t *rs_config_fn_list[TRSU_CH+1] = {
	[MBSU_CH] = rs_config_mbsu,
	[DMSU_CH] = rs_config_dmsu,
	[PBSU_CH] = rs_config_pbsu,
	[MCSU_CH] = rs_config_mcsu,
	[TRSU_CH] = rs_config_trsu,
};

/******************************************************************************
 * This function provides the top level initialization entry point for all
 * Monaco ring switches. The initialization takes into account the variations
 * in DDR channel population and partial goods results for the CPUs and L3
 * caches.
 ******************************************************************************/
unsigned int rs_init(monaco_init_t *init_parms_ptr)
{
	uint8_t idx, su_type, su_inst;
	rs_goods_t rs_goods;

	fabric_report_status_code(QDT_PROGRESS_CODE,
				  QDT_CU_HP_PC_FABRIC_INIT,
				  QDT_INSTANCE_CODE_INVALID);

	rs_build_flag_platform(init_parms_ptr->mode);
	rs_build_list_l3u_sid((l3intlv_t)get_llc_mask());

	/* Build the Hydra apc_no PID list used to configure the switch registers
	 * Only used for MBSUs
	 */
	rs_build_list_apc_pid(get_cpu_mask());
	rs_build_list_ddr_sid();

	/* Build the usable apc_no flags used to configure the MBSU switch
	 * registers. Indicates which clusters connected to an MBSU are usable.
	 * I.e. Have at least one good core.
	 */
	rs_build_flag_apc(get_cpu_mask());

	/* Check the ring constraints about ddr granule, the number of valid ddr
	 * channels, the number of valid l3 interleaves and the number of apc.
	 */
	rs_build_goods_info(&rs_goods);
	if (rs_check_constraint(&rs_goods,
				&init_parms_ptr->mddr_settings) != 0) {
		fabric_report_status_code(QDT_ERROR_CODE,
					  QDT_CU_HP_EC_FABRIC_INIT_FAIL,
					  QDT_INSTANCE_CODE_INVALID);
		return -EFAULT;
	}

	/* Configure the switches */
	for (idx = 0; idx < RS_CH_TOTAL_NUM; idx++) {
		su_type = (uint8_t)su_hw_switch_lists[idx].su_type;
		su_inst = (uint8_t)su_hw_switch_lists[idx].su_inst;
		(*rs_config_fn_list[su_type])(init_parms_ptr, su_inst);
	}
	rs_config_dlbu(init_parms_ptr);

	fabric_report_status_code(QDT_PROGRESS_CODE,
				  QDT_CU_HP_PC_FABRIC_INIT_DONE,
				  QDT_INSTANCE_CODE_INVALID);
	return 0;
}

/* This function configures ADDR and MASK register sets in DDR_SEGMENT
 * according to chapter 2.4.2 TN Address decoding in
 * HPG 80-Q1234-1H Rev. A
 * Each TCM memory map can be defined in one or two addr/mask pair (A,B).
 * Each address and mask pair defines a power-of-2 address range with a
 * minimum size of 128MB.
 * Notes: Available only for MBSU, DMSU, and PBSU
 */
void rs_config_fatcm_segment(uint64_t base, const uint32_t **seg_lists)
{
	uint32_t fatcm_idx;
	uint32_t addr[2], mask[2];

	for (fatcm_idx = 0; fatcm_idx < SU_MAX_FATCMS; fatcm_idx++) {
		__su_seg_addr_decode(seg_lists[fatcm_idx][0],
				     seg_lists[fatcm_idx][1],
				     FATCM_PA_MSB, FATCM_PA_LSB,
				     addr[fatcm_idx], mask[fatcm_idx]);
	}

#ifdef CONFIG_TCM
	__raw_clearl(SU_SEG_EN, SU_FATCM_ADDR_A_REG(base));
	__raw_writel(SU_PA_43_27_SEG_MASK(mask[0]),
		     SU_FATCM_MASK_A_REG(base));
	__raw_writel(SU_SEG_EN | SU_PA_43_27_SEG_BASE(addr[0]),
		     SU_FATCM_ADDR_A_REG(base));
#else
	__raw_clearl(SU_SEG_EN, SU_FATCM_ADDR_A_REG(base));
	__raw_clearl(SU_SEG_EN, SU_FATCM_ADDR_B_REG(base));
	__raw_writel(SU_PA_43_27_SEG_MASK(mask[0]),
		     SU_FATCM_MASK_A_REG(base));
	__raw_writel(SU_PA_43_27_SEG_MASK(mask[1]),
		     SU_FATCM_MASK_B_REG(base));
	__raw_writel(SU_SEG_EN | SU_PA_43_27_SEG_BASE(addr[0]),
		     SU_FATCM_ADDR_A_REG(base));
	__raw_writel(SU_SEG_EN | SU_PA_43_27_SEG_BASE(addr[1]),
		     SU_FATCM_ADDR_B_REG(base));
#endif
}

/* This is to read the shadow counters of each Rx/Tx Interface0/1 of the
 * Ring BUM in each switch and save them in the structure.
 */
__ddrcode void rs_read_bum_counter(rs_type_e su_type, uint32_t su_id,
				   rs_bum_cntr_t *bum_cnt)
{
	uint64_t base;
	static __ddrconst const uint32_t su_id0[TRSU_CH+1] = {
		[MBSU_CH] = MBSU0,
		[DMSU_CH] = DMSU0,
		[PBSU_CH] = PBSU0,
		[MCSU_CH] = MCSU0,
		[TRSU_CH] = TRSU0,
	};

	base = SU_BASE(su_id0[su_type] + su_id);

	/* FixMe: Check if su_type, su_id and pointer are valid */
	if (su_id0[su_type] != INVALID_SU_ID) {
		bum_cnt->rs_intf0_rx =
			__raw_readl(SU_DCVS_INTF0_RX_SHADOW_CNTR_REG(base));
		bum_cnt->rs_intf0_tx =
			__raw_readl(SU_DCVS_INTF0_TX_SHADOW_CNTR_REG(base));
		bum_cnt->rs_intf1_rx =
			__raw_readl(SU_DCVS_INTF1_RX_SHADOW_CNTR_REG(base));
		bum_cnt->rs_intf1_tx =
			__raw_readl(SU_DCVS_INTF1_TX_SHADOW_CNTR_REG(base));
	}
}
