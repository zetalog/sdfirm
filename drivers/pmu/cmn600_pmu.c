#include <target/perf.h>
#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>

#define CMN_MODNAME			"cmn600"

struct cmn_evt_attr {
	uint8_t type;
	uint8_t eventid;
	bool occupl;
#ifdef CONFIG_CONSOLE_VERBOSE
	const char *name;
#endif
};

#ifdef CONFIG_CONSOLE_VERBOSE
#define CMN_EVENT_ATTR(_name, _type, _eventid, _occupl)	\
	{ _type, _eventid, _occupl, #_name }
#else
#define CMN_EVENT_ATTR(_name, _type, _eventid, _occupl)	\
	{ _type, _eventid, _occupl }
#endif

#define CMN_EVENT_DTC(_name, _eventid)			\
	CMN_EVENT_ATTR(dt_##_name, CMN_EVENT_TYPE_DTC, _eventid, false)
#define _CMN_EVENT_HNF(_name, _eventid, _occupl)	\
	CMN_EVENT_ATTR(hn_##_name, CMN_EVENT_TYPE_HNF, CMN_PMU_HN_##_eventid, _occupl)
#define CMN_EVENT_HNF(_name, _eventid)			\
	_CMN_EVENT_HNF(_name, _eventid, false)
#define CMN_EVENT_HNF_OCC(_name, _eventid)		\
	_CMN_EVENT_HNF(_name, _eventid, true)
#define CMN_EVENT_HNI(_name, _eventid)			\
	CMN_EVENT_ATTR(hn_##_name, CMN_EVENT_TYPE_HNI, CMN_PMU_HN_##_eventid, false)
#define CMN_EVENT_RNI(_name, _eventid)			\
	CMN_EVENT_ATTR(rn_##_name, CMN_EVENT_TYPE_RNI, CMN_PMU_RN_##_eventid, false)
#define CMN_EVENT_SBSX(_name, _eventid)			\
	CMN_EVENT_ATTR(sbsx_##_name, CMN_EVENT_TYPE_SBSX, CMN_PMU_SBSX_##_eventid, false)
#define __CMN_EVENT_XP(_name, _eventid)			\
	CMN_EVENT_ATTR(xp_##_name, CMN_EVENT_TYPE_XP, _eventid, false)
#define _CMN_EVENT_XP_MESH(_name, _eventid)					\
	__CMN_EVENT_XP(e_##_name, (_eventid) | CMN_PMU_XP_IF(CMN_PMU_XP_EAST)),	\
	__CMN_EVENT_XP(w_##_name, (_eventid) | CMN_PMU_XP_IF(CMN_PMU_XP_WEST)),	\
	__CMN_EVENT_XP(n_##_name, (_eventid) | CMN_PMU_XP_IF(CMN_PMU_XP_NORTH)),\
	__CMN_EVENT_XP(s_##_name, (_eventid) | CMN_PMU_XP_IF(CMN_PMU_XP_SOUTH))
#define _CMN_EVENT_XP_PORT(_name, _eventid)					\
	__CMN_EVENT_XP(p0_##_name, (_eventid) | CMN_PMU_XP_IF(CMN_PMU_XP_P0)),	\
	__CMN_EVENT_XP(p1_##_name, (_eventid) | CMN_PMU_XP_IF(CMN_PMU_XP_P1))
#define _CMN_EVENT_XP(_name, _eventid)						\
	_CMN_EVENT_XP_MESH(_name, _eventid),					\
	_CMN_EVENT_XP_PORT(_name, _eventid)
#define CMN_EVENT_XP(_name, _eventid)						\
	_CMN_EVENT_XP(req_##_name, (_eventid) | CMN_PMU_XP_PC(CMN_PMU_XP_REQ)),	\
	_CMN_EVENT_XP(rsp_##_name, (_eventid) | CMN_PMU_XP_PC(CMN_PMU_XP_RSP)),	\
	_CMN_EVENT_XP(snp_##_name, (_eventid) | CMN_PMU_XP_PC(CMN_PMU_XP_SNP)),	\
	_CMN_EVENT_XP(dat_##_name, (_eventid) | CMN_PMU_XP_SP(CMN_PMU_XP_DAT))
#define CMN_EVENT_XP_DAT(_name, _eventid)					\
	_CMN_EVENT_XP_PORT(dat_##_name, (_eventid) | CMN_PMU_XP_PC(CMN_PMU_XP_DAT))

struct cmn_pmu {
	cmn_id_t pmu; /* PMU event node */
	uint16_t type;
	cmn_id_t dtc; /* DTC domain controller node */
	cmn_id_t dtm; /* MXP DTM node */
	struct {
		uint8_t val : 4;
		uint8_t count : 4;
	} occupl_id;
	uint8_t events[CMN_MAX_LOCAL_COUNTERS];
};

struct cmn_event {
	uint64_t attr;
	struct cmn_pmu *dn;
	uint64_t dtm_idx[4];
	int8_t dtc_idx[CMN_MAX_DTCS];
	uint8_t num_dns;
	uint8_t dtm_offset;
	bool filter_sel;
};

struct cmn_dtm {
	cmn_id_t dtc; /* DTC domain controller node */
	uint32_t pmu_config_low;
	union {
		uint8_t input_sel[CMN_MAX_LOCAL_COUNTERS];
		uint32_t pmu_config_high;
	};
	int8_t wp_events[4];
};

struct cmn_dtc {
	uint8_t state;
	irq_t irq;
#define CMN_STATE_DISABLED	_BV(0)
#define CMN_STATE_TXN		_BV(1)
	struct cmn_event *counters[CMN_MAX_GLOBAL_COUNTERS];
	struct cmn_event *cycles;
};

struct cmn_val {
	uint8_t dtm_count[CMN_MAX_MXP_COUNT];
	int8_t dtc_count[CMN_MAX_DTCS];
	uint8_t occupl_id[CMN_MAX_DTMS];
	uint8_t wp[CMN_MAX_DTMS][4];
	bool cycles;
};

struct cmn_evt_attr cmn_events[] = {
	CMN_EVENT_DTC(cycles, 0),

	CMN_EVENT_HNF(cache_miss, CACHE_MISS),
	CMN_EVENT_HNF(sf_cache_access, SLC_SF_CACHE_ACCESS),
	CMN_EVENT_HNF(cache_fill, CACHE_FILL),
	CMN_EVENT_HNF(pocq_retry, POCQ_RETRY),
	CMN_EVENT_HNF(pocq_reqs_recvd, POCQ_REQS_RECVD),
	CMN_EVENT_HNF(sf_hit, SF_HIT),
	CMN_EVENT_HNF(sf_evictions, SF_EVICTIONS),
	CMN_EVENT_HNF(dir_snoops_sent, DIR_SNOOPS_SENT),
	CMN_EVENT_HNF(brd_snoops_sent, BRD_SNOOPS_SENT),
	CMN_EVENT_HNF(slc_eviction, SLC_EVICTION),
	CMN_EVENT_HNF(slc_fill_invalid_way, SLC_FILL_INVALID_WAY),
	CMN_EVENT_HNF(mc_retries, MC_RETRIES),
	CMN_EVENT_HNF(mc_reqs, MC_REQS),
	CMN_EVENT_HNF(qos_hh_retry, QOS_HH_RETRY),
	CMN_EVENT_HNF_OCC(pocq_occupancy, POCQ_OCCUPANCY),
	CMN_EVENT_HNF(pocq_addrhz, POCQ_ADDRHAZ),
	CMN_EVENT_HNF(pocq_atomics_addrhz, POCQ_ATOMICS_ADDRHAZ),
	CMN_EVENT_HNF(ld_st_swp_adq_full, LD_ST_SWP_ADQ_FULL),
	CMN_EVENT_HNF(cmp_adq_full, CMP_ADQ_FULL),
	CMN_EVENT_HNF(txdat_stall, TXDAT_STALL),
	CMN_EVENT_HNF(txrsp_stall, TXRSP_STALL),
	CMN_EVENT_HNF(seq_full, SEQ_FULL),
	CMN_EVENT_HNF(seq_hit, SEQ_HIT),
	CMN_EVENT_HNF(snp_sent, SNP_SENT),
	CMN_EVENT_HNF(sfbi_dir_snp_sent, SFBI_DIR_SNP_SENT),
	CMN_EVENT_HNF(sfbi_brd_snp_sent, SFBI_BRD_SNP_SENT),
	CMN_EVENT_HNF(snp_sent_untrk, SNP_SENT_UNTRK),
	CMN_EVENT_HNF(intv_dirty, INTV_DIRTY),
	CMN_EVENT_HNF(stash_snp_sent, STASH_SNP_SENT),
	CMN_EVENT_HNF(stash_data_pull, STASH_DATA_PULL),
	CMN_EVENT_HNF(snp_fwded, SNP_FWDED),

	CMN_EVENT_HNI(rrt_rd_occupancy_cnt_ovfl, RRT_RD_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_HNI(rrt_wr_occupancy_cnt_ovfl, RRT_WR_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_HNI(rdt_rd_occupancy_cnt_ovfl, RDT_RD_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_HNI(rdt_wr_occupancy_cnt_ovfl, RDT_WR_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_HNI(wdb_occupancy_cnt_ovfl, WDB_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_HNI(rrt_rd_allocation, RRT_RD_ALLOCATION),
	CMN_EVENT_HNI(rrt_wr_allocation, RRT_WR_ALLOCATION),
	CMN_EVENT_HNI(rdt_rd_allocation, RDT_RD_ALLOCATION),
	CMN_EVENT_HNI(rdt_wr_allocation, RDT_WR_ALLOCATION),
	CMN_EVENT_HNI(wdb_allocation, WDB_ALLOCATION),
	CMN_EVENT_HNI(txrspflitv_retried, TXRSPFLITV_RETIRIED),
	CMN_EVENT_HNI(arvalid_no_arready, ARVALID_NO_ARREADY),
	CMN_EVENT_HNI(arready_no_arvalid, ARREADY_NO_ARVALID),
	CMN_EVENT_HNI(awvalid_no_awready, AWVALID_NO_AWREADY),
	CMN_EVENT_HNI(awready_no_awvalid, AWREADY_NO_AWVALID),
	CMN_EVENT_HNI(wvalid_no_wready, WVALID_NO_WREADY),
	CMN_EVENT_HNI(txdatfiltv_no_linkcrd, TXDATFLITV_NO_LINKCRD),
	CMN_EVENT_HNI(nonpcie_serialized, NONPCIE_SERIALIZED),
	CMN_EVENT_HNI(pcie_serialized, PCIE_SERIALIZED),

	CMN_EVENT_RNI(rdatabeats_p0, RDATABEATS_P0),
	CMN_EVENT_RNI(rdatabeats_p1, RDATABEATS_P1),
	CMN_EVENT_RNI(rdatabeats_p2, RDATABEATS_P2),
	CMN_EVENT_RNI(rxdatflitv, RXDATFLITV),
	CMN_EVENT_RNI(txdatflitv, TXDATFLITV),
	CMN_EVENT_RNI(txreqflitv, TXREQFLITV),
	CMN_EVENT_RNI(txreqflitv_retried, TXREQFLITV_RETRIED),
	CMN_EVENT_RNI(rrt_occupancy, RRT_OCCUPANCY),
	CMN_EVENT_RNI(wrt_occupancy, WRT_OCCUPANCY),
	CMN_EVENT_RNI(txreqflitv_replayed, TXREQFLITV_REPLAYED),
	CMN_EVENT_RNI(wrcancel_sent, WRCANCEL_SENT),
	CMN_EVENT_RNI(wdatabeat_p0, WDATABEAT_P0),
	CMN_EVENT_RNI(wdatabeat_p1, WDATABEAT_P1),
	CMN_EVENT_RNI(wdatabeat_p2, WDATABEAT_P2),
	CMN_EVENT_RNI(rrtalloc, RRTALLOC),
	CMN_EVENT_RNI(wrtalloc, WRTALLOC),
	CMN_EVENT_RNI(rdb_unord, RDB_UNORD),
	CMN_EVENT_RNI(rdb_replay, RDB_REPLAY),
	CMN_EVENT_RNI(rdb_hybrid, RDB_HYBRID),
	CMN_EVENT_RNI(rdb_ord, RDB_ORD),

	CMN_EVENT_SBSX(rxdat, RXDAT),
	CMN_EVENT_SBSX(txdat, TXDAT),
	CMN_EVENT_SBSX(cmo, CMO),
	CMN_EVENT_SBSX(txrspflitv_retired, TXRSPFLITV_RETIRED),
	CMN_EVENT_SBSX(txdatflitv_total, TXDATFLITV_TOTAL),
	CMN_EVENT_SBSX(txrspflitv_total, TXRSPFLITV_TOTAL),
	CMN_EVENT_SBSX(rrt_rd_occupancy_cnt_ovfl, RRT_RD_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_SBSX(rrt_wr_occupancy_cnt_ovfl, RRT_WR_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_SBSX(rrt_cmo_occupancy_cnt_ovfl, RRT_CMO_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_SBSX(wdb_occupancy_cnt_ovfl, WDB_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_SBSX(rdt_rd_occupancy_cnt_ovfl, RDT_RD_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_SBSX(rdt_cmo_occupancy_cnt_ovfl, RDT_CMO_OCCUPANCY_CNT_OVFL),
	CMN_EVENT_SBSX(arvalid_no_arready, ARVALID_NO_ARREADY),
	CMN_EVENT_SBSX(awvalid_no_awready, AWVALID_NO_AWREADY),
	CMN_EVENT_SBSX(wvalid_no_wready, WVALID_NO_WREADY),
	CMN_EVENT_SBSX(txdatflitv_no_linkcrd, TXDATFLITV_NO_LINKCRD),
	CMN_EVENT_SBSX(txrspflitv_no_linkcrd, TXRSPFLITV_NO_LINKCRD),

	CMN_EVENT_XP(txflit_valid, CMN_PMU_XP_TXFLIT_VALID),
	CMN_EVENT_XP(txflit_stall, CMN_PMU_XP_TXFLIT_STALL),
	CMN_EVENT_XP_DAT(partial_dat_flit_valid, CMN_PMU_XP_PARTIAL_DAT_FLIT_VALID),

	CMN_EVENT_ATTR(watchpoint_up, CMN_EVENT_TYPE_WP, CMN_WP_UP, false),
	CMN_EVENT_ATTR(watchpoint_dn, CMN_EVENT_TYPE_WP, CMN_WP_DN, false),
};

struct cmn_dtm cmn_dtms[CMN_MAX_MXP_COUNT];
struct cmn_dtc cmn_dtcs[CMN_MAX_DTCS];
struct cmn_pmu cmn_pmus[NR_CMN_NODES];
cmn_id_t cmn_pmu_count;
struct cmn_event cmn_evts[CMN_MAX_COUNTERS];

static void cmn_pmu_set_state(cmn_id_t dtc, uint8_t state)
{
	if (!cmn_dtcs[dtc].state)
		cmn_writeq(0, CMN_dt_pmcr(cmn_bases[cmn_dtc_ids[dtc]]),
			   CMN_dt_pmcr, -1);
	cmn_dtcs[dtc].state |= state;
}

static void cmn_pmu_clear_state(cmn_id_t dtc, uint8_t state)
{
	cmn_dtcs[dtc].state &= ~state;
	if (!cmn_dtcs[dtc].state)
		cmn_writeq(CMN_pmu_en | CMN_ovfl_intr_en,
			   CMN_dt_pmcr(cmn_bases[cmn_dtc_ids[dtc]]),
			   CMN_dt_pmcr, -1);
}

static bool cmn_pmu_filter_sel(uint16_t type, uint16_t eventid)
{
	cmn_id_t i;

	for (i = 0; i < ARRAY_SIZE(cmn_events); i++) {
		if (cmn_events[i].type == type && cmn_events[i].eventid == eventid)
			return cmn_events[i].occupl;
	}
	return false;
}

static bool cmn_pmu_match_type(uint16_t node_type, uint16_t event_type)
{
	switch (event_type) {
	case CMN_EVENT_TYPE_HNF:
		if (node_type == CMN_HNF)
			return true;
		break;
	case CMN_EVENT_TYPE_HNI:
		if (node_type == CMN_HNI)
			return true;
		break;
	case CMN_EVENT_TYPE_RNI:
		if (node_type == CMN_RNI || node_type == CMN_RND)
			return true;
		break;
	case CMN_EVENT_TYPE_SBSX:
		if (node_type == CMN_SBSX)
			return true;
		break;
	case CMN_EVENT_TYPE_XP:
		if (node_type == CMN_XP)
			return true;
		break;
	}
	return false;
}

struct cmn_pmu *cmn_pmu_nodes(uint16_t type)
{
	cmn_id_t pmu;

	for (pmu = 0; pmu < cmn_pmu_count; pmu++) {
		if (cmn_pmu_match_type(cmn_pmus[pmu].type, type))
			return &cmn_pmus[pmu];
	}
	return NULL;
}

static inline cmn_nid_t cmn_pmu_nid(struct cmn_pmu *pmu)
{
	return cmn_node_id(cmn_bases[pmu->pmu]);
}

static bool cmn_pmu_set_event_sel_hi(struct cmn_pmu *dn,
				     bool fsel, uint8_t occupl_id)
{
	if (!fsel)
		return true;

	if (!dn->occupl_id.count) {
		dn->occupl_id.val = occupl_id;
		cmn_writeq_mask(CMN_pmu_occupl_id(dn->occupl_id.val),
				CMN_pmu_occupl_id(CMN_pmu_occupl_id_MASK),
				CMN_pmu_event_sel(cmn_bases[dn->pmu]),
				CMN_pmu_event_sel, -1);
	} else if (dn->occupl_id.val != occupl_id)
		return false;
	dn->occupl_id.count++;
	return true;
}

static void cmn_pmu_set_event_sel_lo(struct cmn_pmu *dn,
				     cmn_id_t dtm_idx, uint8_t eventid)
{
	dn->events[dtm_idx] = eventid;
	cmn_writeq_mask(CMN_pmu_event_id(dtm_idx, dn->events[dtm_idx]),
			CMN_pmu_event_id(dtm_idx, CMN_pmu_event_id_MASK),
			CMN_pmu_event_sel(cmn_bases[dn->pmu]),
			CMN_pmu_event_sel, -1);
}

static inline void cmn_pmu_set_index(uint64_t x[], cmn_id_t pos, uint32_t val)
{
	x[pos / 32] |= (uint64_t)val << ((pos % 32) * 2);
}

static inline uint32_t cmn_pmu_get_index(uint64_t x[], cmn_id_t pos)
{
	return (x[pos / 32] >> ((pos % 32) * 2)) & 3;
}

#define for_each_dtc(evt, i)		\
	for (i = 0; i < cmn_dtc_count; i++) if ((evt)->dtc_idx[i] >= 0)
#define for_each_pmu(evt, dn, i)	\
	for (i = 0, dn = (evt)->dn; i < (evt)->num_dns; i++, dn++)

#ifdef CONFIG_CMN600_PMU_WP
static void cmn_pmu_add_wp(struct cmn_event *event, cmn_id_t dtm, struct cmn_val *val)
{
	cmn_id_t wp = cmn_pmu_wp_idx(event);

	val->wp[dtm][wp] = CMN_EVENT_WP_COMBINE(event->attr) + 1;
}

static bool cmn_pmu_val_wp(struct cmn_event *event, cmn_id_t dtm, struct cmn_val *val)
{
	cmn_id_t wp = cmn_pmu_wp_idx(event);
	int wp_cmb;

	if (val->wp[dtm][wp])
		return false;
	wp_cmb = val->wp[dtm][wp ^ 1];
	if (wp_cmb && wp_cmb != CMN_EVENT_WP_COMBINE(event->attr) + 1)
		return false;
	return true;
}

static bool cmn_pmu_sel_wp(struct cmn_event *event, cmn_id_t dtm)
{
	return false;
}

static void cmn_pmu_clr_wp(struct cmn_event *event)
{
}
#else
#define cmn_pmu_add_wp(event, dtm, val)	do { } while (0)
#define cmn_pmu_val_wp(event, dtm, val)	false
#define cmn_pmu_sel_wp(event, dtm)	false
#define cmn_pmu_clr_wp(event)		do { } while (0)
#endif

static void cmn_pmu_val_add_event(struct cmn_event *event, struct cmn_val *val)
{
	cmn_id_t dtc;
	cmn_id_t i;
	struct cmn_pmu *dn;
	uint16_t type;

	type = CMN_EVENT_TYPE(event->attr);
	if (type == CMN_EVENT_TYPE_DTC)
		val->cycles = true;
	for_each_dtc(event, dtc)
		val->dtc_count[dtc]++;
	for_each_pmu(event, dn, i) {
		cmn_id_t dtm = dn->dtm;
		bool sel = event->filter_sel;

		val->dtm_count[dtm]++;
		if (sel)
			val->occupl_id[dtm] = CMN_EVENT_OCCUPLID(event->attr) + 1;
		if (type != CMN_EVENT_TYPE_WP)
			continue;
		cmn_pmu_add_wp(event, dtm, val);
	}
}

static int cmn_pmu_validate_group(struct cmn_event *event)
{
	uint16_t type;
	cmn_id_t i;
	struct cmn_pmu *dn;
	struct cmn_val val;

	cmn_pmu_val_add_event(event, &val);

	type = CMN_EVENT_TYPE(event->attr);
	if (type == CMN_EVENT_TYPE_DTC)
		return val.cycles ? -EINVAL : 0;

	for (i = 0; i < cmn_dtc_count; i++)
		if (val.dtc_count[i] == CMN_MAX_GLOBAL_COUNTERS)
			return -EINVAL;

	for_each_pmu(event, dn, i) {
		cmn_id_t dtm = dn->dtm;
		bool sel = event->filter_sel;

		if (val.dtm_count[dtm] == CMN_MAX_GLOBAL_COUNTERS)
			return -EINVAL;
		if (sel && val.occupl_id[dtm] &&
		    val.occupl_id[dtm] != CMN_EVENT_OCCUPLID(event->attr) + 1)
			return -EINVAL;
		if (type != CMN_EVENT_TYPE_WP)
			continue;
		if (!cmn_pmu_val_wp(event, dtm, val))
			return -EINVAL;
	}
	return 0;
}

static int cmn_pmu_event_init(struct cmn_event *event)
{
	uint16_t type;
	bool bynodeid;
	uint16_t nodeid;
	uint16_t eventid;
	struct cmn_pmu *dn;

	type = CMN_EVENT_TYPE(event->attr);
	if (type == CMN_EVENT_TYPE_DTC)
		return cmn_pmu_validate_group(event);

	eventid = CMN_EVENT_EVENTID(event->attr);
	if (type == CMN_EVENT_TYPE_WP) {
		type = CMN_EVENT_TYPE_XP;
		if (eventid != CMN_WP_UP && eventid != CMN_WP_DN) {
			con_err(CMN_MODNAME ": invalid watchpoint event: event=%d\n",
				eventid);
			return -ENODEV;
		}
	}

	event->filter_sel = cmn_pmu_filter_sel(type, eventid);
	bynodeid = !!(event->attr & CMN_EVENT_BYNODEID);
	nodeid = CMN_EVENT_NODEID(event->attr);

	event->dn = cmn_pmu_nodes(type);
	if (!event->dn) {
		con_err(CMN_MODNAME ": no event node: type=%d\n", type);
		return -EINVAL;
	}
	event->num_dns = 0;

	memset(event->dtc_idx, -1, sizeof(event->dtc_idx));
	for (dn = event->dn; cmn_pmu_match_type(dn->type, type); dn++) {
		if (bynodeid && cmn_pmu_nid(dn) != nodeid) {
			event->dn++;
			continue;
		}
		event->num_dns++;
		if (dn->dtc < 0)
			memset(event->dtc_idx, 0, cmn_dtc_count);
		else
			event->dtc_idx[dn->dtc] = 0;

		if (bynodeid)
			break;
	}
	if (!event->num_dns) {
		con_err(CMN_MODNAME ": no event node: nid=%d\n", nodeid);
		return -EINVAL;
	}

	return cmn_pmu_validate_group(event);
}

static void cmn_pmu_event_clear(struct cmn_event *event, cmn_id_t i)
{
	uint16_t type = CMN_EVENT_TYPE(event->attr);
	cmn_id_t j, idx;

	while (i--) {
		struct cmn_dtm *dtm = &cmn_dtms[event->dn[i].dtm];
		cmn_id_t dtm_idx = cmn_pmu_get_index(event->dtm_idx, i);

		if (type == CMN_EVENT_TYPE_WP)
			cmn_pmu_clr_wp(event);
		if (event->filter_sel)
			event->dn[i].occupl_id.count--;

		dtm->pmu_config_low &= ~CMN_pmevcnt_paired(dtm_idx);
		cmn_writeq(dtm->pmu_config_low,
			   CMN_dtm_pmu_config(cmn_bases[event->dn[i].dtm]),
			   CMN_dtm_pmu_config, -1);
	}
	memset(event->dtm_idx, 0, sizeof(event->dtm_idx));

	for_each_dtc(event, j) {
		idx = event->dtc_idx[j];
		if (idx >= 0)
			cmn_dtcs[j].counters[idx] = NULL;
	}
}

static void cmn_pmu_init_counter(struct cmn_event *event)
{
}

static int cmn_pmu_event_add(struct cmn_event *event)
{
	cmn_id_t i;
	cmn_id_t dtc;
	uint16_t type;
	struct cmn_pmu *dn;

	type = CMN_EVENT_TYPE(event->attr);
	if (type == CMN_EVENT_TYPE_DTC) {
		for (i = 0; i < cmn_dtc_count; i++)
			if (!cmn_dtcs[i].cycles)
				break;
		if (i == cmn_dtc_count)
			return -ENODEV;

		cmn_dtcs[i].cycles = event;
		event->dtc_idx[0] = i;
		return 0;
	}

	/* Initialize global counter */
	for_each_dtc(event, dtc) {
		for (i = 0; i < CMN_MAX_GLOBAL_COUNTERS; i++)
			if (!cmn_dtcs[dtc].counters[i])
				break;
		if (i == CMN_MAX_GLOBAL_COUNTERS)
			return -ENODEV;
		event->dtc_idx[dtc] = i;
	}

	/* Initialize local counters */
	for_each_pmu(event, dn, i) {
		struct cmn_dtm *dtm = &cmn_dtms[dn->dtm];
		cmn_id_t dtm_idx;
		uint8_t input_sel;
		cmn_nid_t nid;

		dtm_idx = 0;
		while (dtm->pmu_config_low & CMN_pmevcnt_paired(dtm_idx))
			if (++dtm_idx == CMN_MAX_GLOBAL_COUNTERS)
				goto free_dtms;

		if (type == CMN_EVENT_TYPE_XP)
			input_sel = CMN_pmevcnt_input_sel_xp(dtm_idx);
		else if (type == CMN_EVENT_TYPE_WP) {
			if (!cmn_pmu_sel_wp(event, dn->dtm))
				goto free_dtms;
		} else {
			nid = cmn_pmu_nid(dn);
			input_sel = CMN_pmevcnt_input_sel_dev(CMN_PID(nid),
							      CMN_DID(nid),
							      dtm_idx);

			if (!cmn_pmu_set_event_sel_hi(dn, event->filter_sel,
						      CMN_EVENT_OCCUPLID(event->attr)))
				goto free_dtms;
		}

		cmn_pmu_set_index(event->dtm_idx, i, dtm_idx);

		dtm->input_sel[dtm_idx] = input_sel;
	}

	/* Go! */
	cmn_pmu_init_counter(event);

free_dtms:
	cmn_pmu_event_clear(event, i);
	return -ENODEV;
}

static void cmn_pmu_dtm_init(cmn_id_t id, caddr_t dtm)
{
	cmn_id_t i;
	cmn_nid_t nid;

	cmn_writeq(CMN_pmu_en, CMN_dtm_pmu_config(dtm),
		   "CMN_dtm_pmu_config", -1);
	nid = cmn_node_id(cmn_bases[id]);
	cmn_dtms[id].dtc = cmn600_hw_dtc_domain(nid);
	for (i = 0; i < 4; i++) {
		cmn_dtms[id].wp_events[i] = (uint8_t)-1;
		cmn_writeq(0, CMN_dtm_wp_mask(dtm, i),
			   "CMN_dtm_wp_mask", i);
		cmn_writeq(~0ULL, CMN_dtm_wp_val(dtm, i),
			   "CMN_dtm_wp_val", i);
	}
}

static void cmn_pmu_dtc_init(cmn_id_t id, caddr_t dtc, irq_t irq)
{
	cmn_writeq(CMN_dt_en, CMN_dt_dtc_ctl(dtc),
		   "CMN_dt_dtc_ctl", -1);
	cmn_writeq(CMN_pmu_en | CMN_ovfl_intr_en, CMN_dt_pmcr(dtc),
		   "CMN_dt_pmcr", -1);
	cmn_writeq(0, CMN_dt_pmccntr(dtc),
		   "CMN_dt_pmccntr", -1);
	cmn_writeq(0x1ff, CMN_dt_pmovsr_clr(dtc),
		   "CMN_dt_pmovsr_clr", -1);
	cmn_pmu_set_state(id, CMN_STATE_DISABLED);
}

static void cmn_pmu_pmu_init(cmn_id_t id, uint16_t type)
{
	cmn_id_t pmu;
	cmn_nid_t nid;

	if (cmn_pmu_count >= NR_CMN_NODES)
		return;

	pmu = cmn_pmu_count;
	nid = cmn_node_id(cmn_bases[id]);
	cmn_pmus[pmu].pmu = id;
	cmn_pmus[pmu].type = type;
	cmn_pmus[pmu].dtc = cmn600_hw_dtc_domain(nid);
	cmn_pmus[pmu].dtm = cmn600_nid2xp(nid);
	cmn_pmu_count++;
}

void cmn600_pmu_configure_event(perf_evt_t evt)
{
	uint64_t attr = PERF_EVENT_TO_CMN_EVENT(evt);
	uint16_t cnt = CMN_EVENT_COUNTER(evt);

	if (cnt >= CMN_MAX_COUNTERS)
		return;
	cmn_pmu_event_add(&cmn_evts[cnt]);
}

void cmn600_pmu_enable_event(perf_evt_t evt)
{
}

void cmn600_pmu_disable_event(perf_evt_t evt)
{
}

perf_cnt_t cmn600_pmu_get_event_count(perf_evt_t evt)
{
	return 0;
}

void cmn600_pmu_set_event_count(perf_evt_t evt, perf_cnt_t cnt)
{
}

void cmn600_pmu_start(void)
{
}

void cmn600_pmu_stop(void)
{
}

void cmn600_pmu_init(void)
{
	cmn_id_t i;

	for (i = 0; i < cmn_xp_count; i++)
		cmn_pmu_dtm_init(i, cmn_bases[cmn_xp_ids[i]]);
#if 0
	/* TODO: CXLA DTM support */
	for (i = 0; i < cmn_cxla_count; i++)
		cmn_cxla_dtm_init(i, cmn_bases[cmn_cxla_ids[i]]);
#endif
	cmn_pmu_count = 0;
	/* TODO: DVM PMU support */
	for (i = 0; i < cmn_dtc_count; i++) {
		cmn_pmu_pmu_init(cmn_dtc_ids[i], CMN_DTC);
		cmn_pmu_dtc_init(i, cmn_bases[cmn_dtc_ids[i]], cmn_dtc_irqs[i]);
	}
	for (i = 0; i < cmn_hnf_count; i++)
		cmn_pmu_pmu_init(cmn_hnf_ids[i], CMN_HNF);
	for (i = 0; i < cmn_hni_count; i++)
		cmn_pmu_pmu_init(cmn_hni_ids[i], CMN_HNI);
	for (i = 0; i < cmn_rni_count; i++)
		cmn_pmu_pmu_init(cmn_rni_ids[i], CMN_RNI);
	for (i = 0; i < cmn_rnd_count; i++)
		cmn_pmu_pmu_init(cmn_rnd_ids[i], CMN_RND);
	for (i = 0; i < cmn_sbsx_count; i++)
		cmn_pmu_pmu_init(cmn_sbsx_ids[i], CMN_SBSX);
#if 0
	/* TODO: CXHA/CXRA PMU support */
	for (i = 0; i < cmn_cxha_count; i++)
		cmn_pmu_pmu_init(cmn_cxha_ids(i), CMN_CXHA);
	for (i = 0; i < cmn_cxra_count; i++)
		cmn_pmu_pmu_init(cmn_cxra_ids(i), CMN_CXRA);
#endif
}

static void do_cmn_pmu_dump_event(uint16_t type)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(cmn_events); i++) {
		if (cmn_pmu_match_type(type, cmn_events[i].type))
			printf("  %s%c\n", cmn_events[i].name,
			       cmn_events[i].occupl ? '*' : '\0');
	}
}

static int do_cmn_pmu_dump(int argc, char *argv[])
{
	cmn_id_t i;
	unsigned long long base;

	if (argc < 3) {
	} else if (strcmp(argv[2], "pmu") == 0) {
		for (i = 0; i < cmn_pmu_count; i++) {
			base = cmn_bases[cmn_pmus[i].pmu];
			printf("PMU %d: %s-%d, DTC=DTC-%d, DTM=XP-%d, %016llx\n",
			       i,
			       cmn600_node_type_name(cmn_pmus[i].type), cmn_node_id(base),
			       cmn_node_id(cmn_bases[cmn_pmus[i].dtc]),
			       cmn_node_id(cmn_bases[cmn_pmus[i].dtm]),
			       base);
			do_cmn_pmu_dump_event(cmn_pmus[i].type);
		}
	} else if (strcmp(argv[2], "dtm") == 0) {
		for (i = 0; i < cmn_xp_count; i++) {
			base = cmn_bases[cmn_xp_ids[i]];
			printf("DTM %d: %s-%d, DTC=DTC-%d, %016llx\n",
			       i,
			       cmn600_node_type_name(CMN_XP), cmn_node_id(base),
			       cmn_node_id(cmn_bases[cmn_dtms[i].dtc]),
			       base);
			do_cmn_pmu_dump_event(CMN_XP);
		}
	}

	return 0;
}

static int do_cmn_pmu_add(int argc, char *argv[])
{
	/* cmn_pmu_event_init() */
	return 0;
}

static int do_cmn_pmu_del(int argc, char *argv[])
{
	/* cmn_pmu_event_deinit() */
	return 0;
}

static int do_cmn_pmu(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0) {
		do_cmn_pmu_dump(argc, argv);
		return 0;
	}
	if (strcmp(argv[1], "add") == 0) {
		do_cmn_pmu_add(argc, argv);
		return 0;
	}
	if (strcmp(argv[1], "del") == 0) {
		do_cmn_pmu_del(argc, argv);
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(cmn_pmu, do_cmn_pmu, "CMN performance manotior unit (PMU) commands",
	"cmn_pmu dump dtm\n"
	"    - dump DTM ndoe information\n"
	"cmn_pmu dump pmu\n"
	"    - dump non-DTM PMU ndoe information\n"
	"cmn_pmu dump\n"
	"    - dump global counter events\n"
	"cmn_pmu add <count> <event> [occupl_id] [node_id]"
	"    - add event to global counter\n"
	"cmn_pmu del <count>"
	"    - remove event from global counter\n"
);
