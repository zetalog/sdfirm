#include <target/noc.h>
#include <target/perf.h>
#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/arch.h>

uint32_t xp_base, wp, dev_sel, chn_sel, dt_base;
uint32_t sel_xp = 0, sel_dt = 0;

static int cmn_trace_wp_config(uint32_t pkt_type, uint32_t pkt_gen,
				uint32_t combine, uint32_t cc_en, uint32_t ctrigger_en,
				uint32_t dbgtrigger_en)
{
	uint64_t val = 0;

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

	__raw_writeq(val, CMN_dtm_wp_config(xp_base, wp));
	return 0;
}

static int cmn_trace_wp_val_mask(uint64_t val, uint64_t mask)
{
	__raw_writeq(val, CMN_dtm_wp_val(xp_base, wp));
	__raw_writeq(mask, CMN_dtm_wp_mask(xp_base, wp));
	return 0;
}

static int cmn_trace_wp_enable(uint32_t enable)
{
	uint64_t val = __raw_readq(CMN_dtm_wp_config(xp_base, wp));
	if (enable)
		val |= dtm_control_en;
	else
		val &= ~dtm_control_en;
	__raw_writeq(val, CMN_dtm_wp_config(xp_base, wp));
	return 0;
}

static int cmn_trace_dt_config(uint32_t dbgtrigger_en, uint32_t atbtrigger_en,
			       uint32_t wait_for_trigger)
{
	uint64_t val = 0;

	if (dbgtrigger_en)
		val |= dt_dtc_ctl_dbgtrigger_en;
	if (atbtrigger_en)
		val |= dt_dtc_ctl_atbtrigger_en;
	if (wait_for_trigger)
		val |= dt_dtc_ctl_wait_for_trigger;

	__raw_setq(dt_trace_control_cc_enable, CMN_dt_trace_control(dt_base));
	__raw_writeq(val, CMN_dt_dtc_ctl(dt_base));
	return 0;
}

static int cmn_trace_dt_enable(uint32_t enable)
{
	uint64_t val = __raw_readq(CMN_dt_dtc_ctl(dt_base));
	if (enable)
		val |= dt_dtc_ctl_en;
	else
		val &= ~dt_dtc_ctl_en;
	__raw_writeq(val, CMN_dt_dtc_ctl(dt_base));
	return 0;
}

