#include <target/spiflash.h>
#include <target/cmdline.h>
#include <target/mem.h>

mtd_t board_flash = INVALID_MTD_ID;

void dpu_ssi_flash_init(void)
{
	board_flash = spiflash_register_bank(0);
}

static int do_flash_dump(int argc, char *argv[])
{
	mtd_t smtd;
	uint8_t buffer[128];
	int i;
	size_t size = sizeof (buffer);

	smtd = mtd_save_device(board_flash);
	mtd_open(OPEN_READ, 0, size);
	for (i = 0; i < size; i++)
		buffer[i] = mtd_read_byte();
	mtd_close();
	mtd_restore_device(smtd);
	mem_print_data(0, buffer, 1, size);
	return 0;
}

static int do_flash(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0)
		return do_flash_dump(argc, argv);
	return -ENODEV;
}

DEFINE_COMMAND(flash, do_flash, "SSI flash commands",
	"dump\n"
	"    - dump first 128 bytes of SSI flash\n"
);
