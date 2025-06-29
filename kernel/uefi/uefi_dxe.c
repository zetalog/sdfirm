#include <target/efi.h>
#include <target/sbi.h>
#include <target/crc32_table.h>
#include <target/fdt.h>
#include <target/acpi.h>
#include <target/bitops.h>

#define MAX_MEMORY_DESCRIPTORS 64
__efi efi_memory_desc_t memory_map[MAX_MEMORY_DESCRIPTORS];
__efi int memory_map_count;

__efi efi_system_table_t efi_core_st = {
	.hdr = {
		.signature = EFI_SYSTEM_TABLE_SIGNATURE,
		.revision = EFI_2_00_SYSTEM_TABLE_REVISION,
		.headersize = sizeof(efi_core_st),
		.crc32 = 0,
		.reserved = 0,
	},
};

__efi efi_boot_services_t		efi_boot_services;
__efi efi_runtime_services_t		efi_runtime_services;
__efi efi_simple_text_output_protocol_t	con_out_proto;
__efi efi_simple_text_input_protocol_t	con_in_proto;

static const char *memory_type_names[] = {
	[EFI_RESERVED_TYPE] = "EFI_RESERVED",
	[EFI_LOADER_CODE] = "EFI_LOADER_CODE",
	[EFI_LOADER_DATA] = "EFI_LOADER_DATA",
	[EFI_BOOT_SERVICES_CODE] = "EFI_BOOT_SERVICES_CODE",
	[EFI_BOOT_SERVICES_DATA] = "EFI_BOOT_SERVICES_DATA",
	[EFI_RUNTIME_SERVICES_CODE] = "EFI_RUNTIME_SERVICES_CODE",
	[EFI_RUNTIME_SERVICES_DATA] = "EFI_RUNTIME_SERVICES_DATA",
	[EFI_CONVENTIONAL_MEMORY] = "EFI_CONVENTIONAL_MEMORY",
	[EFI_UNUSABLE_MEMORY] = "EFI_UNUSABLE_MEMORY",
	[EFI_ACPI_RECLAIM_MEMORY] = "EFI_ACPI_RECLAIM_MEMORY",
	[EFI_ACPI_MEMORY_NVS] = "EFI_ACPI_MEMORY_NVS",
	[EFI_MEMORY_MAPPED_IO] = "EFI_MEMORY_MAPPED_IO",
	[EFI_MEMORY_MAPPED_IO_PORT_SPACE] = "EFI_MEMORY_MAPPED_IO_PORT_SPACE",
	[EFI_PAL_CODE] = "EFI_PAL_CODE",
	[EFI_PERSISTENT_MEMORY] = "EFI_PERSISTENT_MEMORY",
	[EFI_UNACCEPTED_MEMORY] = "EFI_UNACCEPTED_MEMORY",
};

/*
 * Blank Handle implementation
 */
typedef struct {
	efi_handle_t handle;
	void *protocol_interface;
} efi_device_handle_t;

static efi_device_handle_t console_in_handle = {
	.handle = (efi_handle_t)0x1,
	.protocol_interface = &con_in_proto
};

static efi_device_handle_t console_out_handle = {
	.handle = (efi_handle_t)0x2,
	.protocol_interface = &con_out_proto
};

static efi_device_handle_t console_err_handle = {
	.handle = (efi_handle_t)0x3,
	.protocol_interface = &con_out_proto
};

/*
 * Console protocol implementation
 */
static efi_status_t __efiapi con_out_output_string (
	efi_simple_text_output_protocol_t *this,
	efi_char16_t *string
)
{
	while (*string)
		sbi_console_putchar(*string++);

	return 0;
}

efi_simple_text_output_protocol_t con_out_proto = {
	.reset = NULL,
	.output_string = con_out_output_string,
	.test_string = NULL,
};

static efi_status_t __efiapi con_in_read_key (
	efi_simple_text_input_protocol_t *this,
	efi_input_key_t *key
) {
	int ch = sbi_console_getchar();
	if (ch < 0)
		return EFI_NOT_READY;

	key->unicode_char = (efi_char16_t)ch;
	key->scan_code = 0;

	return EFI_SUCCESS;
}

efi_simple_text_input_protocol_t con_in_proto = {
	.reset = NULL,
	.read_keystroke = con_in_read_key,
	.wait_for_key = NULL
};

