#include <target/dataflash.h>

struct dataflash_info {
	spi_t spi;
	mtd_t mtd;
	mtd_size_t offset;
	mtd_size_t length;
};

dataflash_bid_t dataflash_last_bid = 0;
mtd_cid_t dataflash_cid = INVALID_MTD_CID;

dataflash_bid_t dataflash_bid = INVALID_DATAFLASH_BID;
spi_device_t dataflash_banks[DATAFLASH_MAX_BANKS];
struct mtd_info dataflash_infos[DATAFLASH_MAX_BANKS];
struct dataflash_info dataflash_privs[DATAFLASH_MAX_BANKS];

void dataflash_select(dataflash_bid_t bid)
{
	BUG_ON(bid >= dataflash_last_bid);
	BUG_ON(dataflash_privs[bid].spi == INVALID_SPI_DID);
	dataflash_deselect();
	if (bid != INVALID_DATAFLASH_BID) {
		dataflash_bid = bid;
	}
}

void dataflash_deselect(void)
{
	if (dataflash_bid != INVALID_DATAFLASH_BID) {
		dataflash_bid = INVALID_DATAFLASH_BID;
	}
}

uint8_t dataflash_status(void)
{
	uint8_t status;
	spi_select_device(dataflash_privs[dataflash_bid].spi);
	spi_write_byte(DATAFLASH_SPI_STATUS_READ);
	status = spi_read_byte();
	spi_deselect_device();
	return status;
}

uint8_t dataflash_waitready(void)
{
	uint8_t status;
	do {
		status = dataflash_status();
	} while (!(status & DATAFLASH_STATUS_RDY_BUSY));
	return status;
}

static mtd_addr_t dataflash_page2addr(mtd_addr_t addr)
{
	mtd_addr_t max_addr = __roundup16(dataflash_hw_page_size());
	return addr << (__fls32(max_addr));
}

static dataflash_bid_t dataflash_mtd2bid(mtd_t mtd)
{
	dataflash_bid_t bid = 0;

	while (bid != dataflash_last_bid) {
		if (mtd == dataflash_privs[bid].mtd)
			return bid;
		bid++;
	}
	return INVALID_DATAFLASH_BID;
}

static mtd_page_t dataflash_page_number(mtd_addr_t addr)
{
	uint8_t order;
	mtd_addr_t page;

	order = dataflash_infos[dataflash_bid].pageorder;

	page = addr & ~((1 << order) - 1);
	page >>= order;
	return page;
}

static boolean dataflash_sanity_page(mtd_page_t page)
{
	return page < dataflash_infos[dataflash_bid].nr_pages;
}

static void __dataflash_spi_exchange(uint8_t opcode, mtd_addr_t addr)
{
	spi_write_byte(opcode);
	spi_write_byte((uint8_t)(addr >> 16));
	spi_write_byte((uint8_t)(addr >> 8));
	spi_write_byte((uint8_t)(addr >> 0));
}

static void dataflash_spi_exchange(uint8_t opcode, mtd_addr_t addr)
{
	spi_select_device(dataflash_privs[dataflash_bid].spi);
	__dataflash_spi_exchange(opcode, addr);
	spi_deselect_device();
}

#if 0
static mtd_addr_t dataflash_page_offset(mtd_addr_t addr)
{
	return mtd_page_offset(addr, dataflash_infos[dataflash_bid].pageorder);
}

static void dataflash_cache_set_flags(uint8_t flags)
{
	dataflash_caches[dataflash_bid].flags |= flags;
}

static void dataflash_cache_clear_flags(uint8_t flags)
{
	dataflash_caches[dataflash_bid].flags &= ~flags;
}

static void dataflash_buffer_read(uint8_t *buffer,
				  mtd_addr_t offset, mtd_size_t length)
{
	mtd_size_t i;

	spi_select_device(dataflash_privs[dataflash_bid].spi);
	__dataflash_spi_exchange(DATAFLASH_SPI_BUFFER1_READ,
				 dataflash_page2addr(offset));
	spi_write_byte(0xFF);
	for (i = 0; i < length; i++)
		buffer[i] = spi_read_byte();
	spi_deselect_device();
}

static void dataflash_buffer_write(uint8_t *buffer,
				   mtd_addr_t offset, mtd_size_t length)
{
	mtd_size_t i;

	spi_select_device(dataflash_privs[dataflash_bid].spi);
	__dataflash_spi_exchange(DATAFLASH_ANY_BUFFER1_WRITE,
				 dataflash_page2addr(offset));
	for (i = 0; i < length; i++)
		spi_write_byte(buffer[i]);
	spi_deselect_device();
	dataflash_cache_set_flags(MTD_CACHE_DIRTY);
}

