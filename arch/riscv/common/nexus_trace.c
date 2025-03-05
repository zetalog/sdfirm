#include <target/dts.h>
#include <target/cmdline.h>
#include <target/panic.h>
#include <target/console.h>

void trace_list(int type)
{
	int i, j;
	if (type == TYPE_FUNNEL)
		for (i = 0; i < MAX_CPU_CLUSTERS; i++) {
			coresight_log("Trace Funnel %d - 0: 0x%llx\n", cluster_table[i], TRACE_FUNNEL(i));
		}
	if (type == TYPE_TIMESTAMP)
		for (i = 0; i < MAX_CPU_CLUSTERS; i++) {
			coresight_log("Trace Timestamp %d - 0: 0x%llx\n", cluster_table[i], TRACE_TIMESTAMP(i));
		}
	if (type == TYPE_ENCODER)
		for (i = 0; i < MAX_CPU_CLUSTERS; i++)
			for (j = 0; j < CPUS_PER_CLUSTER; j++) {
				coresight_log("Trace Encoder %d - %d: 0x%llx\n", cluster_table[i], j, TRACE_ENCODER(i, j));
			}
	if (type == TYPE_ATBBRIDGE)
		for (i = 0; i < MAX_CPU_CLUSTERS; i++)
			for (j = 0; j < CPUS_PER_CLUSTER; j++) {
				coresight_log("Trace ATB Bridge %d - %d: 0x%llx\n", cluster_table[i], j, TRACE_ATBBRIDGE(i, j));
			}
}

void trace_ctrl(int type, int cls, int io)
{
	int i;
	int id = cluster_to_id[cls];
	if (type == TYPE_FUNNEL) {
		if (io) {
			coresight_write(0x1, TRACE_CTL(TRACE_FUNNEL(id)));
			coresight_write(0x3, TRACE_CTL(TRACE_FUNNEL(id)));
		}
		else
			coresight_write(0x0, TRACE_CTL(TRACE_FUNNEL(id)));
	}
	if (type == TYPE_TIMESTAMP) {
		if (io)
			coresight_write(0xFFFF800B, TRACE_TIMESTAMP(id));
		else
			coresight_write(0x0, TRACE_TIMESTAMP(id));
	}
	if (type == TYPE_ATBBRIDGE) {
		if (io) {
			for (i = 0; i < CPUS_PER_CLUSTER; i++)
				coresight_write(0x1, TRACE_CTL(TRACE_ATBBRIDGE(id, i)));
			for (i = 0; i < CPUS_PER_CLUSTER; i++)
				coresight_write(0x103, TRACE_CTL(TRACE_ATBBRIDGE(id, i)));
		}
		else
			for (i = 0; i < CPUS_PER_CLUSTER; i++)
				coresight_write(0x0, TRACE_CTL(TRACE_ATBBRIDGE(id, i)));
	}
	if (type == TYPE_ENCODER) {
		if (io) {
			for (i = 0; i < CPUS_PER_CLUSTER; i++) {
				coresight_write(0x1, TRACE_CTL(TRACE_ENCODER(id, i)));
				coresight_write(0xC0000400, TENCODER_INSTFEAT(TRACE_ENCODER(id, i)));
			}
			for (i = 0; i < CPUS_PER_CLUSTER; i++)
				coresight_write(0x01420A07, TRACE_CTL(TRACE_ENCODER(id, i)));
		}
		else
			for (i = 0; i < CPUS_PER_CLUSTER; i++)
				coresight_write(0x0, TRACE_CTL(TRACE_ENCODER(id, i)));
	}
}

static int do_nexus_trace(int argc, char *argv[])
{
	int type = 0;
	if (strcmp(argv[2], "funnel") == 0)
		type = TYPE_FUNNEL;
	if (strcmp(argv[2], "timestamp") == 0)
		type = TYPE_TIMESTAMP;
	if (strcmp(argv[2], "encoder") == 0)
		type = TYPE_ENCODER;
	if (strcmp(argv[2], "atbbridge") == 0)
		type = TYPE_ATBBRIDGE;
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		trace_list(type);
		return 0;
	}
	if (strcmp(argv[1], "control") == 0) {
		if (strcmp(argv[3], "enable") == 0)
			trace_ctrl(type, (uint32_t)strtoull(argv[4], 0, 0), 1);
		else if (strcmp(argv[3], "disable") == 0)
			trace_ctrl(type, (uint32_t)strtoull(argv[4], 0, 0), 0);
		else
			return -EINVAL;
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(nexus_trace, do_nexus_trace, "Nexus Trace commands",
	"list <type>\n"
	"    - List all <type> devices\n"
	"    - type: funnel, timestamp, encoder, atbbridge\n"
	"control <type> <enable/disable> <cluster>\n"
	"    - Enable/Disable devices\n"
);
