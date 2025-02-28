#include <target/dts.h>
#include <target/cmdline.h>
#include <target/panic.h>
#include <target/console.h>

caddr_t funnel_base[128];
uint32_t funnel_num = CORESIGHT_HW_MAX_CLUSTERS;

static int coresight_funnel_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight funnel (%016llx)\n",
		      (uint64_t)device->base);
	return 0;
}

struct coresight_device coresight_funnel = {
	.name = "CoreSight funnel",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_TYPE_FUNNEL,
	.handler = coresight_funnel_handler,
};

int coresight_funnel_init(void)
{
	int i;
	for (i = 0; i < CORESIGHT_HW_MAX_CLUSTERS; i++)
		funnel_base[i] = CORESIGHT_FUNNEL(i);
	return coresight_register_device(&coresight_funnel);
}

void funnel_list(void)
{
	for (int i = 0; i < funnel_num; i++) {
		coresight_log("Coresight ETR %d: 0x%lx\n", i, funnel_base[i]);
	}
}

void funnel_ctrl(uint32_t n, uint32_t itf, uint32_t io)
{
	uint32_t tmp = coresight_read(FUNNEL_CTL(funnel_base[n]));
	if (io)
		tmp |= _BV(itf);
	else
		tmp &= ~_BV(itf);
	coresight_write(tmp, FUNNEL_CTL(funnel_base[n]));
}

static int do_coresight_funnel(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		funnel_list();
		return 0;
	}
	if (strcmp(argv[1], "control") == 0) {
		if (strcmp(argv[4], "enable") == 0)
			funnel_ctrl((uint32_t)strtoull(argv[2], 0, 0), (uint32_t)strtoull(argv[3], 0, 0), 1);
		else if (strcmp(argv[4], "disable") == 0)
			funnel_ctrl((uint32_t)strtoull(argv[2], 0, 0), (uint32_t)strtoull(argv[3], 0, 0), 0);
		else
			return -EINVAL;
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(coresight_funnel, do_coresight_funnel, "Coresight Funnel commands",
	"list\n"
	"    - List all funnel devices\n"
	"control <id> <interface> <enable/disable>\n"
	"    - Enable/Disable funnel interface\n"
);
