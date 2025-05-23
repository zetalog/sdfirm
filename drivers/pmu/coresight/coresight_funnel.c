#include <target/dts.h>
#include <target/cmdline.h>
#include <target/panic.h>
#include <target/console.h>

static int coresight_funnel_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight funnel (%016llx)\n",
		      (uint64_t)device->base);
	return 0;
}

struct coresight_device coresight_funnel = {
	.name = "CoreSight funnel",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_TYPE_FUNNEL,
	.handler = coresight_funnel_handler,
};

int coresight_funnel_init(void)
{
	return coresight_register_device(&coresight_funnel);
}

