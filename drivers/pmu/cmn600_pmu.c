#include <target/noc.h>
#include <target/irq.h>
#include <target/cmdline.h>

#define CMN_MAX_GLOBAL_COUNTERS		8
#define CMN_MAX_LOCAL_COUNTERS		4

#define CMN_EVENT_TYPE_OFFSET		0
#define CMN_EVENT_TYPE_MASK		REG_16BIT_MASK
#define CMN_EVENT_TYPE(value)		_SET_FV_ULL(CMN_EVENT_TYPE, value)
#define CMN_EVENT_EVENTID_OFFSET	16
#define CMN_EVENT_EVENTID_MASK		REG_11BIT_MASK
#define CMN_EVENT_EVENTID(value)	_SET_FV_ULL(CMN_EVENT_EVENTID, value)
#define CMN_EVENT_OCCUPID_OFFSET	27
#define CMN_EVENT_OCCUPID_MASK		REG_4BIT_MASK
#define CMN_EVENT_OCCUPID(value)	_SET_FV_ULL(CMN_EVENT_OCCUPID, value)
#define CMN_EVENT_BYNODEID		_BV_ULL(31)
#define CMN_EVENT_NODEID_OFFSET		32
#define CMN_EVENT_NODEID_MASK		REG_16BIT_MASK
#define CMN_EVENT_NODEID(value)		_SET_FV_ULL(CMN_EVENT_NODEID, value)

struct cmn_evt {
	uint8_t type;
#define CMN_EVENT_TYPE_HNF		0
#define CMN_EVENT_TYPE_HNI		1
#define CMN_EVENT_TYPE_RNI		2 /* Including RND */
#define CMN_EVENT_TYPE_XP		3
#define CMN_EVENT_TYPE_DN		4
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

#define _CMN_EVENT_HNF(_name, _eventid, _occupl)	\
	CMN_EVENT_ATTR(_name, CMN_EVENT_TYPE_HNF, CMN_PMU_HN_##_eventid, _occupl)
#define CMN_EVENT_HNF(_name, _eventid)			\
	_CMN_EVENT_HNF(_name, _eventid, false)
#define CMN_EVENT_HNF_OCC(_name, _eventid)		\
	_CMN_EVENT_HNF(_name, _eventid, true)
#define CMN_EVENT_HNI(_name, _eventid)			\
	CMN_EVENT_ATTR(_name, CMN_EVENT_TYPE_HNI, CMN_PMU_HN_##_eventid, false)
#define CMN_EVENT_RNI(_name, _eventid)			\
	CMN_EVENT_ATTR(_name, CMN_EVENT_TYPE_RNI, CMN_PMU_RN_##_eventid, false)

struct cmn_pmu {
	cmn_id_t dtm;
	cmn_id_t dtc;
	uint8_t events[CMN_MAX_LOCAL_COUNTERS];
};

struct cmn_dtm {
	int8_t wp_events[4];
};

struct cmn_dtc {
	uint64_t counters[CMN_MAX_GLOBAL_COUNTERS];
};

struct cmn_evt cmn_events[] = {
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
};

struct cmn_dtm cmn_xp_dtms[CMN_MAX_MXP_COUNT];

static void cmn_dtm_init(cmn_id_t id, caddr_t dtm)
{
	cmn_id_t i;

	cmn_writeq(CMN_pmu_en, CMN_dtm_pmu_config(dtm),
		   "CMN_dtm_pmu_config", -1);
	for (i = 0; i < 4; i++) {
		cmn_xp_dtms[id].wp_events[i] = (uint8_t)-1;
		cmn_writeq(0, CMN_dtm_wp_mask(dtm, i),
			   "CMN_dtm_wp_mask", i);
		cmn_writeq(~0ULL, CMN_dtm_wp_val(dtm, i),
			   "CMN_dtm_wp_val", i);
	}
}

static void cmn_dtc_init(cmn_id_t id, caddr_t dtc, irq_t irq)
{
	cmn_writeq(CMN_dt_en, CMN_dt_dtc_ctl(dtc),
		   "CMN_dt_dtc_ctl", -1);
	cmn_writeq(CMN_pmu_en | CMN_ovfl_intr_en, CMN_dt_pmcr(dtc),
		   "CMN_dt_pmcr", -1);
	cmn_writeq(0, CMN_dt_pmccntr(dtc),
		   "CMN_dt_pmccntr", -1);
	cmn_writeq(0x1ff, CMN_dt_pmovsr_clr(dtc),
		   "CMN_dt_pmovsr_clr", -1);
}

void cmn600_pmu_init(void)
{
	cmn_id_t i;

	for (i = 0; i < cmn_xp_count; i++)
		cmn_dtm_init(i, cmn_bases[cmn_xp_ids[i]]);
	for (i = 0; i < cmn_dtc_count; i++)
		cmn_dtc_init(i, cmn_bases[cmn_dtc_ids[i]], cmn_dtc_irqs[i]);
#if 0
	for (i = 0; i < cmn_cxla_count; i++)
		cmn_cxla_dtm_init(i, cmn_bases[cmn_cxla_ids[i]]);
#endif
}

static int do_cmn_pmu(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	return -EINVAL;
}

DEFINE_COMMAND(cmn_pmu, do_cmn_pmu, "CMN performance manotior unit (PMU) commands",
	"    - dump CMN ndoe information\n"
);
