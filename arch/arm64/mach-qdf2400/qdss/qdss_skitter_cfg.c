#include <stdio.h>
#include <asm/mach/qdss.h>

static int qdss_skitter_cfg_handler(struct coresight_rom_device *device)
{
	coresight_log("QDSS Skitter Config (%016llx)\n", device->base);
	return 0;
}

struct coresight_device qdss_skitter_cfg = {
	.name = "Qualcomm Skitter Config",
	.jep106_ident = CORESIGHT_JEP106_QCOM,
	.arch_id = CORESIGHT_TYPE_QCOM_SKITTER_CFG,
	.handler = qdss_skitter_cfg_handler,
};

int qdss_skitter_cfg_init(void)
{
	return coresight_register_device(&qdss_skitter_cfg);
}
