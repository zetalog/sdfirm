#include <stdio.h>
#include <asm/mach/qdss.h>

static int qdss_cti_handler(struct coresight_rom_device *device)
{
	coresight_log("QDSS CTI (%016llx)\n", device->base);
	return 0;
}

struct coresight_device qdss_cti = {
	.name = "Qualcomm CTI",
	.jep106_ident = CORESIGHT_JEP106_QCOM_CTI,
	.arch_id = CORESIGHT_ARCH_QCOM_CTI,
	.handler = qdss_cti_handler,
};

int qdss_cti_init(void)
{
	return coresight_register_device(&qdss_cti);
}
