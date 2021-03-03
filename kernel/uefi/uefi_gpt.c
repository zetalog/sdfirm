#include <target/uefi.h>

//#define CONFIG_UEFI_GPT_DEBUG

#ifdef CONFIG_UEFI_GPT_DEBUG
static void gpt_header_print(struct gpt_header *header)
{
	unsigned char *byte_ptr;

	if (header == NULL)
		return;
	byte_ptr = (unsigned char *)(&header->disk_guid);
	printf("GPT header @ %p\n", header);
	printf("  size = 0x%x\n", header->size);
	printf("  first_usable_lba = 0x%lx\n",
	       header->first_usable_lba);
	printf("  last_usable_lba = 0x%lx\n",
	       header->last_usable_lba);
	printf("  partition_entry_lba = 0x%lx\n",
	       header->partition_entry_lba);
	printf("  npartition_entries = 0x%x\n",
	       header->npartition_entries);
	printf("  sizeof_partition_entry = 0x%x\n",
	       header->sizeof_partition_entry);
	printf("  disk_guid = ");
	printf("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X\n",
	       byte_ptr[3], byte_ptr[2], byte_ptr[1], byte_ptr[0],
	       byte_ptr[5], byte_ptr[4], byte_ptr[7], byte_ptr[6],
	       byte_ptr[9], byte_ptr[8], byte_ptr[10], byte_ptr[11],
	       byte_ptr[12], byte_ptr[13], byte_ptr[14], byte_ptr[15]);
}

static void gpt_entry_print(struct gpt_entry *entry)
{
	int i;
	unsigned char *byte_ptr;

	if (entry == NULL)
		return;
	byte_ptr = (unsigned char *)(&entry->partition_guid);
	printf("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X ",
	       byte_ptr[3], byte_ptr[2], byte_ptr[1], byte_ptr[0],
	       byte_ptr[5], byte_ptr[4], byte_ptr[7], byte_ptr[6],
	       byte_ptr[9], byte_ptr[8], byte_ptr[10], byte_ptr[11],
	       byte_ptr[12], byte_ptr[13], byte_ptr[14], byte_ptr[15]);
	printf("%016lx ", entry->lba_start);
	printf("%016lx ", entry->lba_end);
	for (i = 0; i < (GPT_PART_NAME_LEN * sizeof(uint16_t)); i++)
		printf("%02x", *(((unsigned char *)(entry->name)) + i));
	printf("\n");
	return;
}
#endif

int gpt_pgpt_init(void)
{
	return 0;
}

