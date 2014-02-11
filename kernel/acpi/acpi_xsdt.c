#include "acpi_int.h"

acpi_addr_t acpi_xsdt_get_entry(uint8_t *table_entry, uint32_t table_entry_size)
{
	acpi_addr_t address;

	if (table_entry_size == ACPI_RSDT_ENTRY_SIZE)
		address = ACPI_DECODE32(table_entry);
	else
		address = ACPI_DECODE64(table_entry);

	return address;
}

acpi_status_t acpi_xsdt_verify(acpi_addr_t xsdt_address)
{
	struct acpi_table_header *table;
	uint8_t *next_entry;
	acpi_addr_t address;
	uint32_t length;
	uint32_t entry_count;
	acpi_status_t status = AE_OK;
	uint32_t i;

	table = acpi_os_map_memory(xsdt_address, sizeof (struct acpi_table_header));
	if (!table)
		return AE_NO_MEMORY;

	length = ACPI_DECODE32(&table->length);
	acpi_os_unmap_memory(table, sizeof (struct acpi_table_header));
	if (length < (sizeof (struct acpi_table_header) + ACPI_XSDT_ENTRY_SIZE))
		return AE_INVALID_TABLE_LENGTH;

	table = acpi_os_map_memory(xsdt_address, length);
	if (!table)
		return AE_NO_MEMORY;
	next_entry = ACPI_ADD_PTR(uint8_t, table, sizeof (struct acpi_table_header));
	entry_count = (length - sizeof (struct acpi_table_header)) / ACPI_XSDT_ENTRY_SIZE;
	for (i = 0; i < entry_count; i++) {
		address = acpi_xsdt_get_entry(next_entry, ACPI_XSDT_ENTRY_SIZE);
		if (!address) {
			status = AE_NULL_ENTRY;
			break;
		}
		next_entry += ACPI_XSDT_ENTRY_SIZE;
	}
	acpi_os_unmap_memory(table, length);

	return status;
}

acpi_status_t acpi_xsdt_parse(acpi_addr_t xsdt_address, uint32_t table_entry_size)
{
	acpi_status_t status;
	uint32_t i, table_count;
	acpi_ddb_t ddb;
	uint8_t *table_entry;
	struct acpi_table_header *table;
	uint32_t length;

	/* Map RSDT/XSDT to obtain table length */
	table = acpi_os_map_memory(xsdt_address, sizeof (struct acpi_table_header));
	if (!table)
		return AE_NO_MEMORY;
	/* acpi_table_dump_header(address, table); */
	length = ACPI_DECODE32(&table->length);
	acpi_os_unmap_memory(table, sizeof (struct acpi_table_header));

	if (length < (sizeof (struct acpi_table_header) + table_entry_size)) {
		acpi_err_bios("Invalid table length 0x%X in RSDT/XSDT", length);
		return AE_INVALID_TABLE_LENGTH;
	}

	/* Map RSDT/XSDT */
	table = acpi_os_map_memory(xsdt_address, length);
	if (!table)
		return AE_NO_MEMORY;
	if (!__acpi_table_checksum_valid(table)) {
		acpi_os_unmap_memory(table, length);
		return AE_BAD_CHECKSUM;
	}

	/* Remember table address in the global table list */
	table_count = (uint32_t)((acpi_table_get_length(table) -
				  sizeof (struct acpi_table_header)) /
				 table_entry_size);
	table_entry = ACPI_ADD_PTR(uint8_t, table, sizeof (struct acpi_table_header));
	for (i = 0; i < table_count; i++) {
		status = acpi_table_install(acpi_xsdt_get_entry(table_entry,
								table_entry_size),
					    ACPI_TAG_NULL,
					    ACPI_TABLE_INTERNAL_PHYSICAL,
					    true, true, &ddb);
		if (ACPI_SUCCESS(status)) {
			acpi_get_table(ddb, &table);
			if (ACPI_NAMECMP(ACPI_SIG_FADT, table->signature))
				acpi_fadt_parse(table);
			acpi_put_table(ddb, table);
			acpi_table_decrement(ddb);
		}
		table_entry += table_entry_size;
	}

	/* Unmap RSDT/XSDT */
	acpi_os_unmap_memory(table, length);

	return AE_OK;
}
