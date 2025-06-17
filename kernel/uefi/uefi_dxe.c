#include <target/efi.h>
#include <target/sbi.h>
#include <target/crc32_table.h>
#include <target/fdt.h>
#include <target/acpi.h>

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

/* Memory map initialization function */
static void init_efi_memory_map(void)
{
	uint64_t fw_start, fw_size, fw_end, ddr_start, ddr_size, ddr_end;
	efi_memory_desc_t *desc;
	int node, len;
	const uint32_t *reg;
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();
	void *fdt = (void *)scratch->next_arg1;
	memory_map_count = 0;

	/* Clear the memory map array */
	memset(memory_map, 0, sizeof(memory_map));

	fw_start = scratch->fw_start;
	fw_size = scratch->fw_size;

#ifdef CONFIG_ARCH_SPIKE
	ddr_start = 0x80000000;
	ddr_size = 0x80000000;
#else
	ddr_start = 0x0;
	ddr_size = 0x100000000;
#endif

	/* Get DDR information from device tree */
	if (fdt) {
		/* Find memory node */
		node = fdt_path_offset(fdt, "/memory");
		if (node < 0) {
			return;
		}
		reg = fdt_getprop(fdt, node, "reg", &len);
		if (reg) {
			/* For 64-bit address, we need to handle both cells */
			ddr_start = ((uint64_t)fdt32_to_cpu(reg[0]) << 32) | fdt32_to_cpu(reg[1]);
			ddr_size = ((uint64_t)fdt32_to_cpu(reg[2]) << 32) | fdt32_to_cpu(reg[3]);
			printf("DDR from FDT: start=0x%llx, size=0x%llx\n", ddr_start, ddr_size);
		}
	}

	/* Add firmware reserved region */
	desc = &memory_map[memory_map_count++];
	desc->type = EFI_RESERVED_TYPE;
	desc->phys_addr = fw_start;
	desc->virt_addr = fw_start;
	desc->num_pages = EFI_SIZE_TO_PAGES(fw_size);
	desc->attribute = EFI_MEMORY_WB|EFI_MEMORY_WT|EFI_MEMORY_WC|EFI_MEMORY_UC;

	/* Add conventional memory region (DDR before firmware) */
	if (ddr_start < fw_start) {
		desc = &memory_map[memory_map_count++];
		desc->type = EFI_CONVENTIONAL_MEMORY;
		desc->phys_addr = ddr_start;
		desc->virt_addr = ddr_start;
		desc->num_pages = EFI_SIZE_TO_PAGES(fw_start - ddr_start);
		desc->attribute = EFI_MEMORY_WB|EFI_MEMORY_WT|EFI_MEMORY_WC|EFI_MEMORY_UC;
	}

	/* Add conventional memory region (DDR after firmware) */
	fw_end = fw_start + fw_size;
	ddr_end = ddr_start + ddr_size;
	if (fw_end < ddr_end) {
		desc = &memory_map[memory_map_count++];
		desc->type = EFI_CONVENTIONAL_MEMORY;
		desc->phys_addr = fw_end;
		desc->virt_addr = fw_end;
		desc->num_pages = EFI_SIZE_TO_PAGES(ddr_end - fw_end);
		desc->attribute = EFI_MEMORY_WB|EFI_MEMORY_WT|EFI_MEMORY_WC|EFI_MEMORY_UC;
	}

#if 0
	/* TODO: Add memory mapped I/O region */
	desc = &memory_map[memory_map_count++];
	desc->type = EFI_MEMORY_MAPPED_IO;
	desc->phys_addr = 0x80000000;
	desc->virt_addr = 0x80000000;
	desc->num_pages = 0x40000000 / EFI_PAGE_SIZE;
	desc->attribute = EFI_MEMORY_UC;
#endif

	/* Add EFI runtime services data region */
	desc = &memory_map[memory_map_count++];
	desc->type = EFI_RUNTIME_SERVICES_CODE;
	desc->phys_addr = (uint64_t)&efi_core_st;
	desc->virt_addr = (uint64_t)&efi_core_st;
	desc->num_pages = EFI_SIZE_TO_PAGES(sizeof(efi_system_table_t));
	desc->attribute = EFI_MEMORY_WB|EFI_MEMORY_WT|EFI_MEMORY_WC|EFI_MEMORY_UC;

	/* Debug output */
	printf("EFI Memory Map initialized with %d entries\n", memory_map_count);
	printf("DDR start: 0x%llx, DDR size: 0x%llx\n", ddr_start, ddr_size);
	printf("Firmware start: 0x%llx, Firmware size: 0x%llx\n", fw_start, fw_size);

	for (int i = 0; i < memory_map_count; i++) {
		desc = &memory_map[i];
		printf("Entry %d: Type: %u, PhysStart: 0x%llx, VirtStart: 0x%llx, Pages: %llu, Attr: 0x%llx\n",
			i, desc->type, desc->phys_addr, desc->virt_addr, desc->num_pages, desc->attribute);
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
	static efi_config_table_t config_tables[2];
	struct sbi_scratch *scratch = sbi_scratch_thishart_ptr();

	/* Initialize EFI Memory Map */
	init_efi_memory_map();

	/* init efi_core_st header */
	efi_core_st.hdr.signature = EFI_SYSTEM_TABLE_SIGNATURE;
	efi_core_st.hdr.revision = EFI_2_30_SYSTEM_TABLE_REVISION;
	efi_core_st.hdr.headersize = sizeof(efi_core_st.hdr);
	efi_core_st.hdr.reserved = 0;

	/* Set firmware information */
	static const efi_char16_t fw_vendor_str[] = u"SpacemiT";
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
	config_tables[0].guid = ACPI_20_TABLE_GUID;
	config_tables[0].table = (void *)(uintptr_t)acpi_os_get_root_pointer();
	config_tables[1].guid = DEVICE_TREE_GUID;
	config_tables[1].table = (void *)scratch->next_arg1;

	efi_core_st.nr_tables = 2;
	efi_core_st.tables = (unsigned long)config_tables;

	/* Calculate CRC32 */
	efi_core_st.hdr.crc32 = 0;
	efi_core_st.hdr.crc32 = efi_crc32(&efi_core_st, sizeof(efi_core_st));
}
