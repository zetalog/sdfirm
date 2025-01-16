#include <target/mtd.h>
#include <target/cmdline.h>

mtd_chip_t *mtd_chips[NR_MTD_CHIPS];
struct mtd_dev mtd_devs[NR_MTD_DEVS];
mtd_t mtd_last_id = 0;
mtd_t mtd_last_cid = 0;
mtd_t mtd_id = INVALID_MTD_ID;

void mtd_restore_device(mtd_t mtd)
{
	mtd_id = mtd;
}

mtd_t mtd_save_device(mtd_t mtd)
{
	mtd_t smtd = mtd_id;
	mtd_restore_device(mtd);
	return smtd;
}

mtd_cid_t mtd_register_chip(mtd_chip_t *chip)
{
	mtd_cid_t cid;
	BUG_ON(mtd_last_cid >= INVALID_MTD_CID);
	cid = mtd_last_cid;
	mtd_chips[cid] = chip;
	mtd_last_cid++;
	return cid;
}

mtd_t mtd_add_device(struct mtd_info *info)
{
	mtd_t mtd;
	BUG_ON(mtd_last_id >= INVALID_MTD_ID);
	mtd = mtd_last_id;
	mtd_devs[mtd].info = info;
	mtd_devs[mtd].flags = 0;
	mtd_last_id++;
	return mtd;
}

static struct mtd_dev *mtd_get_dev(void)
{
	BUG_ON(mtd_id >= mtd_last_id || !mtd_devs[mtd_id].info);
	return &mtd_devs[mtd_id];
}

struct mtd_info *mtd_get_info(mtd_t mtd)
{
	BUG_ON(mtd >= mtd_last_id || !mtd_devs[mtd].info);
	return mtd_devs[mtd].info;
}

boolean mtd_open(uint8_t mode,
		 mtd_addr_t addr, mtd_size_t size)
{
	struct mtd_dev *dev = mtd_get_dev();
	boolean res = false;

	if (dev->flags & MTD_FLAG_OPENED)
		return false;

	BUG_ON(!mtd_chips[dev->info->cid]->open);
	
	res = mtd_chips[dev->info->cid]->open(mode, addr, size);
	if (res) {
		dev->flags |= MTD_FLAG_OPENED;
		dev->flags |= (mode & OPEN_READ) ? 0 : MTD_FLAG_WRITING;
	}

	return res;
}

void mtd_close(void)
{
	struct mtd_dev *dev = mtd_get_dev();

	if (!(dev->flags & MTD_FLAG_OPENED))
		return;

	BUG_ON(!mtd_chips[dev->info->cid]->close);

	mtd_chips[dev->info->cid]->close();
	dev->flags &= ~(MTD_FLAG_OPENED | MTD_FLAG_WRITING);
}

uint8_t mtd_read_byte(void)
{
	struct mtd_dev *dev = mtd_get_dev();

	BUG_ON(!(dev->flags & MTD_FLAG_OPENED) ||
	       !mtd_chips[dev->info->cid]->read);

	return mtd_chips[dev->info->cid]->read();
}

void mtd_write_byte(uint8_t byte)
{
	struct mtd_dev *dev = mtd_get_dev();

	BUG_ON(!(dev->flags & (MTD_FLAG_OPENED | MTD_FLAG_WRITING)) ||
	       !mtd_chips[dev->info->cid]->write);

	mtd_chips[dev->info->cid]->write(byte);
}

#ifdef CONFIG_MTD_BUFFER
void mtd_read_buffer(mtd_addr_t addr,
		     uint8_t *buff, mtd_size_t size)
{
	struct mtd_dev *dev = mtd_get_dev();

	BUG_ON(!(dev->flags & MTD_FLAG_OPENED) ||
	       !mtd_chips[dev->info->cid]->read_buffer);

	return mtd_chips[dev->info->cid]->read_buffer(addr, buffer, size);
}

void mtd_write_buffer(mtd_addr_t addr,
		      uint8_t *buff, mtd_size_t size)
{
	struct mtd_dev *dev = mtd_get_dev();

	BUG_ON(!(dev->flags & (MTD_FLAG_OPENED | MTD_FLAG_WRITING)) ||
	       !mtd_chips[dev->info->cid]->write_buffer);

	mtd_chips[dev->info->cid]->write_buffer(addr, buffer, size);
}
#else
void mtd_read_buffer(mtd_addr_t addr,
		     uint8_t *buff, mtd_size_t size)
{
	if (mtd_open(OPEN_READ, addr, size)) {
		mtd_size_t i;
		for (i = 0; i < size; i++) {
			buff[i] = mtd_read_byte();
		}
		mtd_close();
	}
}