static void dataflash_cache_save(void)
{
	if (dataflash_caches[dataflash_bid].flags & MTD_CACHE_DIRTY) {
		mtd_page_t page = dataflash_caches[dataflash_bid].page;
		/* Buffer to Main Memory Page Program with Built-in Erase */
		dataflash_spi_exchange(DATAFLASH_ANY_BUFFER1_PAGE_PRG_ERASE,
				       dataflash_page2addr(page));
		dataflash_waitready();
		dataflash_cache_clear_flags(MTD_CACHE_DIRTY);
	}
}

static void dataflash_cache_load(mtd_addr_t page)
{
	if (page != dataflash_caches[dataflash_bid].page) {
		dataflash_cache_save();
		dataflash_cache_clear_flags(MTD_CACHE_VALID);
	}

	if (!(dataflash_caches[dataflash_bid].flags & MTD_CACHE_VALID)) {
		/* Main Memory Page to Buffer Transfer */
		dataflash_caches[dataflash_bid].page = page;
		dataflash_spi_exchange(DATAFLASH_ANY_PAGE_BUFFER1_XFR,
				       dataflash_page2addr(page));
		/* wait transfer completion */
		dataflash_waitready();
		dataflash_cache_set_flags(MTD_CACHE_VALID);
	}
}
#endif

#if 0
static void dataflash_page_read(mtd_page_t page)
{
	__dataflash_spi_write_cmd(DATAFLASH_MAIN_MEMORY_PAGE_READ, page);
	spi_write_byte(0xFF);
	spi_write_byte(0xFF);
	spi_write_byte(0xFF);
	spi_write_byte(0xFF);
	dataflash_waitready();
}
#endif

#define DATAFLASH_PAGES_PER_BLOCK	0x8

/* addr is the address in byte */
static void dataflash_erase(mtd_t mtd, mtd_addr_t addr, mtd_size_t size)
{
	dataflash_bid_t bid = dataflash_mtd2bid(mtd);
	mtd_page_t page;
	mtd_size_t eraselen, blocksize;
	boolean do_block;
	uint8_t cmd;
	uint8_t nr_pages;

	dataflash_select(bid);

	blocksize = 1 << (dataflash_infos[bid].pageorder +
			  __fls16(DATAFLASH_PAGES_PER_BLOCK));

	while (size) {
		page = dataflash_page_number(addr);
		if (!dataflash_sanity_page(page))
			return;

		do_block = ((page & (DATAFLASH_PAGES_PER_BLOCK - 1)) == 0) &&
			    (size >= blocksize);

		/* Block Erase or Page Erase */
		if (do_block) {
			eraselen = blocksize;
			nr_pages = DATAFLASH_PAGES_PER_BLOCK;
			page = page & (DATAFLASH_PAGES_PER_BLOCK - 1);
			cmd = DATAFLASH_ANY_BLOCK_ERASE;
		} else {
			eraselen = min(size, mtd_page_size(dataflash_infos[bid].pageorder));
			nr_pages = 1;
			cmd = DATAFLASH_ANY_PAGE_ERASE;
		}

#if 0
		if ((dataflash_caches[bid].page >= page) &&
		    (dataflash_caches[bid].page <= (page + nr_pages - 1))) {
			dataflash_cache_clear_flags(MTD_CACHE_DIRTY | MTD_CACHE_VALID);
		}
#endif
	
		dataflash_spi_exchange(cmd, dataflash_page2addr(page));
		dataflash_waitready();

		addr += eraselen;
		size -= eraselen;
	}
}

mtd_addr_t dataflash_addr2addr(mtd_addr_t addr)
{
	return dataflash_page2addr(dataflash_page_number(addr)) +
	       mtd_page_offset(addr, dataflash_infos[dataflash_bid].pageorder);
}

mtd_addr_t dataflash_page_addr(mtd_addr_t addr)
{
	return dataflash_page2addr(dataflash_page_number(addr));
}

