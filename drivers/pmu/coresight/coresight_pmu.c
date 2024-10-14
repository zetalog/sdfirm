#include <asm/coresight.h>

static int coresight_pmu_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight PMU (%016llx)\n", device->base);
	return 0;
}

struct coresight_device coresight_pmu = {
	.name = "CoreSight PMU",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_ARCH_PMU,
	.handler = coresight_pmu_handler,
};

int coresight_pmu_init(void)
{
	return coresight_register_device(&coresight_pmu);
}