void mtd_write_buffer(mtd_addr_t addr,
		      uint8_t *buff, mtd_size_t size)
{
	if (mtd_open(OPEN_WRITE, addr, size)) {
		mtd_size_t i;
		for (i = 0; i < size; i++) {
			mtd_write_byte(buff[i]);
		}
		mtd_close();
	}
}
#endif

void mtd_erase(mtd_t mtd, mtd_addr_t addr, mtd_size_t size)
{
	struct mtd_info *info = mtd_get_info(mtd);

	BUG_ON(!mtd_chips[info->cid]->erase);
	mtd_chips[info->cid]->erase(mtd, addr, size);
}

mtd_size_t mtd_page_size(uint8_t pageorder)
{
	return ((mtd_size_t)1 << pageorder);
}

mtd_addr_t mtd_page_offset(mtd_addr_t addr, uint8_t pageorder)
{
	return addr & ((1 << pageorder) - 1);
}

#ifdef CONFIG_MTD_CONCAT
mtd_cid_t mtd_concat_cid;
mtd_t mtd_last_concat;
struct mtd_info mtd_concat_infos[NR_MTD_CONCATS];
struct mtd_concat mtd_concat_privs[NR_MTD_CONCATS];

mtd_t mtd_concat_id(mtd_t mtd)
{
	mtd_t i;

	for (i = 0; i < mtd_last_concat; i++) {
		if (mtd_concat_privs[i].reg_mtd == mtd)
			return i;
	}
	BUG();
	return INVALID_MTD_ID;
}

struct mtd_concat *mtd_concat_priv(mtd_t mtd)
{
	mtd_t index = mtd_concat_id(mtd);
	if (mtd != INVALID_MTD_ID)
		return &mtd_concat_privs[index];
	return NULL;
}

#ifdef CONFIG_MTD_CONCAT_PAGE
mtd_t mtd_concat_chip_id(struct mtd_concat *priv, mtd_addr_t addr)
{
	mtd_t i;
	struct mtd_info *info = mtd_get_info(priv->reg_mtd);

	i = (uint8_t)((addr >> info->pageorder) & (priv->nr_devs-1));
	if (i >= priv->nr_devs)
		return INVALID_MTD_ID;
	return priv->devs[i];
}

mtd_addr_t mtd_concat_chip_addr(struct mtd_concat *priv, mtd_addr_t addr)
{
	struct mtd_info *info = mtd_get_info(priv->reg_mtd);
	mtd_addr_t offset;
	mtd_page_t pagenr;

	offset = mtd_page_offset(addr, info->pageorder);
	pagenr = addr & ~((1<<(info->pageorder+__fls16(priv->nr_devs)))-1);
	return offset | (pagenr >> (priv->nr_devs-1));
}

mtd_size_t mtd_concat_chip_size(struct mtd_concat *priv,
				mtd_addr_t addr, mtd_size_t size)
{
	struct mtd_info *info = mtd_get_info(priv->reg_mtd);
	mtd_size_t length;
					      
	length = mtd_page_size(info->pageorder) -
		 mtd_page_offset(mtd_concat_chip_addr(priv, addr),
				 info->pageorder);
	return min(length, size);
}
#endif

#ifdef CONFIG_MTD_CONCAT_CHIP
mtd_t mtd_concat_chip_id(struct mtd_concat *priv, mtd_addr_t addr)
{
	mtd_t i;
	struct mtd_info *info = mtd_get_info(priv->reg_mtd);

	i = (addr >> (info->pageorder+__fls32(info->nr_pages)));
	BUG_ON(i >= priv->nr_devs);
	return priv->devs[i];
}

mtd_addr_t mtd_concat_chip_addr(struct mtd_concat *priv, mtd_addr_t addr)
{
	struct mtd_info *info = mtd_get_info(priv->reg_mtd);

	return addr & ((1<<(info->pageorder+__fls32(info->nr_pages))) - 1);
}

mtd_size_t mtd_concat_chip_size(struct mtd_concat *priv,
				mtd_addr_t addr, mtd_size_t size)
{
	mtd_size_t length;
	
	length = (1<<(info->pageorder+__fls32(info->nr_pages))) -
		 mtd_concat_chip_addr(priv, addr);
	return min(length, size);
}
#endif

static void mtd_concat_erase(mtd_t mtd, mtd_addr_t addr, mtd_size_t length)
{
	mtd_t erase_mtd;
	struct mtd_info *erase_info;
	mtd_size_t offset = 0, erase_size;
	mtd_addr_t erase_addr;
	
	while (length) {
		struct mtd_concat *priv = mtd_concat_priv(mtd);

		erase_mtd = mtd_concat_chip_id(priv, addr);
		BUG_ON(erase_mtd >= mtd_last_cid);
		erase_info = mtd_get_info(erase_mtd);
		erase_addr = mtd_concat_chip_addr(priv, addr+offset);
		erase_size = mtd_page_size(erase_info->eraseorder) -
			     mtd_page_offset(erase_addr,
					     erase_info->eraseorder);
		erase_size = min(length, erase_size);
		mtd_erase(erase_mtd, erase_addr, erase_size);

		offset += erase_size;
		length -= erase_size;
	}
}

