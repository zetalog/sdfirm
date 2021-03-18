#ifndef __MTD_H_INCLUDE__
#define __MTD_H_INCLUDE__

#include <target/generic.h>
#include <target/panic.h>

#ifdef CONFIG_MTD_MAX_CHIPS
#define NR_MTD_CHIPS		CONFIG_MTD_MAX_CHIPS
#else
#define NR_MTD_CHIPS		1
#endif
#define INVALID_MTD_CID		NR_MTD_CHIPS

#ifdef CONFIG_MTD_MAX_DEVS
#define NR_MTD_DEVS		CONFIG_MTD_MAX_DEVS
#else
#define NR_MTD_DEVS		1
#endif
#define INVALID_MTD_ID		NR_MTD_DEVS

#ifdef CONFIG_MTD_MAX_CONCATS
#define NR_MTD_CONCATS		CONFIG_MTD_MAX_CONCATS
#else
#define NR_MTD_CONCATS		1
#endif

/* chip ID */
typedef uint8_t mtd_cid_t;
/* bank ID */
typedef uint8_t mtd_bid_t;
/* MTD device ID */
typedef uint8_t mtd_t;
typedef uint32_t mtd_size_t;
typedef uint32_t mtd_addr_t;
typedef uint32_t mtd_page_t;

struct mtd_chip {
	void (*erase)(mtd_t mtd, mtd_addr_t addr, mtd_size_t size);
	/* APIs for character accessible hardware */
	boolean (*open)(uint8_t mode,
			mtd_addr_t addr, mtd_size_t size);
	void (*close)(void);
	uint8_t (*read)(void);
	void (*write)(uint8_t byte);
#ifdef CONFIG_MTD_BUFFER
	/* APIs for block accessible hardware */
	void (*read_buffer)(mtd_addr_t addr,
			    uint8_t *buff, mtd_size_t size);
	void (*write_buffer)(mtd_addr_t addr,
			     uint8_t *buff, mtd_size_t size);
#endif
};
typedef struct mtd_chip mtd_chip_t;

struct mtd_info {
	mtd_cid_t cid;
	uint8_t pageorder;
	/* mtd_size_t pagesize; */
	mtd_page_t nr_pages;
	uint8_t eraseorder;
};

struct mtd_concat {
	mtd_t reg_mtd;
	mtd_t *devs;
	uint8_t nr_devs;
	/* open/close/read/write */
	mtd_size_t opened_offset;
	mtd_size_t opened_length;
	mtd_t opened_mtd;
	mtd_t opened_mode;
};

struct mtd_dev {
	struct mtd_info *info;
	uint8_t flags;
#define MTD_FLAG_SUBDEV		0x01
#define MTD_FLAG_OPENED		0x02
#define MTD_FLAG_WRITING	0x04
};

extern mtd_t mtd_id;

void mtd_erase(mtd_t mtd, mtd_addr_t addr, mtd_size_t size);

/* following functions should be called under the
 * mtd_save_device/mtd_restore_device context
 */
boolean mtd_open(uint8_t mode,
		 mtd_addr_t addr, mtd_size_t size);
void mtd_close(void);
uint8_t mtd_read_byte(void);
void mtd_write_byte(uint8_t byte);
void mtd_read_buffer(mtd_addr_t addr,
		     uint8_t *buff, mtd_size_t size);
void mtd_write_buffer(mtd_addr_t addr,
		      uint8_t *buff, mtd_size_t size);

mtd_addr_t mtd_page_offset(mtd_addr_t addr, uint8_t pageorder);
mtd_size_t mtd_page_size(uint8_t pageorder);

void mtd_load(mtd_t mtd, void *buf, mtd_addr_t addr, mtd_size_t size);
void mtd_store(mtd_t mtd, void *buf, mtd_addr_t addr, mtd_size_t size);

mtd_cid_t mtd_register_chip(mtd_chip_t *chip);
mtd_t mtd_add_device(struct mtd_info *info);
#ifdef CONFIG_MTD_CONCAT
mtd_t mtd_concat_devices(mtd_t *devs, uint8_t nr_devs);
#else
#define mtd_concat_devices(devs, nr_devs)	INVALID_MTD_ID
#endif
struct mtd_info *mtd_get_info(mtd_t mtd);

void mtd_restore_device(mtd_t mtd);
mtd_t mtd_save_device(mtd_t mtd);

#ifdef CONFIG_MTD_BLOCK
void mtd_block_create(mtd_t mtd);
#else
#define mtd_block_create(mtd)
#endif

#endif /* __MTD_H_INCLUDE__ */
