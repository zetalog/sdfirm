/*
 * Author: Lv Zheng <lv.zheng@hxt-semitech.com>
 */
#include <target/dts.h>
#include <target/cmdline.h>
#include <target/panic.h>
#include <target/console.h>

#define CORESIGHT_CLASS1_ENTRIES	960
#define CORESIGHT_CLASS1_ENTRY_SIZE	4

static struct coresight_table *coresight_tables[MAX_CORESIGHT_TABLES];
static uint8_t coresight_nr_tables = 0;
static struct coresight_device *coresight_devices[MAX_CORESIGHT_DEVICES];
static uint8_t coresight_nr_devices = 0;
static caddr_t *coresight_blacklist = NULL;

int coresight_register_table(struct coresight_table *table)
{
	uint8_t curr;

	curr = coresight_nr_tables;
	BUG_ON(curr == MAX_CORESIGHT_TABLES);
	coresight_nr_tables++;
	coresight_tables[curr] = table;
	return 0;
}

int coresight_register_device(struct coresight_device *device)
{
	uint8_t curr;

	curr = coresight_nr_devices;
	BUG_ON(curr == MAX_CORESIGHT_DEVICES);
	coresight_nr_devices++;
	coresight_devices[curr] = device;
	return 0;
}

int coresight_visit_table(caddr_t rom_table_base)
{
	struct coresight_rom_table table;
	int i;
	bool handled = false;

	if (coresight_blacklist) {
		for (i = 0; coresight_blacklist[i] != 0; i++) {
			if (rom_table_base == coresight_blacklist[i]) {
				coresight_log("Skip rom=%016llx, blacklist\n",
					      (uint64_t)rom_table_base);
				return 0;
			}
		}
	}

	table.base = rom_table_base;
	table.clazz = coresight_get_class(rom_table_base);
	table.jep106_ident = CORESIGHT_JEP106(pidr_jep106_ident(rom_table_base),
					      pidr_jep106_cont(rom_table_base));
	table.part = coresight_get_part(rom_table_base);

	for (i = 0; i < coresight_nr_tables; i++) {
		printf("ROM (%016llx): class_expect=%02x, class=%02x\n",
		       	(uint64_t)table.base, coresight_tables[i]->clazz, table.clazz);
		if (coresight_tables[i]->clazz == table.clazz) {
			coresight_tables[i]->handler(&table);
			handled = true;
			break;
		}
	}

	if (!handled) {
		printf("Unknown ROM (%016llx): "
		       "class=%02x, jep106=%04x, part=%04x\n",
		       (uint64_t)table.base, table.clazz,
		       table.jep106_ident, table.part);
	}
	return 0;
}

int __coresight_visit_device(struct coresight_rom_device *device)
{
	int i;
	bool handled = false;

	for (i = 0; i < coresight_nr_devices; i++) {
		printf("DEV (%016llx): jep106_expect=%04x jep106=%04x arch_expect=%04x, arch=%04x\n",
		       (uint64_t)device->base, coresight_devices[i]->jep106_ident, device->jep106_ident,
		       coresight_devices[i]->arch_id, device->arch_id);
		if (coresight_devices[i]->jep106_ident == device->jep106_ident &&
		    coresight_devices[i]->arch_id == device->arch_id) {
			coresight_devices[i]->handler(device);
			coresight_devices[i]->nr_devs++;
			handled = true;
			break;
		}
	}

	if (!handled) {
		printf("Unknown DEV (%016llx): "
		       "jep106=%04x, arch=%04xv%02x part=%02x\n",
		       (uint64_t)device->base, device->jep106_ident,
		       device->arch_id, device->revision, device->part_no);
	}
	return 0;
}

