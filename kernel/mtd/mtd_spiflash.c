#include <target/spiflash.h>
#include <target/cmdline.h>

#ifdef CONFIG_SPIFLASH_APB_DELAY
#define spiflash_apb_delay(count)	apb_delay(count)
#else
#define spiflash_apb_delay(count)	do { } while (0)
#endif
#ifdef CONFIG_SPIFLASH_MODE_0
#define SPIFLASH_SPI_MODE		SPI_MODE(SPI_MODE_0)
#endif
#ifdef CONFIG_SPIFLASH_MODE_3
#define SPIFLASH_SPI_MODE		SPI_MODE(SPI_MODE_3)
#endif
#ifdef CONFIG_SPIFLASH_STD
#define SPIFLASH_SPI_FLASH		SPI_FLASH(SPI_FLASH_3WIRE)
#endif
#ifdef CONFIG_SPIFLASH_DUAL
#define SPIFLASH_SPI_FLASH		SPI_FLASH(SPI_FLASH_4WIRE)
#endif
#ifdef CONFIG_SPIFLASH_QUAD
#define SPIFLASH_SPI_FLASH		SPI_FLASH(SPI_FLASH_6WIRE)
#endif
#define SPIFLASH_PAGE_SIZE		256
#ifdef CONFIG_SPIFLASH_ERASE_4K
#define SPIFLASH_ERASE_SIZE		4096
#define SF_ERASE_DATA			SF_SECTOR_ERASE_4K
#endif
#ifdef CONFIG_SPIFLASH_ERASE_32K
#define SPIFLASH_ERASE_SIZE		UL(32768)
#define SF_ERASE_DATA			SF_BLOCK_ERASE_32K
#endif
#ifdef CONFIG_SPIFLASH_ERASE_64K
#define SPIFLASH_ERASE_SIZE		UL(65536)
#define SF_ERASE_DATA			SF_BLOCK_ERASE_64K
#endif
#define SPIFLASH_PAGES_PER_BLOCK	\
	(SPIFLASH_ERASE_SIZE/SPIFLASH_PAGE_SIZE)

#define spiflash_tx(byte)			\
	do {					\
		spiflash_apb_delay(APB_DELAY);	\
		spi_write_byte(byte);		\
	} while (0)
#define spiflash_rx()			spi_read_byte()

struct spiflash_info {
	spi_t spi;
	mtd_t mtd;
	mtd_size_t offset;
	mtd_size_t length;
	uint8_t status3;
};

spiflash_bid_t spiflash_last_bid = 0;
mtd_cid_t spiflash_cid = INVALID_MTD_CID;
spiflash_bid_t spiflash_bid = INVALID_SPIFLASH_BID;
spi_device_t spiflash_banks[SPIFLASH_MAX_BANKS];
struct mtd_info spiflash_infos[SPIFLASH_MAX_BANKS];
struct spiflash_info spiflash_privs[SPIFLASH_MAX_BANKS];

void spiflash_select(spiflash_bid_t bid)
{
	BUG_ON(bid >= spiflash_last_bid);
	BUG_ON(spiflash_privs[bid].spi == INVALID_SPI_DID);
	spiflash_deselect();
	if (bid != INVALID_SPIFLASH_BID)
		spiflash_bid = bid;
}

void spiflash_deselect(void)
{
	if (spiflash_bid != INVALID_SPIFLASH_BID)
		spiflash_bid = INVALID_SPIFLASH_BID;
}

/* Address mode dependent exchange */
static void spiflash_eeprom(uint8_t opcode, mtd_addr_t addr, bool ads)
{
	spiflash_tx(opcode);
	if (ads)
		spiflash_tx((uint8_t)(addr >> 24));
	spiflash_tx((uint8_t)(addr >> 16));
	spiflash_tx((uint8_t)(addr >> 8));
	spiflash_tx((uint8_t)(addr >> 0));
}

static void spiflash_spi_eeprom(uint8_t opcode, mtd_addr_t addr, bool ads)
{
	__unused uint8_t status;

	spiflash_eeprom(opcode, addr, ads);
	spi_select_device(spiflash_privs[spiflash_bid].spi);
	status = spiflash_rx();
	spi_deselect_device();
}

