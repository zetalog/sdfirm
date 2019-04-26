#include <stdio.h>
#include <asm/mach/qdss.h>

static int qdss_pmu_handler(struct coresight_rom_device *device)
{
	coresight_log("QDSS PMU (%016llx)\n", device->base);
	return 0;
}

struct coresight_device qdss_pmu = {
	.name = "Qualcomm PMU",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_ARCH_QCOM_PMU,
	.handler = qdss_pmu_handler,
};

int qdss_pmu_init(void)
{
	return coresight_register_device(&qdss_pmu);
}
