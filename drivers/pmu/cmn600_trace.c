#include <target/noc.h>
#include <target/perf.h>
#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/arch.h>

static int cmn_trace_wp_config(caddr_t base, int wp, int dev_sel,
				int chn_sel, int pkt_type, int pkt_gen,
				int combine, int cc_en, int ctrigger_en,
				int dbgtrigger_en)
{
	uint64_t val = 0;

	val |= dtm_wp_cfg_dev_sel;
	val |= dtm_wp_cfg_chn_sel(chn_sel);
	if (dev_sel)
		val |= dtm_wp_cfg_dev_sel;
	if (combine)
		val |= dtm_wp_cfg_combine;
	if (pkt_gen)
		val |= dtm_wp_cfg_pkt_gen;
	val |= dtm_wp_cfg_pkt_type(pkt_type);
	if (cc_en)
		val |= dtm_wp_cfg_cc_en;
	if (ctrigger_en)
		val |= dtm_wp_cfg_ctrig_en;
	if (dbgtrigger_en)
		val |= dtm_wp_cfg_dbgtrig_en;

	__raw_writeq(val, CMN_dtm_wp_config(base, wp));
	return 0;
}

static int cmn_trace_wp_val_mask(caddr_t base, int wp, uint64_t val, uint64_t mask)
{
	__raw_writeq(val, CMN_dtm_wp_val(base, wp));
	__raw_writeq(mask, CMN_dtm_wp_mask(base, wp));
	return 0;
}

static int cmn_trace_wp_enable(caddr_t base, int wp, int enable)
{
	uint64_t val = __raw_readq(CMN_dtm_wp_config(base, wp));
	if (enable)
		val |= dtm_control_en;
	else
		val &= ~dtm_control_en;
	__raw_writeq(val, CMN_dtm_wp_config(base, wp));
	return 0;
}

static int cmn_trace_wp_setup(caddr_t base, int wp, int dev_sel,
				  int chn_sel, int pkt_type, int pkt_gen,
				  int combine, int cc_en, int ctrigger_en,
				  int dbgtrigger_en, uint64_t val, uint64_t mask)
{
	cmn_trace_wp_config(base, wp, dev_sel, chn_sel, pkt_type,
			    pkt_gen, combine, cc_en, ctrigger_en,
			    dbgtrigger_en);
	cmn_trace_wp_val_mask(base, wp, val, mask);
	return cmn_trace_wp_enable(base, wp, 1);
}

static int cmn_trace_dt_setup(caddr_t base, int dbgtrigger_en,
			      int atbtrigger_en, int wait_for_trigger)
{
	uint64_t val = 0;

	if (dbgtrigger_en)
		val |= dt_dtc_ctl_dbgtrigger_en;
	if (atbtrigger_en)
		val |= dt_dtc_ctl_atbtrigger_en;
	if (wait_for_trigger)
		val |= dt_dtc_ctl_wait_for_trigger;
	val |= dt_dtc_ctl_en;

	__raw_setq(dt_trace_control_cc_enable, CMN_dt_trace_control(base));
	__raw_writeq(val, CMN_dt_ctl(base));
	return 0;
}

static int do_cmn_trace(int argc, char *argv[])
{
	int i, id;
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		for (i = 0; i < cmn_dtc_count; i++) {
			caddr_t base = cmn_bases[cmn_dtc_ids[i]];
			printf("DTC-%d: %03d, %016llx\n",
			       cmn_logical_id(base), cmn_node_id(base), base);
		}
		return 0;
	}
	if (strcmp(argv[1], "wp_setup") == 0) {
		if (argc < 14)
			return -EINVAL;
		id = atoi(argv[2]);
		if (id < 0 || id >= cmn_dtc_count)
			return -EINVAL;
		caddr_t base = cmn_bases[cmn_dtc_ids[id]];
		int wp = atoi(argv[3]);
		int dev_sel = atoi(argv[4]);
		int chn_sel = atoi(argv[5]);
		int pkt_type = atoi(argv[6]);
		int pkt_gen = atoi(argv[7]);
		int combine = atoi(argv[8]);
		int cc_en = atoi(argv[9]);
		int ctrigger_en = atoi(argv[10]);
		int dbgtrigger_en = atoi(argv[11]);
		uint64_t val = strtoull(argv[12], NULL, 0);
		uint64_t mask = strtoull(argv[13], NULL, 0);
		return cmn_trace_wp_setup(base, wp, dev_sel, chn_sel,
					  pkt_type, pkt_gen, combine,
					  cc_en, ctrigger_en,
					  dbgtrigger_en, val, mask);
	}
	if (strcmp(argv[1], "dt_setup") == 0) {
		if (argc < 5)
			return -EINVAL;
		id = atoi(argv[2]);
		if (id < 0 || id >= cmn_dtc_count)
			return -EINVAL;
		caddr_t base = cmn_bases[cmn_dtc_ids[id]];
		int dbgtrigger_en = atoi(argv[3]);
		int atbtrigger_en = atoi(argv[4]);
		int wait_for_trigger = argc > 5 ? atoi(argv[5]) : 0;
		return cmn_trace_dt_setup(base, dbgtrigger_en,
					  atbtrigger_en, wait_for_trigger);
	}

	return -EINVAL;
}

DEFINE_COMMAND(cmn_trace, do_cmn_trace, "SpacemiT CMN Debug Trace configuration commands",
	"cmn_trace list\n"
	"  - list all cmn dt registers\n"
	"cmn_trace wp_setup <id> <wp> <dev_sel> <chn_sel> <pkt_type> <pkt_gen> <combine> <cc_en> <ctrigger_en> <dbgtrigger_en> <val> <mask>\n"
	"  - setup a watchpoint with the given parameters\n"
	"cmn_trace dt_setup <id> <dbgtrigger_en> <atbtrigger_en> <wait_for_trigger>\n"
	"  - setup the dt control with the given parameters\n"
);