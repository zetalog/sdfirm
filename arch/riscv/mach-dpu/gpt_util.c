#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef GPT_LOCAL_TEST
#include <asm/mach/spi.h>
#endif

#define GPT_UTIL_DEBUG

/* Size of Sector (Logical Block) */
#define GPT_SECTOR_SIZE_SHIFT 9
#define GPT_SECTOR_SIZE (1 << GPT_SECTOR_SIZE_SHIFT)

#define GPT_PGPT_SECTOR_CNT 34		/* Sector count of Primary GPT */
/* Partition Entry count in Primary GPT.
   - Entries start at the 3rd sector (LBA 2)
   - There are 4 entires in each sector.  */
#define GPT_PGPT_PART_CNT ((GPT_PGPT_SECTOR_CNT-2) * 4)
#define GPT_PART_START_SECTOR 2		/* Sector number (LBA) of the 1st Partition Entry */

#define GPT_HEADER_SIGNATURE 0x5452415020494645LL /* EFI PART */
#define GPT_HEADER_REVISION_V1_02 0x00010200
#define GPT_HEADER_REVISION_V1_00 0x00010000
#define GPT_HEADER_REVISION_V0_99 0x00009900
#define GPT_HEADER_MINSZ          92 /* bytes */

#define GPT_PMBR_LBA        0
#define GPT_MBR_PROTECTIVE  1
#define GPT_MBR_HYBRID      2

#define GPT_PRIMARY_PARTITION_TABLE_LBA 0x00000001ULL

#define EFI_PMBR_OSTYPE     0xEE
#define MSDOS_MBR_SIGNATURE 0xAA55
#define GPT_PART_NAME_LEN   (72 / sizeof(uint16_t))
#define GPT_NPARTITIONS     FDISK_GPT_NPARTITIONS_DEFAULT

/* Globally unique identifier */
struct gpt_guid {
	uint32_t   time_low;
	uint16_t   time_mid;
	uint16_t   time_hi_and_version;
	uint8_t    clock_seq_hi;
	uint8_t    clock_seq_low;
	uint8_t    node[6];
};


/* only checking that the GUID is 0 is enough to verify an empty partition. */
#define GPT_UNUSED_ENTRY_GUID						\
	((struct gpt_guid) { 0x00000000, 0x0000, 0x0000, 0x00, 0x00,	\
			     { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }})

/* Linux native partition type */
#define GPT_DEFAULT_ENTRY_TYPE "0FC63DAF-8483-4772-8E79-3D69D8477DE4"

/*
 * Attribute bits
 */
enum {
	/* UEFI specific */
	GPT_ATTRBIT_REQ      = 0,
	GPT_ATTRBIT_NOBLOCK  = 1,
	GPT_ATTRBIT_LEGACY   = 2,

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
	struct gpt_guid     type; /* purpose and type of the partition */
	struct gpt_guid     partition_guid;
	uint64_t            lba_start;
	uint64_t            lba_end;
	uint64_t            attrs;
	uint16_t            name[GPT_PART_NAME_LEN];
}  __attribute__ ((packed));

/* GPT header */
struct gpt_header {
	uint64_t            signature; /* header identification */
	uint32_t            revision; /* header version */
	uint32_t            size; /* in bytes */
	uint32_t            crc32; /* header CRC checksum */
	uint32_t            reserved1; /* must be 0 */
	uint64_t            my_lba; /* LBA of block that contains this struct (LBA 1) */
	uint64_t            alternative_lba; /* backup GPT header */
	uint64_t            first_usable_lba; /* first usable logical block for partitions */
	uint64_t            last_usable_lba; /* last usable logical block for partitions */
	struct gpt_guid     disk_guid; /* unique disk identifier */
	uint64_t            partition_entry_lba; /* LBA of start of partition entries array */
	uint32_t            npartition_entries; /* total partition entries - normally 128 */
	uint32_t            sizeof_partition_entry; /* bytes for each GUID pt */
	uint32_t            partition_entry_array_crc32; /* partition CRC checksum */
	uint8_t             reserved2[512 - 92]; /* must all be 0 */
} __attribute__ ((packed));

//static struct gpt_header gpt_header_copy = {0};

/* Copy of Primary GPT */
static uint8_t gpt_pgpt[GPT_SECTOR_SIZE * GPT_PGPT_SECTOR_CNT] = {0};
#if 0
static struct gpt_header *gpt_header_ptr = (struct gpt_header *)(gpt_pgpt + GPT_SECTOR_SIZE * 1);
#endif
static struct gpt_entry *gpt_entries_start = (struct gpt_entry *)(gpt_pgpt + GPT_SECTOR_SIZE * 2);