static int __coresight_class9_visit_ngpr_table(caddr_t rom_table_base)
{
	uint16_t offset;
	uint32_t romentry;
	caddr_t base;

	coresight_log("Class 0x9 ROM (%016llx)\n",
		      (uint64_t)rom_table_base);

	for (offset = 0; offset < CORESIGHT_CLASS1_ENTRIES; offset++) {
		romentry = __raw_readl(ROMENTRYn(rom_table_base, offset));
		coresight_log("ROM: offset: %d, romentry: 0x%08x\n",
			      offset, romentry);
		if (!romentry)
			break;
		if (!ROMENTRY_OFFSET(romentry))
			continue;
		if (!(ROMENTRY_PRESENT & romentry)) {
			coresight_log("Skip rom=%016llx, present=%02x\n",
				      (uint64_t)rom_table_base,
				      (uint8_t)(ROMENTRY_PRESENT & romentry));
			continue;
		}
		if (!(ROMENTRY_FORMAT & romentry)) {
			coresight_log("Skip rom=%016llx, format=%02x\n",
				      (uint64_t)rom_table_base,
				      (uint8_t)(ROMENTRY_FORMAT & romentry));
			continue;
		}
		if (!(ROMENTRY_POWERIDVALID & romentry)) {
			coresight_log("Skip rom=%016llx, powerid=%02x\n",
				      (uint64_t)rom_table_base,
				      (uint8_t)(ROMENTRY_POWERID(romentry)));
			continue;
		}
		base = rom_table_base +
		       (ROMENTRY_OFFSET(romentry) <<
			ROMENTRY_OFFSET_OFFSET);
		coresight_log("ROM: offset: %d, base: 0x%016llx\n",
			      offset, (uint64_t)base);
		coresight_visit_table(base);
	}
	return 0;
}

static int __coresight_class9_visit_gpr_table(caddr_t rom_table_base)
{
	uint16_t offset;
	uint64_t romentry;
	caddr_t base;

	coresight_log("Class 0x9 ROM (%016llx)\n",
		      (uint64_t)rom_table_base);

	for (offset = 0; offset < CORESIGHT_CLASS1_ENTRIES; offset++) {
		romentry = __raw_readq(LROMENTRYn(rom_table_base, offset));
		coresight_log("ROM: offset: %d, romentry: 0x%016llx\n",
			      offset, romentry);
		if (!romentry)
			break;
		if (!LROMENTRY_OFFSET(romentry))
			continue;
		if (!(ROMENTRY_PRESENT & romentry)) {
			coresight_log("Skip rom=%016llx, present=%02x\n",
				      (uint64_t)rom_table_base,
				      (uint8_t)(ROMENTRY_PRESENT & romentry));
			continue;
		}
		if (!(ROMENTRY_FORMAT & romentry)) {
			coresight_log("Skip rom=%016llx, format=%02x\n",
				      (uint64_t)rom_table_base,
				      (uint8_t)(ROMENTRY_FORMAT & romentry));
			continue;
		}
		if (!(ROMENTRY_POWERIDVALID & romentry)) {
			coresight_log("Skip rom=%016llx, powerid=%02x\n",
				      (uint64_t)rom_table_base,
				      (uint8_t)(ROMENTRY_POWERID(romentry)));
			continue;
		}
		base = rom_table_base +
		       (LROMENTRY_OFFSET(romentry) <<
			LROMENTRY_OFFSET_OFFSET);
		coresight_log("ROM: offset: %d, base: 0x%016llx\n",
			      offset, (uint64_t)base);
		coresight_visit_table(base);
	}
	return 0;
}

int coresight_visit_device(caddr_t rom_table_base)
{
	struct coresight_rom_device device;
	uint32_t devarch;
	uint32_t devtype;

	devarch = __raw_readl(DEVARCH(rom_table_base));

	device.base = rom_table_base;
	device.jep106_ident = DEVARCH_ARCHITECT(devarch);
	if (DEVARCH_PRESENT & devarch) {
		device.arch_id = DEVARCH_ARCHID(devarch);
		device.revision = DEVARCH_REVISION(devarch);
		device.part_no = coresight_get_part(rom_table_base);
	} else {
		if (devarch != 0)
			con_err("BUG: %016llx: DEVARCH is not PRESENT, but ARCHID is valid\n",
				(uint64_t)rom_table_base);
		device.arch_id = CORESIGHT_ARCH_ROM;
	}
	if (device.arch_id == 0) {
		printf("BUG: %016llx: DEVARCH register is PRESENT, but ARCHID is invalid\n",
		       (uint64_t)rom_table_base);
		device.arch_id = CORESIGHT_ARCH_ROM;
	}
	/* DEVARCH.PRESENT=0 or DEVARCH.ARCHID=ROM */
	if (device.arch_id == CORESIGHT_ARCH_ROM) {
		devtype = __raw_readl(DEVTYPE(rom_table_base));
		device.jep106_ident =
			CORESIGHT_JEP106(pidr_jep106_ident(rom_table_base),
					 pidr_jep106_cont(rom_table_base));
		device.arch_id = CORESIGHT_TYPE(DEVTYPE_MAJOR(devtype),
						DEVTYPE_SUB(devtype));
		if (devtype == 0) {
			if (__raw_readl(DEVID(rom_table_base)) & DEVID_PRR)
				return __coresight_class9_visit_gpr_table(rom_table_base);
			else
				return __coresight_class9_visit_ngpr_table(rom_table_base);
		}
	}
	return __coresight_visit_device(&device);
}

