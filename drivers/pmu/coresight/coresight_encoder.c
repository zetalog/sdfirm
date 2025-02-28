#include <target/dts.h>
#include <target/cmdline.h>
#include <target/panic.h>
#include <target/console.h>

caddr_t encoder_base[32][32];
uint32_t encoder_num = CORESIGHT_HW_MAX_CLUSTERS * 4;

static int coresight_encoder_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight encoder (%016llx)\n",
		      (uint64_t)device->base);
	return 0;
}

struct coresight_device coresight_encoder = {
	.name = "CoreSight encoder",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = 0,
	.handler = coresight_encoder_handler,
};

int coresight_encoder_init(void)
{
	int i, j;
	for (i = 0; i < CORESIGHT_HW_MAX_CLUSTERS; i++)
		for (j = 0; j < 4; j++)
			encoder_base[i][j] = CORESIGHT_ENCODER(i, j);
	return coresight_register_device(&coresight_encoder);
}

void encoder_list(void)
{
	for (i = 0; i < CORESIGHT_HW_MAX_CLUSTERS; i++)
		for (j = 0; j < 4; j++) {
			coresight_log("Coresight Encoder %d %d: 0x%lx\n", i, j, encoder_base[i][j]);
		}
}

void encoder_ctrl(uint32_t n, uint32_t itf, uint32_t io)
{
	uint32_t tmp = coresight_read(ENCODER_CTL(encoder_base[n]));
	if (io)
		tmp |= _BV(itf);
	else
		tmp &= ~_BV(itf);
	coresight_write(tmp, ENCODER_CTL(encoder_base[n]));
}

static int do_coresight_encoder(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		encoder_list();
		return 0;
	}
	/*if (strcmp(argv[1], "control") == 0) {
		if (strcmp(argv[4], "enable") == 0)
			encoder_ctrl((uint32_t)strtoull(argv[2], 0, 0), (uint32_t)strtoull(argv[3], 0, 0), 1);
		else if (strcmp(argv[4], "disable") == 0)
			encoder_ctrl((uint32_t)strtoull(argv[2], 0, 0), (uint32_t)strtoull(argv[3], 0, 0), 0);
		else
			return -EINVAL;
		return 0;
	}*/
	return -EINVAL;
}

DEFINE_COMMAND(coresight_encoder, do_coresight_encoder, "Coresight Encoder commands",
	"list\n"
	"    - List all encoder devices\n"
	"control <id> <interface> <enable/disable>\n"
	"    - Enable/Disable encoder interface\n"
);