static void mtd_concat_close_chip(struct mtd_concat *priv)
{
	if (priv->opened_mtd != INVALID_MTD_ID) {
		mtd_t smtd = mtd_save_device(priv->opened_mtd);
		mtd_close();
		mtd_restore_device(smtd);
		priv->opened_mtd = INVALID_MTD_ID;
	}
}

static void mtd_concat_open_chip(struct mtd_concat *priv)
{
	mtd_t smtd;
	mtd_t opened_mtd = mtd_concat_chip_id(priv,
					      priv->opened_offset);
	BUG_ON(opened_mtd == INVALID_MTD_ID);
	if (opened_mtd != priv->opened_mtd) {
		mtd_size_t length;
		mtd_concat_close_chip(priv);
		length = mtd_concat_chip_size(priv,
					      priv->opened_offset,
					      priv->opened_length);
		smtd = mtd_save_device(opened_mtd);
		if (mtd_open(priv->opened_mode,
			     mtd_concat_chip_addr(priv, priv->opened_offset),
			     length)) {
			priv->opened_mtd = opened_mtd;
		}
		mtd_restore_device(smtd);
	}
}

boolean mtd_concat_open(uint8_t mode,
			mtd_addr_t addr, mtd_size_t size)
{
	struct mtd_concat *priv = mtd_concat_priv(mtd_id);

	BUG_ON(priv->reg_mtd != mtd_id);
	if (mtd_concat_chip_id(priv, addr+size) == INVALID_MTD_ID)
		return false;
	priv->opened_mode = mode;
	priv->opened_offset = addr;
	priv->opened_length = size;
	priv->opened_mtd = INVALID_MTD_ID;
	mtd_concat_open_chip(priv);
	return true;
}

void mtd_concat_close(void)
{
	struct mtd_concat *priv = mtd_concat_priv(mtd_id);
	mtd_concat_close_chip(priv);
	priv->opened_mode = 0;
	priv->opened_offset = 0;
	priv->opened_length = 0;
}

#ifdef CONFIG_MTD_BLOCK_RW
static uint8_t mtd_concat_read(void)
{
	uint8_t byte;
	mtd_t smtd;
	struct mtd_concat *priv = mtd_concat_priv(mtd_id);

	smtd = mtd_save_device(priv->opened_mtd);
	byte = mtd_read_byte();
	mtd_restore_device(smtd);
	return byte;
}

static void mtd_concat_write(uint8_t byte)
{
	mtd_t smtd;
	struct mtd_concat *priv = mtd_concat_priv(mtd_id);

	smtd = mtd_save_device(priv->opened_mtd);
	mtd_write_byte(byte);
	mtd_restore_device(smtd);
}
#else
uint8_t mtd_concat_read(void)
{
	uint8_t byte;
	mtd_t smtd;
	struct mtd_concat *priv = mtd_concat_priv(mtd_id);
	mtd_concat_open_chip(priv);
	BUG_ON(priv->opened_mtd == INVALID_MTD_ID);

	smtd = mtd_save_device(priv->opened_mtd);
	byte = mtd_read_byte();
	mtd_restore_device(smtd);

	priv->opened_offset++;
	priv->opened_length--;
	return byte;
}

void mtd_concat_write(uint8_t byte)
{
	mtd_t smtd;
	struct mtd_concat *priv = mtd_concat_priv(mtd_id);
	mtd_concat_open_chip(priv);
	BUG_ON(priv->opened_mtd == INVALID_MTD_ID);

	smtd = mtd_save_device(priv->opened_mtd);
	mtd_write_byte(byte);
	mtd_restore_device(smtd);

	priv->opened_offset++;
	priv->opened_length--;
}
#endif

mtd_chip_t mtd_concat_chip = {
	mtd_concat_erase,
	mtd_concat_open,
	mtd_concat_close,
	mtd_concat_read,
	mtd_concat_write,
};