/* insert a descriptor to memory map, move the following descriptors to the next */
static void insert_desc(int index, const efi_memory_desc_t *desc)
{
	if (memory_map_count >= MAX_MEMORY_DESCRIPTORS) {
		printf("Memory map full! Cannot insert more entries.\n");
		return;
	}

	memmove(&memory_map[index + 1], &memory_map[index],
		(memory_map_count - index) * sizeof(efi_memory_desc_t));
	memory_map[index] = *desc;
	memory_map_count++;
}

void insert_memory_desc(uint32_t type, uint64_t start, uint64_t end, uint64_t attr)
{
	if (start >= end) {
		printf("Invalid range: [0x%llx, 0x%llx)\n", start, end);
		return;
	}

	start = ALIGN_DOWN(start, EFI_PAGE_SIZE);
	end = ALIGN_UP(end, EFI_PAGE_SIZE);

	printf("Inserting: type=%s, range=[0x%llx, 0x%llx)\n", memory_type_names[type], start, end);

	/* Step 1: Split existing descriptors that overlap with the new range */
	for (int i = 0; i < memory_map_count; ) {
		efi_memory_desc_t *desc = &memory_map[i];
		uint64_t desc_start = desc->phys_addr;
		uint64_t desc_end = desc->phys_addr + (desc->num_pages << EFI_PAGE_SHIFT);

		/* Case 1: No overlap → skip */
		if (end <= desc_start || start >= desc_end) {
			i++;
			continue;
		}

		/* Case 2: New range completely covers existing → remove it */
		if (start <= desc_start && end >= desc_end) {
			printf("  Fully covering entry %d: [0x%llx, 0x%llx)\n", i, desc_start, desc_end);

			desc->type = type;
			desc->phys_addr = start;
			desc->virt_addr = start;
			desc->num_pages = EFI_SIZE_TO_PAGES(end - start);
			desc->attribute = attr;

			if (i < memory_map_count - 1) {
				efi_memory_desc_t* next = &memory_map[i+1];
				if (desc_end == next->phys_addr) {
					uint64_t original_next_end = next->phys_addr + (next->num_pages << EFI_PAGE_SHIFT);
					next->phys_addr = end;
					next->virt_addr = end;
					next->num_pages = EFI_SIZE_TO_PAGES(original_next_end - end);
				}
			}

			i++;
			continue;
		}

		/* Case 3: Partial overlap → split the existing descriptor */
		if (start > desc_start && start < desc_end) {
			// Split into [desc_start, start) and [start, desc_end)
			printf("  Splitting entry %d at 0x%llx\n", i, start);
			printf("	Original: [0x%llx, 0x%llx)\n", desc_start, desc_end);

			// Left part: [desc_start, start) - keep original type
			desc->num_pages = EFI_SIZE_TO_PAGES(start - desc_start);

			// Right part: [start, desc_end) - keep original type
			efi_memory_desc_t right = *desc;
			right.phys_addr = start;
			right.virt_addr = start;
			right.num_pages = EFI_SIZE_TO_PAGES(desc_end - start);
			insert_desc(i + 1, &right);

			i++;
		} else if (end > desc_start && end < desc_end) {
			// Split into [desc_start, end) and [end, desc_end)
			printf("  Splitting entry %d at 0x%llx\n", i, end);

			// Left part: [desc_start, end) - keep original type
			desc->num_pages = EFI_SIZE_TO_PAGES(end - desc_start);
			printf("	Left part: [0x%llx, 0x%llx), pages=%llu\n",
				desc->phys_addr, desc->phys_addr + (desc->num_pages << EFI_PAGE_SHIFT), desc->num_pages);

			// Right part: [end, desc_end) - keep original type
			efi_memory_desc_t right = *desc;
			right.phys_addr = end;
			right.virt_addr = end;
			right.num_pages = EFI_SIZE_TO_PAGES(desc_end - end);
			printf("	Right part: [0x%llx, 0x%llx), pages=%llu\n",
				right.phys_addr, right.phys_addr + (right.num_pages << EFI_PAGE_SHIFT), right.num_pages);
			insert_desc(i + 1, &right);

			i++;
		}
	}

	/* Step 2: Insert the new descriptor in the correct position */
	int insert_pos = 0;
	while (insert_pos < memory_map_count && memory_map[insert_pos].phys_addr < start) {
		insert_pos++;
	}

	efi_memory_desc_t new_desc = {
		.type = type,
		.phys_addr = start,
		.virt_addr = start,
		.num_pages = EFI_SIZE_TO_PAGES(end - start),
		.attribute = attr
	};

	if (insert_pos < memory_map_count && memory_map[insert_pos].phys_addr == start) {
		printf("  Replacing descriptor at position %d\n", insert_pos);
		memory_map[insert_pos] = new_desc;
	} else {
		printf("  Inserting new descriptor at position %d\n", insert_pos);
		insert_desc(insert_pos, &new_desc);
	}

	printf("Entries Info:\n");
	for (int i = 0; i < memory_map_count; i++) {
		efi_memory_desc_t *desc = &memory_map[i];
		printf("  entry %d: type=%s, range=[0x%llx, 0x%llx], attr=0x%llx\n",
			i, memory_type_names[desc->type],
			desc->phys_addr,
			desc->phys_addr + (desc->num_pages << EFI_PAGE_SHIFT),
			desc->attribute);
	}
}

