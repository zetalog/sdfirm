#include <target/spiflash.h>
#include <target/cmdline.h>
#include <target/mem.h>

mtd_t board_flash = INVALID_MTD_ID;

void dpu_ssi_flash_init(void)
{
	board_flash = spiflash_register_bank(0);
}

static int do_flash(int argc, char *argv[])
{
	mtd_t smtd;
	uint8_t buffer[256];
	int i;

	smtd = mtd_save_device(board_flash);
	mtd_open(OPEN_READ, 0, 256);
	for (i = 0; i < 256; i++)
		buffer[i] = mtd_read_byte();
	mtd_close();
	mtd_restore_device(smtd);
	mem_print_data(0, buffer, 1, sizeof(buffer));
	return 0;
}

DEFINE_COMMAND(flash, do_flash, "SSI flash commands",
	"dump\n"
	"    - dump first 256 bytes of SSI flash\n"
);
