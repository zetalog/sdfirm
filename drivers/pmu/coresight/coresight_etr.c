#include <target/dts.h>

uint32_t etr_base[32];
uint32_t etr_num = 0;

static int coresight_etr_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight ETR (%016llx)\n",
		      (uint64_t)device->base);
	etr_base[etr_num++] = device->base;
	return 0;
}

struct coresight_device coresight_etr = {
	.name = "CoreSight ETR",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_ARCH_ETR,
	.handler = coresight_etr_handler,
};

int coresight_etr_init(void)
{
	return coresight_register_device(&coresight_etr);
}

void etr_list(void)
{
	for (int i = 0; i < etr_num; i++) {
		printf("Coresight ETR %d: 0x%x\n", i, etr_base[i]);
	}
}

static int do_coresight_etr(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		etr_list();
		return 0;
	}
}

DEFINE_COMMAND(coresight_etr, do_coresight_etr, "Coresight ETR commands",
	"list\n"
	"    - List all ETR devices\n"
);