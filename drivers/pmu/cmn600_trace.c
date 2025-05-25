#include <target/noc.h>
#include <target/irq.h>
#include <target/cmdline.h>
#include <target/console.h>
#include <target/arch.h>

uint8_t cmn_trace_cfg_watchpoint(caddr_t base, uint8_t wp, uint64_t addr, uint64_t mask)
{
	if (wp >= CMN_TRACE_WP_COUNT)
		return -EINVAL;
	cmn_writeq(addr, CMN_trace_wp_addr(base, wp), "CMN_trace_wp_addr", -1);
	cmn_writeq(mask, CMN_trace_wp_mask(base, wp), "CMN_trace_wp_mask", -1);
	return 0;
}

static int do_cmn_trace(int argc, char *argv[])
{
	int i;
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
}

DEFINE_COMMAND(cmn_trace, do_cmn_trace, "SpacemiT CMN Debug Trace configuration commands",
	"cmn_trace list\n"
	"  - list all cmn dt registers\n"

);