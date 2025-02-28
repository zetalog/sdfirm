#include <target/dts.h>
#include <target/cmdline.h>
#include <target/panic.h>
#include <target/console.h>

caddr_t cti_base[32];
uint32_t cti_num = 0;

static int coresight_cti_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight CTI (%016llx)\n",
		      (uint64_t)device->base);
	cti_base[cti_num++] = (uint64_t)device->base;
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
		printf("Coresight CTI %d: 0x%lx\n", i, cti_base[i]);
	}
}

void cti_cfg(uint32_t id, uint32_t en)
{
	coresight_write(en, CTI_CONTROL(cti_base[id]));
}

void cti_channel_cfg(uint32_t id, uint32_t io, uint32_t n, uint32_t en)
{
	if (io)
		coresight_write(en, CTI_INEN(cti_base[id], n));
	else
		coresight_write(en, CTI_OUTEN(cti_base[id], n));
}

static int do_coresight_cti(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		cti_list();
		return 0;
	}
	if (strcmp(argv[1], "cfg") == 0) {
		uint32_t en = (strcmp(argv[3], "enable") == 0) ? 1 : 0;
		cti_cfg((uint32_t)strtoull(argv[2], 0, 0), en);
		return 0;
	}
	if (strcmp(argv[1], "channel") == 0) {
		if (strcmp(argv[2], "cfg") == 0) {
			uint32_t io = (strcmp(argv[4], "input") == 0) ? 1 : 0;
			uint32_t en = (strcmp(argv[6], "enable") == 0) ? 1 : 0;
			cti_channel_cfg((uint32_t)strtoull(argv[3], 0, 0), io, (uint32_t)strtoull(argv[5], 0, 0), en);
			return 0;
		}
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(coresight_cti, do_coresight_cti, "Coresight CTI commands",
	"list\n"
	"    - List all CTI devices\n"
	"cfg <id> <enable/disable>\n"
	"    - Enable/Disable CTI device <id>\n"
	"channel cfg <id> <io> <channel> <enable/disable>\n"
	"    - Config <id> <input/output> <channel number>[0-31] enable/disable status\n"
);