#include <asm/coresight.h>

static int coresight_cti_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight CTI (%016llx)\n", device->base);
	return 0;
}

struct coresight_device coresight_cti = {
	.name = "CoreSight CTI",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_ARCH_CTI,
	.handler = coresight_cti_handler,
};

int coresight_cti_init(void)
{
	return coresight_register_device(&coresight_cti);
}