static uint8_t spiflash_txrx(uint8_t byte)
{
	spiflash_tx(byte);
	return spiflash_rx();
}

static uint8_t spiflash_spi_status(uint8_t opcode)
{
	uint8_t status;

	/* Status register will be repeated continously unless the CS is
	 * driven high. Thus, unlike spiflash_eeprom(), CS is driven low
	 * prior than preparing the SPI request.
	 */
	spi_select_device(spiflash_privs[spiflash_bid].spi);
	status = spiflash_txrx(opcode);
	spi_deselect_device();
	return status;
}

uint8_t spiflash_waitready(void)
{
	uint8_t status;
	do {
		status = spiflash_spi_status(SF_READ_STATUS_1);
	} while (status & SF_BUSY);
	return status;
}

bool spiflash_is4byte(spiflash_bid_t bid)
{
	uint8_t status;

	status = spiflash_spi_status(SF_READ_STATUS_3);
	spiflash_privs[spiflash_bid].status3 = status;
	return !!(status & SF_ADS);
}

static mtd_addr_t spiflash_page2addr(mtd_addr_t addr)
{
	mtd_addr_t max_addr = __roundup16(SPIFLASH_PAGE_SIZE);
	return addr << (__fls32(max_addr));
}

static spiflash_bid_t spiflash_mtd2bid(mtd_t mtd)
{
	spiflash_bid_t bid = 0;

	while (bid != spiflash_last_bid) {
		if (mtd == spiflash_privs[bid].mtd)
			return bid;
		bid++;
	}
	return INVALID_SPIFLASH_BID;
}

static mtd_page_t spiflash_page_number(mtd_addr_t addr)
{
	uint8_t order;
	mtd_addr_t page;

	order = spiflash_infos[spiflash_bid].pageorder;

	page = addr & ~((1 << order) - 1);
	page >>= order;
	return page;
}

mtd_addr_t spiflash_addr2addr(mtd_addr_t addr)
{
	return spiflash_page2addr(spiflash_page_number(addr)) +
	       mtd_page_offset(addr, spiflash_infos[spiflash_bid].pageorder);
}

mtd_addr_t spiflash_page_addr(mtd_addr_t addr)
{
	return spiflash_page2addr(spiflash_page_number(addr));
}

#ifdef CONFIG_SPIFLASH_ADDR_4BYTE
static void spiflash_spi_write0(uint8_t opcode)
{
	spi_select_device(spiflash_privs[spiflash_bid].spi);
	spiflash_tx(opcode);
	(void)spiflash_rx();
	spi_deselect_device();
}

void spiflash_enter_4byte(spiflash_bid_t bid)
{
	spiflash_spi_write0(SF_ENTER_ADDR_4BYTE);
	(void)spiflash_is4byte(bid);
}

void spiflash_exit_4byte(spiflash_bid_t bid)
{
	spiflash_spi_write0(SF_EXIT_ADDR_4BYTE);
	(void)spiflash_is4byte(bid);
}
#else
#define spiflash_enter_4byte(bid)		do { } while (0)
#define spiflash_exit_4byte(bid)		do { } while (0)
#endif

static void spiflash_erase(mtd_t mtd, mtd_addr_t addr, mtd_size_t size)
{
	spiflash_bid_t bid = spiflash_mtd2bid(mtd);
	mtd_page_t page;
	mtd_size_t eraselen, blocksize;
	boolean do_block;
	bool ads = !!(spiflash_privs[bid].status3 & SF_ADS);

	spiflash_select(bid);

	blocksize = 1 << (spiflash_infos[bid].pageorder +
			  __fls16(SPIFLASH_PAGES_PER_BLOCK));

	while (size) {
		page = spiflash_page_number(addr);
		do_block = ((page & (SPIFLASH_PAGES_PER_BLOCK - 1)) == 0) &&
			    (size >= blocksize);

		BUG_ON(!do_block);

		/* Block Erase or Sector Erase */
		eraselen = blocksize;
		page = page & (SPIFLASH_PAGES_PER_BLOCK - 1);

		spiflash_waitready();
		spiflash_spi_eeprom(SF_ERASE_DATA,
				    spiflash_page2addr(page), ads);

		addr += eraselen;
		size -= eraselen;
	}
}