static boolean dataflash_open(uint8_t mode,
			      mtd_addr_t addr, mtd_size_t size)
{
	dataflash_select(dataflash_mtd2bid(mtd_id));
	if (mode == OPEN_READ) {
		spi_select_device(dataflash_privs[dataflash_bid].spi);
		__dataflash_spi_exchange(DATAFLASH_SPI_ARRAY_READ,
					 dataflash_addr2addr(addr));
		/* dummy bytes */
		spi_write_byte(0xFF);
		spi_write_byte(0xFF);
		spi_write_byte(0xFF);
		spi_write_byte(0xFF);
	} else if (mode == OPEN_WRITE) {
		spi_select_device(dataflash_privs[dataflash_bid].spi);
		if (mtd_page_offset(addr, dataflash_infos[dataflash_bid].pageorder) != 0 ||
		    size < __roundup16(dataflash_hw_page_size())) {
			/* Main Memory Page to Buffer Transfer */
			__dataflash_spi_exchange(DATAFLASH_ANY_PAGE_BUFFER1_XFR,
						 dataflash_page_addr(addr));
			spi_deselect_device();
			dataflash_waitready();
			spi_select_device(dataflash_privs[dataflash_bid].spi);
		}
		/* Main Memory Page Program Through Buffer */
		__dataflash_spi_exchange(DATAFLASH_ANY_PAGE_BUFFER1_PRG,
					 dataflash_addr2addr(addr));
	}
	dataflash_privs[dataflash_bid].offset = addr;
	dataflash_privs[dataflash_bid].length = size;
	return true;
}

static void dataflash_close(void)
{
	spi_deselect_device();
	dataflash_waitready();
}

#ifdef CONFIG_MTD_BLOCK_RW
#define dataflash_read		spi_read_byte
#define dataflash_write		spi_write_byte
#else
uint8_t dataflash_read_byte(void)
{
	dataflash_privs[dataflash_bid].offset++;
	dataflash_privs[dataflash_bid].length--;
	return spi_read_byte();
}

static void dataflash_write_byte(uint8_t byte)
{
	dataflash_privs[dataflash_bid].offset++;
	dataflash_privs[dataflash_bid].length--;
	spi_write_byte(byte);

	if (mtd_page_offset(addr, dataflash_infos[dataflash_bid].pageorder) == 0 &&
	    size < __roundup16(dataflash_hw_page_size())) {
		/* Main Memory Page to Buffer Transfer */
		__dataflash_spi_exchange(DATAFLASH_ANY_PAGE_BUFFER1_XFR,
					 dataflash_page_addr(addr));
		spi_deselect_device();
		dataflash_waitready();
		spi_select_device(dataflash_privs[dataflash_bid].spi);
	}
}

static void dataflash_skip_oob_bytes(boolean read)
{
	mtd_size_t i;
	mtd_size_t oob_size = dataflash_hw_page_size() -
			      mtd_page_size(dataflash_infos[dataflash_bid].pageorder);

	if (mtd_page_offset(dataflash_privs[dataflash_bid].offset+1,
			    dataflash_infos[dataflash_bid].pageorder) != 0) {
		return;
	}
	if (dataflash_privs[dataflash_bid].offset <
	    mtd_page_size(dataflash_infos[dataflash_bid].pageorder)) {
		return;
	}
	for (i = 0; i < oob_size; i++) {
		if (read)
			(void)dataflash_read_byte();
		else
			dataflash_write_byte(0x00);
	}
}

static uint8_t dataflash_read(void)
{
	dataflash_skip_oob_bytes(true);
	return dataflash_read_byte();
}

static void dataflash_write(uint8_t byte)
{
	dataflash_skip_oob_bytes(false);
	dataflash_write_byte(byte);
}
#endif

mtd_chip_t dataflash_chip = {
	dataflash_erase,
	dataflash_open,
	dataflash_close,
	dataflash_read,
	dataflash_write,
};

mtd_t dataflash_register_bank(uint8_t chip)
{
	dataflash_bid_t bid;

	BUG_ON(dataflash_cid == INVALID_MTD_CID);

	/* allocate dataflash bank */
	BUG_ON(dataflash_last_bid >= DATAFLASH_MAX_BANKS);
	bid = dataflash_last_bid;
	dataflash_last_bid++;

	/* initialize mtd_cache */
	//dataflash_cache_clear_flags(MTD_CACHE_VALID | MTD_CACHE_DIRTY);

	/* register SPI device */
	dataflash_banks[bid].mode = DATAFLASH_HW_SPI_MODE;
	dataflash_banks[bid].max_freq_khz = DATAFLASH_HW_MAX_FREQ;
	dataflash_banks[bid].chip = chip;
	dataflash_privs[bid].spi = spi_register_device(&dataflash_banks[bid]);

	/* register MTD device */
	dataflash_infos[bid].cid = dataflash_cid;
	dataflash_infos[bid].pageorder = __fls16(dataflash_hw_page_size());
	dataflash_infos[bid].nr_pages = DATAFLASH_HW_MAX_PAGES;
	dataflash_privs[bid].mtd = mtd_add_device(&dataflash_infos[bid]);

	return dataflash_privs[bid].mtd;
}

void dataflash_init(void)
{
	dataflash_hw_chip_init();
	dataflash_cid = mtd_register_chip(&dataflash_chip);
}
