#include <target/noc.h>
#include <target/irq.h>
#include <target/cmdline.h>

struct cmn_dtm {
	uint8_t events[4];
};

struct cmn_dtm cmn_xp_dtms[CMN_MAX_MXP_COUNT];

static void cmn_dtm_init(cmn_id_t id, caddr_t dtm)
{
	cmn_id_t i;

	cmn_writeq(CMN_pmu_en, CMN_dtm_pmu_config(dtm),
		   "CMN_dtm_pmu_config", -1);
	for (i = 0; i < 4; i++) {
		cmn_xp_dtms[id].events[i] = (uint8_t)-1;
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
