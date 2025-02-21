#include <target/dts.h>
#include <target/cmdline.h>
#include <target/panic.h>
#include <target/console.h>

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
	.arch_id = CORESIGHT_TYPE_ETR,
	.handler = coresight_etr_handler,
};

int coresight_etr_init(void)
{
	return coresight_register_device(&coresight_etr);
}

void etr_list(void)
{
	for (int i = 0; i < etr_num; i++) {
		coresight_log("Coresight ETR %d: 0x%x\n", i, etr_base[i]);
	}
}

void etr_handle_irq(uint32_t n)
{
	uint32_t sts = coresight_read(ETR_STS(etr_base[n]));
	if (sts & ETR_STS_MEMERR) {
		coresight_log("CoreSight ETR %d: Memory Error!\n", n);
		coresight_write(ETR_STS(etr_base[n]), 0xFFFFFFFF & ETR_STS_MEMERR);
	}
	if (sts & ETR_STS_EMPTY) {
		coresight_log("CoreSight ETR %d: Trace Buffer Empty!\n", n);
	}
	if (sts & ETR_STS_FTEMPTY) {
		coresight_log("CoreSight ETR %d: Trace Completed\n", n);
	}
	if (sts & ETR_STS_TMCREADY) {
		coresight_log("CoreSight ETR %d: Trace Finished\n", n);
	}
	if (sts & ETR_STS_TRIGGERED) {
		coresight_log("CoreSight ETR %d: TMC Triggered\n", n);
	}
	if (sts & ETR_STS_FULL) {
		coresight_log("CoreSight ETR %d: Trace Memory Full!\n", n);
		coresight_write(ETR_STS(etr_base[n]), 0xFFFFFFFF & ETR_STS_FULL);
	}
}

void etr_tracecapt(uint32_t n, uint32_t io)
{
	coresight_write(ETR_CTL(etr_base[n]), io);
}

static int do_coresight_etr(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		etr_list();
		return 0;
	}
	if (strcmp(argv[1], "trace") == 0) {
		if (strcmp(argv[3], "enable") == 0)
			etr_tracecapt((uint32_t)strtoull(argv[2], 0, 0), 1);
		else if (strcmp(argv[3], "disable") == 0)
			etr_tracecapt((uint32_t)strtoull(argv[2], 0, 0), 0);
		else
			return -EINVAL;
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(coresight_etr, do_coresight_etr, "Coresight ETR commands",
	"list\n"
	"    - List all ETR devices\n"
	"trace <id> <enable/disable>\n"
	"    - Enable/Disable trace capture\n"
);