void uefi_memmap_init(void *fdt)
{
	uint64_t reserved_start, reserved_size, reserved_end;
	uint64_t ddr_start, ddr_size, ddr_end;
	uint64_t efi_start, efi_size, efi_end;
	uint64_t acpi_start, acpi_size, acpi_end;
	int node, child, len, i;
	const uint32_t *reg;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	memory_map_count = 0;

	memset(memory_map, 0, sizeof(memory_map));

#ifdef CONFIG_ARCH_SPIKE
	ddr_start = 0x80000000;
	ddr_size = 0x80000000;
#else
	ddr_start = 0x0;
	ddr_size = 0x100000000;
#endif
	reserved_start = scratch->fw_start;
	reserved_size = scratch->fw_size;

	if (fdt) {
		/* Find memory node */
		node = fdt_path_offset(fdt, "/memory");
		if (node < 0) {
			printf("uefi_memmap_init: memory node not found\n");
			return;
		}
		reg = fdt_getprop(fdt, node, "reg", &len);
		if (reg) {
			ddr_start = ((uint64_t)fdt32_to_cpu(reg[0]) << 32) | fdt32_to_cpu(reg[1]);
			ddr_size = ((uint64_t)fdt32_to_cpu(reg[2]) << 32) | fdt32_to_cpu(reg[3]);
			printf("DDR from FDT: start=0x%llx, size=0x%llx\n", ddr_start, ddr_size);
		}
		/* Find reserved-memory node */
		node = fdt_path_offset(fdt, "/reserved-memory");
		if (node < 0) {
			printf("uefi_memmap_init: reserved-memory node not found\n");
			return;
		}
		reg = fdt_getprop(fdt, node, "reg", &len);
		if (reg && len >= 16) {
			reserved_start = ((uint64_t)fdt32_to_cpu(reg[0]) << 32) | fdt32_to_cpu(reg[1]);
			reserved_size = ((uint64_t)fdt32_to_cpu(reg[2]) << 32) | fdt32_to_cpu(reg[3]);
			printf("Reserved memory from FDT: start=0x%llx, size=0x%llx\n", reserved_start, reserved_size);
		} else {
			fdt_for_each_subnode(child, fdt, node) {
				reg = fdt_getprop(fdt, child, "reg", &len);
				if (reg && len >= 16) {
					reserved_start = ((uint64_t)fdt32_to_cpu(reg[0]) << 32) | fdt32_to_cpu(reg[1]);
					reserved_size = ((uint64_t)fdt32_to_cpu(reg[2]) << 32) | fdt32_to_cpu(reg[3]);
					printf("Reserved memory from child node %s: start=0x%llx, size=0x%llx\n",
						fdt_get_name(fdt, child, NULL), reserved_start, reserved_size);
					break;
				}
			}
		}
	}
	reserved_end = reserved_start + reserved_size;
	ddr_end = ddr_start + ddr_size;

	insert_memory_desc(EFI_CONVENTIONAL_MEMORY, ddr_start, ddr_end,
		EFI_MEMORY_WB|EFI_MEMORY_WT|EFI_MEMORY_WC|EFI_MEMORY_UC);

	if (reserved_size > 0)
		insert_memory_desc(EFI_RESERVED_TYPE, reserved_start, reserved_end,
			EFI_MEMORY_WB|EFI_MEMORY_WT|EFI_MEMORY_WC|EFI_MEMORY_UC);

	efi_start = (uint64_t)&efi_core_st;
	efi_size = sizeof(efi_core_st);
	efi_end = efi_start + efi_size;
	insert_memory_desc(EFI_RUNTIME_SERVICES_DATA, efi_start, efi_end,
		EFI_MEMORY_WB|EFI_MEMORY_WT|EFI_MEMORY_WC|EFI_MEMORY_UC);

	acpi_start = acpi_get_start_addr();
	acpi_size = acpi_get_total_size();
	acpi_end = acpi_start + acpi_size;

	if (acpi_start && acpi_size > 0 && acpi_start < ddr_end)
		insert_memory_desc(EFI_RUNTIME_SERVICES_DATA, acpi_start, acpi_end,
			EFI_MEMORY_WB|EFI_MEMORY_WT|EFI_MEMORY_WC|EFI_MEMORY_UC);

	for (i = 0; i < memory_map_count; i++) {
		efi_memory_desc_t *desc = &memory_map[i];
		printf("Entry %d: Type: %s, Region[0x%llx - 0x%llx], Pages: %llu, Attr: 0x%llx\n",
			i, memory_type_names[desc->type], desc->phys_addr,
			desc->phys_addr + desc->num_pages * EFI_PAGE_SIZE - 1,
			desc->num_pages, desc->attribute);
	}
}

