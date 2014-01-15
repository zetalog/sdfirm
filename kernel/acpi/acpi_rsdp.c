#include <target/acpi.h>

boolean acpi_rsdp_checksum_valid(struct acpi_table_rsdp *rsdp)
{
	if (!ACPI_RSDP_SIG_CMP(rsdp->signature))
		return false;
	if (acpi_checksum_calc(rsdp, ACPI_RSDP_CHECKSUM_LENGTH) != 0)
		return false;
	if (ACPI_DECODE8(rsdp->revision) >= 2 &&
	    acpi_checksum_calc(rsdp, ACPI_RSDP_XCHECKSUM_LENGTH) != 0)
		return false;

	return true;
}

void acpi_rsdp_calc_checksum(struct acpi_table_rsdp *rsdp)
{
	ACPI_ENCODE8(&rsdp->checksum, 0);
	ACPI_ENCODE8(&rsdp->checksum,
		     acpi_checksum_calc(rsdp, ACPI_RSDP_CHECKSUM_LENGTH));
	if (ACPI_DECODE32(&rsdp->length)) {
		ACPI_ENCODE8(&rsdp->extended_checksum, 0);
		ACPI_ENCODE8(&rsdp->extended_checksum,
			     acpi_checksum_calc(rsdp, ACPI_RSDP_XCHECKSUM_LENGTH));
	}
}

acpi_status_t acpi_rsdp_parse(acpi_addr_t rsdp_address)
{
	struct acpi_table_rsdp *rsdp;
	acpi_addr_t address;
	acpi_status_t status;
	uint32_t table_entry_size;

	/* Map RSDP and obtain RSDT/XSDT information */
	rsdp = acpi_os_map_memory(rsdp_address, sizeof (struct acpi_table_rsdp));
	if (!rsdp)
		return AE_NO_MEMORY;
	/* acpi_table_dump_header(rsdp_address, ACPI_CAST_PTR(struct acpi_table_header, rsdp)); */
	if ((ACPI_DECODE8(&rsdp->revision) > 1) &&
	    ACPI_DECODE64(&rsdp->xsdt_physical_address)/* && !AcpiGbl_DoNotUseXsdt*/) {
		address = (acpi_addr_t)ACPI_DECODE64(&rsdp->xsdt_physical_address);
		table_entry_size = ACPI_XSDT_ENTRY_SIZE;
	} else {
		address = (acpi_addr_t)ACPI_DECODE32(&rsdp->rsdt_physical_address);
		table_entry_size = ACPI_RSDT_ENTRY_SIZE;
	}
	if (table_entry_size == ACPI_XSDT_ENTRY_SIZE) {
		status = acpi_xsdt_verify(address);
		if (ACPI_FAILURE(status)) {
			acpi_warn_bios("XSDT is invalid, using RSDT");
			/* Fall back to the RSDT */
			address = (acpi_addr_t)ACPI_DECODE64(&rsdp->rsdt_physical_address);
			table_entry_size = ACPI_RSDT_ENTRY_SIZE;
		}
	}
	acpi_os_unmap_memory(rsdp, sizeof (struct acpi_table_rsdp));

	return acpi_xsdt_parse(address, table_entry_size);
}
