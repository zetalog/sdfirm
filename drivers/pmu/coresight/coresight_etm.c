#include <asm/coresight.h>

static int coresight_etm_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight ETM (%016llx)\n", device->base);
	return 0;
}

struct coresight_device coresight_etm = {
	.name = "CoreSight ETM",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_ARCH_ETM,
	.handler = coresight_etm_handler,
};

int coresight_etm_init(void)
{
	return coresight_register_device(&coresight_etm);
}
