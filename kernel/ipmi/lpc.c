#include <target/lpc.h>
#include <target/cmdline.h>

void lpc_init(void)
{
	lpc_hw_ctrl_init();
}

static int do_lpc_read(int argc, char *argv[])
{
	caddr_t addr;
	uint32_t val;

	if (argc < 3)
		return -EINVAL;

	if (strcmp(argv[2], "mem") == 0) {
		if (argc < 4)
			return -EINVAL;
		addr = (caddr_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[3], "1") == 0) {
			val = lpc_mem_read8(addr);
			printf("Memory: 0x%08lx=%02x\n", addr, (uint8_t)val);
		} else if (strcmp(argv[3], "2") == 0) {
			val = lpc_mem_read16(addr);
			printf("Memory: 0x%08lx=%04x\n", addr, (uint16_t)val);
		} else if (strcmp(argv[3], "4") == 0) {
			val = lpc_mem_read32(addr);
			printf("Memory: 0x%08lx=%08x\n", addr, val);
		} else
			return -EINVAL;
		return 0;
	} else {
		addr = (caddr_t)strtoull(argv[3], 0, 0);
		if (strcmp(argv[2], "io") == 0) {
			val = lpc_io_read8(addr);
			printf("IO: 0x%08lx=%02x\n", addr, (uint8_t)val);
		} else
			return -EINVAL;
		return 0;
	}
	return -EINVAL;
}

static int do_lpc_write(int argc, char *argv[])
{
	caddr_t addr;

	if (argc < 5)
		return -EINVAL;
	if (strcmp(argv[2], "mem") == 0) {
		uint32_t v;
		int size;
		if (argc < 6)
			return -EINVAL;
		size = (uint32_t)strtoull(argv[3], 0, 0);
		v = (uint32_t)strtoull(argv[4], 0, 0);
		addr = (caddr_t)strtoull(argv[5], 0, 0);
		if (size == 1)
			lpc_mem_write8(v, addr);
		else if (size == 2)
			lpc_mem_write16(v, addr);
		else if (size == 4)
			lpc_mem_write32(v, addr);
		else
			return -EINVAL;
		return 0;
	} else {
		uint8_t v;

		v = (uint32_t)strtoull(argv[3], 0, 0);
		addr = (caddr_t)strtoull(argv[4], 0, 0);
		if (strcmp(argv[2], "io") == 0)
			lpc_io_write8(v, addr);
		else
			return -EINVAL;
		return 0;
	}
	return -EINVAL;
}

static int do_lpc(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "read") == 0)
		return do_lpc_read(argc, argv);
	if (strcmp(argv[1], "write") == 0)
		return do_lpc_write(argc, argv);
	return -EINVAL;
}

DEFINE_COMMAND(lpc, do_lpc, "Low Pin Count (LPC) commands",
	"lpc read io <addr>\n"
	"lpc read mem [1|2|4] <addr>\n"
	"    -LPC read sequence\n"
	"lpc write io <value> <addr>\n"
	"lpc write mem [1|2|4] <value> <addr>\n"
	"    -LPC write sequence\n"
);
