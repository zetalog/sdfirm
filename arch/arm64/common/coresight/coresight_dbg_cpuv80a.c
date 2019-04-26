#include <asm/coresight.h>

static int coresight_dbg_cpuv80a_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight DBG CPUv8.0A (%016llx)\n", device->base);
	return 0;
}

struct coresight_device coresight_dbg_cpuv80a = {
	.name = "CoreSight DBG CPUv8.0A",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_ARCH_DBG_CPUv80A,
	.handler = coresight_dbg_cpuv80a_handler,
};

int coresight_dbg_cpuv80a_init(void)
{
	return coresight_register_device(&coresight_dbg_cpuv80a);
}
