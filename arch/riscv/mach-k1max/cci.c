#include <target/arch.h>
#include <target/noc.h>

/* Cluster to CCI slave map */
const int cci_hw_slave_map[] = {
	0,
	1,
	2,
	3,
};

void k1max_cci_init(void)
{
	uint8_t clusterid;

	for (clusterid = 0; clusterid < MAX_CPU_CLUSTERS; clusterid++)
		cci_enable_snoop_dvm_reqs(clusterid);
}
