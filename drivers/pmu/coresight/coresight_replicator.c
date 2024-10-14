#include <asm/coresight.h>

static int coresight_replicator_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight replicator (%016llx)\n", device->base);
	return 0;
}

struct coresight_device coresight_replicator = {
	.name = "CoreSight replicator",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_TYPE_REPLICATOR,
	.handler = coresight_replicator_handler,
};

int coresight_replicator_init(void)
{
	return coresight_register_device(&coresight_replicator);
}
