#include <target/scsi.h>
#include <target/mtd.h>

#define NR_MTD_BLOCKS	1
#define MTD_BLOCK_SIZE	((mtd_size_t)512)

struct mtd_block {
	struct sbc_device sbc;
	mtd_t mtd;
	mtd_t saved_mtd;
};

struct mtd_block mtd_blocks[NR_MTD_BLOCKS];
uint8_t mtd_nr_blocks = 0;

void mtd_block_open(struct sbc_device *dev, uint8_t type,
		    scsi_lba_t lba, scsi_lbs_t blks)
{
	struct mtd_block *blk = (struct mtd_block *)dev;
	struct mtd_info *info = mtd_get_info(blk->mtd);
	mtd_addr_t addr = (mtd_addr_t)lba << info->pageorder;
	mtd_size_t size = (mtd_size_t)blks << info->pageorder;

	blk->saved_mtd = mtd_save_device(blk->mtd);
	mtd_open(type, addr, size);
}

void mtd_block_close(struct sbc_device *dev)
{
	struct mtd_block *blk = (struct mtd_block *)dev;
	mtd_close();
	mtd_restore_device(blk->saved_mtd);
}

void mtd_block_create(mtd_t mtd)
{
	uint8_t blknr = mtd_nr_blocks;
	struct mtd_info *info = mtd_get_info(mtd);

#ifdef CONFIG_MTD_BLOCK
	mtd_blocks[blknr].sbc.block_size = (bulk_size_t)mtd_page_size(info->pageorder);
	mtd_blocks[blknr].sbc.nr_blocks = info->nr_pages;
#else
	mtd_blocks[blknr].sbc.block_size = MTD_BLOCK_SIZE;
	mtd_blocks[blknr].sbc.nr_blocks = info->nr_pages << 1;
#endif
	mtd_blocks[blknr].sbc.open_block = mtd_block_open;
	mtd_blocks[blknr].sbc.close_block = mtd_block_close;
	mtd_blocks[blknr].sbc.read_byte = mtd_read_byte;
	mtd_blocks[blknr].sbc.write_byte = mtd_write_byte;
	mtd_blocks[blknr].mtd = mtd;
	sbc_register_device(&mtd_blocks[blknr].sbc);
	mtd_nr_blocks++;
}