mtd_t mtd_concat_devices(mtd_t *devs, uint8_t nr_devs)
{
	mtd_t mtd;
	mtd_cid_t concat_mtd;
	mtd_t i;
	struct mtd_info *info;

	BUG_ON(mtd_last_concat >= NR_MTD_CONCATS);
	concat_mtd = mtd_last_concat;
	mtd_last_concat++;

	info = mtd_get_info(devs[0]);
	mtd_concat_infos[concat_mtd].cid = mtd_concat_cid;
	mtd_concat_infos[concat_mtd].nr_pages = 0;
	mtd_concat_infos[concat_mtd].pageorder = info->pageorder;

	for (i = 0; i < nr_devs; i++) {
		info = mtd_get_info(devs[i]);

		BUG_ON(mtd_devs[devs[i]].flags & MTD_FLAG_SUBDEV);
		BUG_ON(info->pageorder !=
		       mtd_concat_infos[concat_mtd].pageorder);
		BUG_ON(info->nr_pages !=
		       mtd_get_info(devs[0])->nr_pages);

		mtd_devs[devs[i]].flags |= MTD_FLAG_SUBDEV;
		mtd_concat_infos[concat_mtd].nr_pages += info->nr_pages;
	}

	mtd = mtd_add_device(&mtd_concat_infos[concat_mtd]);

	mtd_concat_privs[concat_mtd].reg_mtd = mtd;
	mtd_concat_privs[concat_mtd].devs = devs;
	mtd_concat_privs[concat_mtd].nr_devs = nr_devs;
	mtd_concat_privs[concat_mtd].opened_offset = 0;
	mtd_concat_privs[concat_mtd].opened_length = 0;
	mtd_concat_privs[concat_mtd].opened_mtd = INVALID_MTD_ID;
	mtd_concat_privs[concat_mtd].opened_mode = 0;

	return mtd;
}

void mtd_concat_init(void)
{
	mtd_concat_cid = mtd_register_chip(&mtd_concat_chip);
}
#else
#define mtd_concat_init()
#endif

void mtd_load(mtd_t mtd, void *buf, mtd_addr_t addr, mtd_size_t size)
{
	mtd_t smtd;
	int i;
	uint8_t *dst = buf;

	if (mtd == INVALID_MTD_ID)
		return;

	smtd = mtd_save_device(mtd);
	mtd_open(OPEN_READ, addr, size);
	for (i = 0; i < size; i++)
		dst[i] = mtd_read_byte();
	mtd_close();
	mtd_restore_device(smtd);
}

void mtd_store(mtd_t mtd, void *buf, mtd_addr_t addr, mtd_size_t size)
{
	mtd_t smtd;
	int i;
	uint8_t *dst = buf;

	if (mtd == INVALID_MTD_ID)
		return;

	smtd = mtd_save_device(mtd);
	mtd_open(OPEN_WRITE, addr, size);
	for (i = 0; i < size; i++)
		mtd_write_byte(dst[i]);
	mtd_close();
	mtd_restore_device(smtd);
}

void mtd_init(void)
{
	mtd_concat_init();
}

static int do_mtd_core(int argc, char *argv[])
{
	int i;
	if (argc < 2)
		return -EINVAL;
	if (strcmp(argv[1], "dump") == 0) {
		for (i = 0; i < NR_MTD_DEVS; i++) {
			printf("mtddevice %d: cid = %d, pageorder = %x, nr_pages = %d, eraseorder = %x\n", 
				i,
				mtd_devs[i].info->cid, 
				mtd_devs[i].info->pageorder, 
				mtd_devs[i].info->nr_pages, 
				mtd_devs[i].info->eraseorder);
		}
		return 0;
	}
	if (strcmp(argv[1], "load") == 0) {
		if (argc < 5)
			return -EINVAL;
		mtd_t mid = (uint8_t)strtoull(argv[2], 0, 0);
		mtd_addr_t addr = (uint32_t)strtoull(argv[3], 0, 0);
		mtd_size_t size = (uint32_t)strtoull(argv[4], 0, 0);
		uint8_t buf[64];
		mtd_load(mid, *buf, addr, size);
		for (i = 0; i < size; i++)
			printf("0x%x ", buf[i]);
		printf("\n");
		return 0;
	}
	if (strcmp(argv[1], "store") == 0) {
		if (argc < 6)
			return -EINVAL;
		mtd_t mid = (uint8_t)strtoull(argv[2], 0, 0);
		mtd_addr_t addr = (uint32_t)strtoull(argv[3], 0, 0);
		mtd_size_t size = (uint32_t)strtoull(argv[4], 0, 0);
		uint8_t buf[64];
		for (i = 0; i < size; i++)
			buf[i] = (uint8_t)strtoull(argv[i + 5], 0, 0);
		mtd_store(mid, *buf, addr, size);
		return 0;
	}
	return -EINVAL;
}

DEFINE_COMMAND(mtd_core, do_mtd_core, "MTD Core commands",
	"dump\n"
	"    - dump all mtd device\n"
	"load <mid> <addr> <size>\n"
	"    - load data from mtd device\n"
	"store <mid> <addr> <size> <data1> [data2] ... [dataN]\n"
	"    - store data from mtd device\n"
);