#ifdef GPT_UTIL_DEBUG
static void gpt_header_print(struct gpt_header *header)
{
	unsigned char *byte_ptr;
	if (header == NULL) return;
	byte_ptr = (unsigned char *)(&header->disk_guid);
	printf("GPT header @ %p\n", header);
	printf("  size = 0x%x\n", header->size);
	printf("  first_usable_lba = 0x%lx\n", header->first_usable_lba);
	printf("  last_usable_lba = 0x%lx\n", header->last_usable_lba);
	printf("  partition_entry_lba = 0x%lx\n", header->partition_entry_lba);
	printf("  npartition_entries = 0x%x\n", header->npartition_entries);
	printf("  sizeof_partition_entry = 0x%x\n", header->sizeof_partition_entry);
	printf("  disk_guid = ");
	printf("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
			byte_ptr[3], byte_ptr[2], byte_ptr[1], byte_ptr[0],
			byte_ptr[5], byte_ptr[4],
			byte_ptr[7], byte_ptr[6],
			byte_ptr[9], byte_ptr[8],
			byte_ptr[10], byte_ptr[11], byte_ptr[12], byte_ptr[13], byte_ptr[14], byte_ptr[15]);
}

static void gpt_entry_print(struct gpt_entry *entry)
{
	int i;
	unsigned char *byte_ptr;
	if (entry == NULL) return;
	byte_ptr = (unsigned char *)(&entry->partition_guid);
	printf("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X ",
			byte_ptr[3], byte_ptr[2], byte_ptr[1], byte_ptr[0],
			byte_ptr[5], byte_ptr[4],
			byte_ptr[7], byte_ptr[6],
			byte_ptr[9], byte_ptr[8],
			byte_ptr[10], byte_ptr[11], byte_ptr[12], byte_ptr[13], byte_ptr[14], byte_ptr[15]);
	printf("%016lx ", entry->lba_start);
	printf("%016lx ", entry->lba_end);
	for (i = 0; i < (GPT_PART_NAME_LEN * sizeof(uint16_t)); i++) {
		printf("%02x", *(((unsigned char *)(entry->name)) + i));
	}
	printf("\n");
	return;
}
#endif

#ifdef GPT_LOCAL_TEST
int gpt_pgpt_init(uint8_t *image_start)
#else
int gpt_pgpt_init(void)
#endif
{
	/* PGPT header is at the 2nd sector */
	uint32_t flash_addr = GPT_SECTOR_SIZE;
	uint32_t copy_size = GPT_SECTOR_SIZE;
#ifdef GPT_UTIL_DEBUG
	printf("Debug: Enter %s\n", __func__);
	printf("Debug: Copy flash addr = 0x%x, size = 0x%x\n", flash_addr, copy_size);
#endif

#ifdef GPT_LOCAL_TEST
	/* Copy header and all entries in one time */
	if (image_start == NULL) return -1;
	memcpy(gpt_pgpt, image_start, sizeof(gpt_pgpt));
#else
	/* Copy header only */
	dpu_ssi_flash_copy(gpt_pgpt + GPT_SECTOR_SIZE, flash_addr, copy_size);
#endif
#ifdef GPT_UTIL_DEBUG
	gpt_header_print((struct gpt_header *)(gpt_pgpt + GPT_SECTOR_SIZE));
#endif
	return 0;
}

static int gpt_entry_check_name(struct gpt_entry *entry, uint8_t *name_str)
{
	int i;
	if (entry == NULL) return -1;
	if (name_str == NULL) return -1;
	for (i = 0; i < GPT_PART_NAME_LEN; i++) {
		if (name_str[i] == '\0' && (uint8_t)(entry->name[i]) == '\0')
			return 0;
		if (name_str[i] != (uint8_t)(entry->name[i]))
			return 1;
	}
	return 1;
}

/*
 * Return Partition Number (1...GPT_PGPT_PART_CNT) if found.
 */
