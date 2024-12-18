#include <target/dts.h>

uint32_t cti_base[32];
uint32_t cti_num = 0;

static int coresight_cti_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight CTI (%016llx)\n",
		      (uint64_t)device->base);
	cti_base[cti_num++] = device->base;
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

void cti_list(void)
{
	for (int i = 0; i < cti_num; i++) {
		printf("Coresight CTI %d: 0x%x\n", i, cti_base[i]);
	}
}

void cti_enable(uint32_t id)
{
	__raw_writel(0x1, CTICONTROL(cti_base[id]));
}

void cti_disable(uint32_t id)
{
	__raw_writel(0x0, CTICONTROL(cti_base[id]));
}

static int do_coresight_cti(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		cti_list();
		return 0;
	}
	if (strcmp(argv[1], "enable") == 0) {
		cti_enable((uint32_t)strtoull(argv[2], 0, 0));
		return 0;
	}
	if (strcmp(argv[1], "disable") == 0) {
		cti_disable((uint32_t)strtoull(argv[2], 0, 0));
		return 0;
	}
}

DEFINE_COMMAND(coresight_cti, do_coresight_cti, "Coresight CTI commands",
	"list\n"
	"    - List all CTI devices\n"
	"enable <id>\n"
	"    - Enable CTI device <id>\n"
	"disable <id>\n"
	"    - Disable CTI device <id>\n"
);