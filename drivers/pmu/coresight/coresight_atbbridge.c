#include <target/dts.h>
#include <target/cmdline.h>
#include <target/panic.h>
#include <target/console.h>

caddr_t atbbridge_base[32][32];
uint32_t atbbridge_num = CORESIGHT_HW_MAX_CLUSTERS * 4;

static int coresight_atbbridge_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight atbbridge (%016llx)\n",
		      (uint64_t)device->base);
	return 0;
}

struct coresight_device coresight_atbbridge = {
	.name = "CoreSight ATB Bridge",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = 0,
	.handler = coresight_atbbridge_handler,
};

int coresight_atbbridge_init(void)
{
	int i, j;
	for (i = 0; i < CORESIGHT_HW_MAX_CLUSTERS; i++)
		for (j = 0; j < 4; j++)
			atbbridge_base[i][j] = CORESIGHT_ATBBRIDGE(i, j);
	return coresight_register_device(&coresight_atbbridge);
}

void atbbridge_list(void)
{
	int i, j;
	for (i = 0; i < CORESIGHT_HW_MAX_CLUSTERS; i++)
		for (j = 0; j < 4; j++) {
			coresight_log("Coresight Atbbridge %d %d: 0x%lx\n", i, j, atbbridge_base[i][j]);
		}
}

static int do_coresight_atbbridge(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		atbbridge_list();
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(coresight_atbbridge, do_coresight_atbbridge, "Coresight Atbbridge commands",
	"list\n"
	"    - List all atbbridge devices\n"
);
