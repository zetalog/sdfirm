#include <asm/coresight.h>

static int coresight_etb_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight ETB (%016llx)\n", device->base);
	return 0;
}

struct coresight_device coresight_etb = {
	.name = "CoreSight ETB",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_TYPE_ETB,
	.handler = coresight_etb_handler,
};

int coresight_etb_init(void)
{
	return coresight_register_device(&coresight_etb);
}
