#include <target/dts.h>
#include <target/cmdline.h>

caddr_t mem_ap_base[32];
uint32_t mem_ap_num = 0;

static int coresight_mem_ap_handler(struct coresight_rom_device *device)
{
	coresight_log("CoreSight MEM AP (%016llx)\n",
		      (uint64_t)device->base);
	mem_ap_base[mem_ap_num++] = device->base;
	return 0;
}

struct coresight_device coresight_mem_ap = {
	.name = "CoreSight MEM AP",
	.jep106_ident = CORESIGHT_JEP106_ARM,
	.arch_id = CORESIGHT_ARCH_MEM_AP,
	.handler = coresight_mem_ap_handler,
};

int coresight_mem_ap_init(void)
{
	return coresight_register_device(&coresight_mem_ap);
}

uint32_t mem_ap_read(caddr_t addr)
{
	if ((addr & 0xffff0000) != CORESIGHT_ROM_TABLE_0_BASE) {
		__raw_writel(addr, CORESIGHT_MEM_AP_TAR(mem_ap_base[0]));
		return __raw_readl(CORESIGHT_MEM_AP_DRW(mem_ap_base[0]));
	}
	else
		return __raw_readl(addr);
}

void mem_ap_write(uint32_t data, caddr_t addr)
{
	if ((addr & 0xffff0000) != CORESIGHT_ROM_TABLE_0_BASE) {
		__raw_writel(addr, CORESIGHT_MEM_AP_TAR(mem_ap_base[0]));
		__raw_writel(data, CORESIGHT_MEM_AP_DRW(mem_ap_base[0]));
	}
	else
		__raw_writel(data, addr);
}

void mem_ap_list(void)
{
	for (int i = 0; i < mem_ap_num; i++) {
		printf("MEM AP %d: Base = 0x%x\n", i, mem_ap_base[i]);
	}
}

void read_rom_table(void)
{
	uint64_t addr = CORESIGHT_ROM_TABLE_1_BASE;
	for (int i = 0; i < CORESIGHT_HW_MAX_DEVICES / 4; i++) {
		uint32_t entry = mem_ap_read(addr);
		if ((entry & 0x1) && (entry != 0xFFFFFFFF)) {
			uint32_t component_address = entry & 0xFFFFF000;
			printf("ROM Table Entry %d: Component Address = 0x%X\n", i, component_address);
		}
		addr += 4;
	}
}

static int do_coresight_mem_ap(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "list") == 0) {
		mem_ap_list();
		return 0;
	}
	if (strcmp(argv[1], "read") == 0) {
		printf("0x%x\n", mem_ap_read((uint32_t)strtoull(argv[2], 0, 0)));
		return 0;
	}
	if (strcmp(argv[1], "write") == 0) {
		mem_ap_write((uint32_t)strtoull(argv[2], 0, 0), (uint32_t)strtoull(argv[3], 0, 0));
		return 0;
	}
	if (strcmp(argv[1], "enum") == 0) {
		read_rom_table();
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(coresight_mem, do_coresight_mem_ap, "Coresight MEM AP commands",
	"list\n"
	"     -List all MEM APs\n"
	"read <addr>\n"
	"     -MEM AP read data\n"
	"write <addr> <data>\n"
	"     -MEM AP write data\n"
	"enum\n"
	"     -MEM AP enum ROM tables\n"
);
