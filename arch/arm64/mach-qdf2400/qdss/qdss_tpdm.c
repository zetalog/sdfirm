#include <stdio.h>
#include <asm/mach/qdss.h>

static int qdss_tpdm_handler(struct coresight_rom_device *device)
{
	coresight_log("QDSS TPDM (%016llx)\n", device->base);
	return 0;
}

struct coresight_device qdss_tpdm = {
	.name = "Qualcomm TPDM",
	.jep106_ident = CORESIGHT_JEP106_QCOM,
	.arch_id = CORESIGHT_TYPE_QCOM_TPDM,
	.handler = qdss_tpdm_handler,
};

int qdss_tpdm_init(void)
{
	return coresight_register_device(&qdss_tpdm);
}