static uint32_t efi_crc32(const void *data, size_t len)
{
	const uint8_t *buf = (const uint8_t *)data;
	uint32_t crc = 0xFFFFFFFF;

	while (len--) {
		crc = (crc >> 8) ^ crc32_le_table[(crc ^ *buf++) & 0xFF];
	}

	return ~crc;
}

void uefi_dxe_init(void) {
#if CONFIG_ACPI_BIOS
	__efi static efi_config_table_t config_tables[2];
#else
	__efi static efi_config_table_t config_tables[1];
#endif
	__efi static efi_char16_t fw_vendor_str[] = u"SpacemiT";
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* init efi_core_st header */
	efi_core_st.hdr.signature = EFI_SYSTEM_TABLE_SIGNATURE;
	efi_core_st.hdr.revision = EFI_2_30_SYSTEM_TABLE_REVISION;
	efi_core_st.hdr.headersize = sizeof(efi_core_st.hdr);
	efi_core_st.hdr.reserved = 0;

	/* Set firmware information */
	efi_core_st.fw_vendor = (unsigned long)fw_vendor_str;
	efi_core_st.fw_revision = 0x00010000;  // 1.0

	/* Set console Handle and protocol */
	efi_core_st.con_in_handle = (unsigned long)console_in_handle.handle;
	efi_core_st.con_in = &con_in_proto;
	efi_core_st.con_out_handle = (unsigned long)console_out_handle.handle;
	efi_core_st.con_out = &con_out_proto;
	efi_core_st.con_err_handle = (unsigned long)console_err_handle.handle;
	efi_core_st.con_err = (unsigned long)&con_out_proto;

	/* Set runtime and boot services */
	efi_core_st.runtime = NULL;
	efi_core_st.boottime = NULL;

	/* set config tables */

#if CONFIG_ACPI_BIOS
	config_tables[0].guid = ACPI_20_TABLE_GUID;
	config_tables[0].table = (void *)(uintptr_t)acpi_os_get_root_pointer();
	config_tables[1].guid = DEVICE_TREE_GUID;
	config_tables[1].table = (void *)scratch->next_arg1;
#else
	config_tables[0].guid = DEVICE_TREE_GUID;
	config_tables[0].table = (void *)scratch->next_arg1;
#endif

	efi_core_st.nr_tables = ARRAY_SIZE(config_tables);
	efi_core_st.tables = (unsigned long)config_tables;

	/* Calculate CRC32 */
	efi_core_st.hdr.crc32 = 0;
	efi_core_st.hdr.crc32 = efi_crc32(&efi_core_st, sizeof(efi_core_st));
}
