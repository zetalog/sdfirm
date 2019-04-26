#include <stdio.h>
#include <asm/mach/qdss.h>

static int qdss_switch_cfg_handler(struct coresight_rom_device *device)
{
	coresight_log("QDSS Switch Config (%016llx)\n", device->base);
	return 0;
}

struct coresight_device qdss_switch_cfg = {
	.name = "Qualcomm Switch Config",
	.jep106_ident = CORESIGHT_JEP106_QCOM,
	.arch_id = CORESIGHT_TYPE_QCOM_SWITCH_CFG,
	.handler = qdss_switch_cfg_handler,
};

int qdss_switch_cfg_init(void)
{
	return coresight_register_device(&qdss_switch_cfg);
}
