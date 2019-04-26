#include <stdio.h>
#include <asm/mach/qdss.h>

static int qdss_switch_pmu_handler(struct coresight_rom_device *device)
{
	coresight_log("QDSS Switch PMU (%016llx)\n", device->base);
	return 0;
}

struct coresight_device qdss_switch_pmu = {
	.name = "Qualcomm Switch PMU",
	.jep106_ident = CORESIGHT_JEP106_QCOM,
	.arch_id = CORESIGHT_TYPE_QCOM_SWITCH_PMU,
	.handler = qdss_switch_pmu_handler,
};

int qdss_switch_pmu_init(void)
{
	return coresight_register_device(&qdss_switch_pmu);
}
