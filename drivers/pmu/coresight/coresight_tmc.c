#include <asm/coresight.h>

static int coresight_tmc_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight TMC (%016llx)\n", device->base);
	return 0;
}

struct coresight_device coresight_tmc = {
	.name = "CoreSight TMC",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_TYPE_TMC,
	.handler = coresight_tmc_handler,
};

int coresight_tmc_init(void)
{
	return coresight_register_device(&coresight_tmc);
}
