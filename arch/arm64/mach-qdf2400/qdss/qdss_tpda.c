#include <stdio.h>
#include <asm/mach/qdss.h>

static int qdss_tpda_handler(struct coresight_rom_device *device)
{
	coresight_log("QDSS TPDA (%016llx)\n", device->base);
	return 0;
}

struct coresight_device qdss_tpda = {
	.name = "Qualcomm TPDA",
	.jep106_ident = CORESIGHT_JEP106_QCOM,
	.arch_id = CORESIGHT_TYPE_QCOM_TPDA,
	.handler = qdss_tpda_handler,
};

int qdss_tpda_init(void)
{
	return coresight_register_device(&qdss_tpda);
}
