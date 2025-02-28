#include <target/dts.h>
#include <target/cmdline.h>
#include <target/panic.h>
#include <target/console.h>

caddr_t etr_base[32];
uint32_t etr_num = 0;

static int coresight_etr_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight ETR (%016llx)\n",
		      (uint64_t)device->base);
	etr_base[etr_num++] = (uint64_t)device->base;
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
		coresight_log("Coresight ETR %d: 0x%lx\n", i, etr_base[i]);
	}
}

void etr_handle_irq(uint32_t n)
{
	uint32_t sts = coresight_read(ETR_STS(etr_base[n]));
	if (sts & ETR_STS_MEMERR) {
		coresight_log("CoreSight ETR %d: Memory Error!\n", n);
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
	}
	coresight_write(sts, ETR_STS(etr_base[n]));
}

void etr_tracecapt(uint32_t n, uint32_t io)
{
	coresight_write(io, ETR_CTL(etr_base[n]));
}

void etr_config_addr(uint32_t n, caddr_t addr, uint32_t size)
{
	uint32_t hiddr = HIDWORD(addr);
	uint32_t loddr = LODWORD(addr);
	uint32_t sizeb = size * 1024 * 1024;
	coresight_write(size, ETR_RSZ(etr_base[n]));
	coresight_write(loddr, ETR_DBALO(etr_base[n]));
	coresight_write(hiddr, ETR_DBAHI(etr_base[n]));
	coresight_write(loddr, ETR_RRP(etr_base[n]));
	coresight_write(hiddr, ETR_RRPHI(etr_base[n]));
	coresight_write(loddr, ETR_RWP(etr_base[n]));
	coresight_write(hiddr, ETR_RWPHI(etr_base[n]));
	coresight_write(sizeb / 2, ETR_BUFWM(etr_base[n]));
}

void etr_config_mode(uint32_t n, uint32_t mode)
{
	uint32_t tmp = (coresight_read(ETR_MODE(etr_base[n])) & 0xfffffffc) | (mode & 0x2);
	uint32_t axi = (coresight_read(ETR_AXICTL(etr_base[n])) & 0xfffff0fc) | 0x00000402;
	coresight_write(tmp, ETR_MODE(etr_base[n]));
	coresight_write(axi, ETR_AXICTL(etr_base[n]));
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
	if (strcmp(argv[1], "getstatus") == 0) {
		etr_handle_irq((uint32_t)strtoull(argv[2], 0, 0));
		return 0;
	}
	if (strcmp(argv[1], "config") == 0) {
		if (strcmp(argv[2], "addr") == 0) {
			etr_config_addr((uint32_t)strtoull(argv[3], 0, 0), 
					(caddr_t)strtoull(argv[4], 0, 0), 
					(uint32_t)strtoull(argv[5], 0, 0));
			return 0;
		}
		if (strcmp(argv[2], "mode") == 0) {
			etr_config_mode((uint32_t)strtoull(argv[3], 0, 0), 
					(uint32_t)strtoull(argv[4], 0, 0));
			return 0;
		}
		return -EINVAL;
	}
	return -EINVAL;
}

DEFINE_COMMAND(coresight_etr, do_coresight_etr, "Coresight ETR commands",
	"list\n"
	"    - List all ETR devices\n"
	"trace <id> <enable/disable>\n"
	"    - Enable/Disable trace capture\n"
	"getstatus <id>\n"
	"    - Get ETR <id> Status\n"
	"config addr <id> <addr> <size>\n"
	"    - Config ETR start address and RAM size in MB (Max 4GB)\n"
	"config mode <id> <mode>\n"
	"    - Config operating mode\n"
	"        - 0: CB, Circular Buffer mode\n"
	"        - 1: SWF1, Software Read FIFO mode 1\n"
	"        - 2: Same as 1\n"
	"        - 3: SWF2, Software Read FIFO mode 2\n"
);