static uint8_t spiflash_read(void)
{
	uint8_t byte = 0;
	bool ads = !!(spiflash_privs[spiflash_bid].status3 & SF_ADS);

	if (spiflash_privs[spiflash_bid].length) {
		spiflash_eeprom(SF_READ_DATA,
				spiflash_privs[spiflash_bid].offset, ads);
		spi_select_device(spiflash_privs[spiflash_bid].spi);
		byte = spiflash_rx();
		spiflash_privs[spiflash_bid].offset++;
		spiflash_privs[spiflash_bid].length--;
		spi_deselect_device();
	}
	return byte;
}

static void spiflash_write(uint8_t byte)
{
	bool ads = !!(spiflash_privs[spiflash_bid].status3 & SF_ADS);

	if (spiflash_privs[spiflash_bid].length) {
		spiflash_eeprom(SF_PAGE_PROGRAM,
				spiflash_privs[spiflash_bid].offset, ads);
		spi_select_device(spiflash_privs[spiflash_bid].spi);
		spiflash_tx(byte);
		spiflash_privs[spiflash_bid].offset++;
		spiflash_privs[spiflash_bid].length--;
		spi_deselect_device();
	}
}

static boolean spiflash_open(uint8_t mode,
			     mtd_addr_t addr, mtd_size_t size)
{
	spiflash_select(spiflash_mtd2bid(mtd_id));
	spiflash_waitready();
	spiflash_privs[spiflash_bid].offset = addr;
	spiflash_privs[spiflash_bid].length = size;
	spiflash_enter_4byte(spiflash_bid);
	return true;
}

static void spiflash_close(void)
{
	spiflash_exit_4byte(spiflash_bid);
}

mtd_chip_t spiflash_chip = {
	spiflash_erase,
	spiflash_open,
	spiflash_close,
	spiflash_read,
	spiflash_write,
};

mtd_t spiflash_register_bank(uint8_t chip)
{
	spiflash_bid_t bid;

	BUG_ON(spiflash_cid == INVALID_MTD_CID);

	/* allocate flash bank */
	BUG_ON(spiflash_last_bid >= SPIFLASH_MAX_BANKS);
	bid = spiflash_last_bid;
	spiflash_last_bid++;

	/* register SPI device */
	spiflash_banks[bid].mode = SPI_ORDER(SPI_MSB) |
		SPIFLASH_SPI_MODE | SPIFLASH_SPI_FLASH;
	spiflash_banks[bid].max_freq_khz = SPIFLASH_HW_MAX_FREQ;
	spiflash_banks[bid].chip = chip;
	spiflash_privs[bid].spi = spi_register_device(&spiflash_banks[bid]);

	/* register MTD device */
	spiflash_infos[bid].cid = spiflash_cid;
	spiflash_infos[bid].pageorder = __fls16(SPIFLASH_PAGE_SIZE);
	spiflash_infos[bid].eraseorder = __fls16(SPIFLASH_ERASE_SIZE);
	spiflash_infos[bid].nr_pages = SPIFLASH_HW_MAX_SIZE/SPIFLASH_PAGE_SIZE;
	spiflash_privs[bid].mtd = mtd_add_device(&spiflash_infos[bid]);

	return spiflash_privs[bid].mtd;
}

void spiflash_init(void)
{
	spiflash_hw_chip_init();
	spiflash_cid = mtd_register_chip(&spiflash_chip);
}

static int spiflash_readid(int argc, char *argv[])
{
	spiflash_tx(SF_JEDEC_ID);
	printf("0x%x\n", spiflash_rx());
	return 0;
}

static int do_spiflash(int argc, char *argv[])
{
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "readid") == 0)
		return spiflash_readid(argc, argv);
}

DEFINE_COMMAND(spiflash, do_spiflash, "SPIflash commands",
	"readid\n"
	"    - read spi flash device ID\n"
);