static int coresight_class1_table_handler(struct coresight_rom_table *table)
{
	uint16_t offset;
	uint32_t romentry;
	uint32_t memtype;
	caddr_t base = (caddr_t)table;

	coresight_log("Class 0x1 ROM (%016llx): jep106=%04x, part=%04x\n",
		      (uint64_t)table->base,
		      table->jep106_ident, table->part);

	for (offset = 0; offset < CORESIGHT_CLASS1_ENTRIES; offset++) {
		romentry = __raw_readl(ROMENTRYn(table->base, offset));
		memtype = __raw_readl(MEMTYPE(table->base));
		coresight_log("offset: %d, romentry: 0x%x, memtype: 0x%x\n",
			      offset, romentry, memtype);
		if (!romentry) {
			break;
		}
		if (!ROMENTRY_OFFSET(romentry)) {
			continue;
		}
		if (!(ROMENTRY_PRESENT & romentry)) {
			coresight_log("Skip rom=%016llx, present=%02x\n",
				      (uint64_t)base,
				      (uint8_t)(ROMENTRY_PRESENT & romentry));
			continue;
		}
		if (!(ROMENTRY_FORMAT & romentry)) {
			coresight_log("Skip rom=%016llx, format=%02x\n",
				      (uint64_t)base,
				      (uint8_t)(ROMENTRY_FORMAT & romentry));
			continue;
		}
		if (!(ROMENTRY_POWERIDVALID & romentry)) {
			coresight_log("Skip rom=%016llx, powerid=%02x\n",
				      (uint64_t)base,
				      (uint8_t)(ROMENTRY_POWERID(romentry)));
			continue;
		}
		base = table->base +
		       (ROMENTRY_OFFSET(romentry) <<
			ROMENTRY_OFFSET_OFFSET);
		coresight_log("ROM: offset: %d, base: 0x%016llx\n",
			      offset, (uint64_t)base);
		coresight_visit_table(base);
	}
	return 0;
}

struct coresight_table coresight_class1_rom_table = {
	.name = "Class 0x1 ROM Table",
	.clazz = 0x01,
	.handler = coresight_class1_table_handler,
};

static int coresight_class9_table_handler(struct coresight_rom_table *table)
{
	return coresight_visit_device(table->base);
}

struct coresight_table coresight_class9_rom_table = {
	.name = "Class 0x9 ROM Table",
	.clazz = 0x09,
	.handler = coresight_class9_table_handler,
};

static int coresight_class15_table_handler(struct coresight_rom_table *table)
{
	return coresight_visit_device(table->base);
}

struct coresight_table coresight_class15_rom_table = {
	.name = "Class 0xF ROM Table",
	.clazz = 0x0f,
	.handler = coresight_class15_table_handler,
};

int coresight_init(caddr_t rom_table_base, caddr_t *blacklist)
{
	coresight_blacklist = blacklist;

	/* DEVTYPE enumerated devices */
	coresight_tpiu_init();
	coresight_etb_init();
	coresight_funnel_init();
	coresight_replicator_init();
	coresight_tmc_init();

	/* DEVARCH enumerated devices */
	coresight_stm_init();
	coresight_etm_init();
	coresight_pmu_init();
	coresight_cti_init();
	coresight_dbg_cpuv80a_init();

	coresight_register_table(&coresight_class1_rom_table);
	coresight_register_table(&coresight_class9_rom_table);
	coresight_visit_table(rom_table_base);
	return 0;
}

void err_coresight(const char *hint)
{
	printf("--ERROR: %s\n", hint);
	(void)cmd_help("coresight");
}

static int cmd_coresight(int argc, char **argv)
{
	char *cmd;
	int i;

	if (argc < 2) {
		err_coresight("Missing command!");
		return -EINVAL;
	}
	cmd = argv[1];
	if (0 == strcmp(cmd, "show")) {
		for (i = 0; i < coresight_nr_devices; i++) {
			if (!(coresight_devices[i]->flags &
			      CORESIGHT_FLAGS_ENUMERATOR)) {
				printf("%s - %d\n",
				       coresight_devices[i]->name,
				       coresight_devices[i]->nr_devs);
			}
		}
	} else {
		err_coresight("Missing command!");
		return -EINVAL;
	}
	return 0;
}

DEFINE_COMMAND(coresight, cmd_coresight, "Dump coresight visible components",
	" show\n"
	"    -List the coresight visible components\n"
);