int gpt_get_part_by_name(uint8_t *part_name, uint32_t *offset, uint32_t *size, uint16_t *pad_size)
{
	uint32_t flash_addr = GPT_SECTOR_SIZE * GPT_PART_START_SECTOR;
	uint32_t copy_size = sizeof(struct gpt_entry);
	int i;

#ifdef GPT_UTIL_DEBUG
	printf("Debug: Enter %s\n", __func__);
#endif
	if (part_name == NULL) return -1;
	if (offset == NULL) return -1;
	if (size == NULL) return -1;

	for (i = 0; i < GPT_PGPT_PART_CNT; i++) {
		struct gpt_entry *entry_ptr = gpt_entries_start + i;
		uint32_t *guid_words = (uint32_t *)(&entry_ptr->partition_guid);
		unsigned char *guid_bytes = (unsigned char *)(&entry_ptr->partition_guid);
#ifdef GPT_LOCAL_TEST
		/* No need to copy, because all data is copied in gpt_pgpt_init() */
#else
#ifdef GPT_UTIL_DEBUG
		printf("Debug: Copy Partition Entry %d flash addr = 0x%x size = 0x%x\n", i, flash_addr, copy_size);
#endif
		dpu_ssi_flash_copy(entry_ptr, flash_addr, copy_size);
		flash_addr += copy_size;
#endif
#ifdef GPT_UTIL_DEBUG
		printf("Debug: Checking partition %d\n", (i + 1));
		gpt_entry_print(entry_ptr);
#endif
		/* Stop searching at empty entry */
		if (guid_words[0] == 0 && guid_words[1] == 0 && guid_words[2] == 0 && guid_words[3] == 0)
			break;
		if (gpt_entry_check_name(entry_ptr, part_name) != 0) {
			continue;
		}
		*offset = (uint32_t)(entry_ptr->lba_start << GPT_SECTOR_SIZE_SHIFT);
		*size = (uint32_t)((entry_ptr->lba_end + 1 - entry_ptr->lba_start) << GPT_SECTOR_SIZE_SHIFT);
		*pad_size = guid_bytes[14];
		*pad_size <<= 8;
		*pad_size += guid_bytes[15];
#ifdef GPT_UTIL_DEBUG
		//for (int j = 0; j < sizeof(struct gpt_guid); j++) printf("%d-%02x ", j, guid_bytes[j]);
		printf("Debug: Found partition %d name = %s offset = %d size = %d pad_size = %d\n", (i + 1), part_name, *offset, *size, *pad_size);
		gpt_entry_print(entry_ptr);
#endif
		return (i + 1);
	}
	return 0;
}

int gpt_get_file_by_name(uint8_t *file_name, uint32_t *offset, uint32_t *size)
{
	int ret;
	uint16_t pad_size;
	ret = gpt_get_part_by_name(file_name, offset, size, &pad_size);
	*size -= pad_size;
	return ret;
}

//int dpu_ssi_flash_find(uint32_t *addr, uint32_t *size);

#ifdef GPT_LOCAL_TEST
#include <stdio.h>

#define DATA_BUF_SIZE (GPT_SECTOR_SIZE * GPT_PGPT_SECTOR_CNT)

static char image_file_default[] ="gpt.img";

int main(int argc, char **argv)
{
	char *image_file_name = image_file_default;
	FILE *image_file_fd = NULL;
	char data_buf[DATA_BUF_SIZE] = {0};
	size_t f_ret, f_size, f_nmemb;
	uint32_t part_offset;
	uint32_t part_size;
	uint16_t pad_size;
	int ret;

	if (argc >= 2) {
		image_file_name = argv[1];
	}

	printf("Open image file %s\n", image_file_name);
	image_file_fd = fopen(image_file_name, "r");
	if (image_file_fd == NULL) {
		printf("Error: Failed to open image file %s\n", image_file_name);
		return -1;
	}

	f_size = 1;
	f_nmemb = DATA_BUF_SIZE;
	printf("Read image header data. size = %ld\n", (f_size * f_nmemb));
	f_ret = fread(data_buf, f_size, f_nmemb, image_file_fd);
	if (f_ret != f_nmemb) {
		printf("Error: Failed to read image header data. nmemb = %ld, ret = %ld\n", f_nmemb, f_ret);
		goto err_exit;
	}

	printf("Initiate Primary GPT\n");
	ret = gpt_pgpt_init(data_buf);
	if (ret != 0) {
		printf("Error: Failed to initiate Primary GPT. ret = %d\n", ret);
		goto err_exit;
	}

	gpt_get_part_by_name("not-exist", &part_offset, &part_size, &pad_size);
	gpt_get_file_by_name("not-exist", &part_offset, &part_size);
	gpt_get_part_by_name("fsbl.bin", &part_offset, &part_size, &pad_size);
	gpt_get_file_by_name("fsbl.bin", &part_offset, &part_size);
	gpt_get_part_by_name("config", &part_offset, &part_size, &pad_size);
	gpt_get_file_by_name("config", &part_offset, &part_size);

err_exit:
	if (image_file_fd != NULL) {
		fclose(image_file_fd);
	}
	return 0;
}

#endif
