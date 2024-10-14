#include <asm/coresight.h>

static int coresight_stm_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight STM (%016llx)\n", device->base);
	return 0;
}

struct coresight_device coresight_stm = {
	.name = "CoreSight STM",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_ARCH_STM,
	.handler = coresight_stm_handler,
};

int coresight_stm_init(void)
{
	return coresight_register_device(&coresight_stm);
}