static int do_cmn_trace(int argc, char *argv[])
{
	int i, id;
	caddr_t base;
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		for (i = 0; i < cmn_dtc_count; i++) {
			base = cmn_bases[cmn_dtc_ids[i]];
			printf("DTC-%d: %03d, %016lx\n",
			       cmn_logical_id(base), cmn_node_id(base), base);
		}
		for (i = 0; i < cmn_xp_count; i++) {
			base = cmn_bases[cmn_xp_ids[i]];
			printf("DTM-%d: %03d, %016lx\n",
			       cmn_logical_id(base), cmn_node_id(base), base);
		}
		return 0;
	}
	if (strcmp(argv[1], "wp") == 0) {
		if (strcmp(argv[2], "select") == 0) {
			if (argc < 7)
				return -EINVAL;
			id = (uint32_t)strtoull(argv[3], 0, 0);
			if (id < 0 || id >= cmn_xp_count)
				return -EINVAL;
			xp_base = cmn_bases[cmn_xp_ids[id]];
			wp = (uint32_t)strtoull(argv[4], 0, 0);
			dev_sel = (uint32_t)strtoull(argv[5], 0, 0);
			chn_sel = (uint32_t)strtoull(argv[6], 0, 0);
			sel_xp = 1;
			return 0;
		}
		if (strcmp(argv[2], "config") == 0) {
			if (argc < 9)
				return -EINVAL;
			if (!sel_xp) {
				printf("Select watchpoint first\n");
				return -EINVAL;
			}
			uint32_t pkt_type = (uint32_t)strtoull(argv[3], 0, 0);
			uint32_t pkt_gen = (uint32_t)strtoull(argv[4], 0, 0);
			uint32_t combine = (uint32_t)strtoull(argv[5], 0, 0);
			uint32_t cc_en = (uint32_t)strtoull(argv[6], 0, 0);
			uint32_t ctrigger_en = (uint32_t)strtoull(argv[7], 0, 0);
			uint32_t dbgtrigger_en = (uint32_t)strtoull(argv[8], 0, 0);
			return cmn_trace_wp_config(pkt_type, pkt_gen, combine,
						  cc_en, ctrigger_en, dbgtrigger_en);
		}
		if (strcmp(argv[2], "val") == 0) {
			if (argc < 5)
				return -EINVAL;
			if (!sel_xp) {
				printf("Select watchpoint first\n");
				return -EINVAL;
			}
			uint64_t val = (uint64_t)strtoull(argv[3], 0, 0);
			uint64_t mask = (uint64_t)strtoull(argv[4], 0, 0);
			return cmn_trace_wp_val_mask(val, mask);
		}
		if (strcmp(argv[2], "enable") == 0) {
			if (argc < 4)
				return -EINVAL;
			if (!sel_xp) {
				printf("Select watchpoint first\n");
				return -EINVAL;
			}
			uint32_t enable = (uint32_t)strtoull(argv[3], 0, 0);
			return cmn_trace_wp_enable(enable);
		}
	}
	if (strcmp(argv[1], "dt") == 0) {
		if (strcmp(argv[2], "select") == 0) {
			if (argc < 4)
				return -EINVAL;
			id = (uint32_t)strtoull(argv[3], 0, 0);
			if (id < 0 || id >= cmn_dtc_count)
				return -EINVAL;
			dt_base = cmn_bases[cmn_dtc_ids[id]];
			sel_dt = 1;
			return 0;
		}
		if (strcmp(argv[2], "config") == 0) {
			if (argc < 5)
				return -EINVAL;
			if (!sel_dt) {
				printf("Select DTC first\n");
				return -EINVAL;
			}
			uint32_t dbgtrigger_en = (uint32_t)strtoull(argv[3], 0, 0);
			uint32_t atbtrigger_en = (uint32_t)strtoull(argv[4], 0, 0);
			uint32_t wait_for_trigger = (argc > 5) ?
						    (uint32_t)strtoull(argv[5], 0, 0) : 0;
			return cmn_trace_dt_config(dbgtrigger_en, atbtrigger_en,
						   wait_for_trigger);
		}
		if (strcmp(argv[2], "enable") == 0) {
			if (argc < 4)
				return -EINVAL;
			if (!sel_dt) {
				printf("Select DTC first\n");
				return -EINVAL;
			}
			uint32_t enable = (uint32_t)strtoull(argv[3], 0, 0);
			return cmn_trace_dt_enable(enable);
		}
	}
	return -EINVAL;
}

DEFINE_COMMAND(cmn_trace, do_cmn_trace, "SpacemiT CMN Debug Trace configuration commands",
	"cmn_trace list\n"
	"  - list all cmn dt&dtm registers\n"
	"cmn_trace wp select <id> <wp> <dev_sel> <chn_sel>\n"
	"  - select watchpoint id, wp, device and channel\n"
	"    - channel: 0 - REQ, 1 - RSP, 2 - SNP, 3 - DAT\n"
	"cmn_trace wp config <pkt_type> <pkt_gen> <combine> <cc_en> <ctrigger_en> <dbgtrigger_en>\n"
	"  - configure watchpoint parameters\n"
	"cmn_trace wp val <val> <mask>\n"
	"  - set watchpoint value and mask\n"
	"cmn_trace wp enable <enable>\n"
	"  - enable or disable watchpoint\n"
	"cmn_trace dt select <id>\n"
	"  - select DTC <id> for debug trace\n"
	"cmn_trace dt config <dbgtrigger_en> <atbtrigger_en> [wait_for_trigger]\n"
	"  - configure DTC for debug trace\n"
	"cmn_trace dt enable <enable>\n"
	"  - enable or disable DTC for debug trace\n"
);