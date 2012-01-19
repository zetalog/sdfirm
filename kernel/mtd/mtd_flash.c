#include <target/flash.h>

struct flash_info {
	mtd_t mtd;
	mtd_addr_t addr;
	mtd_size_t size;
	mtd_size_t offset;
	mtd_size_t length;
};

mtd_cid_t flash_cid = INVALID_MTD_CID;
flash_bid_t flash_last_bid = 0;
struct mtd_info flash_infos[FLASH_MAX_BANKS];
struct flash_info flash_privs[FLASH_MAX_BANKS];

static boolean flash_open(uint8_t mode, mtd_addr_t addr, mtd_size_t size)
{
	flash_privs[flash_bid].offset = addr;
	flash_privs[flash_bid].length = size;
	return true;
}

static void flash_close(void)
{
}

static void flash_erase(mtd_t mtd, mtd_addr_t addr, mtd_size_t size)
{
}

static uint8_t flash_read(void)
{
	mtd_addr_t addr = flash_privs[flash_bid].addr + flash_privs[flash_bid].offset;
	flash_privs[flash_bid].offset++;
	flash_privs[flash_bid].length--;
	return *(uint8_t *)addr;
}

static void flash_write(uint8_t byte)
{
	mtd_addr_t addr = flash_privs[flash_bid].addr + flash_privs[flash_bid].offset;
	flash_word_t word;
	uint8_t offset = (addr & (FLASH_HW_WORD_SIZE-1)) << 3;

	word = *(flash_word_t *)(addr & ~(FLASH_HW_WORD_SIZE-1));
	word &= ~(flas_word_t)0xff << offset;
	word |= (flas_word_t)byte << offset;

	flash_privs[flash_bid].offset++;
	flash_privs[flash_bid].length--;
	flash_hw_program_word(word);
}

mtd_chip_t flash_chip = {
	flash_erase,
	flash_open,
	flash_close,
	flash_read,
	flash_write,
};

mtd_t flash_register_bank(mtd_addr_t addr, mtd_size_t size)
{
	flash_bid_t bid;

	BUG_ON(flash_cid == INVALID_MTD_CID);

	/* allocate dataflash bank */
	BUG_ON(flash_last_bid >= FLASH_MAX_BANKS);
	bid = flash_last_bid;
	flash_last_bid++;

	/* register flash bank */
	flash_privs[bid].addr = addr;
	flash_privs[bid].size = size;

	/* register MTD device */
	flash_infos[bid].cid = flash_cid;
	flash_infos[bid].pageorder = __fls16(FLASH_HW_PAGE_SIZE);
	flash_infos[bid].nr_pages = div32u(size, FLASH_HW_PAGE_SIZE);
	flash_privs[bid].mtd = mtd_add_device(&flash_infos[bid]);

	return flash_privs[bid].mtd;
}

void flash_init(void)
{
	flash_hw_ctrl_init();
	flash_cid = mtd_register_chip(&flash_chip);
}