static int gpt_entry_check_name(struct gpt_entry *entry, const char *name_str)
{
	int i;

	if (entry == NULL || name_str == NULL)
		return -1;
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
int gpt_get_part_by_name(mtd_t mtd, const char *part_name,
			 mtd_addr_t *offset, mtd_size_t *size,
			 uint16_t *pad_size)
{
	/* PGPT header is at the 2nd sector */
	uint32_t flash_addr_header = GPT_SECTOR_SIZE;
	uint32_t copy_size_header = GPT_SECTOR_SIZE;
	/* For the PGPT header (1 sector) or a GPT entry (1/4 sector) */
	uint8_t sector_buffer[GPT_SECTOR_SIZE] = {0};
	struct gpt_entry *entry_ptr = (struct gpt_entry *)sector_buffer;
	uint32_t flash_addr = GPT_SECTOR_SIZE * GPT_PART_START_SECTOR;
	uint32_t copy_size = sizeof(struct gpt_entry);
	int i;

#ifdef CONFIG_UEFI_GPT_DEBUG
	printf("gpt: Enter %s\n", __func__);
#endif
	if (part_name == NULL || offset == NULL || size == NULL)
		return -EINVAL;

	gpt_mtd_copy(mtd, sector_buffer,
		     flash_addr_header, copy_size_header);
#ifdef CONFIG_UEFI_GPT_DEBUG
	gpt_header_print((struct gpt_header *)sector_buffer);
#endif

	for (i = 0; i < GPT_PGPT_PART_CNT; i++) {
		uint32_t *guid_words =
			(uint32_t *)(&entry_ptr->partition_guid);
		unsigned char *guid_bytes =
			(unsigned char *)(&entry_ptr->partition_guid);
#ifdef CONFIG_UEFI_GPT_DEBUG
		printf("gpt: Copying partion%d addr=0x%x size=0x%x..\n",
		       i, flash_addr, copy_size);
#endif
		gpt_mtd_copy(mtd, entry_ptr, flash_addr, copy_size);
		flash_addr += copy_size;
#ifdef CONFIG_UEFI_GPT_DEBUG
		printf("gpt: Checking partition%d...\n", (i + 1));
		gpt_entry_print(entry_ptr);
#endif
		/* Stop searching at empty entry */
		if (guid_words[0] == 0 && guid_words[1] == 0 &&
		    guid_words[2] == 0 && guid_words[3] == 0)
			break;
		if (gpt_entry_check_name(entry_ptr, part_name) != 0)
			continue;
		*offset = (uint32_t)(entry_ptr->lba_start <<
				     GPT_SECTOR_SIZE_SHIFT);
		*size = (uint32_t)((entry_ptr->lba_end + 1 -
				    entry_ptr->lba_start) <<
				   GPT_SECTOR_SIZE_SHIFT);
		*pad_size = guid_bytes[14];
		*pad_size <<= 8;
		*pad_size += guid_bytes[15];
#ifdef CONFIG_UEFI_GPT_DEBUG
		printf("gpt: Found partition%d: name=%s offset=%d size=%d pad_size=%d\n",
		       i + 1, part_name, *offset, *size, *pad_size);
		gpt_entry_print(entry_ptr);
#endif
		return i + 1;
	}
	return 0;
}

int gpt_get_file_by_name(mtd_t mtd, const char *file_name,
			 mtd_addr_t *offset, mtd_size_t *size)
{
	int ret;
	uint16_t pad_size;

	ret = gpt_get_part_by_name(mtd, file_name, offset, size, &pad_size);
	*size -= pad_size;
	return ret;
}

static void gpt_dump_partition(int id, gpt_partition_entry *entry)
{
	char name[GPT_PART_NAME_U16_LEN + 1] = { 0 };
	int i;

	/* Convert partition name to ASCII string */
	for (i = 0; i < GPT_PART_NAME_U16_LEN; i++) {
		if (entry->name[i] == 0)
			break;
		name[i] = (unsigned char)entry->name[i];
	}

	printf("%02d %s %s %lld %lld\n", id,
	       uuid_export(entry->partition_guid.u.uuid), name,
	       entry->first_lba, entry->last_lba);
}

void gpt_mtd_copy(mtd_t mtd, void *buf, mtd_addr_t addr, mtd_size_t size)
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

void gpt_mtd_dump(mtd_t mtd)
{
	uint8_t gpt_buf[GPT_LBA_SIZE];
	gpt_header hdr;
	uint64_t partition_entries_lba_end;
	gpt_partition_entry *gpt_entries;
	uint64_t i;
	uint32_t j;
	uint32_t num_entries;
	int part = 0;

	if (mtd == INVALID_MTD_ID) {
		printf("gpt: Error: Invalid MTD device\n");
		return;
	}
	gpt_mtd_copy(mtd, &hdr, GPT_HEADER_LBA * GPT_LBA_SIZE,
		     GPT_HEADER_BYTES);
	partition_entries_lba_end = (hdr.partition_entries_lba +
		(hdr.num_partition_entries * hdr.partition_entry_size +
		 GPT_LBA_SIZE - 1) / GPT_LBA_SIZE);
	for (i = hdr.partition_entries_lba;
	     i < partition_entries_lba_end; i++) {
		gpt_mtd_copy(mtd, gpt_buf, i * GPT_LBA_SIZE, GPT_LBA_SIZE);
		gpt_entries = (gpt_partition_entry *)gpt_buf;
		num_entries = GPT_LBA_SIZE / hdr.partition_entry_size;
		for (j = 0; j < num_entries; j++) {
			/* Stop with empty UUID */
			if (uuid_empty(&gpt_entries[j].partition_guid.u.uuid))
				return;

			part++;
			gpt_dump_partition(part, &gpt_entries[j]);
		}
	}
}
