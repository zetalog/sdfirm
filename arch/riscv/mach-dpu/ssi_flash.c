#include <target/spiflash.h>
#include <target/cmdline.h>
#include <target/mem.h>
#include <target/efi.h>

#define FLASH_PAGE_SIZE		256
#define FLASH_TOTAL_SIZE	4000000

mtd_t board_flash = INVALID_MTD_ID;

void dpu_ssi_flash_init(void)
{
	board_flash = spiflash_register_bank(0);
}

int dw_ssi_flash_copy(uint8_t *buf, uint32_t addr, uint32_t size)
{
	mtd_t smtd;
	int i;

	smtd = mtd_save_device(board_flash);
	mtd_open(OPEN_READ, addr, size);
	for (i = 0; i < size; i++)
		buf[i] = mtd_read_byte();
	mtd_close();
	mtd_restore_device(smtd);
	return 0;
}

static int do_flash_gpt(int argc, char *argv[])
{
	uint8_t gpt_buf[FLASH_PAGE_SIZE];
	gpt_header hdr;
	uint64_t partition_entries_lba_end;
	gpt_partition_entry *gpt_entries;
	uint64_t i;
	uint32_t j;
	int err;
	uint32_t num_entries;

	err = dw_ssi_flash_copy((uint8_t *)&hdr,
		GPT_HEADER_LBA * FLASH_PAGE_SIZE, GPT_HEADER_BYTES);
	if (err)
		return -EINVAL;
	mem_print_data(0, &hdr, 1, sizeof (gpt_header));
	partition_entries_lba_end = (hdr.partition_entries_lba +
		(hdr.num_partition_entries * hdr.partition_entry_size +
		 FLASH_PAGE_SIZE - 1) / FLASH_PAGE_SIZE);
	for (i = hdr.partition_entries_lba;
	     i < partition_entries_lba_end; i++) {
		dw_ssi_flash_copy(gpt_buf, i * FLASH_PAGE_SIZE,
				  FLASH_PAGE_SIZE);
		gpt_entries = (gpt_partition_entry *)gpt_buf;
		num_entries = FLASH_PAGE_SIZE / hdr.partition_entry_size;
		for (j = 0; j < num_entries; j++) {
			printf("%s:\n",
			       uuid_export(gpt_entries[j].partition_type_guid.u.uuid));
			printf("%016llX - %016llX \n",
			       gpt_entries[j].first_lba,
			       gpt_entries[i].last_lba);
		}
	}
	return 0;
}

static int do_flash_dump(int argc, char *argv[])
{
	uint8_t buffer[FLASH_PAGE_SIZE];
	uint32_t addr = 128;
	size_t size = 32;

	if (argc > 2)
		addr = strtoul(argv[2], NULL, 0);
	if (argc > 3)
		size = strtoul(argv[3], NULL, 0);
	if (addr >= FLASH_TOTAL_SIZE) {
		printf("addr should be less than %d\n", FLASH_TOTAL_SIZE);
		return -EINVAL;
	}
	if (addr > FLASH_PAGE_SIZE) {
		printf("size should be less or equal than %d\n",
		       FLASH_PAGE_SIZE);
		return -EINVAL;
	}
	dw_ssi_flash_copy(buffer, addr, size);
	mem_print_data(0, buffer, 1, size);
	return 0;
}

static int do_flash(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;

	if (strcmp(argv[1], "dump") == 0)
		return do_flash_dump(argc, argv);
	if (strcmp(argv[1], "gpt") == 0)
		return do_flash_gpt(argc, argv);
	return -ENODEV;
}

DEFINE_COMMAND(flash, do_flash, "SSI flash commands",
	"dump [addr] [size]\n"
	"    - dump content of SSI flash\n"
	"      addr: default to 128\n"
	"      size: default to 32\n"
	"gpt\n"
	"    - dump GPT partitions from SSI flash\n"
);
