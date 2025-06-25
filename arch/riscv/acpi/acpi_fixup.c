#include <target/acpi.h>
#include <target/sbi.h>

static char *acpi_vid;
static char *acpi_pid;

#ifndef CONFIG_ACPI_RHCT
extern void acpi_fixup_rhct(void);
#else
#define acpi_fixup_rhct()		do { } while (0)
#endif

char *acpi_get_vid(void)
{
	return acpi_vid;
}

char *acpi_get_pid(void)
{
	return acpi_pid;
}

#define FIXUP_TABLE_WITH_HEADER(table) \
	do { \
		if (table) { \
			ACPI_OEMCPY(table->header.oem_id, acpi_get_vid(), ACPI_OEM_ID_SIZE); \
			ACPI_OEMCPY(table->header.oem_table_id, acpi_get_pid(), ACPI_OEM_TABLE_ID_SIZE); \
			acpi_table_calc_checksum(&table->header); \
			printf("Fixing up table %4.4s at %p\n", table->header.signature, table);\
		} \
	} while (0)

#define FIXUP_TABLE_WITHOUT_HEADER(table) \
	do { \
		if (table) { \
			ACPI_OEMCPY(table->oem_id, acpi_get_vid(), ACPI_OEM_ID_SIZE); \
			ACPI_OEMCPY(table->oem_table_id, acpi_get_pid(), ACPI_OEM_TABLE_ID_SIZE); \
			acpi_table_calc_checksum(table); \
			printf("Fixing up table %4.4s at %p\n", table->signature, table); \
		} \
	} while (0)

#define FIXUP_RSDP_TABLE(table) \
	do { \
		if (table) { \
			ACPI_OEMCPY(table->oem_id, acpi_get_vid(), ACPI_OEM_ID_SIZE); \
			acpi_rsdp_calc_checksum(table); \
			printf("Fixing up RSDP table at %p\n", table); \
		} \
	} while (0)

