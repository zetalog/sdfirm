#include <asm/coresight.h>

static int coresight_tpiu_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight TPIU (%016llx)\n", device->base);
	return 0;
}

struct coresight_device coresight_tpiu = {
	.name = "CoreSight TPIU",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_TYPE_TPIU,
	.handler = coresight_tpiu_handler,
};

int coresight_tpiu_init(void)
{
	return coresight_register_device(&coresight_tpiu);
}
