#include <target/generic.h>
#include <target/cmdline.h>

bh_t efuse_bh;

static int efuse_repair_start()
{
	uint32_t v = efuse_read_repair_rf()
	efuse_repair_prepare();
	return v;
}

static int efuse_repair(uint16_t addr)
{
	return efuse_repair_single(addr);
}

static int efuse_repair_end()
{
	return efuse_repair_finish();
}

static int efuse_test(uint16_t waddr, uint16_t raddr, uint16_t mode)
{
	return efuse_test_func(waddr, raddr, mode);
}

static int do_efuse(int argc, char *argv[])
{
	if (argc < 3)
		return -EINVAL;
	uint16_t addr = (uint16_t)strtoull(argv[3], 0, 0);
	if (strcmp(argv[1], "repair")) {
		if (strcmp(argv[2], "start"))
			return efuse_repair_start();
		else if (strcmp(argv[2], "end"))
			return efuse_repair_end();
		else 
			return efuse_repair(addr);
	}
	uint16_t value = (uint16_t)strtoull(argv[4], 0, 0);
	if (strcmp(argv[1], "read"))
		return efuse_read(addr, value);
	if (strcmp(argv[1], "write"))
		return efuse_write(addr, value);
	uint16_t mode = (uint16_t)strtoull(argv[5], 0, 0);
	if (strcmp(argv[1], "test"))
		return efuse_test(addr, value, mode);
}

DEFINE_COMMAND(efuse, do_efuse, "K1MXLite eFuse commands",
	"efuse read <addr> <mode>\n"
	"	-efuse read regs\n"
	"efuse write <addr> <value>\n"
	"	-efuse write regs\n"
	"efuse repair start\n"
	"	-display efuse repair bit & get ready for repair\n"
	"efuse repair end\n"
	"	-finish efuse repair\n"
	"efuse repair <addr>\n"
	"	-repair efuse bits\n"
	"efuse test <waddr> <raddr> <mode>\n"
	"	-efuse test function\n"
);