void acpi_fixups(char *oem, char *oem_table)
{
	struct acpi_table_rsdp *rsdp;
	struct acpi_table_xsdt *xsdt;
	struct acpi_table_fadt *fadt;
	struct acpi_table_header *dsdt;
	struct acpi_table madt, spcr;
	int i;
	acpi_status_t status;

	acpi_addr_t dsdt_addr = 0;
	acpi_addr_t rsdp_addr = 0;
	acpi_addr_t xsdt_addr = 0;
	acpi_addr_t fadt_addr = 0;
	acpi_vid = oem;
	acpi_pid = oem_table;

	/* Get RSDP address and map it */
	rsdp_addr = acpi_os_get_root_pointer();
	if (!rsdp_addr) {
		printf("Failed to get RSDP address\n");
		return;
	}
	printf("RSDP address: 0x%llx\n", (unsigned long long)rsdp_addr);

	rsdp = acpi_os_map_memory(rsdp_addr, sizeof(struct acpi_table_rsdp));
	if (!rsdp) {
		printf("Failed to map RSDP table\n");
		return;
	}

	/* Fixup RSDP table */
	FIXUP_RSDP_TABLE(rsdp);

	/* Get XSDT address from RSDP */
	if (rsdp->revision >= 2 && rsdp->xsdt_physical_address) {
		xsdt_addr = rsdp->xsdt_physical_address;
		printf("XSDT address: 0x%llx\n", (unsigned long long)xsdt_addr);

		xsdt = acpi_os_map_memory(xsdt_addr, sizeof(struct acpi_table_xsdt));
		if (xsdt) {
			FIXUP_TABLE_WITH_HEADER(xsdt);

			/* Find FADT in XSDT */
			int table_count = (xsdt->header.length - sizeof(struct acpi_table_xsdt)) / sizeof(uint64_t);
			for (i = 0; i < table_count; i++) {
				acpi_addr_t table_addr = xsdt->table_offset_entry[i];
				struct acpi_table_header *table = acpi_os_map_memory(table_addr, sizeof(struct acpi_table_header));

				if (table && ACPI_NAMECMP(ACPI_SIG_FADT, table->signature)) {
					fadt_addr = table_addr;
					printf("Found FADT at address: 0x%llx\n", (unsigned long long)fadt_addr);
					acpi_os_unmap_memory(table, sizeof(struct acpi_table_header));
					break;
				}

				if (table)
					acpi_os_unmap_memory(table, sizeof(struct acpi_table_header));
			}
		}
	} else {
		printf("XSDT not available, using RSDT\n");
		/* Fallback to RSDT if XSDT not available */
		acpi_addr_t rsdt_addr = rsdp->rsdt_physical_address;
		if (rsdt_addr) {
			struct acpi_table_rsdt *rsdt = acpi_os_map_memory(rsdt_addr, sizeof(struct acpi_table_rsdt));
			if (rsdt) {
				FIXUP_TABLE_WITH_HEADER(rsdt);

				/* Find FADT in RSDT */
				int table_count = (rsdt->header.length - sizeof(struct acpi_table_rsdt)) / sizeof(uint32_t);
				for (i = 0; i < table_count; i++) {
					acpi_addr_t table_addr = rsdt->table_offset_entry[i];
					struct acpi_table_header *table = acpi_os_map_memory(table_addr, sizeof(struct acpi_table_header));

					if (table && ACPI_NAMECMP(ACPI_SIG_FADT, table->signature)) {
						fadt_addr = table_addr;
						printf("Found FADT at address: 0x%llx\n", (unsigned long long)fadt_addr);
						acpi_os_unmap_memory(table, sizeof(struct acpi_table_header));
						break;
					}

					if (table)
						acpi_os_unmap_memory(table, sizeof(struct acpi_table_header));
				}
				acpi_os_unmap_memory(rsdt, sizeof(struct acpi_table_rsdt));
			}
		}
	}

	/* Map and fixup FADT */
	if (fadt_addr) {
		fadt = acpi_os_map_memory(fadt_addr, sizeof(struct acpi_table_fadt));
		if (fadt) {
			FIXUP_TABLE_WITH_HEADER(fadt);

			/* Get DSDT address from FADT */
			if (fadt->Xdsdt) {
				dsdt_addr = fadt->Xdsdt;
				printf("Using 64-bit DSDT address: 0x%llx\n", (unsigned long long)dsdt_addr);
			} else if (fadt->dsdt) {
				dsdt_addr = fadt->dsdt;
				printf("Using 32-bit DSDT address: 0x%x\n", (unsigned int)dsdt_addr);
			}

			if (dsdt_addr) {
				dsdt = acpi_os_map_memory(dsdt_addr, sizeof(struct acpi_table_header));
				if (dsdt && ACPI_NAMECMP(ACPI_SIG_DSDT, dsdt->signature)) {
					FIXUP_TABLE_WITHOUT_HEADER(dsdt);
					acpi_os_unmap_memory(dsdt, sizeof(struct acpi_table_header));
				} else {
					printf("DSDT not found or invalid signature\n");
					if (dsdt)
						acpi_os_unmap_memory(dsdt, sizeof(struct acpi_table_header));
				}
			}
		}
	}

	/* Cleanup */
	if (rsdp)
		acpi_os_unmap_memory(rsdp, sizeof(struct acpi_table_rsdp));
	if (xsdt)
		acpi_os_unmap_memory(xsdt, sizeof(struct acpi_table_xsdt));
	if (fadt)
		acpi_os_unmap_memory(fadt, sizeof(struct acpi_table_fadt));

	status = acpi_get_table_by_inst(ACPI_NAME2TAG(ACPI_SIG_MADT), 0, &madt);
	if(ACPI_SUCCESS(status))
		FIXUP_TABLE_WITHOUT_HEADER(madt.pointer);


	status = acpi_get_table_by_inst(ACPI_NAME2TAG(ACPI_SIG_SPCR), 0, &spcr);
	if(ACPI_SUCCESS(status))
		FIXUP_TABLE_WITHOUT_HEADER(spcr.pointer);

	acpi_fixup_rhct();

	printf("ACPI fixups completed\n");
}
