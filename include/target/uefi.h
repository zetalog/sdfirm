#ifndef __UEFI_H_INCLUDE__
#define __UEFI_H_INCLUDE__

#include <target/generic.h>
#include <target/efi.h>
#include <target/uuid.h>
#include <target/mtd.h>

/* Size of Sector (Logical Block) */
#define GPT_SECTOR_SIZE_SHIFT	9
#define GPT_SECTOR_SIZE		(1 << GPT_SECTOR_SIZE_SHIFT)

#define GPT_PGPT_SECTOR_CNT	34	/* Sector count of Primary GPT */
/* Partition Entry count in Primary GPT.
 * - Entries start at the 3rd sector (LBA 2)
 * - There are 4 entires in each sector.
 */
#define GPT_PGPT_PART_CNT	((GPT_PGPT_SECTOR_CNT-2) * 4)
/* Sector number (LBA) of the 1st Partition Entry */
#define GPT_PART_START_SECTOR	2

#define GPT_HEADER_SIGNATURE		0x5452415020494645LL /* EFI PART */
#define GPT_HEADER_REVISION_V1_02	0x00010200
#define GPT_HEADER_REVISION_V1_00	0x00010000
#define GPT_HEADER_REVISION_V0_99	0x00009900
#define GPT_HEADER_MINSZ		92 /* bytes */

#define GPT_PMBR_LBA		0
#define GPT_MBR_PROTECTIVE	1
#define GPT_MBR_HYBRID		2

#define GPT_PRIMARY_PARTITION_TABLE_LBA	ULL(0x00000001)

#define EFI_PMBR_OSTYPE		0xEE
#define MSDOS_MBR_SIGNATURE	0xAA55
#define GPT_PART_NAME_LEN	(72 / sizeof(uint16_t))
#define GPT_NPARTITIONS		FDISK_GPT_NPARTITIONS_DEFAULT

/* Globally unique identifier */
struct gpt_guid {
	uint32_t   time_low;
	uint16_t   time_mid;
	uint16_t   time_hi_and_version;
	uint8_t    clock_seq_hi;
	uint8_t    clock_seq_low;
	uint8_t    node[6];
};

/* GUID 0 indicates an empty partition */
#define GPT_UNUSED_ENTRY_GUID ((struct gpt_guid)	\
	{	0x00000000, 0x0000, 0x0000, 0x00, 0x00,	\
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }	\
	})

/* Linux native partition type */
#define GPT_DEFAULT_ENTRY_TYPE	"0FC63DAF-8483-4772-8E79-3D69D8477DE4"

/* Attribute bits */
enum {
	/* UEFI specific */
	GPT_ATTRBIT_REQ		= 0,
	GPT_ATTRBIT_NOBLOCK	= 1,
	GPT_ATTRBIT_LEGACY	= 2,

	/* GUID specific (range 48..64)*/
	GPT_ATTRBIT_GUID_FIRST	= 48,
	GPT_ATTRBIT_GUID_COUNT	= 16
};

#define GPT_ATTRSTR_REQ		"RequiredPartition"
#define GPT_ATTRSTR_REQ_TYPO	"RequiredPartiton"
#define GPT_ATTRSTR_NOBLOCK	"NoBlockIOProtocol"
#define GPT_ATTRSTR_LEGACY	"LegacyBIOSBootable"

/* The GPT Partition entry array contains an array of GPT entries. */
struct gpt_entry {
	struct gpt_guid type; /* purpose and type of the partition */
	struct gpt_guid partition_guid;
	uint64_t lba_start;
	uint64_t lba_end;
	uint64_t attrs;
	uint16_t name[GPT_PART_NAME_LEN];
} __packed;

/* GPT header */
struct gpt_header {
	uint64_t signature;
	uint32_t revision;
	uint32_t size;
	uint32_t crc32;
	uint32_t reserved1;
	/* LBA of block that contains this struct (LBA 1) */
	uint64_t my_lba;
	/* backup GPT header */
	uint64_t alternative_lba;
	/* first usable logical block for partitions */
	uint64_t first_usable_lba;
	/* last usable logical block for partitions */
	uint64_t last_usable_lba;
	/* unique disk identifier */
	struct gpt_guid disk_guid;
	/* LBA of start of partition entries array */
	uint64_t partition_entry_lba;
	/* total partition entries - normally 128 */
	uint32_t npartition_entries;
	/* bytes for each GUID pt */
	uint32_t sizeof_partition_entry;
	uint32_t partition_entry_array_crc32;
	uint8_t  reserved2[512 - 92];
} __attribute__ ((packed));

#ifdef CONFIG_UEFI
#ifdef GPT_LOCAL_TEST
int gpt_pgpt_init(uint8_t *image_start);
#else
int gpt_pgpt_init(void);
#endif
int gpt_get_part_by_name(mtd_t mtd, const char *part_name,
			 mtd_addr_t *offset, mtd_size_t *size,
			 uint16_t *pad_size);
int gpt_get_file_by_name(mtd_t mtd, const char *file_name,
			 mtd_addr_t *offset, mtd_size_t *size);
void gpt_mtd_dump(mtd_t mtd);
bool gpt_mtd_test(mtd_t mtd);
#else
#define gpt_pgpt_init()				do { } while (0)
#define gpt_get_file_by_name(mtd, n, o, s)	-EINVAL
#define gpt_get_part_by_name(mtd, n, o, s, p)	-EINVAL
#define gpt_mtd_dump(mtd)			do { } while (0)
#define gpt_mtd_test(mtd)			false
#endif

#endif /* __UEFI_H_INCLUDE__ */
